#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <list>

#define LOCAL_HOST_ADDR "127.0.0.1"

using namespace std;

class FtpClient{
    private:
    int controlFd,controlPort,dataPort;
    bool loginned;
    int lastResponse;
    std::string userName;
    list<std::string> catchedFileList;

    public:
        FtpClient();

    bool connectToServer();

    void disconnectFromServer();

    bool loginLoop();

// api
    void sendBytes(int fd, const char *bytes, int len);
    std::string exportCommandName(char* buff,int recivedLen);
    void apiWaitResponse(int fd, std::string command);
    void apiSend(int fd, std::string commandName, const char *args, int argLen=-1);
    bool checkUserName(std::string userNameIn);
    bool tryLogin(std::string userNameIn,std::string passwordIn);
    void onNewApiCommand(int fd, string commandName, char *args);
    void onNewLoginResponse(char* args);
    void onNewUserNameCheckResponse(char* args);
    void onLsResponse(char* args);
    void onRetrResonse(char* args);
    list<std::string> getListFiles();
    int retFile(std::string fileName);

    bool getLoginned() const;
    void setLoginned(bool newLoginned);
    void displayMessage(char* args);
    int getLastResponse() const;
    void setLastResponse(int newLastResponse);
    std::string getUserName() const;
    void setUserName(const std::string &newUserName);



// cli
    void cliLs(std::stringstream &ss);
    list<std::string> getCatchedFileList() const;
    void setCatchedFileList(const list<std::string> &newCatchedFileList);


    static bool is_ok_code(int code){return 200<=code && code <= 300;}

};


#endif // FTPCLIENT_H
