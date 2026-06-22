#include"include/RedisCommandHandler.h"
#include<vector>
#include<sstream>
#include<algorithm>
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

        if(pos +len >input.size()) break;
        std::string token =input.substr(pos,len);
        tokens.push_back(token);
        pos+=len+2;//goes to $ again
    }


}

RedisCommandHandler::RedisCommandHandler(){}

std::string RedisCommandHandler::processCommand(const std::string& commandLine){
    //use resp parser
    auto tokens=parseRespCommand(commandLine);
    if(tokens.empty())return "-Error : Empty command \r\n";

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

    //check commands

    return response.str();
}