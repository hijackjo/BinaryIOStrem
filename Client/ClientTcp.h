#pragma once
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

class ClientTcp final
{
public:
    ClientTcp(const char * ip, const char * port);
    ~ClientTcp();
    void init();
    void send(const char * str, int length);
    void receive(std::string & str);
private:
    const char *   _ip;
    const char * _port;
    int        _sockfd;
    int         _netfd;
};
    
