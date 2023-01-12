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

class HttpMessage
{
public:
    std::unordered_map<std::string, std::string> headerData;
    HttpHeader header;
    HttpMessage();
    HttpMessage(std::string str);
    void importData(std::string raw_str);
};

#endif // HTTPPARSER_H
