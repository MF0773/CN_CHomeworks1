#include "ftpserver.h"
#include <fstream>
#include <string>
#include <user.h>
#include "../../common/include/utils.h"
//json library imports
#include "../../common/include/nlohmann/json.hpp"

#include "../../common/include/ftpstatics.h"
#include "../../common/include/filepipe.h"

void FtpServer::addAccountInfo(const AccountInfo &account)
{
    accountsMap.emplace(account.userName,account);
}

void FtpServer::addFdSet(int fd)
{
    FD_SET(fd,&fdSet);
    if (getLastFd() < fd){
        setLastFd(fd);
    }
}

void FtpServer::removeFdSet(int fd)
{
    FD_CLR(fd, &fdSet);
    if(getLastFd() == fd){
        setLastFd(fd-1);
    }
}

void FtpServer::initLogFile()
{

}

FtpServer::FtpServer()
{
    lastFd = 0;
    mlog.setFilePath(LOG_PATH);
    FD_ZERO(&fdSet);
}

int FtpServer::generateNewDataPort()
{
    return lastDataPort++;
}

bool FtpServer::start(int port){
    bool result = importConfigFromFile();
    if(!result){
        return false;
    }

    mlog<<"starting server on port "<<port<<mendl;

    int options = 1;
    sockaddr_in addressIn;
    this->serverFd = socket(AF_INET, SOCK_STREAM , 0);
    setsockopt( serverFd , SOL_SOCKET, SO_REUSEADDR , &options , sizeof(int));

    if (serverFd < 0){
        mlog << "could not start server"<<mendl;
        return false;
    }

    addressIn.sin_addr.s_addr = INADDR_ANY;
    addressIn.sin_port = htons(controlPort);
    addressIn.sin_family = AF_INET;

    bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
    listen(serverFd, 4);

    addFdSet(serverFd);

    return true;
}

int FtpServer::sample_setup(){
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(2121);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    return server_fd;
}

int FtpServer::acceptClient(int server_fd){
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
    printf("Client connected!\n");

    return client_fd;
}

void FtpServer::sample(){
    int server_fd, client_fd;
    char buff[1024] = {0};

    server_fd = sample_setup();

    int client_count = 0;
    while (1) {
        client_fd = acceptClient(server_fd);
        client_count++;

        memset(buff, 0, 1024);
        recv(client_fd, buff, 1024, 0);

        printf("Client said: %s\n", buff);

        sprintf(buff, "Hello from server, you're client %d", client_count);
        send(client_fd, buff, strlen(buff), 0);

        // close(client_fd);
    }
}

void FtpServer::event_loop(){
    setLastFd(serverFd);
    fd_set fdSetClone;
    while (true)
    {
        fdSetClone = fdSet;
        select(getLastFd() + 1, &fdSetClone, NULL, NULL, NULL);
//        mlog<<"some events"<<mendl;
        for (int fdIter = 0; fdIter <= lastFd; fdIter++) {
            if (FD_ISSET( fdIter , &fdSetClone)){
                onEventOccur(fdIter,fdSetClone);
            }
        }
    }
}

void FtpServer::onEventOccur(int fdIter, const fd_set &eventFdSet){
    auto pipeIter = filepipes.find(fdIter);
    if(pipeIter != filepipes.end()){
//        mlog<<"pipe event"<<mendl;
        onNewFilePipeEvent(fdIter);
        return;
    }

    char recvBuf[RECEIVE_BUFFER_SIZE];
    memset(recvBuf, NULL , RECEIVE_BUFFER_SIZE);
    if (fdIter == serverFd){ //new client
        acceptNewClient();
        return;
    }

    int recvChars = recv(fdIter , recvBuf, RECEIVE_BUFFER_SIZE, 0);
    if (recvChars == 0) {//client disconnected
        disconnectClient(fdIter);
        return;
    }

    onNewApiCommandRecived(fdIter,recvBuf,recvChars);
    memset(recvBuf, NULL , RECEIVE_BUFFER_SIZE);
    return;
}

void FtpServer::acceptNewClient(){
    sockaddr_in clientAddr;
    int clientControlFd;
    socklen_t addrSize = sizeof(clientAddr);

    clientControlFd = accept(serverFd, (struct sockaddr *)&clientAddr, &addrSize);

    addFdSet(clientControlFd);

    mlog<<"new client accepted "<<clientControlFd<<mendl;
}

void FtpServer::disconnectClient(int clientFd){
    removeFdSet(clientFd);
    close(clientFd);
    mlog<<"client "<<clientFd <<" disconnected"<<mendl;

    removeOnlineUser(clientFd);
}

void FtpServer::removeOnlineUser(int fd)
{
    auto iter = onlineUsers.find(fd);
    if (iter == onlineUsers.end())
        return;

    string userName = iter->second->getUserName();
    auto &info = accountsMap[userName];
    info.maxUsageSize = iter->second->getSize();
    accountsMap.emplace(userName,info);
    onlineUsers.erase(fd);
}

void FtpServer::onNewPacketRecived(int fdIter, char *recvBuf){
    mlog<<"new packet from "<<fdIter<<": "<<recvBuf<<mendl;
}

void FtpServer::end(){
    mlog<<"ending server at port "<<controlPort<<mendl;
    close(serverFd);
}

bool FtpServer::importConfigFromFile()
{
    using json = nlohmann::json;
    string buffer,fileStr;
    ifstream file(CONFIG_FILE_PATH);
    if (!file){
        cerr<<"Couldnt open json file:"<<CONFIG_FILE_PATH<<mendl;
        return false;
    }
    while(std::getline(file,buffer)) fileStr += string() + "\n" + buffer;
    auto jsonObj = json::parse(fileStr);


    controlPort = jsonObj["commandChannelPort"];
    lastDataPort = jsonObj["dataChannelPort"];
    adminFiles = jsonObj["files"];

    for (auto userData:jsonObj["users"]){
        AccountInfo newAccount = {
                .userName=userData["user"],
                .password=userData["password"],
                .admin=userData["admin"]=="true",
                .maxUsageSize= std::stoi ( (string) userData["size"]) * 1000//to bytes
            };
        addAccountInfo(newAccount);
    }

    file.close();
    mlog<<"config imported"<<mendl;
    return true;
}

bool FtpServer::checkSyntax(stringstream& ss)
{
    if(!ss){
        return false;
    }
    string temp;
    ss>>temp;
    return !ss;
}

void FtpServer::addOnlineUser(int fd, AccountInfo account)
{
    auto newUser = new User(account);
    onlineUsers.emplace(fd,newUser);
}

void FtpServer::addFilePipe(FilePipe* pipe)
{
    filepipes.emplace(pipe->getDataFd(), pipe);
    addFdSet(pipe->getDataFd());
}

void FtpServer::removeFilePipe(FilePipe *pipe)
{
    filepipes.erase(pipe->getDataFd());
    FD_CLR(pipe->getDataFd() ,&fdSet);
}

User *FtpServer::findUser(int fd)
{
    auto iter = onlineUsers.find(fd);
    if(iter == onlineUsers.end()){
        return nullptr;
    }

    return iter->second;
}

string FtpServer::makeResponseMessage(int code, std::string text)
{
    return string()+ std::to_string(code) +" "+text;
}

bool FtpServer::isAdminFile(string fileName)
{
    auto iter = adminFiles.find(fileName);
    return iter != adminFiles.end();
}

void FtpServer::apiSend(int fd, string commandName, char *args, int argLen)
{

}

void FtpServer::apiSend(int fd, string commandName, string args)
{
    stringstream ss;
    ss<<commandName<<" "<<args;
    string output = ss.str();

    send(fd,output.c_str(),output.size(),0);
}

std::string FtpServer::exportCommandName(char *buff, int recivedLen)
{
    char nameBuffer[MAX_COMMAND_NAME_LEN];
    int exportLen = min(recivedLen,MAX_COMMAND_NAME_LEN) ;
    strncpy(nameBuffer,buff, exportLen);
    nameBuffer[exportLen] = 0;

    stringstream ss;
    ss.str(nameBuffer);
    string commandName;
    ss>>commandName;
    return commandName;
}

#define COMMAND_CASE(FUN,NAME) if(commandName==NAME) return FUN(fd,buffer,len)

void FtpServer::onNewApiCommandRecived(int fd, char *buffer, int len)
{
    string commandName = exportCommandName(buffer,len);

    try{
        COMMAND_CASE(onNewLoginRequest,LOGIN_REQUEST_COMMAND);
        COMMAND_CASE(onNewUserCheckRequest,USER_CHECK_REUQEST_COMMAND);
        COMMAND_CASE(onLsRequest,LS_COMMAND);
        COMMAND_CASE(onRetrRequest,RETR_COMMAND);
        COMMAND_CASE(onUploadRequest,UPLOAD_COMMAND);
        COMMAND_CASE(onHelpRequest,HELP_COMMAND);
        COMMAND_CASE(onQuitRequest,QUIT_COMMAND);

        mlog<<"unknown command: "<<commandName<<mendl;
        sendSyntaxMessage(fd,commandName);
    }
    catch (...){
        mlog<<"exception in events!"<<mendl;
    }
}

void FtpServer::onNewFilePipeEvent(int pipeIter)
{
    auto pipe = filepipes[pipeIter];
    int len = pipe->eventloop();
    if(len == 0){
        if(pipe->getDir() == FilePipe::sender)
            sendRetrAck(pipe->getUserFd());
        else
            sendUploadAck(pipe->getUserFd());

        pipe->endConnection();
        removeFilePipe(pipe);
    }
}

void FtpServer::onNewUserCheckRequest(int fd, char *buffer, int len)
{
    stringstream ss;
    ss.str(buffer);
    string command,user;
    ss>>command>>user;
    mlog<<user<<mendl;
    if(!checkSyntax(ss)){
        sendSyntaxMessage(fd,command);
        return;
    }

    auto user_iter = onlineUsers.find(fd);
    if (user_iter != onlineUsers.end() ){
        mlog<<"resetting username of "<<fd<<mendl;
        removeOnlineUser(fd);
    }

    auto iter = accountsMap.find(user);
    if (iter == accountsMap.end()){
        mlog<<"invalid username"<<mendl;
        apiSendMessage(fd, USER_CHECK_RESPONSE_COMMAND, 430,"Invalid username or password");
        return;
    }

    loginReqSet.emplace(fd,user);
    apiSendMessage(fd, USER_CHECK_RESPONSE_COMMAND, 331,"Username OK, need password.");
}

void FtpServer::onNewLoginRequest(int fd, char *buffer, int len)
{
    mlog<<"new login request"<<mendl;
    if( loginReqSet.count(fd) == 0 ){
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND, 503,"Bad sequence of commands.");
        return;
    }

    stringstream ss;
    ss.str(buffer);
    string command,user,pass;
    ss>>command>>pass;
    user = loginReqSet[fd];
    mlog<<user<<" "<<pass<<mendl;
    if(!checkSyntax(ss)){
        sendSyntaxMessage(fd,command);
        return;
    }

    auto accountIter = accountsMap.find(user);
    if(accountIter == accountsMap.end()){
        mlog<<"invalid username"<<mendl;
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND, 430,"Invalid username or password");
        return;
    }

    auto account = accountIter->second;
    if(account.password != pass){
        mlog<<"invalid pass"<<mendl;
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND,430, "Invalid username or password");
        return;
    }

    addOnlineUser(fd,account);
    apiSendMessage(fd,LOGIN_RESPONSE_COMMAND,230, "Logged in, proceed. Logged out if appropriate.");
    loginReqSet.erase(fd);
    mlog<<"logged in"<<mendl;
}

void FtpServer::onLsRequest(int fd, char *buffer, int len)
{
    stringstream ss(buffer);
    string command;
    ss>>command;
    if(!checkSyntax(ss)){
        sendSyntaxMessage(fd,command);
        return;
    }

    string cmd = string()+ "ls "+ SERVER_BASE_DIR;
    string lsOutput = exec(cmd.c_str());
    apiSend(fd,LS_COMMAND,lsOutput);
}

void FtpServer::onRetrRequest(int fd, char *buffer, int len)
{
    mlog<<"new donwnload request from "<<fd<<mendl;
    auto user = findUser(fd);
    if (user == nullptr){
        apiSendMessage(fd, RETR_COMMAND, 332, NEED_ACCOUNT_ERROR);
        return;
    }
    auto userInfo = user->getAccountInfo();

    stringstream ss(buffer);
    string fileName,commandName;
    ss>>commandName>>fileName;
    if(!checkSyntax(ss)){
        sendSyntaxMessage(fd,commandName);
        return;
    }

    if (isAdminFile(fileName) && userInfo.admin == false ){
        mlog<<"rejected, not admin"<<mendl;
        apiSendMessage(fd, RETR_COMMAND, 550, FILE_UNAVAILABLE_ERROR);
        return;
    }

    string filePath = SERVER_BASE_DIR + fileName;
    if(!isFileExist(filePath)){
        mlog<<"rejected, not exist file"<<mendl;
        apiSendMessage(fd, RETR_COMMAND, 550, FILE_UNAVAILABLE_ERROR);
        return;
    }
    int fileSize = getFileSize(filePath);
    if(fileSize > user->getSize() ){
        mlog<<"rejected, not enough size"<<mendl;
        apiSendMessage(fd, RETR_COMMAND, 425, SIZE_ERROR);
        return;
    }

    int dataPort = generateNewDataPort();
    string args = std::to_string(dataPort)+" "+ fileName + " - server started sending";
    apiSendMessage(fd, RETR_COMMAND, 226, args);
    mlog<<"sending "<<fileName<<" to "<<user->getAccountInfo().userName<<mendl;

    auto pipe = new FilePipe(FilePipe::server,FilePipe::sender,filePath);
    pipe->setup(dataPort);

    int dataFd = pipe->getDataFd();
    if( dataFd < 0 ){
        cerr<<"cant open file pipe"<<mendl;
        apiSendMessage(fd, RETR_COMMAND, 500, "Internal server error");
        return;
    }

    user->reduceSize(fileSize);
    mlog<<"remaining size: "<<user->getSize();
    pipe->setUserFd(fd);
    addFilePipe(pipe);
}

void FtpServer::sendRetrAck(int fd)
{
    apiSendMessage(fd,RETR_ACK_COMMAND, 226, "Successful Download");
}

void FtpServer::onUploadRequest(int fd, char *buffer, int len)
{
    mlog<<"new upload request from "<<fd<<mendl;
    auto user = findUser(fd);
    if (user == nullptr){
        mlog<<"rejected, not loginned"<<mendl;
        apiSendMessage(fd, UPLOAD_COMMAND, 332, NEED_ACCOUNT_ERROR);
        return;
    }
    auto userInfo = user->getAccountInfo();
    if (userInfo.admin == false){
        mlog<<"rejected, not admin"<<mendl;
        apiSendMessage(fd, UPLOAD_COMMAND, 550, FILE_UNAVAILABLE_ERROR);
        return;
    }

    stringstream ss(buffer);
    string fileName,commandName;
    ss>>commandName>>fileName;
    string filePath = SERVER_BASE_DIR + fileName;
    if(!checkSyntax(ss)){
        sendSyntaxMessage(fd,commandName);
        return;
    }

    int dataPort = generateNewDataPort();

    auto pipe = new FilePipe(FilePipe::server,FilePipe::reciver,filePath);
    string args = std::to_string(dataPort)+" "+ fileName + " - server started reciving";
    apiSendMessage(fd, UPLOAD_COMMAND, 226, args);
    mlog<<"reciving "<<fileName<<" to "<<user->getAccountInfo().userName<<mendl;

    pipe->setup(dataPort);
    int dataFd = pipe->getDataFd();
    if( dataFd < 0 ){
        cerr<<"cant open file pipe"<<mendl;
        apiSendMessage(fd, RETR_COMMAND, 500, "Internal server error");
        return;
    }
    pipe->setUserFd(fd);
    addFilePipe(pipe);
}

void FtpServer::sendUploadAck(int fd)
{
    apiSendMessage(fd,UPLOAD_ACK_COMMAND, 226, "Successful Upload");
}

void FtpServer::apiSendMessage(int fd,std::string commandName ,int code, string message)
{
    string args = makeResponseMessage(code,message);
    apiSend(fd,commandName,args);
}

void FtpServer::sendSyntaxMessage(int fd, std::string commandName)
{
    apiSendMessage(fd, commandName, 501, "Syntax error in parameters or arguments");
}

void FtpServer::onHelpRequest(int fd, char *buffer, int len)
{
    string fileBuffer,fileStr;
    ifstream file(HELP_FILE_PATH);
    if (!file){
        cerr<<"Couldnt open help file:"<<HELP_FILE_PATH<<mendl;
        apiSendMessage(fd,"help" ,500 , "Internal Error");
        return;
    }
    while(std::getline(file,fileBuffer)) fileStr += string() + "\n" + fileBuffer;
    apiSendMessage(fd, "help", 214, fileStr);
}

void FtpServer::onQuitRequest(int fd, char *buffer, int len)
{
    stringstream ss(buffer);
    string fileName,commandName;
    ss>>commandName;
    if( !checkSyntax(ss) ){
        sendSyntaxMessage(fd,QUIT_COMMAND);
        return;
    }
    auto userIter = findUser(fd);
    if(userIter == nullptr){
        apiSendMessage(fd,QUIT_COMMAND, 332,NEED_ACCOUNT_ERROR);
        return;
    }

    removeOnlineUser(fd);
    apiSendMessage(fd,QUIT_COMMAND, 221, "successful quit");
}
