#include "ftpclienttest.h"
#include <algorithm>
#include "../../common/include/utils.h"
#include "../../common/include/ftpstatics.h"
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <chrono>

#define ASSERT(COND,MSG) if ((COND)) clog<<"PASSED: "<<MSG<<endl; else throw MSG


bool FtpClientTest::run(char **argv)
{
    try{
        testAccount();
        testDownloadFile();
        testUpload();
        testMultiDownload();
        testMultiUpload();
    }
    catch (const char* msg){
        cout<<"failed : "<<msg<<endl;
        return false;
    }
    catch (...){
        cout<<"failed"<<endl;
        return false;
    }

    testClient.disconnectFromServer();
    cout<<"test completed"<<endl;
    return true;
}


bool FtpClientTest::doLogin(FtpClient &client, string user, string pass)
{
    client.checkUserName(user);
    ASSERT(client.getLastResponse() == 331,"user name stage");

    client.tryLogin(user,pass);
    ASSERT(client.getLastResponse() == 230,"login stage");
}

bool FtpClientTest::doLogin(string user, string pass)
{
    return doLogin(testClient, user, pass);
}

bool FtpClientTest::checkSameFiles(string path1, string path2)
{
    string cmd = "cmp "+path1+" "+path2;
    string output = exec(cmd.c_str());
    return output.size()==0;
}

void FtpClientTest::_baseDownloadFile(FtpClient &client, string fileName)
{
    int code = client.retFile(fileName);
    ASSERT(FtpClient::is_ok_code(code),"download code :" + fileName);

    string path1 = CLIENTS_BASE_DIR + fileName;
    string path2 = string("../server/")+SERVER_BASE_DIR + fileName;
    ASSERT(checkSameFiles(path1,path2),"same download file file :"+ fileName);
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

bool FtpClientTest::shouldConnect(FtpClient &client)
{
    bool r = client.connectToServer();
    if (!r){
        throw "cant connect!";
    }
    return true;
}

bool FtpClientTest::shouldConnect()
{
    return shouldConnect(testClient);
}


bool FtpClientTest::shouldLogin()
{
    bool r = testClient.tryLogin("Ali","1234");
    if (!r){
//        throw "cant login!";
    }
    return true;
}

bool FtpClientTest::testBadSequence()
{
    testClient.tryLogin("Ali","1234");
    ASSERT(testClient.getLastResponse() == 503,"Bad sequence not detected");
}

bool FtpClientTest::testWrongUsername()
{
    testClient.checkUserName("sdlkfja");
    ASSERT(testClient.getLastResponse() == 430,"wrong user name not worked");
}

void FtpClientTest::testWronPassword()
{
    testClient.checkUserName("Ali");
    ASSERT(testClient.getLastResponse() == 331,"user name ali");

    testClient.tryLogin("Ali","wrontpass");
    ASSERT(testClient.getLastResponse() == 430,"wrong pass not worked");
}

void FtpClientTest::testCorrectLogin()
{
    testClient.checkUserName("Ali");
    ASSERT(testClient.getLastResponse() == 331,"user name ali");

    testClient.tryLogin("Ali","1234");
    ASSERT(testClient.getLastResponse() == 230,"correct login not worked");
}

void FtpClientTest::_baseDownloadFile(std::string fileName)
{
    return _baseDownloadFile(testClient, fileName);
}

void FtpClientTest::testDownloadFile()
{
    getFileList();
    shouldntAccessAdminFile();
    testNotExitingFile();
    testDownloadText();
    testDownloadMovie();
    testDownloadPdf();

    testClient.disconnectFromServer();
}

void FtpClientTest::getFileList()
{
    shouldConnect();
    doLogin("Ali","1234");
    auto files = testClient.getListFiles();
    ASSERT(files.size()>3,"file count");
    auto findResult = std::find(files.begin(),files.end(),"text1.txt");
    ASSERT(findResult!=files.end(),"sample file in the list");

    cout<<"file list: ";
    for (const auto &a:files)
        cout<<a<<", ";
    cout<<endl;
    testClient.disconnectFromServer();
}

void FtpClientTest::shouldntAccessAdminFile()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    int r = testClient.retFile("config.json");
    ASSERT( !FtpClient::is_ok_code(r) ,"no admin access");
}

void FtpClientTest::testNotExitingFile()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    int r = testClient.retFile("abbas_bou_azar.mp4");
    ASSERT( !FtpClient::is_ok_code(r) ,"test Not Exiting File");
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

void FtpClientTest::_baseUploadFile(FtpClient& client,string fileName)
{
    int code = client.uploadFile(fileName);
    ASSERT(FtpClient::is_ok_code(code),"download code :" + fileName);

    string path1 = CLIENTS_BASE_DIR + fileName;
    string path2 = string("../server/")+SERVER_BASE_DIR + fileName;
    ASSERT(checkSameFiles(path1,path2),"same upload file file :"+ fileName);

}

void FtpClientTest::_baseUploadFile(string fileName)
{
    return _baseUploadFile(testClient, fileName);
}

void FtpClientTest::testUpload()
{
    nonAdminCantUpload();
    adminUploadText();
    adminUploadImage();
    testClient.disconnectFromServer();
}

void FtpClientTest::nonAdminCantUpload()
{
    shouldConnect();
    doLogin("Mohsen","1234");
    int code = testClient.uploadFile("text2.txt");
    ASSERT(!FtpClient::is_ok_code(code),"non admin block upload");
}

void FtpClientTest::adminUploadText()
{
    shouldConnect();
    doLogin("Ali","1234");
    _baseUploadFile("text2.txt");
}

void FtpClientTest::adminUploadImage()
{
    shouldConnect();
    doLogin("Ali","1234");
    _baseUploadFile("image2.jpg");
}

void FtpClientTest::testMultiDownload()
{
    pid_t c_pid = fork();

    if (c_pid == -1) {
        ASSERT(false, "fork process");
    }
    else if (c_pid > 0) {
        clog << "printed from parent process " << getpid() << endl;
        testDownloadUser1();
        pid_t wpid;
        int status = 0;
        while ((wpid = wait(&status)) > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        ASSERT(status == 0, "second user download");
    }
    else {
        clog << "printed from child process " << getpid() << endl;
        try{
            testDownloadUser2();
            exit(0);
        }
        catch (...){
            exit(-1);
        }
    }
}

void FtpClientTest::testDownloadUser1()
{
    FtpClient client1;
    shouldConnect(client1);
    doLogin(client1, "Ali", "1234");
    _baseDownloadFile(client1,"image1.png");
    client1.disconnectFromServer();
}

void FtpClientTest::testDownloadUser2()
{
    FtpClient client2;
    shouldConnect(client2);
    doLogin(client2, "Mohsen", "1234");
    _baseDownloadFile(client2,"movie1.mp4");
    client2.disconnectFromServer();
}

void FtpClientTest::testMultiUpload()
{
    pid_t c_pid = fork();

    if (c_pid == -1) {
        ASSERT(false, "fork process");
    }
    else if (c_pid > 0) {
        clog << "printed from parent process " << getpid() << endl;
        testUploadUser1();
        pid_t wpid;
        int status = 0;
        while ((wpid = wait(&status)) > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));;
        ASSERT(status == 0, "second user upload");
    }
    else {
        clog << "printed from child process " << getpid() << endl;
        try{
            testUploadUser2();
            exit(0);
        }
        catch (...){
            exit(-1);
        }
    }
}

void FtpClientTest::testUploadUser1()
{
    FtpClient client1;
    shouldConnect(client1);
    doLogin(client1, "Mahdi", "1234");
    _baseUploadFile(client1,"image2.jpg");
    client1.disconnectFromServer();
}

void FtpClientTest::testUploadUser2()
{
    FtpClient client2;
    shouldConnect(client2);
    doLogin(client2, "Ali", "1234");
    _baseUploadFile(client2,"image3.jpg");
    client2.disconnectFromServer();
}
