#include "RedisServer.h"
#include "RedisDatabase.h"
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
        if(RedisDatabase::getInstance().dump("dump.my_rdb")){
            std::cerr<<"Error Dumping Database \n";
        }
        else{
            std::cout<<"database Dumped to dump.mu_rdb\n";
        }

        }

    });
    persistanceThread.detach();/*This thread should continue running independently. I (the main thread) will no longer manage it.
     Main Thread
      |
      |------ creates worker thread
      |
      |------ detach()
      |
      |------ exits

Worker Thread
      |
      |------ continues running independently*/

    server.run();//socket connection function
 
    return 0;

}