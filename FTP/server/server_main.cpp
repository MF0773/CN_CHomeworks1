#include<iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>

using namespace std;

#define RECEIVE_BUFFER_SIZE 1024
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
        FD_SET(serverFd ,&fdSet );
        // FD_SET(serverPort ,fdSet.set );
        

        listen(serverFd, 4);

        return true;
    }

    int sample_setup(){
        struct sockaddr_in address;
        int server_fd;
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(2121);

        bind(server_fd, (struct sockaddr *)&address, sizeof(address));
        
        

        return server_fd;
    }
    
    int acceptClient(int server_fd){
        int client_fd;
        struct sockaddr_in client_address;
        int address_len = sizeof(client_address);
        
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
        printf("Client connected!\n");

        return client_fd;
    }

    void sample(){
        int server_fd, client_fd;
        char buff[1024] = {0};

        server_fd = sample_setup();

        int client_count = 0;
        while (1) {
            client_fd = acceptClient(server_fd);
            client_count++;

            memset(buff, 0, 1024);
            recv(client_fd, buff, 1024, 0);

            printf("Client said: %s\n", buff);

            sprintf(buff, "Hello from server, you're client %d", client_count);
            send(client_fd, buff, strlen(buff), 0);

            // close(client_fd);
        }
    }

    void event_loop(){
        fd_set eventFdSet;
        FD_ZERO(&eventFdSet);
        FD_SET(serverFd, &eventFdSet);
        
        setLastFd(serverFd);
        while (true)//SOS
        {
            eventFdSet = fdSet;
            
            select(getLastFd() + 1, &eventFdSet, NULL, NULL, NULL);
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
        char recvBuf[RECEIVE_BUFFER_SIZE];
        memset(recvBuf, NULL , RECEIVE_BUFFER_SIZE);
        if (fdIter == serverFd){
            acceptNewClient();
            return;
        }

        int recvChars = recv(fdIter , recvBuf, RECEIVE_BUFFER_SIZE, 0);
        if (recvChars == 0) {//player disconnected
            disconnectClient(fdIter);
            return;
        }

        onNewPacketRecived(fdIter,recvBuf);
        memset(recvBuf, NULL , RECEIVE_BUFFER_SIZE);
        return;
}

    void acceptNewClient(){
        sockaddr_in clientAddr;
        int clientControlFd;
        socklen_t addrSize = sizeof(clientAddr);

        clientControlFd = accept(serverFd, (struct sockaddr *)&clientAddr, &addrSize);

        FD_SET(clientControlFd, &fdSet);
        if (clientControlFd > getLastFd())
            setLastFd(clientControlFd);

        clog<<"new client accepted "<<clientControlFd<<endl;
    }

    void disconnectClient(int clientFd){
        FD_CLR(clientFd, &fdSet );
        close(clientFd);
        clog<<"client "<<clientFd <<" disconnected"<<endl;
    }

    void onNewPacketRecived(int fdIter,char* recvBuf){
        clog<<"new packet from "<<fdIter<<": "<<recvBuf<<endl;
    }
     

    void end(){
        clog<<"ending server at port "<<serverPort<<endl;
        close(serverFd);
    }
};

int main(int argc,char** argv){
    FtpServer server;
    server.start(2121);
    // server.sample();

    server.event_loop();

    server.end();
    return 0;
}