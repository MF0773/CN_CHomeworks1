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
#include "msgStruct.hpp"

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

void set_to_buff(std::string usernameRecvi, std::string message_str, char *buff)
{
    // strcpy(buff[0], '0');
    // strcpy(buff[1], message_str.c_str());
}

int main(int argc, char const *argv[])
{
    int fd;
    msgStruct msg;
    char buff[1024] = {0};
    std::string username, usernameRecvi, message_str, order;

    if (argc >= 3)
    {
        fd = connectServer(atoi(argv[1]));
        username = argv[2];
    }

    while (true)
    {
        /*
        std::cin >> order;
        if (order == "List")
        {

        }
        else if (order == "Exit")
        {
            close(fd);
        }
        else
        {
        */
        std::cout << sizeof(buff);
        std::cin >> usernameRecvi >> message_str;
        msg.M.mess_id = 0;
        msg.M.lentgh = 10;
        // set_to_buff(usernameRecvi, message_str, buff);
        send(fd, buff, sizeof(buff), 0);
        //}
        recv(fd, buff, 1024, 0);

        printf("Server said: %s\n", buff);

        close(fd);
    }
}