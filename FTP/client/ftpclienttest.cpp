#include "ftpclienttest.h"
#include <algorithm>
#include "../../common/include/utils.h"
#include "../../common/include/ftpstatics.h"
#define ASSERT(COND,MSG) if (!(COND)) throw MSG

bool FtpClientTest::doLogin(string user, string pass)
{
    client.checkUserName(user);
    ASSERT(client.getLastResponse() == 331,"user name stage");

    client.tryLogin(user,pass);
    ASSERT(client.getLastResponse() == 230,"login stage");
}

bool FtpClientTest::checkSameFiles(string path1, string path2)
{
    string cmd = "cmp "+path1+" "+path2;
    string output = exec(cmd.c_str());
    return output.size()==0;
}

FtpClientTest::FtpClientTest()
{

}

void FtpClientTest::testAccount()
{
    shouldConnect();
    testBadSequence();
    testWrongUsername();
    testWronPassword();
    testCorrectLogin();
}

bool FtpClientTest::run(char **argv)
{
    try{
//        testAccount();
        testDownloadFile();
    }
    catch (const char* msg){
        cout<<"failed : "<<msg<<endl;
        return false;
    }
    catch (...){
        cout<<"failed"<<endl;
        return false;
    }

    client.disconnectFromServer();
    cout<<"test completed"<<endl;
    return true;
}

bool FtpClientTest::shouldConnect()
{
    bool r = client.connectToServer();
    if (!r){
        throw "cant connect!";
    }
    return true;
}

bool FtpClientTest::shouldLogin()
{
    bool r = client.tryLogin("Ali","1234");
    if (!r){
//        throw "cant login!";
    }
    return true;
}

bool FtpClientTest::testBadSequence()
{
    client.tryLogin("Ali","1234");
    ASSERT(client.getLastResponse() == 503,"Bad sequence not detected");
}

bool FtpClientTest::testWrongUsername()
{
    client.checkUserName("sdlkfja");
    ASSERT(client.getLastResponse() == 430,"wrong user name not worked");
}

void FtpClientTest::testWronPassword()
{
    client.checkUserName("Ali");
    ASSERT(client.getLastResponse() == 331,"user name ali");

    client.tryLogin("Ali","wrontpass");
    ASSERT(client.getLastResponse() == 430,"wrong pass not worked");
}

void FtpClientTest::testCorrectLogin()
{
    client.checkUserName("Ali");
    ASSERT(client.getLastResponse() == 331,"user name ali");

    client.tryLogin("Ali","1234");
    ASSERT(client.getLastResponse() == 230,"correct login not worked");
}

void FtpClientTest::_baseDownloadFile(std::string fileName)
{

    int code = client.retFile(fileName);
    ASSERT(FtpClient::is_ok_code(code),"download code :" + fileName);

    string path1 = CLIENTS_BASE_DIR + fileName;
    string path2 = string("../server/")+SERVER_BASE_DIR + fileName;
    ASSERT(checkSameFiles(path1,path2),"same download file file :"+ fileName);
}

void FtpClientTest::testDownloadFile()
{
    getFileList();
    shouldntAccessAdminFile();
    testNotExitingFile();
    testDownloadText();
    testDownloadMovie();
    testDownloadPdf();
}

void FtpClientTest::getFileList()
{
    shouldConnect();
    doLogin("Ali","1234");
    auto files = client.getListFiles();
    ASSERT(files.size()>3,"file count");
    auto findResult = std::find(files.begin(),files.end(),"text1.txt");
    ASSERT(findResult!=files.end(),"sample file in the list");

    cout<<"file list: ";
    for (const auto &a:files)
        cout<<a<<", ";
    cout<<endl;
    client.disconnectFromServer();
}

void FtpClientTest::shouldntAccessAdminFile()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    int r = client.retFile("config.json");
    ASSERT( !r ,"no admin access");
}

void FtpClientTest::testNotExitingFile()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    int r = client.retFile("abbas_bou_azar.mp4");
    ASSERT( !r ,"test Not Exiting File");
}

void FtpClientTest::testDownloadText()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    _baseDownloadFile("text1.txt");
}

void FtpClientTest::testDownloadPdf()
{
    shouldConnect();
    doLogin("Ali","1234");
    _baseDownloadFile("doc1.pdf");
}

void FtpClientTest::testDownloadMovie()
{
    shouldConnect();
    doLogin("Ali","1234");
    _baseDownloadFile("movie1.mp4");
}
