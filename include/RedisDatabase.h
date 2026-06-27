#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include<string>
#include<mutex>
#include<unordered_map>
#include<vector>
class RedisDatabase {

    public:
    //singleton instance
    static RedisDatabase& getInstance();

    //persistance: dump,load the database from a file
     bool dump(const std::string& filename);
     bool load(const std::string& filename);


    private:
    RedisDatabase()=default;//Use the compiler-generated constructor.
    ~RedisDatabase()=default;//Use the compiler-generated destructor.
    RedisDatabase(const RedisDatabase&)=delete;//use to delete the copy constructor ex.RedisDatabase copy = db;this done using complier hidden copy constructor
    RedisDatabase& operator=(const RedisDatabase&)=delete;//use to delete the copy assignment operator RedisDatabase db1; RedisDatabase db2; db2 = db1;

    std::mutex db_mutex;
    std::unordered_map<std::string,std::string>kv_store;
    std::unordered_map<std::string,std::vector<std::string>>list_store;
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>hash_store;

};

#endif