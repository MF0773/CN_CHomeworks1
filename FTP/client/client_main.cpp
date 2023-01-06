#include<iostream>
#include "ftpclient.h"
#include "ftpclienttest.h"
using namespace std;

int main(int argc,char **argv){
    if (argc>1){
        string arg = argv[1];
        if (arg=="test"){
            FtpClientTest tester;
            tester.run(argv);
            return 0;
        }

    }
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
