#include "ftpclient.h"
#include <sstream>
#include <string.h>
#include "../../common/include/ftpstatics.h"

bool FtpClient::getLoginned() const
{
    return loginned;
}

void FtpClient::setLoginned(bool newLoginned)
{
    loginned = newLoginned;
}

void FtpClient::displayMessage(char *args)
{
    stringstream ss;
    string cmd,message;
    int code;

    ss.str(args);
    ss>>cmd>>code;
    getline(ss,message);
    cout<<code<<": "<<message<<endl;
    setLastResponse(code);
}

int FtpClient::getLastResponse() const
{
    return lastResponse;
}

void FtpClient::setLastResponse(int newLastResponse)
{
    lastResponse = newLastResponse;
}

FtpClient::FtpClient()
{
    setLoginned(false);
}

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

        bool result = tryLogin(userNameIn,passwordIn);
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

string FtpClient::exportCommandName(char *buff, int recivedLen)
{
    char nameBuffer[MAX_COMMAND_NAME_LEN];
    strncpy(nameBuffer,buff, min(recivedLen,MAX_COMMAND_NAME_LEN) );

    stringstream ss;
    ss.str(nameBuffer);
    string commandName;
    ss>>commandName;
    return commandName;
}

void FtpClient::apiWaitResponse(int fd, string command)
{
    int recivedLen=0;
    char buff[RECIVE_BUFFER_SIZE] = {0};

    while(true){
        memset(buff, 0, RECIVE_BUFFER_SIZE);
        recivedLen = recv(fd, buff, RECIVE_BUFFER_SIZE, 0);
        string commandName = exportCommandName(buff,recivedLen);
        onNewApiCommand(fd,command,buff);
        if (commandName == command){
            return;
        }
    }

}

void FtpClient::apiSend(int fd, string commandName,const char *args, int argLen)
{
    if (argLen < 0){
        string sendBuf = commandName + " " + string(args);
        sendBytes(fd, sendBuf.c_str(),sendBuf.size());
    }

}

bool FtpClient::checkUserName(string userNameIn)
{
    apiSend(controlFd, USER_CHECK_REUQEST_COMMAND,userNameIn.c_str());
    apiWaitResponse(controlFd,USER_CHECK_RESPONSE_COMMAND);
    return getLastResponse()==331;
}

bool FtpClient::tryLogin(string userNameIn, string passwordIn)
{
    string args = userNameIn+" "+passwordIn;
    apiSend(controlFd,LOGIN_REQUEST_COMMAND,args.c_str());

    apiWaitResponse(controlFd,LOGIN_RESPONSE_COMMAND);
    return getLoginned();
}

#define COMMAND_CASE(FUN,NAME) if(commandName==NAME) return FUN(args)
void FtpClient::onNewApiCommand(int fd, string commandName, char *args)
{
    COMMAND_CASE(onNewLoginResponse,LOGIN_RESPONSE_COMMAND);
    COMMAND_CASE(onNewUserNameCheckResponse,USER_CHECK_RESPONSE_COMMAND);
}

#define USER_LOGGED_IN_CODE 230

void FtpClient::onNewLoginResponse(char *args)
{
    stringstream ss;
    string commandName;
    int code;
    string message;

    ss.str(args);
    ss>>commandName>>code;
    setLoginned( code==USER_LOGGED_IN_CODE );

    displayMessage(args);
}

void FtpClient::onNewUserNameCheckResponse(char *args)
{
    displayMessage(args);
}
