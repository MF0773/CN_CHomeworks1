#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <iostream>
#include <list>
#include <set>

#define LOCAL_HOST_ADDR "127.0.0.1"

using namespace std;
/**< @brief Implemention of FTP client*/
class FtpClient{
    private:
    int controlFd; /**< @brief file descriptor of command channel*/
    int controlPort; /**< @brief port descriptor of command channel*/
    int dataPort; /**< @brief port number of data channel*/
    bool loginned; /**< @brief shows client has been loginned or not.*/
    int lastResponse; /**< @brief stores status code of the last response from server*/
    std::string userName; /**< @brief input username*/
    list<std::string> catchedFileList; /**< @brief chahed server file names*/

    public:
        FtpClient();

    bool connectToServer(); /**< @brief connect to ftp server. Note: port will be imported from config.json file.*/

    void disconnectFromServer();

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
    void onRetrAckResonse(char* args);
    void onHelpResponse(char* args);
    void onQuitResponse(char* args);
    list<std::string> getListFiles();
    int retFile(std::string fileName);

    int uploadFile(std::string fileName);
    void onUploadResponse(char* args);
    void onUploadAckResponse(char* args);

    bool getLoginned() const;
    void setLoginned(bool newLoginned);
    void displayMessage(char* args);
    int getLastResponse() const;
    void setLastResponse(int newLastResponse);
    std::string getUserName() const;
    void setUserName(const std::string &newUserName);



// cli
    void commandLoop();/**< @brief a while loop that recive commands from user. then runs it and shows it's response.*/
    list<std::string> getCatchedFileList() const;
    void setCatchedFileList(const list<std::string> &newCatchedFileList);


    /**< @brief returns true if response code has psitive meaning. otherwise false.
     * @param code status code if response message
     * .*/
    static bool is_ok_code(int code){return 200<=code && code <= 300;}

};


#endif // FTPCLIENT_H
