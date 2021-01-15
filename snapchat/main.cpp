#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "defines.h"
#include <string>

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {

    //Tworzenie kolejki
    key_t key_Q = ftok(KEYQ, 65);

    int msgid = msgget(key_Q, 0666 | IPC_CREAT);
    if(msgid == -1)
    {
        std::cout << "Nie udalo sie utworzyc kolejki" << std::endl;
        exit(1);
    }
    
    //Tworzenie pamieci dzielonej
    key_t key_M = ftok(KEYM,66);
    int shmid = shmget(key_M,sizeof(memory),PERMS | IPC_CREAT);
    if (shmid == -1) {
      perror("Nie udalo sie utworzyc pamieci dzielonej");
      exit(1);
   }

    //Tworzenie procesow
    char* prog;
    char *args[2];
    pid_t pid_A,pid_B,pid_C;

    pid_A = fork();

    if(pid_A == 0)    //proces A
    {
        prog = (char*)"./procesA";
        args[0] = prog;
        args[1] = NULL;
        execv(args[0],args);

        exit(1);
    }
    else if(pid_A == -1)
    {
        std::cout << "Wystapil blad przy tworzeniu procesu A" << std::endl;
    }

    pid_B = fork();

    if(pid_B == 0)    //proces B
    {
        prog = (char*)"./procesB";
        args[0] = prog;
        args[1] = NULL;
        execv(args[0],args);

        exit(1);
    }
    else if(pid_B == -1)
    {
        std::cout << "Wystapil blad przy tworzeniu procesu B" << std::endl;
    }

    pid_C = fork();

    if(pid_C == 0)    //proces C
    {
        prog = (char*)"./procesC";
        args[0] = prog;
        args[1] = NULL;
        execl("/usr/bin/gnome-terminal", "gnome-terminal", "--", prog, NULL);
        //cout << "Hello" << endl;
        //execv(args[0],args);

        exit(1);
    }
    else if(pid_C == -1)
    {
        std::cout << "Wystapil blad przy tworzeniu procesu C" << std::endl;
    }

    std::string command = "";
    std::cout << "Komendy:" << std::endl << " q - zamknij program" << std::endl;
    while(command != "q")
    {
        std::cin >> command;
    }

    send_signal(msgid,CLOSE_ALL,'Z');
    while (wait(NULL) > 0);

    //usuniecie kolejki
    msgctl(msgid, IPC_RMID, NULL);
    //usuniecie pamieci dzielonej
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}
