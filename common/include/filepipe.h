#ifndef FILEPIPE_H
#define FILEPIPE_H
#include <string>
#include <fstream>

class FilePipe
{
private:
    int dataPort;
    int socketFd;
    std::fstream file;
    enum Role{
        sender,reciver
    }role;

    void reciver_run();
    void sender_run();
    bool setupServer();
    bool setupClient();
public:
    FilePipe();

    int connectToPort(int port);
    int initSender(std::string path);
    int initReciver(std::string path);

    void run();
    void eventloop(int fd, char* data, int len);
};

#endif // FILEPIPE_H
