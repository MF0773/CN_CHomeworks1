#include "user.h"

AccountInfo User::getAccountInfo() const
{
    return accountInfo;
}

int User::getDataPort() const
{
    return dataPort;
}

void User::setDataPort(int newDataPort)
{
    dataPort = newDataPort;
}

User::User(AccountInfo accountInfo) : accountInfo(accountInfo)
{
    setDataPort(-1);
}
