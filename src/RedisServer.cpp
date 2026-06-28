#include "RedisServer.h"
#include "RedisCommandHandler.h"
#include "RedisDatabase.h"
#include <iostream>
#include <sys/socket.h>//socket prgming,socket apis
#include <unistd.h>//linux syscall ,close()
#include <netinet/in.h>
#include<vector>
#include<thread>
#include<cstring>
#include<signal.h>

static RedisServer *globalServer=nullptr;//signal handler!!

void signalHandler(int signum){
    if(globalServer){
        std::cout<<" Caught Signal "<<signum<<",shutting down....\n";
        globalServer->shutdown();
    }
    exit(signum);
}

void RedisServer::setupSignalHandler(){
    signal(SIGINT,signalHandler);
}


// RedisServer::RedisServer(int port) {
//     this->port = port;
//     server_socket = -1;
//     running = true;

//     globalServer = this;
// }
RedisServer::RedisServer(int port):port(port),server_socket(-1),running(true){
    globalServer=this;
    setupSignalHandler();

}


void RedisServer::shutdown() {
    running = false;
    if (server_socket != -1) {
        if(RedisDatabase::getInstance().dump("dump.my_rdb")){
            std::cout<<"Database Dumped to dump.my_rdb\n";
        }else{
            std::cerr<<"Error dumping database \n";
        }
        close(server_socket);
    }
    std::cout << "Server Shutdown Complete!\n";
}


void RedisServer::run(){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);//fd

    if (server_socket < 0) {
        std::cerr << "Error Creating Server Socket\n";//cerr no buffer , 
        return;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};//defining server address//inbuild struct// netinet.in.h
    serverAddr.sin_family = AF_INET;//any family
    serverAddr.sin_port = htons(port);//htons(): Converts port to network byte order.
    serverAddr.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY: Accept connections on any IP.

    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {//binding -1 shows error
        std::cerr << "Error Binding Server Socket\n";
        return;
    }//gives port and ip to socket instance of kernal pgrm

    if (listen(server_socket, 10) < 0) {//-1 shows error
        std::cerr << "Error Listening On Server Socket\n";
        return;
    }//listens the incoming client request and putting in queue
    
    std::cout << "Redis Server Listening On Port " << port << "\n";

    std::vector<std::thread>threads;//for every single user , congueryncy
    RedisCommandHandler cmdHandler;//parsing class object

    while(running){//Keep accepting new client connections until the server stops
        int client_socket=accept(server_socket,nullptr,nullptr);
        if(client_socket<0){//error of fd -1
            if(running){
                std::cerr<<"Error accepting client connection\n";
            }
            break;
        }

        threads.emplace_back([client_socket,&cmdHandler](){//it skips creating a temporary object. emplaceback vs pushback
            char buffer[1024];
            while(true){
                /*recieve the commands and check them in the command handler ,
                 we need to check for multiple clients thats why client*/
                 memset(buffer,0,sizeof(buffer)); //memset(pointer, value, number_of_bytes);
                 int bytes=recv(client_socket,buffer,sizeof(buffer)-1,0);
                 if(bytes<=0)break;
                std::string request(buffer,bytes);//buffer → starting address of the character array.bytes → copy exactly bytes characters.
                std::string response=cmdHandler.processCommand(request);
                send(client_socket,response.c_str(),response.size(),0);
            }
            close(client_socket);
        });
        /*
Main thread continuously accepts new client connections.

For every new client:
1. A new std::thread is created and stored in the 'threads' vector using emplace_back().
2. As soon as the thread is created, it immediately starts executing the lambda function.
3. The lambda enters its own loop (while(true)) to receive, process, and respond to requests from that specific client.
4. Meanwhile, the main thread does not wait—it immediately goes back to accept() to listen for more clients.

Therefore, if 5 clients connect, there will be:
- 1 main thread accepting new connections.
- 5 worker threads running the lambda concurrently, each serving one client independently.

This is what allows multiple clients to be handled simultaneously without blocking each other.
*/  

        for(auto&t:threads){
            if(t.joinable())t.join();// thread t waits for other threads to finish .ie join();
        }
/*
    Before shutting down the server, wait for all worker threads to finish.
    When 'running' becomes false, the main thread exits the accept() loop and
    reaches this point. However, some client-handling threads may still be
    processing requests.
    Calling join() waits for each worker thread to complete, ensuring a
    graceful shutdown and preventing std::terminate() when the std::thread
    objects are destroyed.
*/
        //shutdown function.
        //before shutdown, persist the database 
        if(RedisDatabase::getInstance().dump("dump.my_rdb")){
            std::cout<<"Database Dumped to dump.my_rdb\n";
        }else{
            std::cerr<<"Error dumping database \n";
        }


        

    }

}
   
