#include <iostream>
#include <vector>
#include "leveldb/db.h"

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::DB::Open(options, "/tmp/testdb", &db);

    // Insert 20 keys
    for (int i = 0; i < 20; i++) {
        char k[16], v[16];
        snprintf(k, sizeof(k), "key%02d", i);
        snprintf(v, sizeof(v), "val%02d", i);
        db->Put(leveldb::WriteOptions(), k, v);
    }

    // Delete range [key05, key10)
    db->DeleteRange(leveldb::WriteOptions(), "key05", "key10");

    // Check deleted key
    std::string val;
    leveldb::Status s = db->Get(leveldb::ReadOptions(), "key05", &val);
    std::cout << "key05 found? " << (s.ok() ? "YES (BAD)" : "NO (GOOD)") << std::endl;

    // Check key that should still exist
    s = db->Get(leveldb::ReadOptions(), "key04", &val);
    std::cout << "key04 = " << val << std::endl;

    s = db->Get(leveldb::ReadOptions(), "key10", &val);
    std::cout << "key10 = " << val << std::endl;

    // Scan full range - should show 15 keys
    std::vector<std::pair<std::string, std::string>> result;
    db->Scan(leveldb::ReadOptions(), "key00", "key99", &result);
    std::cout << "Total keys after delete: " << result.size() << std::endl;

    delete db;
    return 0;
}