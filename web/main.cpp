#include <iostream>
#include "httpserver.h"
using namespace std;

HttpServer server;

IpPort importIpPort(std::string str){

    int pos = str.find(":");
    string ipPart = str.substr(0,pos);
    int port = std::stoi( str.substr(pos+1) );

    return (IpPort){ipPart,port};
}

#include <stdio.h>
#include <stdlib.h>
void funcall(void);
void fnExit1 (void)
{
  printf ("Exit function \n");
  server.end();
}


int main(int argc, char** argv)
{

    string cmd;
    cin>>cmd;
    auto ipPort = importIpPort(cmd);
    server.setup(ipPort);
    server.end();
    bool result = server.setup(ipPort);
    if(!result){
        return -1;
    }
    atexit (fnExit1);
    server.runLoop();

    return 0;
}
