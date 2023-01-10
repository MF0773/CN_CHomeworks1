#include<iostream>
#include "ftpclient.h"
#include "ftpclienttest.h"
#include <sstream>
using namespace std;

bool userNameLoop(FtpClient& client)
{
    while(!std::cin.eof()){
        string userNameIn;

        cout<<"Enter Username:"<<endl;
        cin>>userNameIn;

        bool result = client.checkUserName(userNameIn);
        if (result){
            client.setUserName(userNameIn);
            return true;
        }
    }

    return false;
}

bool passwordLoop(FtpClient& client)
{
    while(!std::cin.eof()){
        string passwordIn;

        cout<<"Enter Password:"<<endl;
        cin>>passwordIn;

        string userName = client.getUserName();

        bool result = client.tryLogin(userName,passwordIn);
        if (result){
            return true;
        }
    }

    return false;
}

void commandLoop(FtpClient& client){
    string commandLine,commandName;
    stringstream ss;
    while(!std::cin.eof()){
        cin>>commandName;

        if (commandName=="ls"){
            client.cliLs(ss);
        }
        else if(commandName=="quit"){
            break;
        }
    }
}

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

    bool result = client.connectToServer();
    if (!result){
        return -1;
    }

    client.commandLoop();

    client.disconnectFromServer();
    return 0;
}
