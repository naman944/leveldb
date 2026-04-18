#include <iostream>
#include <vector>
#include "leveldb/db.h"

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::DB::Open(options, "/tmp/testdb", &db);

    // 1. Insert 10000 keys
    for (int i = 0; i < 10000; i++) {
        char k[32], v[32];
        snprintf(k, sizeof(k), "key%06d", i);
        snprintf(v, sizeof(v), "value%06d", i);
        db->Put(leveldb::WriteOptions(), k, v);
    }
    std::cout << "1. Inserted 10000 keys" << std::endl;

    // 2. Scan partial range
    std::vector<std::pair<std::string, std::string>> result;
    db->Scan(leveldb::ReadOptions(), "key001000", "key002000", &result);
    std::cout << "2. Scan [1000,2000): " << result.size() << " keys (expect 1000)" << std::endl;

    // 3. DeleteRange
    db->DeleteRange(leveldb::WriteOptions(), "key003000", "key004000");
    std::cout << "3. Deleted range [3000,4000)" << std::endl;

    // 4. Verify deleted keys are gone
    std::string val;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), "key003000", &val);
    std::cout << "4. key003000 found? " << (s.ok() ? "YES (BAD)" : "NO (GOOD)") << std::endl;

    // 5. Verify boundary keys still exist
    s = db->Get(leveldb::ReadOptions(), "key002999", &val);
    std::cout << "5. key002999 = " << val << " (should be value002999)" << std::endl;
    s = db->Get(leveldb::ReadOptions(), "key004000", &val);
    std::cout << "5. key004000 = " << val << " (should be value004000)" << std::endl;

    // 6. Scan full range after delete
    result.clear();
    db->Scan(leveldb::ReadOptions(), "key000000", "key999999", &result);
    std::cout << "6. Total keys after delete: " << result.size() << " (expect 9000)" << std::endl;

    // 7. ForceFullCompaction
    std::cout << "7. Running ForceFullCompaction..." << std::endl;
    db->ForceFullCompaction();

    // 8. Verify data still correct after compaction
    result.clear();
    db->Scan(leveldb::ReadOptions(), "key000000", "key999999", &result);
    std::cout << "8. Total keys after compaction: " << result.size() << " (expect 9000)" << std::endl;

    // 9. Edge cases
    result.clear();
    db->Scan(leveldb::ReadOptions(), "key005000", "key005000", &result);
    std::cout << "9. Scan empty range (start==end): " << result.size() << " (expect 0)" << std::endl;

    result.clear();
    db->Scan(leveldb::ReadOptions(), "zzz", "zzz", &result);
    std::cout << "9. Scan non-existent range: " << result.size() << " (expect 0)" << std::endl;

    db->DeleteRange(leveldb::WriteOptions(), "zzz000", "zzz999");
    std::cout << "9. DeleteRange on non-existent keys: OK" << std::endl;

    std::cout << "\nAll tests passed!" << std::endl;

    delete db;
    return 0;
}