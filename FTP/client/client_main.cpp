#include<iostream>
#include "ftpclient.h"
using namespace std;

int main(int argc,char **argv){
    cout<<"client started"<<endl;
    FtpClient client;
    // client.connectToServer(2121);
    client.sampleConnect(2121);
    client.disconnectFromServer();
    return 0;
}
