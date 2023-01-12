#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVER_RECV_BUFFER_SIZE 8000

struct IpPort{
    std::string ip;
    int port;
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
    void sendResponse();
};

#endif // HTTPSERVER_H
