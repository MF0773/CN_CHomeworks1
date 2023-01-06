#include "ftpclienttest.h"

FtpClientTest::FtpClientTest()
{

}

void FtpClientTest::_run()
{
    shouldConnect();
    should_not_login();
    shouldLogin();
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

bool FtpClientTest::should_not_login()
{
    bool b = client.tryLogin("foo","*****");
    if(!b)
        return true;
    else{
        throw "wrong login!";
        return false;
    }
}
