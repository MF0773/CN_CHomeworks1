#include "ftpclienttest.h"
#include <algorithm>
#define ASSERT(COND,MSG) if (!(COND)) throw MSG

bool FtpClientTest::doLogin(string user, string pass)
{
    client.checkUserName(user);
    ASSERT(client.getLastResponse() == 331,"user name stage");

    client.tryLogin(user,pass);
    ASSERT(client.getLastResponse() == 230,"login stage");
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
        testFile();
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

void FtpClientTest::testFile()
{
    getFileList();
    shouldntAccessAdminFile();
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
