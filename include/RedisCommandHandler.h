#ifndef REDIS_COMMAND_HANDLER_H
#define REDIS_COMMAND_HANDLER_H

#include<string>


class RedisCommandHandler {
    public:
    RedisCommandHandler();

    std::string processCommand(const std::string& commandLine);//will process a command from a client and rerturn RESP format respone
};

#endif