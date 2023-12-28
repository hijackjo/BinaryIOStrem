#pragma once
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
class ServerTcp final
{
public:
    ServerTcp();
    ~ServerTcp();
    void init();
    void send(const char * pBuffer, int nBufferSize);
    void receive(char * buf, size_t length);
private:
    int _sockfd;
    int _netfd;
};

