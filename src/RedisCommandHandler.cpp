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

RedisCommandHandler::RedisCommandHandler(){}

std::string RedisCommandHandler::processCommand(const std::string& commandLine){
    //use resp parser
    auto tokens=parseRespCommand(commandLine);
    if(tokens.empty())return "-Error : Empty command \r\n";
  
    // //testing area 
    // std::cout<<commandLine<<"\n";
    // for(auto &t:tokens){
    //     std::cout<<t<<"\n";
    // }
    // //

//simply putting 

    std::string cmd=tokens[0];
    std::transform(cmd.begin(),cmd.end(),cmd.begin(),::toupper);//changing the string to uppercase

    std::ostringstream response;// ostringstream =Used to write into a string.oppsite of istringstream
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
    if (cmd == "PING"){
        response << "+PONG\r\n";
    } 
    else if(cmd=="ECHO"){
        if(tokens.size()<2){
            response<<"Error: ECHO needs two parameters";
        }
        else{
            response<<"+"<<tokens[1]<<"\r\n";
        }
    }
    else if(cmd=="FLUSHALL"){
        db.flushAll();
        response<<"+OK\r\n";
    }
    // keyvalue operations
    else if(cmd=="SET"){
            if(tokens.size()<3){
                response<<"Error : SET needs 3 parameters";
            }
            else{
                db.set(tokens[1],tokens[2]);
                response<<"+OK\r\n";
            }
    }
    else if(cmd=="GET"){
            if(tokens.size()<2){
                response<<"Error : GET requires key\r\n";
            }
            else{
                std::string value;
                if(db.get(tokens[1],value)){
                    response<<"$"<<value.size()<<"\r\n"<<value<<"\r\n";
                }
                else{
                    response<<"$-1\r\n";
                }
            }
    }
    else if( cmd=="KEYS"){
        std::vector<std::string> allkeys=db.keys();
        response<<"*"<<allkeys.size()<<"\r\n";
        for(const auto&key:allkeys){
            response<<"$"<<key.size()<<"\r\n"<<key<<"\r\n";
        }
    }
    else if(cmd=="TYPE"){
          if(tokens.size()<2){
                response<<"Error : TYPE requires key\r\n";
            }
            else{
                response<<"+"<<db.type(tokens[1])<<"\r\n";
            }
    }
    else if(cmd=="DEL"|| cmd=="UNLINK"){
        if(tokens.size()<2){
            response<<"-Error :"<<cmd<<"requires key \r\n";
        }
        else{
            bool res=db.del(tokens[1]);
            response<<":"<<(res?1:0)<<"\r\n";
        }
    }
    else if(cmd=="EXPIRE"){
        if(tokens.size()<3){
            response<<"-Error: EXPIRE requires key and time in seconds\r\n";
        }
        else{
            if(db.expire(tokens[1],tokens[2])){
                response<<"+OK\r\n";
            }//will complete in future
            else{
                // response<<will do in next stage 
            }
        }
    }
    else if(cmd=="RENAME"){
         if(tokens.size()<3){
            response<<"-Error: RENAME requires oldkey and newkey name \r\n";
        }else{
            if(db.rename(tokens[1],tokens[2])){
                response<<"+OK\r\n";
            }
        }
    }


    //list operations
    //hash operations
    else{
        response<<"-Error : Unknown Command \r\n";
    }
    return response.str();
}