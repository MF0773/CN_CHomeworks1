#include<iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class FtpServer{
    private:
    int serverPort,serverFd;
    int lastFd;
    fd_set fdSet;


    public:
    FtpServer(){
            lastFd = 0;
            FD_ZERO(&fdSet);
        }
    
    int getLastFd(){return lastFd;}
    void setLastFd(int val){lastFd=val;}

    bool start(int port){
        this->serverPort = port;
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
        addressIn.sin_port = htons(serverPort);
        addressIn.sin_family = AF_INET;

        bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
        return true;
    }
    
    void event_loop(){
        fd_set eventFdSet;
        FD_ZERO(&eventFdSet);
        FD_SET(serverFd, &eventFdSet);

        setLastFd(serverFd);
        while (true)//SOS
        {
            eventFdSet = fdSet;
            select(lastFd + 1, &eventFdSet, NULL, NULL, NULL);
            clog<<"some events";
            for (int fdIter = 0; fdIter <= lastFd; fdIter++) {
                if (FD_ISSET( fdIter , &eventFdSet)){
                    // gameManager_reactionEvent(gm,fdIter,&eventFdSet);
                    onEventOccur(fdIter,eventFdSet);
                }
            }
        }
    }

    void onEventOccur(int fdIter,const fd_set &eventFdSet){

    }

    void end(){
        clog<<"ending server at port "<<serverPort<<endl;
        close(serverFd);
    }
};

int main(int argc,char** argv){
    FtpServer server;
    server.start(2121);

    server.event_loop();

    server.end();
    return 0;
}