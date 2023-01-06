#ifndef FTPCLIENTTEST_H
#define FTPCLIENTTEST_H
#include "ftpclient.h"

class FtpClientTest
{
private:
    FtpClient client;

    bool connect(int port);
public:
    FtpClientTest();
    void _run();
    bool run(char** argv);

    bool shouldConnect();
    bool shouldLogin();
    bool should_not_login();
};

#endif // FTPCLIENTTEST_H