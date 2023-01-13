#ifndef USER_H
#define USER_H
#include <string>
#include "accountinfo.h"

/** @brief stores all users information. used in FtpServer class.
* we use this class after user login to manage users.
*/
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
    int getSize(); /**< @brief get remaining download size.*/
    int reduceSize(int value); /**< @brief reduce remaining download size. @param value decreament value by Bytes*/
    std::string getUserName();
};

#endif // USER_H
