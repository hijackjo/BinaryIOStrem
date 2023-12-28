#include <stdio.h>
#include "../BinaryIOStream.h"
#include "ClientTcp.h"
    

int main()
{
    //Build A TCP Connection
    ClientTcp connection("127.0.0.1", "1234");
    connection.init();
    //Prepare A Login Request
    std::string outbuf;
    BinaryStreamWriter writeStream(&outbuf);
    std::cout << outbuf.size() << std::endl;
    int32_t cmd = 1002;
    int32_t seq = 0;
    
    char szLoginInfo[256] = { 0 }; //准备要发送给客户端连接请求的协议部分
    snprintf(szLoginInfo,sizeof(szLoginInfo) - 1 , //sprintf_s can't be used in linux, use snprintf instead
        "{\"username\": \"%s\", \"password\": \"%s\", \"clienttype\": %d, \"status\": %d}",
        "Marry",  "123", 1, 1);
    
    Flush flush;
    writeStream << cmd << seq << szLoginInfo << flush; 
    
    //Send Login Request To Server
    connection.send(outbuf.c_str(), outbuf.size());

    //Receive Login Response
    //std::string inbuf;
    //connection.receive(inbuf);
    //std::cout << inbuf << std::endl;

    return 0;
}

