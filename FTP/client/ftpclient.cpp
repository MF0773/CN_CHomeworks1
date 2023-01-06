#include "ftpclient.h"


int FtpClient::sampleConnect(int port) {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0) { // checking for errors
        controlFd = fd;
        controlPort = port;

        clog<< "connected on port : "<< controlPort;
        string buff = "Hello,Iam new client";
        send(controlFd, buff.c_str(), buff.size(), 0);
        return true;
    }

    clog<< "could not connected on port : "<< controlPort;
    return false;
}

bool FtpClient::connectToServer(int port){
    struct sockaddr_in serverAdrr;
    sockaddr_in serverAdrress;
    controlPort = port;
    controlFd = socket(AF_INET, SOCK_STREAM, 0);
    serverAdrress.sin_family = AF_INET;
    serverAdrress.sin_port = htons(controlPort);
    serverAdrress.sin_addr.s_addr = inet_addr(LOCAL_HOST_ADDR);

    int result;
    result = connect( controlFd,(sockaddr *)&serverAdrr, sizeof(serverAdrr) ) ;
    if ( result >= 0 ) {
        clog<< "connected on port : "<< controlPort;
        string buff = "Hello,Iam new client";
        send(controlFd, buff.c_str(), buff.size(), 0);
        return true;
    }
    else{
        clog<< "could not connected on port : "<< controlPort;
        return false;
    }
}

void FtpClient::disconnectFromServer(){
    close(controlFd);
}
