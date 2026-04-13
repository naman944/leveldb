#include <iostream>
#include <string>
#include "leveldb/db.h"

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
    if (!status.ok()) {
        std::cerr << "Failed: " << status.ToString() << std::endl;
        return 1;
    }

    db->Put(leveldb::WriteOptions(), "name", "Alice");
    db->Put(leveldb::WriteOptions(), "age", "25");

    std::string value;
    db->Get(leveldb::ReadOptions(), "name", &value);
    std::cout << "name = " << value << std::endl;

    db->Delete(leveldb::WriteOptions(), "age");

    delete db;
    return 0;
}