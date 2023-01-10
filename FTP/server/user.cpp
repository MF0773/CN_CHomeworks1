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

int User::getSize()
{
    return accountInfo.maxUsageSize;
}

int User::reduceSize(int value)
{
    accountInfo.maxUsageSize -= value;
    return getSize();
}

std::string User::getUserName()
{
    return accountInfo.userName;
}

User::User(AccountInfo accountInfo) : accountInfo(accountInfo)
{
    setDataPort(-1);
}
