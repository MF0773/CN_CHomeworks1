#ifndef FILEPIPE_H
#define FILEPIPE_H
#include <string>
#include <fstream>
#include "../../common/include/ftpstatics.h"

class FilePipe
{
private:
    int dataPort,dataFd;
    int serverFd;
    int userFd;
    std::string path;
    std::fstream file;
    char fileBuffer[FILE_PIPE_BUFFER_SIZE];
    bool firstBlock;
    int debugDelayInterval;

    void reciverRun();
    void senderRun();
    bool setupServer();
    bool setupClient();

    int sendNextBlock();
    int reciveNextBlock();
    void sendAck();
    void reciveAck();
public:
    enum Dir{
        sender,reciver
    }dir;

    enum Role{
        server,client
    }role;

    FilePipe(Role pipeRole, Dir pipeDir, std::string filePath);

    bool setup(int port);
    void endConnection();
    void run();
    int eventloop();
    int getServerFd() const;
    int getDataFd() const;
    void debugDelay();
    int getDataPort() const;
    int getUserFd() const;
    void setUserFd(int newUserFd);
    void setDebugDelay(int newDebugDelay);
};

#endif // FILEPIPE_H
