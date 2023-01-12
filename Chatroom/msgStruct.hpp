#include <string>

#define SIZE_BUFF 1024

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
        char *payload;
    } M;
};

void initialMSG(msgStruct &msg, MessageType _mess_type, int _mess_id, int _length, char *_payload)
{
    memset(msg.buff, 0, SIZE_BUFF);
    msg.M.mess_type = _mess_type;
    msg.M.mess_id = _mess_id;
    msg.M.length = _length;
    strcpy(msg.M.payload, _payload);
};