#ifndef FTPCLIENTTEST_H
#define FTPCLIENTTEST_H
#include "ftpclient.h"
#include <string>
class FtpClientTest
{
private:
    FtpClient client;

    bool doLogin(std::string user,std::string pass);
    bool connect(int port);
    bool checkSameFiles(std::string path1,std::string path2);
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
    void _baseDownloadFile(string fileName);
    void testDownloadFile();
    void getFileList();
    void shouldntAccessAdminFile();
    void testNotExitingFile();
    void testDownloadText();
    void testDownloadPdf();
    void testDownloadMovie();

    //one user upload tests
    void _baseUploadFile(string fileName);
    void testUpload();
    void nonAdminCantUpload();
    void adminUploadText();
    void adminUploadImage();
};

#endif // FTPCLIENTTEST_H
