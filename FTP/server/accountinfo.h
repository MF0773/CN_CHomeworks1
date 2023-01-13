#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H
#include <string>

/** @brief a simple structure that FtpServer used it for storing users account information*/
struct AccountInfo
{
public:
    std::string userName,password;
    bool admin;
    int maxUsageSize;
};

#endif // ACCOUNTINFO_H
