#ifndef HTTPPARSER_H
#define HTTPPARSER_H
#include <string>
#include <unordered_map>

struct HttpHeader{
    std::string method;
    std::string url;
    std::string version;
    int statusCode;
    std::string statusMessage;
    std::unordered_map<std::string,std::string> metaData;
};

class HttpParser
{
public:
    std::unordered_map<std::string, std::string> headerData;
    HttpHeader header;
    HttpParser();
    HttpParser(std::string str);
    void import(std::string str);
    void importHeaders(std::string str);
};

#endif // HTTPPARSER_H
