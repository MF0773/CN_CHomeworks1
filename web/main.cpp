#include <iostream>
#include "httpserver.h"
#include <fstream>
using namespace std;

HttpServer server;

IpPort importIpPort(std::string str){

    int pos = str.find(":");
    string ipPart = str.substr(0,pos);
    int port = std::stoi( str.substr(pos+1) );

    return (IpPort){ipPart,port};
}

void increaseTestPort(){
    fstream file("default_input.txt", ios_base::out | ios_base::in);
    if(!file){
        cerr<<"error in openning default file"<<endl;
        return;
    }
    string str;
    getline(file,str);

    auto ipPort = importIpPort(str);
    file.seekp(0);
    file<<ipPort.ip<<":"<<ipPort.port+1<<endl;
    file.close();
}

void openTestBrowser()
{
    system("chromium $(cat default_input.txt) &");
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

    increaseTestPort();
    auto ipPort = importIpPort(cmd);
    ipPort.port++;
//    openTestBrowser();

    bool result = server.setup(ipPort);
    if(!result){
        return -1;
    }

    atexit (fnExit1);
    server.runLoop();

    return 0;
}
