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
#include <set>

using namespace std;

#define RECEIVE_BUFFER_SIZE 1024
#define USER_FILE_PATH "../server/config.json"

class FtpServer{
    private:
    int serverPort,serverFd;
    int lastFd;
    fd_set fdSet;
    unordered_map<std::string,AccountInfo> accountsMap;
    unordered_map<int,User*> onlineUsers;
    set<int> loginReqSet;

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
    void removeOnlineUser(int fd);

    void onNewPacketRecived(int fdIter,char* recvBuf);

    void end();

    bool importUsersFromFile(std::string filePath);

    AccountInfo findAccountInfo(std::string user,std::string pass);
    void addOnlineUser(int fd, AccountInfo account);
    User* findUser(int fd);
    std::string makeResponseMessage(int code,std::string text);

    //communicate commands
    void apiSend(int fd, string commandName, char *args, int argLen);
    void apiSend(int fd, std::string commandName, string str);
    std::string exportCommandName(char *buff, int recivedLen);
    void onNewApiCommandRecived(int fd, char* buffer, int len);
    void onNewUserCheckRequest(int fd,char* buffer,int len);
    void onNewLoginRequest(int fd, char* buffer, int len);
    void onLsRequest(int fd, char* buffer,int len);
    void apiSendMessage(int fd, string commandName, int code, std::string message);
};

#endif // FTPSERVER_H
