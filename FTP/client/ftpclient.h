#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#define LOCAL_HOST_ADDR "127.0.0.1"

using namespace std;

class FtpClient{
    private:
    int controlFd,controlPort,dataPort;

    public:

    bool connectToServer(int port);

    void disconnectFromServer();

    bool loginLoop();
// api
    void sendBytes(int fd, const char *bytes, int len);
    std::string importCommandName(char* buff,int recivedLen);
    void apiWaitResponse(int fd,std::string command, char* args);
    void apiSend(int fd, std::string commandName, const char *args, int argLen=-1);
    bool sendLoginRequest(std::string userNameIn,std::string passwordIn);
};


#endif // FTPCLIENT_H
