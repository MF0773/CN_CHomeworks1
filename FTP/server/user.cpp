#include "user.h"

AccountInfo User::getAccountInfo() const
{
    return accountInfo;
}

User::User(AccountInfo accountInfo) : accountInfo(accountInfo)
{

}
