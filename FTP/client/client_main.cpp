#include<iostream>
#include "ftpclient.h"
using namespace std;

int main(int argc,char **argv){
    cout<<"client started"<<endl;
    FtpClient client;

    bool result = client.connectToServer(2121);
    if (!result){
        return -1;
    }

    bool isLogined = client.loginLoop();
    if(!result){
        return 0;
    }

    client.disconnectFromServer();
    char ch;
    cout<<"enter somthing to exit"<<endl;
    cin>>ch;
    return 0;
}
