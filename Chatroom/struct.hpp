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

struct Message
{
    unsigned int mess_type : 4;
    unsigned int mess_id : 4;
    unsigned int lentgh : 8;
    char *payload;
};
