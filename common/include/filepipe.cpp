#include "filepipe.h"

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

int FilePipe::getServerFd() const
{
    return serverFd;
}

int FilePipe::getDataFd() const
{
    return dataFd;
}

void FilePipe::debugDelay()
{
    if(debugDelayInterval<=0){
        return ;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(debugDelayInterval));
}

int FilePipe::getDataPort() const
{
    return dataPort;
}

int FilePipe::getUserFd() const
{
    return userFd;
}

void FilePipe::setUserFd(int newUserFd)
{
    userFd = newUserFd;
}

void FilePipe::setDebugDelay(int newDebugDelay)
{
    debugDelayInterval = newDebugDelay;
}

void FilePipe::reciverRun()
{
    int recivedCount = 0;
    do{
        recivedCount = reciveNextBlock();
    }while(recivedCount > 0);
}

void FilePipe::senderRun()
{
    int sendLen = 0;
    do{
        sendLen = sendNextBlock();
    }while(sendLen > 0);
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

    serverFd = server_fd;
    listen(serverFd, 4);

    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    dataFd = accept(serverFd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    setDebugDelay(0);

    return true;
}

bool FilePipe::setupClient()
{
    struct sockaddr_in server_address;

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(dataPort);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    bool connectResult = -1;
    int attemps=0;
    do{
        connectResult = connect(fd, (struct sockaddr *)&server_address, sizeof(server_address));
        attemps++;
        if (attemps > 100) { // checking for errors
            cerr<< "Error in connecting to server"<<endl;
            return false;
        }
    }while(connectResult<0);
    this->dataFd = fd;

    setDebugDelay(1);
    return true;
}

int FilePipe::sendNextBlock()
{
    reciveAck();
    if(!file){
        fileBuffer[0] = 0;
        send(dataFd,fileBuffer,0,0);
        return 0;
    }

    file.readsome(fileBuffer,FILE_PIPE_BUFFER_SIZE-1);
    int len = file.gcount();
    send(dataFd,fileBuffer,len,0);
    fileBuffer[len] = 0;
    clog<<"a block sended"<<fileBuffer<<endl;
    debugDelay();
    return len;
}

int FilePipe::reciveNextBlock()
{
    int len = recv(dataFd, fileBuffer, FILE_PIPE_BUFFER_SIZE, 0);
    fileBuffer[len] = 0;
//    clog<<"a block recived "<<fileBuffer;
    file.write((char*) &fileBuffer, len);
    sendAck();
    debugDelay();
    return len;
}

void FilePipe::sendAck()
{
    send(dataFd,"1",2,0);
}

void FilePipe::reciveAck()
{
    char buf[2];
    recv(dataFd,buf,2,0);
}

FilePipe::Dir FilePipe::getDir() const
{
    return dir;
}

void FilePipe::endConnection()
{
    file.flush();
    file.close();
    if (this->role == Role::server){
        close(serverFd);
        close(dataFd);
    }
    else{
        close(dataFd);
    }
}

FilePipe::FilePipe(Role pipeRole, Dir pipeDir, string filePath) : role(pipeRole), dir(pipeDir),path(filePath)
{
    if (this->dir == Dir::sender){
        file.open(path,std::ios_base::in);
    }
    else{
        file.open(path,std::ios_base::out | std::ios_base::binary);
    }
}

bool FilePipe::setup(int port)
{
    this->dataPort = port;
    bool r;
    if (this->role == Role::server){
        r = setupServer();
    }
    else{
        r = setupClient();
    }
    firstBlock = true;

    if(dir == Dir::reciver){
        sendAck();
    }
    return r;
}

void FilePipe::run()
{
    if (dir == FilePipe::sender){
        senderRun();
    }
    else{
        reciverRun();
    }
    endConnection();
}

int FilePipe::eventloop()
{
    int len=-1;
    if (dir == FilePipe::sender){
        len = sendNextBlock();
    }
    else{
        len = reciveNextBlock();
    }
    return len;
}
