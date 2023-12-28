#include "ServerTcp.h"
#include "../BinaryIOStream.h"

int main()
{
    //Build A TCP Connection
    ServerTcp connection;
    connection.init();

    //Receive Message
    char inbuf[256] = { 0 };
    connection.receive(inbuf, sizeof(inbuf));

    //Read LoginRequest
    BinaryStreamReader bio(inbuf, strlen(inbuf));
    int32_t cmd, seq;

    std::string data;
    bio >> cmd >> seq >> data;
    std::cout << "Server Receive A Login Request." << std::endl;
    std::cout << "cmd = " << cmd << std::endl;
    std::cout << "seq = " << seq << std::endl;
    std::cout << "data = " << data << std::endl;

    return 0;
}

