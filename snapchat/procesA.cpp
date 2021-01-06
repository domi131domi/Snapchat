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

    cv::Mat image;
    image.create(H,W,CV_8UC3);

    cv::VideoCapture cap(0);
    if(!cap.isOpened())
    {
        perror("Nie udalo sie znalezc kamery.");
        exit(1);
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH,W);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,H);


    if(image.cols != W || image.rows != H)
    {
        std::cout <<  "Nie udalo sie ustawic wymiarow kamery." << std::endl ;
        exit(1);
    }

    uint8_t* pixelPtr;
    msg_buffer msg;

    while(true)
    {

        cap.read(image);

        pixelPtr = (uint8_t*)image.data;

        for(int i = 0; i < image.rows; i++)
        {
            for(int j = 0; j < image.cols; j++)
            {
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0]; // B
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1]; // G
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2]; // R

            }
        }
        send_signal(msgid,AtoB,'Z');
        if(check_if_exit(msgid,CLOSE_A))
        {
            break;
        }
        wait_for_signal(msgid,BtoA,'Z');

    }

    return 0;
}
