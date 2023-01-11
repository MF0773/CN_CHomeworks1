#include "httpparser.h"
#include <sstream>
#include <iostream>
#include "utils.hpp"
using namespace std;
HttpParser::HttpParser(std::string str)
{
    import(str);
}

void HttpParser::import(std::string str)
{
    importHeaders(str);
}

void HttpParser::importHeaders(string str)
{
//    Request-Line = Method SP Request-URI SP HTTP-Version CRLF
    stringstream ss(str);

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

HttpParser::HttpParser()
{

}
