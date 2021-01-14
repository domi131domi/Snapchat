#define PERMS 0666
#define KEYQ "ID_KOLEJKA2"
#define KEYM "ID_PAMIEC2"
#define W 1280
#define H 720
#define CHANNELS 3
#define CLOSE_ALL 15
#define CLOSE_A 16
#define CLOSE_C 17
#define MOUSE_POS 19
#define AtoB 1
#define BtoA 2
#define CtoB 3
#define BtoC 4
#define LICZBA_BLOKOW 1
#define OPTION 20

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

struct msg_buffer
{
    long mesg_type;
    char data;
};

struct mouse_pos
{
    long mesg_type;
    int x;
    int y;
};

struct memory
{
    //int n;
    //cv::Mat picture;
    uint8_t picture[W*H*CHANNELS];
};

void send_signal(int msgid, int mesg_type, char letter)
{
    msg_buffer msg;
    msg.mesg_type = mesg_type;
    msg.data = letter;
    msgsnd(msgid, &msg, sizeof(msg), 0);
}

void wait_for_signal(int msgid, int mesg_type, char letter)
{
    msg_buffer msg;
    msg.data = 0;
    while(msg.data != letter)
    {
        msgrcv(msgid, &msg, sizeof(msg), mesg_type, 0);
    }
}

bool check_if_exit(int msgid, int mesg_type)
{
    msg_buffer msg;
    msg.data = 0;
    return (msgrcv(msgid, &msg, sizeof(msg), mesg_type, IPC_NOWAIT) >= 0);
}
