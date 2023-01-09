#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H
#include <string>

struct AccountInfo
{
public:
    std::string userName,password;
    bool admin;
    int maxUsageSize;
};

#endif // ACCOUNTINFO_H
