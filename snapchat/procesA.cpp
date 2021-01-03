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

    memory *str = (memory*) shmat(shmid,NULL,0);
    str->data = 9;
    shmdt(str);

    msg_buffer msg;
    msg.mesg_type = 1;
    msg.data = 'Z';

    msgsnd(msgid, &msg, sizeof(msg), 0);
    std::cout << "wiadomosc wyslana:" << msg.data << std::endl;

    return 0;
}
