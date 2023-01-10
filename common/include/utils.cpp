#include "utils.h"
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <fstream>
#include <ctime>
using namespace std;

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool isFileExist(std::string path)
{
    string cmdOut = exec(string("ls " + path).c_str());
    int pos = cmdOut.find(path);
    bool found = pos >= 0;
    return found;
}

int getFileSize(std::string path)
{
    ifstream mySource;
    mySource.open(path, ios_base::binary);
    mySource.seekg(0,ios_base::end);
    int size = mySource.tellg();
    mySource.close();
    return size;
}

void MyLogger::setFilePath(std::string path)
{
    fileOut.open(path, std::ios_base::app);
}

MyEndLine mendl;
string MyLogger::getTimeStr()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string str(buffer);

    return str;
}

ostream& operator<<(ostream& os, const MyEndLine& dt)
{
    os << endl;
    return os;
}
