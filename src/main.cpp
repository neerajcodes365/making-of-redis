#include "../include/RedisServer.h"
#include<iostream>

int main(int argc,char*argv[]){
    int port=6379;// deafult value
    if(argc>=2) port=std::atoi(argv[1]);

    RedisServer server(port);

    return 0;

}