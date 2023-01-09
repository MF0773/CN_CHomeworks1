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
    std::string path;
    std::fstream file;
    char fileBuffer[FILE_PIPE_BUFFER_SIZE];


    void reciverRun();
    void senderRun();
    bool setupServer();
    bool setupClient();

    int sendNextBlock();
    int reciveNextBlock();
    void endConnection();
public:
    enum Dir{
        sender,reciver
    }dir;

    enum Role{
        server,client
    }role;

    FilePipe(Role pipeRole, Dir pipeDir, std::string filePath);

    bool setup(int port);

    void run();
    void eventloop(int fd, char* data, int len);
    int getServerFd() const;
    int getDataFd() const;
    void sleep_ms(int ms);
};

#endif // FILEPIPE_H
