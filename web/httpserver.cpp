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

    int options = 1;
    sockaddr_in addressIn;
    this->serverFd = socket(AF_INET, SOCK_STREAM , 0);
    setsockopt( serverFd , SOL_SOCKET, SO_REUSEADDR , &options , sizeof(int));

    if (serverFd < 0){
        clog << "could not start server"<<endl;
        return false;
    }

    addressIn.sin_addr.s_addr = INADDR_ANY;
    addressIn.sin_port = htons(serverPort);
    addressIn.sin_family = AF_INET;

    bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
    listen(serverFd, 4);

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
