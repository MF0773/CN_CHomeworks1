#include <string>

#define SIZE_BUFF 1024
#define FAKE_MESS_ID 10

enum MessageType
{
    CONNECT = 1,
    CONNACK,
    LIST,
    LISTREPLY,
    INFO,
    INFOREPLY,
    SEND,
    SENDREPLY,
    RECEIVE,
    RECEIVEREPLY
};
union msgStruct
{
    char buff[SIZE_BUFF];
    struct Message
    {
        unsigned int mess_type : 4;
        unsigned int mess_id : 4;
        unsigned int length : 8;
        char payload[SIZE_BUFF - 2];
    } M;
};

void initialMSG(msgStruct &msg, MessageType _mess_type, const int &_mess_id, const int &_length, const char *_payload)
{
    memset(msg.buff, 0, SIZE_BUFF);
    msg.M.mess_type = _mess_type;
    msg.M.mess_id = _mess_id;
    msg.M.length = _length;
    strcpy(msg.M.payload, _payload);
};

void inline initial_CONNECT(msgStruct &msg, const int &_mess_id, const char *name)
{
    initialMSG(msg, CONNECT, _mess_id, 2 + strlen(name), name);
}
void inline initial_CONNACK(msgStruct &msg)
{
    initialMSG(msg, CONNACK, FAKE_MESS_ID, 2, "\0");
}
void inline initial_LIST(msgStruct &msg)
{
    initialMSG(msg, LIST, FAKE_MESS_ID, 2, "\0");
}
void inline initial_LISTREPLY(msgStruct &msg, const int &n, const char *_userID)
{
    initialMSG(msg, LISTREPLY, FAKE_MESS_ID, 2 + 2 * n, _userID);
}
void inline initial_INFO(msgStruct &msg, const char *userID)
{
    initialMSG(msg, INFO, FAKE_MESS_ID, 2 + 2, userID);
}
void inline initial_INFOREPLY(msgStruct &msg, const char *username)
{
    initialMSG(msg, INFOREPLY, FAKE_MESS_ID, 2 + strlen(username), username);
}
void inline initial_SEND(msgStruct &msg, const char *_userID, char *_message)
{
    initialMSG(msg, SEND, FAKE_MESS_ID, 2 + 2 + strlen(_message), strcat(_message, _userID));
}
void inline initial_SENDREPLY(msgStruct &msg, bool isSuccess)
{
    char fail[1] = {'1'};
    char succ[1] = {'0'};
    initialMSG(msg, SENDREPLY, FAKE_MESS_ID, 2 + 1, (isSuccess ? succ : fail));
}
void inline initial_RECEIVE(msgStruct &msg, bool isSuccess)
{
    initialMSG(msg, RECEIVE, FAKE_MESS_ID, 2, "\0");
}
void inline initial_RECEIVEREPLY(msgStruct &msg, char *_sendID, char *_message)
{
    initialMSG(msg, RECEIVEREPLY, FAKE_MESS_ID, 2 + 2 + strlen(_message), strcat(_message, _sendID));
}
