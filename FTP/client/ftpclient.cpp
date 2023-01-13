#include "ftpclient.h"
#include <sstream>
#include <string.h>
#include "../../common/include/ftpstatics.h"
#include "../../common/include/nlohmann/json.hpp"
#include "../../common/include/filepipe.h"
#include <fstream>

#include <arpa/inet.h>
#include <unistd.h>

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

std::string FtpClient::getUserName() const
{
    return userName;
}

void FtpClient::setUserName(const std::string &newUserName)
{
    userName = newUserName;
}

void FtpClient::commandLoop()
{
    string commandLine;
    stringstream ss;
    while(!std::cin.eof()){
        std::getline(cin,commandLine);
        ss.clear();
        ss.flush();
        if(commandLine.size()==0)
            continue;
        apiSend(controlFd, "" , commandLine.c_str());

        string command;
        ss.str(commandLine);
        ss>>command;
        apiWaitResponse(controlFd, command);
    }
}

void FtpClient::cliLs(stringstream &ss)
{
    auto fileList = getListFiles();
    cout<<"file list: ";
    for (const auto &a:fileList)
        cout<<a<<", ";
    cout<<endl;
}

list<std::string> FtpClient::getCatchedFileList() const
{
    return catchedFileList;
}

void FtpClient::setCatchedFileList(const list<std::string> &newCatchedFileList)
{
    catchedFileList = newCatchedFileList;
}

FtpClient::FtpClient()
{
    setLoginned(false);

    /**< import control port from json file*/
    using json = nlohmann::json;
    string buffer,fileStr;
    ifstream file(CONFIG_FILE_PATH);
    if (!file){
        cerr<<"Couldnt open json file:"<<CONFIG_FILE_PATH<<endl;
        return ;
    }
    while(std::getline(file,buffer)) fileStr += string() + "\n" + buffer;
    auto jsonObj = json::parse(fileStr);
    controlPort = jsonObj["commandChannelPort"];
}

bool FtpClient::connectToServer() {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(controlPort);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) >= 0) { // checking for errors
        controlFd = fd;

        clog<< "connected on port : "<< controlPort<<endl;
        return true;
    }

    clog<< "could not connected on port : "<< controlPort<<endl;
    return false;
}

void FtpClient::disconnectFromServer(){
    close(controlFd);
}

/**
 * @brief sends array of bytes to the given file descriptor.
 */
void FtpClient::sendBytes(int fd,const char *bytes, int len)
{
    send(fd, bytes, len, 0);
}

/**
 * @brief exports command name from the recived raw string.
 *
 * @param buff recive buffer (raw data)
 * @param recivedLen number of recived bytes
 * @return name of command
 */
string FtpClient::exportCommandName(char *buff, int recivedLen)
{
    char nameBuffer[MAX_COMMAND_NAME_LEN];

    if (recivedLen<0){
        throw "command recive error!";
    }

    strncpy(nameBuffer,buff, min(recivedLen,MAX_COMMAND_NAME_LEN) );

    stringstream ss;
    ss.str(nameBuffer);
    string commandName;
    ss>>commandName;
    return commandName;
}

/**
 * @brief waits for response command.
 *
 * @param server file descriptor
 * @param command name of the target command that will wait for it.[removed in release version]
 */
void FtpClient::apiWaitResponse(int fd, string command)
{
    int recivedLen=0;
    char buff[RECIVE_BUFFER_SIZE] = {0};

    while(true){
        memset(buff, 0, RECIVE_BUFFER_SIZE);
        recivedLen = recv(fd, buff, RECIVE_BUFFER_SIZE, 0);
        buff[recivedLen] = 0;

        if( recivedLen == 0){
            cerr<<"server down when waiting for response"<<endl;
            lastResponse = 400;
            return;
        }
        string commandName = exportCommandName(buff,recivedLen);
        onNewApiCommand(fd,command,buff);
        return;
//        if (commandName == command){
//            return;
//        }
    }

}

/**
 *@brief  sends request into the server in api form.
 *
 * @param fd server file descriptor
 * @param commandName name of command
 * @param args command arguments in string form
 * @param argLen length of `args` string
 */
void FtpClient::apiSend(int fd, string commandName,const char *args, int argLen)
{
    if (argLen < 0){
        string sendBuf = commandName + " " + string(args);
        sendBytes(fd, sendBuf.c_str(),sendBuf.size());
    }
}

/**
 * @brief check given user name with server. returns true if server tells this username is exist.
 * @param userNameIn input username from cli
 * @return result of checking with server
 */
bool FtpClient::checkUserName(string userNameIn)
{
    apiSend(controlFd, USER_CHECK_REUQEST_COMMAND,userNameIn.c_str());
    apiWaitResponse(controlFd,USER_CHECK_RESPONSE_COMMAND);
    return getLastResponse()==331;
}

/**
 * @brief try to login with given username and password
 */
bool FtpClient::tryLogin(string userNameIn, string passwordIn)
{
    string args = passwordIn;
    apiSend(controlFd,LOGIN_REQUEST_COMMAND,args.c_str());

    apiWaitResponse(controlFd,LOGIN_RESPONSE_COMMAND);
    return getLoginned();
}

#define COMMAND_CASE(FUN,NAME) if(commandName==NAME) return FUN(args)
/**
 * @brief general event handler of the client.
 * it will check recived command from the server, then runs related function.
 *
 * @param fd server file descriptor
 * @param commandName command name
 * @param args argument of command in string form
 */
void FtpClient::onNewApiCommand(int fd, string commandName, char *args)
{
    /*< #define COMMAND_CASE(FUN,NAME) if(commandName==NAME) return FUN(args) */
    COMMAND_CASE(onNewLoginResponse,LOGIN_RESPONSE_COMMAND);
    COMMAND_CASE(onNewUserNameCheckResponse,USER_CHECK_RESPONSE_COMMAND);
    COMMAND_CASE(onLsResponse,LS_COMMAND);
    COMMAND_CASE(onRetrResonse,RETR_COMMAND);
    COMMAND_CASE(onRetrAckResonse,RETR_ACK_COMMAND);
    COMMAND_CASE(onUploadResponse,UPLOAD_COMMAND);
    COMMAND_CASE(onUploadAckResponse,UPLOAD_ACK_COMMAND);
    COMMAND_CASE(onHelpResponse,HELP_COMMAND);

    //display recived message to the user
    displayMessage(args);
}

#define USER_LOGGED_IN_CODE 230

/**
 * @brief event handler of login response from the server.
 */
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

/**
 * @brief event handler of checking user name response.
 */
void FtpClient::onNewUserNameCheckResponse(char *args)
{
    displayMessage(args);
}

void FtpClient::onLsResponse(char *args)
{
    stringstream ss;
    string cmd;
    list<string> tockens;
    ss.str(args);
    ss>>cmd;
    while(!ss.eof()){
        string newTocken;
        ss>>newTocken;
        tockens.push_back(newTocken);
    }
    setCatchedFileList(tockens);
}

/**
 * @brief event handler of retr command response.
 * Note: this function is blocking.
 * @param args response content. if it has error code do nothing. and if status code is ok, so start downloading from the server.
 */
void FtpClient::onRetrResonse(char *args)
{
    displayMessage(args);
    int code = getLastResponse();
    if(! FtpClient::is_ok_code(code)){
        return;
    }

    clog<<"initializing download pipe."<<endl;

    /**< import port number channel and the file name from response contents*/
    stringstream ss(args);
    string cmd,status,port,fileName;
    ss>>cmd>>status>>port>>fileName;

    /**< initialize file pipe for downloading*/
    string filePath = CLIENTS_BASE_DIR + fileName;
    FilePipe pipe(FilePipe::client, FilePipe::reciver,filePath);
    pipe.setup(std::stoi(port));
    int fd = pipe.getServerFd();

    /**< do downloading*/
    clog<<"downloading the file..."<<endl;
    pipe.run();
    clog<<"download finished."<<endl;

    if (!is_ok_code(getLastResponse())){
        return ;
    }
    /**< wait for Ack message from the server*/
    apiWaitResponse(controlFd, RETR_ACK_COMMAND);
}

/** @brief event handler when retr ack recived*/
void FtpClient::onRetrAckResonse(char *args)
{
    displayMessage(args);
}

/** @brief display help content when response recived from the server*/
void FtpClient::onHelpResponse(char *args)
{
    stringstream ss;
    string cmd,message;
    int code;

    ss.str(args);
    ss>>cmd>>code;
    cout<<ss.str()<<endl;
    setLastResponse(code);
}

/** @brief event handler of quit command */
void FtpClient::onQuitResponse(char *args)
{
    displayMessage(args);
    if(FtpClient::is_ok_code(getLastResponse())){
        setLoginned(false);
    }
}

list<string> FtpClient::getListFiles()
{
    apiSend(controlFd,LS_COMMAND,"");
    apiWaitResponse(controlFd,LS_COMMAND);
    return getCatchedFileList();
}

/** @brief download given file form the server
    @return status code of response
*/
int FtpClient::retFile(string fileName)
{
    apiSend(controlFd,RETR_COMMAND,fileName.c_str());
    apiWaitResponse(controlFd, RETR_COMMAND);
    return getLastResponse();
}

/** @brief upload given file to the server
    @return status code of response
*/
int FtpClient::uploadFile(string fileName)
{
    apiSend(controlFd,UPLOAD_COMMAND,fileName.c_str());
    apiWaitResponse(controlFd, UPLOAD_COMMAND);
    return getLastResponse();
}

/** @brief event handler of upload command.
 *  if server dosent' accept do nothing. And if every thing was ok, upload it with `FilePipe` class.
    @param args response content from the server that determine upload has been accepted or not. also it contains file name.
*/
void FtpClient::onUploadResponse(char *args)
{
    displayMessage(args);
    int code = getLastResponse();
    if(! FtpClient::is_ok_code(code)){
        return;
    }

    clog<<"initializing upload pipe."<<endl;

    stringstream ss(args);
    string cmd,status,port,fileName;
    ss>>cmd>>status>>port>>fileName;

    string filePath = CLIENTS_BASE_DIR + fileName;
    FilePipe pipe(FilePipe::client, FilePipe::sender,filePath);
    pipe.setup(std::stoi(port));
    int fd = pipe.getServerFd();
    clog<<"upload started..."<<endl;
    pipe.run();
    clog<<"upload finished."<<endl;

    if (!is_ok_code(getLastResponse())){
        return ;
    }
    apiWaitResponse(controlFd, UPLOAD_ACK_COMMAND);
}

/** @brief event handler of upload ack from the ftp server */
void FtpClient::onUploadAckResponse(char *args)
{
    displayMessage(args);
}
