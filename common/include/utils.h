#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
std::string exec(const char* cmd);
bool isFileExist(std::string path);
int getFileSize(std::string path);
#endif // UTILS_H
