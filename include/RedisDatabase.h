#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include<string>
#include<mutex>
#include<unordered_map>
#include<vector>
#include<chrono>

class RedisDatabase {

    public:
    //singleton instance
    static RedisDatabase& getInstance();
    //common commands
    bool flushAll();
    
    // key/value operations
    void set(const std::string&key,const std::string&value);
    bool get(const std::string&key ,std::string&value);
    std::vector<std::string>keys();
    std::string type(const std::string&key);
    bool del(const std::string& key);
    bool expire(const std::string&key,int seconds);
    bool rename(const std::string& oldkey,const std::string& newkey);
    
    // List Opreations
    // ee stageil indo doubt ind 
    //std::vector<std::string> lget(const std::string& key);
    ssize_t llen(const std::string& key);
    void lpush(const std::string& key, const std::string& value);
    void rpush(const std::string& key, const std::string& value);
    bool lpop(const std::string& key, std::string& value);
    bool rpop(const std::string& key, std::string& value);
    int lrem(const std::string& key, int count, const std::string& value);
    bool lindex(const std::string& key, int index, std::string& value);
    bool lset(const std::string& key, int index, const std::string& value);
    

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

    std::unordered_map<std::string,std::chrono::steady_clock::time_point>expiry_map;//for expire fun.
};
#endif