#include<iostream>
#include "ftpserver.h"

int main(int argc,char** argv){
    FtpServer server;
    server.start();

    server.event_loop();

    server.end();
    return 0;
}
