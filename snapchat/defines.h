#define PERMS 0666
#define KEYQ "ID_KOLEJKgA2bb"
#define KEYM "ID_PAMIEC6g9bb6aa"
#define W 1280
#define H 720
#define CHANNELS 3
#define CLOSE_ALL 15
#define CLOSE_A 16
#define CLOSE_C 17
#define MOUSE_POS 19
#define AtoB 1
#define CtoA 2
#define CtoB 3
#define BtoC 4
#define OPTION 20
#define MAX_SKALA_KOLOR 255
#define BLOCK_SIZE 2
#define DURATION 0.001

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
    uint8_t picture[BLOCK_SIZE][W*H*CHANNELS];
};

void send_signal(int msgid, int mesg_type, char letter)
{
    msg_buffer msg;
    msg.mesg_type = mesg_type;
    msg.data = letter;
    msgsnd(msgid, &msg, sizeof(msg), 0);
}

char wait_for_signal(int msgid, int mesg_type)
{
    msg_buffer msg;
    msg.data = 0;
    msgrcv(msgid, &msg, sizeof(msg), mesg_type, 0);
    return msg.data;
}

bool check_if_exit(int msgid, int mesg_type)
{
    msg_buffer msg;
    msg.data = 0;
    return (msgrcv(msgid, &msg, sizeof(msg), mesg_type, IPC_NOWAIT) >= 0);
}
