#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
using namespace std;
std::string exec(const char* cmd);
bool isFileExist(std::string path);
int getFileSize(std::string path);
class MyEndLine{
    public:
    friend ostream& operator<<(ostream& os, const MyEndLine& dt);
};

extern MyEndLine mendl;


class MyLogger{
    std::ofstream fileOut;
public:
    void setFilePath(std::string path);
    std::string getTimeStr();

    template<typename T>
    friend MyLogger& operator<<(MyLogger& logger, T data);
};
template<typename T>
MyLogger& operator<<(MyLogger& logger, T data){

    if (std::is_same<T, MyEndLine>::value){
        clog<<"- ["<<logger.getTimeStr()<<"]";
        logger.fileOut<<"- ["<<logger.getTimeStr()<<"]";
        logger.fileOut.flush();
    }

    std::clog<<data;
    logger.fileOut<<data;
    return logger;
}

#endif // UTILS_H
