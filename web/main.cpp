#include <iostream>
#include "httpserver.h"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

HttpServer server;

IpPort importIpPort(std::string str){

    int pos = str.find(":");
    string ipPart = str.substr(0,pos);
    int port = std::stoi( str.substr(pos+1) );

    return (IpPort){ipPart,port};
}
/**< @brief we had some problems on running program with old port. and we wrote this function to change port number in each run */
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

/**< @brief open browser for testing */
void openTestBrowser(int port)
{
    string cmd = "bash open_browser.sh &";
    system(cmd.c_str());
}

int main(int argc, char** argv)
{
    IpPort ipPort;
    string cmd;
    cout<<"enter ip and port in format: <ip>:<port>"<<endl;
    cin>>cmd;

    ipPort = importIpPort(cmd);
    bool result = server.setup(ipPort);

    if(!result){
        return -1;
    }

    server.runLoop();
    server.end();

    return 0;
}
