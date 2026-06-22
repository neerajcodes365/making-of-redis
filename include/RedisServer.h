#ifndef REDIS_SERVER_H// macro ,only one instance is possible
#define REDIS_SERVER_H

#include <string>
#include <atomic>


class RedisServer{
    public:
    RedisServer(int port);
    void run();
    void shutdown();
    


    private:
    int port;
    int server_socket;
    std::atomic<bool>running;


};
#endif