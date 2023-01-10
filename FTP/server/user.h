#ifndef USER_H
#define USER_H
#include <string>
#include "accountinfo.h"

class User
{
private:
    AccountInfo accountInfo;
public:
    User(AccountInfo accountInfo);
    AccountInfo getAccountInfo() const;
    int dataPort;
    int getDataPort() const;
    void setDataPort(int newDataPort);
    int getSize();
    int reduceSize(int value);
    std::string getUserName();
};

#endif // USER_H
