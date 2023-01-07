#include "ftpserver.h"
#include <fstream>
#include <string>
#include <user.h>
#include "../../common/include/utils.h"
//json library imports
#include "../../common/include/nlohmann/json.hpp"

#include "../../common/include/ftpstatics.h"

void FtpServer::addAccountInfo(const AccountInfo &account)
{
    accountsMap.emplace(account.userName,account);
}

FtpServer::FtpServer(){
    lastFd = 0;
    FD_ZERO(&fdSet);
}

bool FtpServer::start(int port){
    this->serverPort = port;
    clog<<"starting server on port "<<port<<endl;

    int options = 1;
    sockaddr_in addressIn;
    this->serverFd = socket(AF_INET, SOCK_STREAM , 0);
    setsockopt( serverFd , SOL_SOCKET, SO_REUSEADDR , &options , sizeof(int));

    if (serverFd < 0){
        clog << "could not start server"<<endl;
        return false;
    }

    addressIn.sin_addr.s_addr = INADDR_ANY;
    addressIn.sin_port = htons(serverPort);
    addressIn.sin_family = AF_INET;

    bind( serverFd, (struct sockaddr *)&addressIn, sizeof(addressIn));
    FD_SET(serverFd ,&fdSet );
    // FD_SET(serverPort ,fdSet.set );


    listen(serverFd, 4);

    bool result = importUsersFromFile(USER_FILE_PATH);
    if (!result){
        return false;
    }

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
    fd_set eventFdSet;
    FD_ZERO(&eventFdSet);
    FD_SET(serverFd, &eventFdSet);

    setLastFd(serverFd);
    while (true)//SOS
    {
        eventFdSet = fdSet;

        select(getLastFd() + 1, &eventFdSet, NULL, NULL, NULL);
        clog<<"some events";
        for (int fdIter = 0; fdIter <= lastFd; fdIter++) {
            if (FD_ISSET( fdIter , &eventFdSet)){
                // gameManager_reactionEvent(gm,fdIter,&eventFdSet);
                onEventOccur(fdIter,eventFdSet);
            }
        }
    }
}

void FtpServer::onEventOccur(int fdIter, const fd_set &eventFdSet){
    char recvBuf[RECEIVE_BUFFER_SIZE];
    memset(recvBuf, NULL , RECEIVE_BUFFER_SIZE);
    if (fdIter == serverFd){
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

    FD_SET(clientControlFd, &fdSet);
    if (clientControlFd > getLastFd())
        setLastFd(clientControlFd);

    clog<<"new client accepted "<<clientControlFd<<endl;
}

void FtpServer::disconnectClient(int clientFd){
    FD_CLR(clientFd, &fdSet );
    close(clientFd);
    clog<<"client "<<clientFd <<" disconnected"<<endl;

    removeOnlineUser(clientFd);
}

void FtpServer::removeOnlineUser(int fd)
{
    auto iter = onlineUsers.find(fd);
    if (iter == onlineUsers.end())
        return;

    onlineUsers.erase(fd);
}

void FtpServer::onNewPacketRecived(int fdIter, char *recvBuf){
    clog<<"new packet from "<<fdIter<<": "<<recvBuf<<endl;
}

void FtpServer::end(){
    clog<<"ending server at port "<<serverPort<<endl;
    close(serverFd);
}

bool FtpServer::importUsersFromFile(string filePath)
{
    using json = nlohmann::json;
    string buffer,fileStr;
    ifstream file(filePath);
    if (!file){
        cerr<<"Couldnt open json file:"<<filePath<<endl;
        return false;
    }
    while(std::getline(file,buffer)) fileStr += string() + "\n" + buffer;

    auto jsonObj = json::parse(fileStr);
    for (auto userData:jsonObj["users"]){

        AccountInfo newAccount = {
                .userName=userData["user"],
                .password=userData["password"],
                .admin=userData["admin"]=="true",
                .maxUsageSize= std::stoi ( (string) userData["size"])
            };

        addAccountInfo(newAccount);
    }

    return true;
}

void FtpServer::addOnlineUser(int fd, AccountInfo account)
{
    auto newUser = new User(account);
    onlineUsers.emplace(fd,newUser);
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
    strncpy(nameBuffer,buff, min(recivedLen,MAX_COMMAND_NAME_LEN) );

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
//        if (commandName == LOGIN_REQUEST_COMMAND){
//            return onNewLoginRequest(fd,buffer,len);
//        }
        COMMAND_CASE(onNewLoginRequest,LOGIN_REQUEST_COMMAND);
        COMMAND_CASE(onNewUserCheckRequest,USER_CHECK_REUQEST_COMMAND);
        COMMAND_CASE(onLsRequest,LS_COMMAND);

        clog<<"unknown command: "<<commandName<<endl;
    }
    catch (...){
        clog<<"exception in events!"<<endl;
    }
}

void FtpServer::onNewUserCheckRequest(int fd, char *buffer, int len)
{
    stringstream ss;
    ss.str(buffer);
    string command,user;
    ss>>command>>user;
    clog<<user<<endl;

    auto iter = accountsMap.find(user);
    if (iter == accountsMap.end()){
        clog<<"invalid username"<<endl;
        apiSendMessage(fd, USER_CHECK_RESPONSE_COMMAND, 430,"Invalid username or password");
        return;
    }

    loginReqSet.insert(fd);
    apiSendMessage(fd, USER_CHECK_RESPONSE_COMMAND, 331,"Username OK, need password.");
}

void FtpServer::onNewLoginRequest(int fd, char *buffer, int len)
{
    clog<<"new login request"<<endl;
    if( loginReqSet.count(fd) == 0 ){
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND, 503,"Bad sequence of commands.");
        return;
    }

    stringstream ss;
    ss.str(buffer);
    string command,user,pass;
    ss>>command>>user>>pass;
    clog<<user<<" "<<pass<<endl;

    auto accountIter = accountsMap.find(user);
    if(accountIter == accountsMap.end()){
        clog<<"invalid username"<<endl;
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND, 430,"Invalid username or password");
        return;
    }

    auto account = accountIter->second;
    if(account.password != pass){
        clog<<"invalid pass"<<endl;
        apiSendMessage(fd, LOGIN_RESPONSE_COMMAND,430, "Invalid username or password");
        return;
    }

    addOnlineUser(fd,account);
    apiSendMessage(fd,LOGIN_RESPONSE_COMMAND,230, "Logged in, proceed. Logged out if appropriate.");
    loginReqSet.erase(fd);
    clog<<"logged in"<<endl;
}

void FtpServer::onLsRequest(int fd, char *buffer, int len)
{
    string cmd = string()+ "ls "+ SERVER_BASE_DIR;
    string lsOutput = exec(cmd.c_str());
    apiSend(fd,LS_COMMAND,lsOutput);
}

void FtpServer::apiSendMessage(int fd,std::string commandName ,int code, string message)
{
    string args = makeResponseMessage(code,message);
    apiSend(fd,commandName,args);
}
