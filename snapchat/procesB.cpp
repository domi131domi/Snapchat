#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
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

    //podpiecie sie do pamieci dzielonej
    key_t key_M = ftok(KEYM,65);

    int shmid = shmget(key_M,sizeof(memory),PERMS);
    if (shmid == -1) {
      perror("Nie udalo sie podlaczyc do pamieci dzielonej");
      exit(1);
   }

    memory* str = (memory*) shmat(shmid,NULL,0);

    msg_buffer msg;

    msgrcv(msgid, &msg, sizeof(msg), 1, 0);
    std::cout << "wiadomosc odebrana:" << msg.data << std::endl;
    std::cout << "wiadomosc w memory:" << str->data << std::endl;
    shmdt(str);

    return 0;
}
