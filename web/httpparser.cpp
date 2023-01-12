#include "httpparser.h"
#include <sstream>
#include <iostream>
#include "utils.hpp"
using namespace std;
HttpMessage::HttpMessage(std::string str)
{
    importData(str);
}

void HttpMessage::importData(string raw_str)
{
//    Request-Line = Method SP Request-URI SP HTTP-Version CRLF
    stringstream ss(raw_str);

    string firstLine;
    std::getline(ss,firstLine);
    stringstream fss;
    fss.str(firstLine);

    fss>>header.method>>header.url>>header.version;

    while(ss){
        string line;
        getline(ss,line);

        auto tokens = strSplit(line,":");
        if(tokens.size()<2){
            continue;
        }

        header.metaData.emplace(tokens[0],tokens[1]);
    }
}

HttpMessage::HttpMessage()
{

}
