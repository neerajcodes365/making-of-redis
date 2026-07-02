#include"RedisCommandHandler.h"
#include"RedisDatabase.h"
#include<vector>
#include<sstream>
#include<algorithm>
#include <iostream>//for testing

// RESP Parser will be used :
/*
 *2\r\n
$4\r\n
PING\r\n
$4\r\n
TEST\r\n
Array of 2 elements
 ├─ String of length 4 -> "PING"
 └─ String of length 4 -> "TEST"
*/

/*
s.find(str, pos);
str → what you want to search for (character, string, C-string, etc.)
pos → starting index from where the search begins (optional, default = 0)
*/


/*
std::vector<std::string> parseRespCommand( const std::string &input){
    std::vector<std::string> tokens;

    if(input.empty())return tokens;

    //if it doesnt start with * , fallback to splitting by whitespace
    if(input[0]!='*'){
        std::istringstream iss(input);
        std::string token ;
        while(iss>>token){
            tokens.push_back(token);
        }
        return tokens;
    }

    size_t pos=0;//unsigned interger
    if(input[pos]!='*')return tokens;
    pos++;//skiped the * //1

    /*
    CRLF
CRLF = Carriage Return + Line Feed
It is represented as:
"\r\n"
where:
\r = CR (Carriage Return, ASCII 13)
\n = LF (Line Feed, ASCII 10)
\r moved the cursor to the beginning of the line.
\n moved the cursor down to the next line.
    */

/*
    size_t crlf=input.find("\r\n",pos);//2
    if(crlf==std::string::npos)return tokens;

    int numElements=std::stoi(input.substr(pos,crlf-pos));//what if duble or triple digit number
    pos=crlf+2;

    for(int i=0;i<numElements;i++){
        if(pos>=input.size()||input[pos]!='$')break;//format error
        pos++;//skips dollar
        crlf=input.find("\r\n",pos);
        if(crlf==std::string::npos)break;
        int len=std::stoi(input.substr(pos,crlf-pos));

        pos=crlf+2;
        if(pos +len >input.size()) break;
        std::string token =input.substr(pos,len);
        tokens.push_back(token);
        pos+=len+2;//goes to $ again
    }
    return tokens;

}
*/
std::vector<std::string> parseRespCommand(const std::string &input) {
    std::vector<std::string> tokens;
    if (input.empty()) return tokens;

    // If it doesn't start with '*', fallback to splitting by whitespace
    if (input[0] != '*') {
        std::istringstream iss(input);
        std::string token;
        while (iss >> token)
            tokens.push_back(token);
        return tokens;
    }

    size_t pos = 0; // unsigned integer
    // Expect '*' followed by number of elements
    if (input[pos] != '*') return tokens;
    pos++; // skip '*'

    // CRLF = Carriage Return + Line Feed = "\r\n"
    // \r (CR, ASCII 13) moves cursor to beginning of line
    // \n (LF, ASCII 10) moves cursor down to next line
    size_t crlf = input.find("\r\n", pos);
    if (crlf == std::string::npos) return tokens;

    // Handles double or triple digit numbers too (stoi reads the full number)
    int numElements = std::stoi(input.substr(pos, crlf - pos));
    pos = crlf + 2;

    for (int i = 0; i < numElements; i++) {
        if (pos >= input.size() || input[pos] != '$') break; // format error
        pos++; // skip '$'

        crlf = input.find("\r\n", pos);
        if (crlf == std::string::npos) break;
        int len = std::stoi(input.substr(pos, crlf - pos));
        pos = crlf + 2;

        if (pos + len > input.size()) break;
        std::string token = input.substr(pos, len);
        tokens.push_back(token);
        pos += len + 2; // skip token and CRLF, goes to next '$'
    }
    return tokens;
}

RedisCommandHandler::RedisCommandHandler(){}

std::string RedisCommandHandler::processCommand(const std::string& commandLine){
    //use resp parser
    auto tokens=parseRespCommand(commandLine);
    if(tokens.empty())return "-Error : Empty command \r\n";
  
    //testing area 
   /*
    std::cout<<commandLine<<"\n";
    for(auto &t:tokens){
        std::cout<<t<<"\n";
    }
    */
    //

//simply putting 

    std::string cmd=tokens[0];
    std::transform(cmd.begin(),cmd.end(),cmd.begin(),::toupper);//changing the string to uppercase

    //now no need of that// std::ostringstream response;// ostringstream =Used to write into a string.oppsite of istringstream
/*
std::ostringstream oss;
oss << "Hello ";
oss << "World";
Nothing is printed yet.The stream internally contains:"Hello World"
std::string s = oss.str()
1;
Now:s == "Hello World"
*/


    //connect to database
    RedisDatabase& db=RedisDatabase::getInstance();

    //check common commands
  // Common Commands
    if (cmd == "PING")
        return handlePing(tokens, db);
    else if (cmd == "ECHO")
        return handleEcho(tokens, db);
    else if (cmd == "FLUSHALL")
        return handleFlushAll(tokens, db);
    // Key/Value Operations
    else if (cmd == "SET")
        return handleSet(tokens, db);
    else if (cmd == "GET")
        return handleGet(tokens, db);
    else if (cmd == "KEYS")
        return handleKeys(tokens, db);
    else if (cmd == "TYPE")
        return handleType(tokens, db);
    else if (cmd == "DEL" || cmd == "UNLINK")
        return handleDel(tokens, db);
    else if (cmd == "EXPIRE")
        return handleExpire(tokens, db);
    else if (cmd == "RENAME")
        return handleRename(tokens, db);
    //list operations
    // else if (cmd == "LGET") 
    //     return handleLget(tokens, db);
    else if (cmd == "LLEN") 
        return handleLlen(tokens, db);
    else if (cmd == "LPUSH")
        return handleLpush(tokens, db);
    else if (cmd == "RPUSH")
        return handleRpush(tokens, db);
    else if (cmd == "LPOP")
        return handleLpop(tokens, db);
    else if (cmd == "RPOP")
        return handleRpop(tokens, db);
    else if (cmd == "LREM")
        return handleLrem(tokens, db);
    else if (cmd == "LINDEX")
        return handleLindex(tokens, db);
    else if (cmd == "LSET")
        return handleLset(tokens, db);
    //hash operations
    // else{
        return "-Error : Unknown Command \r\n";
    // }
}

//----------------------
// Common Commands
//----------------------
static std::string handlePing(const std::vector<std::string>& tokens, RedisDatabase& db) {
    return "+PONG\r\n";
}

static std::string handleEcho(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 2)
        return "-Error: ECHO requires a message\r\n";
    return "+" + tokens[1] + "\r\n";
}

static std::string handleFlushAll(const std::vector<std::string>& tokens, RedisDatabase& db) {
    db.flushAll();
    return "+OK\r\n";
}

//----------------------
// Key/Value Operations
//----------------------
static std::string handleSet(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 3)
        return "-Error: SET requires key and value\r\n";
    db.set(tokens[1], tokens[2]);
    return "+OK\r\n";
}

static std::string handleGet(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 2)
        return "-Error: GET requires key\r\n";
    std::string value;
    if (db.get(tokens[1], value))
        return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
    return "$-1\r\n";
}

static std::string handleKeys(const std::vector<std::string>& /*tokens*/, RedisDatabase& db) {
    auto allKeys = db.keys();
    std::ostringstream oss;
    oss << "*" << allKeys.size() << "\r\n";
    for (const auto& key : allKeys)
        oss << "$" << key.size() << "\r\n" << key << "\r\n";
    return oss.str();
}

static std::string handleType(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 2)
        return "-Error: TYPE requires key\r\n";
    return "+" + db.type(tokens[1]) + "\r\n";
}

static std::string handleDel(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 2)
        return "-Error: DEL requires key\r\n";
    bool res = db.del(tokens[1]);
    return ":" + std::to_string(res ? 1 : 0) + "\r\n";
}

static std::string handleExpire(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 3)
        return "-Error: EXPIRE requires key and time in seconds\r\n";
    try {
        int seconds = std::stoi(tokens[2]);
        if (db.expire(tokens[1], seconds))
            return "+OK\r\n";
        else
            return "-Error: Key not found\r\n";
    } catch (const std::exception&) {
        return "-Error: Invalid expiration time\r\n";
    }
}

static std::string handleRename(const std::vector<std::string>& tokens, RedisDatabase& db) {
    if (tokens.size() < 3)
        return "-Error: RENAME requires old key and new key\r\n";
    if (db.rename(tokens[1], tokens[2]))
        return "+OK\r\n";
    return "-Error: Key not found or rename failed\r\n";
}


//List operations
// static std::string handleLget(const std::vector<std::string>&tokens , RedisDatabase& db){

// }

static std::string handleLlen  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 2) 
        return "-Error: LLEN requires key\r\n";
    ssize_t len = db.llen(tokens[1]);
    return ":" + std::to_string(len) + "\r\n";
}

static std::string handleLpush  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 3) 
        return "-Error: LPUSH requires key and value\r\n";
    db.lpush(tokens[1], tokens[2]);
    ssize_t len = db.llen(tokens[1]);
    return ":" + std::to_string(len) + "\r\n";
}

static std::string handleRpush  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 3) 
        return "-Error: RPUSH requires key and value\r\n";
    db.rpush(tokens[1], tokens[2]);
    ssize_t len = db.llen(tokens[1]);
    return ":" + std::to_string(len) + "\r\n";
}

static std::string handleLpop  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 2) 
        return "-Error: LPOP requires key \r\n";
    std::string val;
    if(db.lpop(tokens[1],val))
        return "$" + std::to_string(val.size())+"\r\n"+val+"\r\n";
    return "$-1\r\n";
}

static std::string handleRpop  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 2) 
        return "-Error: RPOP requires key \r\n";
    std::string val;
    if(db.lpop(tokens[1],val))
        return "$" + std::to_string(val.size())+"\r\n"+val+"\r\n";
    return "$-1\r\n";
}

static std::string handleLrem  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 4) 
        return "-Error: LREM requires key,Count and Value\r\n";
    try{
        int count=std::stoi(tokens[2]);
        int removed=db.lrem(tokens[1],count,tokens[3]);
        return ":"+std::to_string(removed)+"\r\n";
    }catch(std::exception&){
        return "-Error: INvalid count \r\n";
    }   
    }

static std::string handleLindex  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 3) 
        return "-Error: LINDEX requires key and index\r\n";
    try {
        int index = std::stoi(tokens[2]);
        std::string value;
        if (db.lindex(tokens[1], index, value)) 
            return "$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";
        else 
            return "$-1\r\n";
    } catch (const std::exception&) {
        return "-Error: Invalid index\r\n";
    }
}

static std::string handleLset  (const std::vector<std::string>&tokens , RedisDatabase& db){
    if (tokens.size() < 4) 
        return "-Error: LEST requires key, index and value\r\n";
    try {
        int index = std::stoi(tokens[2]);
        if (db.lset(tokens[1], index, tokens[3]))
            return "+OK\r\n";
        else 
            return "-Error: Index out of range\r\n";
    } catch (const std::exception&) {
        return "-Error: Invalid index\r\n";
    }    
}