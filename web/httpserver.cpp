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

//    addFdSet(serverFd);

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
//        while(true)
        scanOnly(clientFd);
//        return;
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

void HttpServer::scanOnly(int clientFd)
{
    char buffer[SERVER_RECV_BUFFER_SIZE] = {0};

    int len = read( clientFd , buffer, SERVER_RECV_BUFFER_SIZE);
    buffer[len] = 0;

    clog<<"scanning result:"<<endl<<buffer<<endl;
    HttpParser parser;
    parser.import(buffer);

    string indexStr = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1>Ya Mahdi!</h1>";
    send(clientFd, indexStr.c_str(),indexStr.size(), 0);
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
}

void HttpServer::end()
{
    close(serverFd);
}

void HttpServer::sendResponse()
{
//    /*
//        char imageheader[] =
//        "HTTP/1.1 200 Ok\r\n"
//        "Content-Type: image/jpeg\r\n\r\n";
//        */
//        struct stat stat_buf;  /* hold information about input file */

//        write(fd, head, strlen(head));

//        int fdimg = open(image_path, O_RDONLY);

//        if(fdimg < 0){
//            printf("Cannot Open file path : %s with error %d\n", image_path, fdimg);
//        }

//        fstat(fdimg, &stat_buf);
//        int img_total_size = stat_buf.st_size;
//        int block_size = stat_buf.st_blksize;
//        //printf("image block size: %d\n", stat_buf.st_blksize);
//        //printf("image total byte st_size: %d\n", stat_buf.st_size);
//        if(fdimg >= 0){
//            ssize_t sent_size;

//            while(img_total_size > 0){
//                //if(img_total_size < block_size){
//                 //   sent_size = sendfile(fd, fdimg, NULL, img_total_size);
//                //}
//                //else{
//                //    sent_size = sendfile(fd, fdimg, NULL, block_size);
//                //}
//                //img_total_size = img_total_size - sent_size;

//                //if(sent_size < 0){
//                 //   printf("send file error --> file: %d, send size: %d , error: %s\n", fdimg, sent_size, strerror(errno));
//                 //   img_total_size = -1;
//                  int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size);
//                  int done_bytes = sendfile(fd, fdimg, NULL, block_size);
//                  img_total_size = img_total_size - done_bytes;
//                //}
//            }
//            if(sent_size >= 0){
//                printf("send file: %s \n" , image_path);
//            }
//            close(fdimg);
//        }
}
