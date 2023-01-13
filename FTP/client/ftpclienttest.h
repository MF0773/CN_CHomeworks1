#ifndef FTPCLIENTTEST_H
#define FTPCLIENTTEST_H
#include "ftpclient.h"
#include <string>

/**< @brief unit test for ftp client*/
class FtpClientTest
{
private:
    FtpClient testClient;

    bool doLogin(FtpClient &client, std::string user,std::string pass);
    bool doLogin(std::string user,std::string pass);
    bool connect(int port);
    bool checkSameFiles(std::string path1,std::string path2);

    void _baseDownloadFile(FtpClient& client,string fileName);
    void _baseDownloadFile(string fileName);

    void _baseUploadFile(FtpClient& client, string fileName);
    void _baseUploadFile(string fileName);
public:
    FtpClientTest();
    bool run(char** argv);

    //account tests
    void testAccount();
    bool shouldConnect(FtpClient& client);
    bool shouldConnect();
    bool shouldLogin();
    bool testBadSequence();
    bool testWrongUsername();
    void testWronPassword();
    void testCorrectLogin();

    //file tests
    void testDownloadFile();
    void getFileList();
    void shouldntAccessAdminFile();
    void testNotExitingFile();
    void testDownloadText();
    void testDownloadPdf();
    void testDownloadMovie();

    //one user upload tests
    void testUpload();
    void nonAdminCantUpload();
    void adminUploadText();
    void adminUploadImage();

    //multi user download tests
    void testMultiDownload();
    void testDownloadUser1();
    void testDownloadUser2();

    //multi upload tests
    void testMultiUpload();
    void testUploadUser1();
    void testUploadUser2();

    //size limit tests
    void testSizeLimit();
    void testCanDownloadPdf();
    void testCanDownloadText();
    void testCantDownloadPdfSecond();
};

#endif // FTPCLIENTTEST_H
