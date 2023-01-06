#include "ftpclient.h"
#include <sstream>
#include <string.h>
#include "../../common/include/ftpstatics.h"

bool FtpClient::connectToServer(int port) {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0) { // checking for errors
        controlFd = fd;
        controlPort = port;

        clog<< "connected on port : "<< controlPort<<endl;
        return true;
    }

    clog<< "could not connected on port : "<< port<<endl;
    return false;
}

void FtpClient::disconnectFromServer(){
    close(controlFd);
}

bool FtpClient::loginLoop()
{
    while(!std::cin.eof()){
        string userNameIn,passwordIn;

        cout<<"Enter Username:"<<endl;
        cin>>userNameIn;

        cout<<"Enter Password:"<<endl;
        cin>>passwordIn;

        bool result = sendLoginRequest(userNameIn,passwordIn);
        if (result){
            cout<<"Logined"<<endl;
            return true;
        }
        else{
            cerr<<"couldn't login!"<<endl;
        }
    }

    return false;
}

void FtpClient::sendBytes(int fd,const char *bytes, int len)
{
    send(fd, bytes, len, 0);
}

string FtpClient::importCommandName(char *buff, int recivedLen)
{
    char nameBuffer[MAX_COMMAND_NAME_LEN];
    strncpy(nameBuffer,buff, min(recivedLen,MAX_COMMAND_NAME_LEN) );

    stringstream ss;
    ss.str(nameBuffer);
    string commandName;
    ss>>commandName;
    return commandName;
}

void FtpClient::apiWaitResponse(int fd, string command, char *args)
{
    int recivedLen=0;
    char buff[RECIVE_BUFFER_SIZE] = {0};

    while(true){
        memset(buff, 0, RECIVE_BUFFER_SIZE);
        recivedLen = recv(fd, buff, RECIVE_BUFFER_SIZE, 0);
        string commandName = importCommandName(buff,recivedLen);
    }

}

void FtpClient::apiSend(int fd, string commandName,const char *args, int argLen)
{
    if (argLen < 0){
        string sendBuf = commandName + " " + string(args);
        sendBytes(fd, sendBuf.c_str(),sendBuf.size());
    }

}

bool FtpClient::sendLoginRequest(string userNameIn, string passwordIn)
{
    string args = userNameIn+" "+passwordIn;
    apiSend(controlFd,LOGIN_REQUEST_COMMAND,args.c_str());

}
