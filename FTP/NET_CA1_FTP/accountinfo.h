#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H
#include <string>

struct AccountInfo
{
    std::string userName,password;
    bool admin;
    int maxUsageSize;
};

#endif // ACCOUNTINFO_H
