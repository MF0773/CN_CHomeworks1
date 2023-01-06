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

    int sampleConnect(int port);

    bool connectToServer(int port);

    void disconnectFromServer();
};


#endif // FTPCLIENT_H
