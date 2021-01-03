#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "defines.h"

using namespace std;


int main( int argc, char** argv ) {


    //podpiecie sie do kolejki
    key_t key = ftok(KEYQ, 65);

    int msgid = msgget(key, 0666);
    if(msgid == -1)
    {
        std::cout << "procesA: nie udalo sie podpiac do kolejki" << std::endl;
        exit(1);
    }

    mesg_buffer msg;

    msgrcv(msgid, &msg, sizeof(msg), 1, 0);
    std::cout << "wiadomosc odebrana:" << msg.data << std::endl;

    return 0;
}
