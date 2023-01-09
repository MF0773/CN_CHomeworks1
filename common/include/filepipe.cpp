#include "filepipe.h"

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

void FilePipe::reciver_run()
{
    char buff[1025] = {0};
    clog<<"reciver run"<<endl;
    int recivedLen = recv(this->socketFd, buff, 1024, 0);
    clog<<"recived"<<buff<<endl;
}

void FilePipe::sender_run()
{
    clog<<"sender run"<<endl;
    string str = "AA";

    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    int clientFd = accept(socketFd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    send(clientFd,str.c_str(),str.size(),0);
}

bool FilePipe::setupServer()
{
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(dataPort);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 4);
    socketFd = server_fd;

    return true;
}

bool FilePipe::setupClient()
{
    struct sockaddr_in server_address;

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(dataPort);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
        return false;
    }

    this->socketFd = fd;
    return true;
}

FilePipe::FilePipe()
{

}

int FilePipe::connectToPort(int port)
{
    this->dataPort = port;
//    int fd;
//    struct sockaddr_in server_address;

//    int options = 1;
//    sockaddr_in addressIn;
//    int serverFd = socket(AF_INET, SOCK_STREAM , 0);
//    setsockopt( serverFd , SOL_SOCKET, SO_REUSEADDR , &options , sizeof(int));

//    if (serverFd < 0){
//        cerr << "could not pipe server"<<endl;
//        return serverFd;
//    }

//    addressIn.sin_addr.s_addr = INADDR_ANY;
//    addressIn.sin_port = htons(port);
//    addressIn.sin_family = AF_INET;

//    bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
//    socketFd = serverFd;
//    return serverFd;
}

int FilePipe::initSender(std::string path)
{
    setupServer();
    file.open(path,fstream::in);
    if(!file){
        return -1;
    }
    this->role = Role::sender;
    return socketFd;
}

int FilePipe::initReciver(std::string path)
{
    setupClient();
    file.open(path,fstream::out);
    if (!file){
        return -1;
    }
    this->role = Role::reciver;
    return socketFd;
}

void FilePipe::run()
{
    if (role == Role::reciver){
        reciver_run();
    }
    else{
        sender_run();
    }
}

void FilePipe::eventloop(int fd, char *data, int len)
{

}
