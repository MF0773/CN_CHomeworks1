#ifndef FTPSERVER_H
#define FTPSERVER_H

#include<iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <unordered_map>
#include "user.h"
#include "accountinfo.h"

using namespace std;

#define RECEIVE_BUFFER_SIZE 1024
#define USER_FILE_PATH "../server/config.json"

class FtpServer{
    private:
    int serverPort,serverFd;
    int lastFd;
    fd_set fdSet;
    unordered_map<std::string,AccountInfo> accountsMap;

private:
    void addAccountInfo(const AccountInfo& account);
    public:
    FtpServer();

    int getLastFd(){return lastFd;}
    void setLastFd(int val){lastFd=val;}

    bool start(int port);

    int sample_setup();

    int acceptClient(int server_fd);

    void sample();

    void event_loop();

    void onEventOccur(int fdIter,const fd_set &eventFdSet);

    void acceptNewClient();

    void disconnectClient(int clientFd);

    void onNewPacketRecived(int fdIter,char* recvBuf);

    void end();

    //json functions
    bool importUsersFromFile(std::string filePath);
};

#endif // FTPSERVER_H
