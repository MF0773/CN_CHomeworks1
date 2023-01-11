#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <string>
using namespace std;
vector<string> strSplit(string str, string delimiter){
    vector<string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    return result;
}

#endif // UTILS_HPP
