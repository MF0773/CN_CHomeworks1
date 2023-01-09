#include "utils.h"
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <fstream>
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
    int pos = cmdOut.find("No such file or directory");
    bool found = pos <0;
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
