#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "httpparser.h"

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <exception>
#define SERVER_RECV_BUFFER_SIZE 8000
#define SERVER_BASE_DIR "disk"

/**
 * @brief A simple structure that stores ip as string and port as integer
 */
struct IpPort{
    std::string ip;
    int port;
};

/**
 *@brief subclass of std::exception that used to detect 404 exception
 */
class Error404: public std::exception{

};

/**
 *@brief Implemention of HTTP server
 */

class HttpServer
{
private:
    IpPort ipPort; /**< input ip and port of the server */
    int serverFd;
    int serverPort;
    sockaddr address; /**< address of server in unix structure */
    socklen_t addrlen; /**< lenght of server address in unix structure */
    std::string serverIp;

public:
    HttpServer();
    bool setup(IpPort ipPort); /**< start server on given ip and port. @return result of starting server. */
    void end(); /**< close server file discripltor and off the server */
    void runLoop(); /**< run event loop of the server **/
    int waitForClient(); /**< waits until a new client connects. @return file descriptor of new client. we call this method in event loop **/
    HttpMessage fetchRequest(int clientFd); /**< reads client request and extract information of the recived data. @param clientFd file descriptor of the client @return request information**/
    void handleRequest(int clientFd, HttpMessage& request);/**< after fetching http request by `fetchRequest` this method handles the request by giving suitable response*/
    std::string getContentType(std::string fileName); /**< returns content file of the given file. this will be used for response header*/
    void sendFile(int clientFd, std::string fileName); /**< sends a file to the client. Every response will be used this function to send html and media files(like mp3). This method will throw 404 Error if file not found.*/
};

#endif // HTTPSERVER_H
