#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <iostream>

int connectServer(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}

int main(int argc, char const *argv[])
{
    int fd;
    char buff[1024] = {0};
    std::string username, usernameRecvi, msg, order;

    if (argc >= 3)
    {
        fd = connectServer(atoi(argv[1]));
        username = argv[2];
    }

    while (true)
    {
        std::cin >> order;
        if (order == "List")
        {
            /* code */
        }
        else if (order == "Exit")
        {
            close(fd);
        }
        else
        {
            std::cin >> usernameRecvi >> msg;
        }
    }
}