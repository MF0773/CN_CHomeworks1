#include "httpserver.h"
#include <fstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <memory.h>
#include "httpparser.h"

using namespace std;
HttpServer::HttpServer()
{

}

bool HttpServer::setup(IpPort ipPort)
{
    serverPort = ipPort.port;
    serverIp = ipPort.ip;

    clog<<"starting server on "<<serverIp<<":"<<serverPort<<endl;

    struct sockaddr_in address;

    // Creating socket file descriptor
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In sockets");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( serverPort );

    int val=0;
    setsockopt(serverFd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof val);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        close(serverFd);
        exit(EXIT_FAILURE);
    }
    if (listen(serverFd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
//    addFdSet(serverFd);

    return true;
}

void HttpServer::runLoop()
{
    while(true){
        int clientFd = waitForClient();
        if(clientFd < 0){
            clog<<"failed to submit client"<<endl;
            continue;
        }
        clog<<"new client:"<<clientFd<<endl;
        scanOnly(clientFd);
        return;
    }
}

int HttpServer::waitForClient()
{
    clog<<"waiting for new client"<<endl;

    sockaddr_in clientAddr;
    int clientFd;
    socklen_t addrSize = sizeof(clientAddr);

    clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &addrSize);
    return clientFd;
}

void HttpServer::scanOnly(int clientFd)
{
    char buffer[SERVER_RECV_BUFFER_SIZE] = {0};

    int len = read( clientFd , buffer, SERVER_RECV_BUFFER_SIZE);
    buffer[len] = 0;

    clog<<"scanning result:"<<endl<<buffer<<endl;
    HttpParser parser;
    parser.import(buffer);

    string indexStr = "<!DOCTYPE html>\
            <html>\
            <body>\
            \
            <h1>My First Heading</h1>\
            <p>My first paragraph.</p>\
            \
            </body>\
            </html>";
    send(clientFd, indexStr.c_str(),indexStr.size(), 0);
}

void HttpServer::end()
{
    close(serverFd);
}
