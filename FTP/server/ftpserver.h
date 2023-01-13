#ifndef FTPSERVER_H
#define FTPSERVER_H

#include<iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <unordered_map>
#include "user.h"
#include "accountinfo.h"
#include <set>
#include "../../common/include/utils.h"

#include "../../common/include/filepipe.h"
using namespace std;

#define RECEIVE_BUFFER_SIZE 1024

class basic_json;
/**< @brief implemention of fpt server*/
class FtpServer{
    private:
    int controlPort; /**< @brief command channel port*/
    int serverFd; /**< @brief command channel file descriptor*/
    int lastFd; /**< @brief largest file descriptor. it will be used for `select` method*/
    fd_set fdSet; /**< @brief unix fd_set for storing all file descriptors. it will be used for `select` method*/
    unordered_map<std::string,AccountInfo> accountsMap; /**< @brief to access user account info by username*/
    unordered_map<int,User*> onlineUsers; /**< @brief to access online users by file descriptor*/

    /** @brief file pipes. for downloading and uploading. the key is file descriptor of data socket*/
    unordered_map<int,FilePipe*> filepipes;
    set<std::string> adminFiles;  /**< @brief a set of string that stores admin-access files*/
    /** @brief a map to store clients user commands. after user command, clients will be stored
    * within their input username. and we check them in login time.
    * key: client file descriptor
    * value: client input user name
    */
    unordered_map<int,std::string> loginReqSet;

    /** @brief costumized log class instance*/
    MyLogger mlog;

    /** @brief we store the last assigned data port for transfering data.
    */
    int lastDataPort;


private:
    void addAccountInfo(const AccountInfo& account);/**< @brief add account info after reading from the file*/
    void addFdSet(int fd); /**< @brief add file descriptor to the `fdSet` private member*/
    void removeFdSet(int fd); /**< @brief remove given file descriptor from the `fdSet` private member*/
    void initLogFile(); /**< @brief initialize logger `mlog`*/
    public:
    FtpServer();

    int getLastFd(){return lastFd;} /**< @brief get largest file descriptor. it will be used for `select` method*/
    void setLastFd(int val){lastFd=val;} /**< @brief set largest file descriptor. it will be used for `select` method*/
    int generateNewDataPort(); /**< @brief generate new data port by incresing last assigned dataport and return it*/

    bool start(); /**< @brief start ftp server. */

    /** @brief shutdown the ftp server*/
    void end();

    /** @brief import config.json*/
    bool importConfigFromFile();

    /** @brief check for trigger on unix sockets by select method.
    *if an event has been accoured run `onEventOccur` method.
    */
    void event_loop();


    /** @brief general event handler of the ftp server
     * we split event states in this function and call suitable function:
     * if it was about data channel (file pipes) -> `onNewFilePipeEvent`
     * if it was a new connection request -> `acceptNewClient`
     * if it was a disconnect event -> `disconnectClient`
     * if it was a command from connected client -> `onNewApiCommandRecived`
     *
     * @param fdIter triggered file descriptor. we discovered it by iteration in the `event_loop` method.
     */
    void onEventOccur(int fdIter);

    /** @brief accept new client
     * this method will be called new client detected in `onEventOccur` function.
     * @return client file descriptor
     */
    void acceptNewClient();

    /** @brief remove disconnected client
     * in the method we remove it from `fdSet` and from `onlineUsers` fields.
     * and close it file descriptor.
     */
    void disconnectClient(int clientFd);
    void removeOnlineUser(int fd);

    /** @brief event handler of clients commands.
     * @param fdIter file descriptor of the client
     * @param recvBuf reciving buffer
     */
    void onNewPacketRecived(int fdIter,char* recvBuf);

    /** @brief check recived command syntax issues.
    * @return false if it has syntax error. otherwise true.
    */
    bool checkSyntax(stringstream &ss);

    /** @brief find account with given username and password
    * @param user input username
    * @param pass input password
    * @return
    */
    AccountInfo findAccountInfo(std::string user,std::string pass);

    /** @brief after login add this client to online users
     * @param fd client file descriptor
     * @param account account information of loginned user
    */
    void addOnlineUser(int fd, AccountInfo account);


    void addFilePipe(FilePipe *pipe);/**< @brief add file pipe while starting upload/download*/
    void removeFilePipe(FilePipe *pipe);/**< @brief remove file pipe while finishing upload/download*/
    User* findUser(int fd); /**< @brief find user by file descriptor*/


    /** @brief prepare and respnose message to the clients
    * @param code status code e.g. 200, 404 ...
    * @param status message
    * @return string that has been made to send with `send` method
    */
    void apiSendMessage(int fd, string commandName, int code, std::string message);
    std::string makeResponseMessage(int code,std::string text);

    bool isAdminFile(std::string fileName);

    //communicate commands
    /** @brief event handler while new command message recived from clients*/
    void onNewApiCommandRecived(int fd, char* buffer, int len);

    void onNewUserCheckRequest(int fd,char* buffer,int len); /**< @brief response to `user` command from the client*/
    void onNewLoginRequest(int fd, char* buffer, int len);/**< @brief response to `pass` command from the client*/
    void onLsRequest(int fd, char* buffer,int len);
    void onRetrRequest(int fd, char* buffer,int len); /**< @brief response to `retr` command from the client*/
    void sendRetrAck(int fd); /**< @brief response send ack after download to the client*/
    void onUploadRequest(int fd, char* buffer,int len); /**< @brief response to `upload` command*/
    void sendUploadAck(int fd);/**< @brief send ack after upload to the client*/
    void sendSyntaxMessage(int fd, string commandName); /**< @brief send syntax error to the client*/
    void onHelpRequest(int fd, char *buffer, int len); /**< @brief response `help` command*/
    void onQuitRequest(int fd, char *buffer, int len); /**< @brief response `quit` command*/

    /** @brief send command in api form
    * @param fd client file descriptor
    * @param commandName command name identifier
    * @param args argument of command
    * @param argLen length of the argument part
    */
    void apiSend(int fd, string commandName, char *args, int argLen);
    /** @brief simiular with void apiSend(int fd, string commandName, char *args, int argLen);*/
    void apiSend(int fd, std::string commandName, string str);

    /** @brief export command name from recived client message*/
    std::string exportCommandName(char *buff, int recivedLen);

    /** @brief event handler when new block of file recived
    * @param piepIter file descriptor of data channel
    */
    void onNewFilePipeEvent(int pipeIter);

};

#endif // FTPSERVER_H
