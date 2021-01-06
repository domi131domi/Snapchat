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

using namespace std;
void mirror_line(memory* str);

int main( int argc, char** argv ) {


    //podpiecie sie do kolejki
    key_t key = ftok(KEYQ, 65);

    int msgid = msgget(key, 0666);
    if(msgid == -1)
    {
        std::cout << "procesB: nie udalo sie podpiac do kolejki" << std::endl;
        exit(1);
    }

    //podpiecie sie do pamieci dzielonej
    key_t key_M = ftok(KEYM,65);

    int shmid = shmget(key_M,sizeof(memory),PERMS);
    if (shmid == -1) {
      perror("procesB: Nie udalo sie podlaczyc do pamieci dzielonej");
      exit(1);
   }

   msg_buffer msg;
   memory* str = (memory*) shmat(shmid,NULL,0);

   while(true)
   {

       wait_for_signal(msgid, AtoB,'Z');
       wait_for_signal(msgid, CtoB,'Z');

       mirror_line(str);

       send_signal(msgid,BtoA,'Z');
       send_signal(msgid,BtoC,'Z');

       if(check_if_exit(msgid,CLOSE_ALL))
       {
           send_signal(msgid,CLOSE_A,'Z');
           send_signal(msgid,CLOSE_C,'Z');
           break;
       }
   }
    return 0;
}


void mirror_line(memory* str)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;


    //ADAM TUTAJ SIE MODYFIKUJE OBRAZ
    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            // OBRAZ ZAPIYWANY                                                                  OBRAZ ODCZYTYWANY
            str->picture[i*image.cols*CHANNELS + (image.cols - j-1)*CHANNELS + 0] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0];    // B
            str->picture[i*image.cols*CHANNELS + (image.cols - j-1)*CHANNELS + 1] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1];    // G
            str->picture[i*image.cols*CHANNELS + (image.cols - j-1)*CHANNELS + 2] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2];    // R

            // jak zrobisz to na dole to bedzie zwykly obraz, (ale w odbiciu, nwm czemu ale to tak jest po prostu)
            //str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0];    // B
            //str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1];    // G
            //str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2];    // R
        }
    }
}
