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
};

#endif // USER_H
