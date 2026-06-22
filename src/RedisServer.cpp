#include "include/RedisServer.h"
#include <iostream>
#include <sys/socket.h>//socket prgming,socket apis
#include <unistd.h>//linux syscall ,close()
#include <netinet/in.h>

static RedisServer *globalServer=nullptr;//signal handler!!

RedisServer::RedisServer(int port) {
    this->port = port;
    server_socket = -1;
    running = true;

    globalServer = this;
}


void RedisServer::shutdown() {
    running = false;
    if (server_socket != -1) {
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

}
   
