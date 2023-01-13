/**
 * This header file contains some structure to import Http messages data in useful form.
 */
#ifndef HTTPPARSER_H
#define HTTPPARSER_H
#include <string>
#include <unordered_map>

/**
 * @brief header of http message.
 */
struct HttpHeader{
    std::string method; /**< @brief  HTTP method. e.g. GET,POST,PUT*/
    std::string url; /**< @brief  request/response url */
    std::string version; /**< @brief  HTML version */
    int statusCode; /**< @brief  status code of http message e.g. 200->OK , 404->Not found */
    std::string statusMessage; /**< @brief  status message  */
    std::unordered_map<std::string,std::string> metaData; /**< @brief a map that stores all information about header in key-value form */
};

/**
 * @brief Http message class
 */
class HttpMessage
{
public:
    std::unordered_map<std::string, std::string> headerData; /**< @brief headerData in key-value form*/
    HttpHeader header; /**< @brief header information in form of `HttpHeader structure`*/
    HttpMessage();
    HttpMessage(std::string str); /**< @brief constructor. @param str raw message stirng. it will import information by this parameter*/
    void importData(std::string raw_str); /**< @brief import http message information @param raw_str raw string of http message*/
};

#endif // HTTPPARSER_H
