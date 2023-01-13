/** collection of utiliy functions and any module of project can use it
 * */
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <fstream>
#include <ostream>
using namespace std;
std::string exec(const char* cmd); /**< @brief run linux command line @return output of the command.*/
bool isFileExist(std::string path); /**< @brief check if the given file is exist or not.*/
int getFileSize(std::string path); /**< @brief get file size in Bytes.*/

/** @brief Costumized log class.
* It has been designed to be a replacement of std::clog.
* This class can print logs in output and redirect them to a special file.
* Also it adds timestamp to the logs.
*/
class MyLogger{
    std::ofstream fileOut;
public:
    void setFilePath(std::string path);/**< @brief set log file path*/
    std::string getTimeStr(); /**< @brief returns time stamp*/

    /** @brief << oprator that will used to submit the logs
    * by this operator overriding we can use the class in form of std::clog
    * e.g. mlog<<"starting server on port "<<port<<mendl;
    */
    template<typename T>
    friend MyLogger& operator<<(MyLogger& logger, T data);
};

/** @brief Equivalent of std::endl for MyLogger class
 MyLogger class was not competitable with std::endl. So we had to write a replacement for it.
*/
class MyEndLine{
    public:
    friend ostream& operator<<(ostream& os, const MyEndLine& dt);
};
extern MyEndLine mendl;


template<typename T>
MyLogger& operator<<(MyLogger& logger, T data){

    if (std::is_same<T, MyEndLine>::value){ // if mendl(endl) has been detected; add timestamp and flush log buffer
        clog<<"- ["<<logger.getTimeStr()<<"]";
        logger.fileOut<<"- ["<<logger.getTimeStr()<<"]";
        logger.fileOut.flush();
    }

    std::clog<<data;//print log with std::clog
    logger.fileOut<<data; // append log to the log file
    return logger;
}

#endif // UTILS_H
