#include "ftpclienttest.h"

FtpClientTest::FtpClientTest()
{

}

#define ASSERT(COND,MSG) if (!(COND)) throw MSG

void FtpClientTest::_run()
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
        _run();
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
    bool r = client.connectToServer(2121);
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
