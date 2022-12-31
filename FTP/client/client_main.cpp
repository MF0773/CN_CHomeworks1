#include<iostream>
#include <arpa/inet.h>
#include <unistd.h>

#define LOCAL_HOST_ADDR "127.0.0.1"

using namespace std;

class FtpClient{
    private:
    int controlFd,controlPort,dataPort;

    public:
    FtpClient(){
        
    }

    bool connectToServer(int port){
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
            write(controlFd,"s",2);
            return true;
        }
        else{
            clog<< "could not connected on port : "<< controlPort;
            return false;
        }
    }

    void disconnectFromServer(){
        close(controlFd);
    }
};


int main(int argc,char **argv){
    cout<<"client started"<<endl;
    FtpClient client;
    client.connectToServer(2121);
    client.disconnectFromServer();
    return 0;
}