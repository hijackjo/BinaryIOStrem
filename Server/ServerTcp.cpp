#include "ServerTcp.h"

ServerTcp::ServerTcp()
    :_sockfd(-1)
     ,_netfd(-1){

     }


ServerTcp::~ServerTcp(){
    ::close(_sockfd);
}//end Destructor

void ServerTcp::init(){
    //socket
    _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd == -1){
        ::perror("socket");
        return;
    }
    //No TIME_WAIT
    int reuseArg = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseArg, sizeof(reuseArg));

    //bind
    struct sockaddr_in addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t length = sizeof(addr);
    if(::bind(_sockfd, (struct sockaddr *) & addr, length) == -1){
        perror("bind");
        close(_sockfd);
        return;
    }

    //listen
    if(::listen(_sockfd, 128) == -1)
    {
        perror("listen");
        close(_sockfd);
        return;
    } 

    //accept
    _netfd = ::accept(_sockfd, nullptr, nullptr);
    if(_netfd == -1){
        perror("accept");
        close(_sockfd);
        return;
    }
    std::cout << "A TCP Connection Established." << std::endl; 
}

void ServerTcp::send(const char * pBuffer, int nBufferSize){
    if(::send(_netfd, pBuffer, nBufferSize, 0) == -1){
        perror("send");
        return;
    };
    std::cout << "Server Send Meaaage Successfully." << std::endl;    
}

void ServerTcp::receive(char * pBuf, size_t length){
    int ret = ::recv(_netfd, pBuf, 999, 0 );
    if(ret == -1){
        perror("recv");
        return;
    }
    std::cout << "Server Receive a Meaaage." << std::endl;    
    
}
