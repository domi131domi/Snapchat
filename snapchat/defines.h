#define PERMS 0666
#define KEYQ "ID_KOLEJKA"
#define KEYM "ID_PAMIEC"

struct msg_buffer
{
    long mesg_type;
    char data;
};

struct memory
{
    int data;
};
