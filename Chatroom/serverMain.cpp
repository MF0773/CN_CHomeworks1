#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "msgStruct.hpp"
#include <map>
#include <sstream>

std::map<int, std::string> users;

int start_server(int port)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    listen(server_fd, 4);

    return server_fd;
}

int acceptClient(int server_fd)
{
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);
    printf("Client connected!\n");

    return client_fd;
}
std::string get_userID()
{
    std::stringstream ss;
    for (auto [ID, str] : users)
    {
        ss << ID << std::endl;
    }
    return ss.str();
}

void response(msgStruct &msg, int i)
{
    switch (msg.M.mess_type)
    {
    case CONNECT:
        users.insert({(int)msg.M.mess_id, msg.M.payload});

        initial_CONNACK(msg);
        send(i, msg.buff, strlen(msg.buff), 0);
        break;

    case LIST:
        // printf("%s\n", get_userID().c_str());
        initial_LISTREPLY(msg, users.size(), get_userID().c_str());
        send(i, msg.buff, strlen(msg.buff), 0);
        break;

    case INFO:
        initial_INFOREPLY(msg, users[atoi(msg.M.payload)].c_str());
        send(i, msg.buff, strlen(msg.buff), 0);

        break;

    default:
        break;
    }
}

void event_loop(fd_set &master_set, fd_set &working_set, int &server_fd, int &new_socket, int &max_sd, msgStruct &msg)
{
    working_set = master_set;
    select(max_sd + 1, &working_set, NULL, NULL, NULL);

    for (int i = 0; i <= max_sd; i++)
    {
        if (FD_ISSET(i, &working_set))
        {
            if (i == server_fd)
            { // new clinet
                new_socket = acceptClient(server_fd);
                FD_SET(new_socket, &master_set);
                if (new_socket > max_sd)
                    max_sd = new_socket;
                printf("New client connected. fd = %d\n", new_socket);
            }
            else
            { // client sending msg
                int bytes_received;
                bytes_received = recv(i, msg.buff, SIZE_BUFF, 0);

                printf("MessType recv:%d\n", msg.M.mess_type);
                response(msg, i);

                if (bytes_received == 0)
                { // EOF
                    printf("client fd = %d closed\n", i);
                    users.erase(msg.M.mess_id);
                    close(i);
                    FD_CLR(i, &master_set);
                    continue;
                }

                // printf("client %d: %s\n", i, buffer);
                // memset(buffer, 0, 1024);
            }
        }
    }
}
int main(int argc, char const *argv[])
{
    int server_fd, client_fd, new_socket, max_sd;
    msgStruct msg;

    fd_set master_set, working_set;

    server_fd = start_server(8080);
    // write(1, "Server is running\n", 18);

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

    while (true)
    {
        event_loop(master_set, working_set, server_fd, new_socket, max_sd, msg);
    }
}