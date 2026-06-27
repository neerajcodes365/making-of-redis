#include"RedisDatabase.h"
#include<string>
#include<sstream>

#include<mutex>//every five mintues we are dumping , also we get client requests, so there is a chance for data corruption
#include<fstream>//i/o header
/*
#include <fstream> enables file operations in C++.
std::ifstream → Read from a file.
std::ofstream → Write to a file.
*/




RedisDatabase& RedisDatabase::getInstance(){
    static RedisDatabase instance;
    return instance;
}

//key value operations
//list operations
//hash operations

/*
memory to file dump()
file to memory load()

K=key value
L=list
H=hash

*/


/*
Key-Value (K)
kv_store["name"] = "Alice";
kv_store["city"] = "Berlin";

List (L)
list_store["fruits"] = {"apple", "banana", "orange"};
list_store["colors"] = {"red", "green", "blue"};

Hash (H)
hash_store["user:100"] = {
    {"name", "Bob"},
    {"age", "30"},
    {"email", "bob@example.com"}
};

hash_store["user:200"] = {
    {"name", "Eve"},
    {"age", "25"},
    {"email", "eve@example.com"}
};
*/
/* dumpfile data 
K name Alice
K city Berlin
L fruits apple banana orange
L colors red green blue
H user:100 name:Bob age:30 email:bob@example.com
H user:200 name:Eve age:25 email:eve@example.com
*/


bool RedisDatabase::dump(const std::string&filename){
    std::lock_guard<std::mutex>lock(db_mutex);//no deadlock from now ,automatic locking , no manual locking
    std::ofstream ofs(filename,std::ios::binary);//used to write data into a file ,open the file in binary mode, binary mode =raw bytes,
    /*
    Text mode stores data as text characters (ASCII/UTF-8 characters).
Binary mode stores data as raw bytes exactly as they exist in memory
each memory location have some bits , they just write it in the file
*/
    if(!ofs) return false;
//writing to file from maps
    for(const auto& kv:kv_store){
        ofs<<"K"<<kv.first<<" "<<kv.second<<"\n";//writes into ofs 
    }
    for (const auto& kv : list_store) {
        ofs << "L" << kv.first;
        for (const auto& item : kv. second)
            ofs << " " << item;
        ofs << "\n";
    }
    for (const auto& kv : hash_store){
        ofs << "H " << kv.first;
        for (const auto& field_val : kv.second)
            ofs << "" << field_val.first << ":" << field_val.second;
        ofs << "\n";
    }

    return true;
}


bool RedisDatabase::load(const std::string&filename){
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ifstream ifs(filename,std::ios::binary);
    if(!ifs)return false;



    kv_store.clear();//clearing this , later we will add it 
    list_store.clear();
    hash_store.clear();

    //parsing&storing in maps from the input file
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        char type;
        iss >> type;
        if (type == 'K') {
            std::string key, value;
            iss >> key >> value;
            kv_store[key] = value;
        } else if (type == 'L') {
            std::string key;
            iss >> key;
            std::string item;
            std::vector<std::string> list;
            while (iss >> item)
                list.push_back(item);
            list_store[key] = list;
        } else if (type == 'H') {
            std::string key;
            iss >> key;
            std::unordered_map<std::string, std::string> hash;
            std::string pair;
            while (iss >> pair) {
                auto pos = pair.find(':');
                if (pos != std::string::npos) {
                    std::string field = pair.substr(0, pos);
                    std::string value = pair.substr(pos+1);
                    hash[field] = value;
                }
            }
            hash_store[key] = hash;
        }
    } 
    return true;
}
