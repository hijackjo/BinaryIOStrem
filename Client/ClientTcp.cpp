#include "ClientTcp.h"

ClientTcp::ClientTcp(const char * ip, const char * port)
:_ip(ip)
,_port(port){

}
ClientTcp::~ClientTcp(){
    close(_sockfd);
}
void ClientTcp::init(){
    //socket
    _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd == -1){
        ::perror("socket");
        return;
    }

    //connect
    struct sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(_port));
	addr.sin_addr.s_addr = inet_addr(_ip);
    if(::connect(_sockfd, (struct sockaddr *) & addr, sizeof(addr)) == -1){
        perror("connect");
        close(_sockfd);
        return;
    }
    
    std::cout << "Connection Established." << std::endl;

}

//void ClientTcp::send(const std::string & str){
void ClientTcp::send(const char * pBuffer, int nBufferSize){
    if(::send(_sockfd, pBuffer, nBufferSize, 0) == -1){
        perror("send");
        return;
    };
    std::cout << "Client Send Message Successfully." << std::endl; 
}
void ClientTcp::receive(std::string & str){
    str.clear();
    char buf[1000] = {0};
    if(::recv(_sockfd, buf, 999, 0 ) == -1){
        perror("recv");
        return;
    }
    str.append(buf, ::strlen(buf));
    std::cout << "Client Receive A Meaaage." << std::endl; 
}
