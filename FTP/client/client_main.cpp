#include<iostream>
#include "ftpclient.h"
#include "ftpclienttest.h"
#include <sstream>
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

    bool result = client.connectToServer();
    if (!result){
        return -1;
    }

    client.commandLoop();

    client.disconnectFromServer();
    return 0;
}
