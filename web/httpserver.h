#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "httpparser.h"

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <exception>
#define SERVER_RECV_BUFFER_SIZE 8000
#define SERVER_BASE_DIR "disk"
struct IpPort{
    std::string ip;
    int port;
};

class Error404: public std::exception{

};

class HttpServer
{
private:
    IpPort ipPort;
    int serverFd;
    int serverPort;
    sockaddr address;
    socklen_t addrlen;
    std::string serverIp;

public:
    HttpServer();
    bool setup(IpPort ipPort);
    void runLoop();
    int waitForClient();
    void scanOnly(int clientFd);
    void end();
    HttpMessage fetchRequest(int clientFd);
    void handleRequest(int clientFd, HttpMessage& request);
    void sendResponse();
    std::string getContentType(std::string fileName);
    void sendSampleHtml(int clientFd);
    void sendSampleImage(int clientFd);
    void sendFile(int clientFd, std::string fileName);
};

#endif // HTTPSERVER_H
