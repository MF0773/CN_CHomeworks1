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
#include <sstream>
#include <vector>
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
    std::string username, usernameRecvi, message_str, order, temp;

    if (argc >= 3)
    {
        fd = connectServer(atoi(argv[1]));
        username = argv[2];
        int userIDrand = rand() % (username.size());

        initial_CONNECT(msg, userIDrand, username.c_str());
        send(fd, msg.buff, sizeof(msg), 0);

        recv(fd, msg.buff, SIZE_BUFF, 0);
        if (msg.M.mess_type != CONNACK)
        {
            printf("Error on reponse from server. listen %d Instead CONNACK\n", msg.M.mess_type);
        }
    }
    while (true)
    {
        std::cin >> order;
        if (order == "List")
        {
            initial_LIST(msg);
            send(fd, msg.buff, sizeof(msg), 0);

            recv(fd, msg.buff, SIZE_BUFF, 0);
            if (msg.M.mess_type != LISTREPLY)
            {
                printf("Error on reponse from server. listen %d Instead LISTREPLY\n", msg.M.mess_type);
            }
            std::istringstream iss(msg.M.payload);
            std::string line;
            std::vector<std::string> strs;
            // printf("pay%s\n", msg.M.payload);

            while (std::getline(iss, line))
            {
                strs.push_back(line);
            }
            for (std::string line : strs)
            {
                initial_INFO(msg, line.c_str());
                send(fd, msg.buff, sizeof(msg), 0);

                recv(fd, msg.buff, SIZE_BUFF, 0);
                if (msg.M.mess_type != INFOREPLY)
                {
                    printf("Error on reponse from server. listen %d Instead INFOREPLY\n", msg.M.mess_type);
                }
                printf("-%s\n", msg.M.payload);
            }
        }
        else if (order == "Exit")
        {
            close(fd);
        }
        else
        {
            std::cin >> usernameRecvi >> message_str;
            // initial_SEND(msg, usernameRecvi.c_str(), ""); // message_str.c_str());
            send(fd, msg.buff, sizeof(msg), 0);

            recv(fd, msg.buff, SIZE_BUFF, 0);
            if (msg.M.mess_type != SENDREPLY)
            {
                printf("Error on reponse from server. listen %d Instead SENDREPLY\n", msg.M.mess_type);
            }

            printf("Server said: %d\n", msg.M.mess_id);
            printf("Server said: %d\n", msg.M.length);
        }
        // std::cin >> message_str;
    }
}