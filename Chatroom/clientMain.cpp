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

int _mess_id = FAKE_MESS_ID;

std::vector<std::string> get_list_userID(msgStruct &msg, const int &fd)
{
    std::string line;
    std::vector<std::string> strs;

    initial_LIST(msg);
    send(fd, msg.buff, sizeof(msg), 0);

    recv(fd, msg.buff, SIZE_BUFF, 0);
    if (msg.M.mess_type != LISTREPLY)
    {
        printf("Error on reponse from server. listen %d Instead LISTREPLY\n", msg.M.mess_type);
    }

    std::istringstream iss(msg.M.payload);
    while (std::getline(iss, line))
    {
        strs.push_back(line);
    }
    return strs;
}
std::string get_username_from_userID(msgStruct &msg, const int &fd, const char *userId)
{
    initial_INFO(msg, userId);
    send(fd, msg.buff, sizeof(msg), 0);

    recv(fd, msg.buff, SIZE_BUFF, 0);
    if (msg.M.mess_type != INFOREPLY)
    {
        printf("Error on reponse from server. listen %d Instead INFOREPLY\n", msg.M.mess_type);
    }
    return msg.M.payload;
}
std::vector<std::string> get_list_userName(msgStruct &msg, const int &fd)
{
    std::vector<std::string> strs;
    for (std::string userId : get_list_userID(msg, fd))
    {
        strs.push_back(get_username_from_userID(msg, fd, userId.c_str()));
    }
    return strs;
}

void recive_message(msgStruct &msg, const int &fd)
{
    std::string senderId, message;

    while (true)
    {
        initial_RECEIVE(msg);
        send(fd, msg.buff, sizeof(msg), 0);

        recv(fd, msg.buff, SIZE_BUFF, 0);
        if (msg.M.mess_type != RECEIVEREPLY)
        {
            printf("Error on reponse from server. listen %d Instead RECEIVEREPLY\n", msg.M.mess_type);
        }

        encode_payload(msg.M.payload, senderId, message);
        if (atoi(senderId.c_str()) != 0)
            printf("send %s %s\n", get_username_from_userID(msg, fd, senderId.c_str()).c_str(), message.c_str());
        else
            break;
    }
}

int main(int argc, char const *argv[])
{
    int fd;
    msgStruct msg;
    std::string username, recvier_username, recvier_userID, message_str, order;

    if (argc >= 3)
    {
        fd = connectServer(atoi(argv[1]));
        username = argv[2];
        _mess_id = rand() % (username.size());

        initial_CONNECT(msg, _mess_id, username.c_str());
        send(fd, msg.buff, sizeof(msg), 0);

        recv(fd, msg.buff, SIZE_BUFF, 0);
        if (msg.M.mess_type != CONNACK)
        {
            printf("Error on reponse from server. listen %d Instead CONNACK\n", msg.M.mess_type);
        }
    }
    while (true)
    {
        recive_message(msg, fd);

        std::cin >> order;
        if (order == "list")
        {
            for (std::string name : get_list_userName(msg, fd))
            {
                printf("-%s\n", name.c_str());
            }
        }
        else if (order == "exit")
        {
            close(fd);
            break;
        }
        else if (order == "send")
        {
            std::cin >> recvier_username >> message_str;

            // std::vector<std::string> strs;
            for (std::string line : get_list_userID(msg, fd))
            {
                initial_INFO(msg, line.c_str());
                send(fd, msg.buff, sizeof(msg), 0);

                recv(fd, msg.buff, SIZE_BUFF, 0);
                if (msg.M.mess_type != INFOREPLY)
                {
                    printf("Error on reponse from server. listen %d Instead INFOREPLY\n", msg.M.mess_type);
                }

                if (msg.M.payload == recvier_username)
                {
                    recvier_userID = line;
                    break;
                }
            }

            initial_SEND(msg, recvier_userID, message_str);
            send(fd, msg.buff, sizeof(msg), 0);

            recv(fd, msg.buff, SIZE_BUFF, 0);
            if (msg.M.mess_type != SENDREPLY)
            {
                printf("Error on reponse from server. listen %d Instead SENDREPLY\n", msg.M.mess_type);
            }
            printf("Message delivered: %d\n", msg.M.payload[0] - '0');
        }
        else
        {
            printf("Error on input order\n");
        }
        // check atoi c_str const &
    }
}