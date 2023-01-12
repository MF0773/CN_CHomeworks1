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

    file.open("browser_input.txt", ios_base::out);
    file<<"localhost:"<<ipPort.port+1;
    file.close();
}

void openTestBrowser(int port)
{
    string cmd = "bash open_browser.sh &";
    system(cmd.c_str());
}

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{

    string cmd;
    cin>>cmd;

    increaseTestPort();
    auto ipPort = importIpPort(cmd);
    ipPort.port++;

    bool result = server.setup(ipPort);

    if(!result){
        return -1;
    }

    openTestBrowser(ipPort.port);
    server.runLoop();
    server.end();

    return 0;
}
