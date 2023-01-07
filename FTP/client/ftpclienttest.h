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
    bool testBadSequence();
    bool testWrongUsername();
    void testWronPassword();
    void testCorrectLogin();
};

#endif // FTPCLIENTTEST_H
