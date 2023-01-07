#ifndef FTPCLIENTTEST_H
#define FTPCLIENTTEST_H
#include "ftpclient.h"

class FtpClientTest
{
private:
    FtpClient client;

    bool doLogin(std::string user,std::string pass);
    bool connect(int port);
public:
    FtpClientTest();
    bool run(char** argv);

    //account tests
    void testAccount();
    bool shouldConnect();
    bool shouldLogin();
    bool testBadSequence();
    bool testWrongUsername();
    void testWronPassword();
    void testCorrectLogin();

    //file tests
    void testFile();
    void getFileList();
};

#endif // FTPCLIENTTEST_H
