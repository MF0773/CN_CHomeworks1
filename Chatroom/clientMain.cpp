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

int main(int argc, char const *argv[])
{
    int fd;
    msgStruct msg;
    std::string username, usernameRecvi, message_str, order;

    if (argc >= 3)
    {
        fd = connectServer(atoi(argv[1]));
        username = argv[2];

        initial_CONNECT(msg, username.c_str());
        send(fd, msg.buff, sizeof(msg), 0);

        recv(fd, msg.buff, SIZE_BUFF, 0);
        if (msg.M.mess_type != CONNACK)
        {
            printf("Error on reponse from server\n");
        }
    }
    while (true)
    {
        std::cin >> order;
        if (order == "List")
        {
            initial_LIST(msg);
            send(fd, msg.buff, sizeof(msg), 0);

            // Ack
            for (int i = 0; i < strlen(msg.M.payload); i++) // size
            {
                initial_INFO(msg, msg.M.payload);
                send(fd, msg.buff, sizeof(msg), 0);

                // recv
            }
        }
        else if (order == "Exit")
        {
            close(fd);
        }
        else
        {
            // std::cin >> usernameRecvi >> message_str;
            msg.M.mess_id = 12;
            msg.M.length = 10;
            // set_to_buff(usernameRecvi, message_str, buff);

            send(fd, msg.buff, sizeof(msg), 0);

            // recv(fd, msg.buff, SIZE_BUFF, 0);
            printf("Server said: %d\n", msg.M.mess_id);
            printf("Server said: %d\n", msg.M.length);
        }
        // std::cin >> message_str;
    }
}