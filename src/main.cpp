#include "../include/RedisServer.h"
#include<iostream>
#include<thread>
#include<chrono>


int main(int argc,char*argv[]){
    int port=6379;// deafult value
    if(argc>=2) port=std::atoi(argv[1]);

    RedisServer server(port);


    //backgournd persistance : database dump after every 300s . using  thread concept
    std::thread persistanceThread([]{
        while(true){
        std::this_thread::sleep_for(std::chrono::seconds(300));
        // dump the database,to be implemented
        }

    });
    persistanceThread.detach();

    server.run();//socket connection function
 
    return 0;

}