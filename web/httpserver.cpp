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
#include "../common/include/utils.h"
#include "httpparser.h"

using namespace std;
HttpServer::HttpServer()
{

}

bool HttpServer::setup(IpPort ipPort)
{
    int port  = ipPort.port;
    clog<<"starting server on port "<<port<<endl;

    int options = 1;
    sockaddr_in addressIn;
    this->serverFd = socket(AF_INET, SOCK_STREAM , 0);
    setsockopt( serverFd , SOL_SOCKET, SO_REUSEADDR , &options , sizeof(int));

    if (serverFd < 0){
        clog << "could not start server"<<endl;
        return false;
    }

    addressIn.sin_addr.s_addr = INADDR_ANY;
    addressIn.sin_port = htons(port);
    addressIn.sin_family = AF_INET;

    bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
    listen(serverFd, 4);

    serverPort = port;
    serverIp = ipPort.ip;
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
        auto request = fetchRequest(clientFd);
        handleRequest(clientFd,request);
    }
}

int HttpServer::waitForClient()
{
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(serverFd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
    printf("Client connected!\n");

    return client_fd;
}

#define SEND_BUFFER_SIZE 1024

void HttpServer::end()
{
    close(serverFd);
}

HttpMessage HttpServer::fetchRequest(int clientFd)
{
    char buffer[SERVER_RECV_BUFFER_SIZE] = {0};

    int len = read( clientFd , buffer, SERVER_RECV_BUFFER_SIZE);
    buffer[len] = 0;
    clog<<"fetch request:"<<endl<<buffer<<endl;
    HttpMessage message(buffer);
    return message;
}

void HttpServer::handleRequest(int clientFd,HttpMessage &request)
{
    string url = request.header.url;
    try{
        if(url=="/"){ /**< redirect root into index.html */
            sendFile(clientFd,"/index.html");
        }
        else{ /**< otherwise send request url file */
            sendFile(clientFd,url);
        }
    }
    catch (Error404){ /**< it's possible that sendFile throw 404 exception. that we here handle it. */
        cerr<<"Error 404"<<endl;
        sendFile(clientFd,"/404.html"); /**< send 404 page */
    }

    /**< after that close the current socket to continue fetching request */
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
}

string HttpServer::getContentType(std::string fileName)
{
    if (fileName.find(".pdf") != std::string::npos) {
        return "Partial Content";
    }

    return "text/html";
}

void HttpServer::sendFile(int clientFd, string fileName)
{
    std::string filePath = SERVER_BASE_DIR+fileName;
    if(! isFileExist(filePath)){
        throw Error404();
    }

    string contentType = getContentType(fileName);
    string header = "HTTP/1.1 200 OK\nContent-Type: "+contentType+"\n\n";
    send(clientFd,header.c_str(),header.size(),0);

    int len;
    fstream file(filePath,ios_base::in | ios_base::binary);
    if(!file){
        cerr<<"cant open file"<<endl;
        return;
    }

    char sendBuffer[SEND_BUFFER_SIZE]={0};
    do{
        file.read(sendBuffer,SEND_BUFFER_SIZE);
        len = file.gcount();
        if(len == 0){
            sendBuffer[0]=0;
        }
        send(clientFd,sendBuffer,len,0);
    }while(len>0);
}
