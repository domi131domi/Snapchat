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
#include <chrono>


using namespace std;

cpu_set_t  mask;

int main( int argc, char** argv ) {

    CPU_ZERO(&mask);
    CPU_SET(10, &mask);
    int result = sched_setaffinity(0, sizeof(mask), &mask);

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
    int diff = BLOCK_SIZE;
    std::chrono::duration<double> elapsed;
    double elapsed_d = DURATION;
    auto now = std::chrono::high_resolution_clock::now();
    auto lastImg = std::chrono::high_resolution_clock::now();
    bool init = true;

    cap.read(image);

    pixelPtr = (uint8_t*)image.data;

    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            str->picture[0][i*image.cols*CHANNELS + j*CHANNELS + 0] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0]; // B
            str->picture[0][i*image.cols*CHANNELS + j*CHANNELS + 1] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1]; // G
            str->picture[0][i*image.cols*CHANNELS + j*CHANNELS + 2] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2]; // R

        }
    }
    send_signal(msgid,AtoB,'Z');
    diff--;

    while(true)
    {
        for(int block = 0; block < BLOCK_SIZE; block++)
        {
            if(init)
            {
                block = 1;
                init = false;
            }
            if(check_if_exit(msgid,CtoA))
            {
                diff++;
            }
            if(diff == 0)
            {
                std::cout << "Czekam" << diff << std::endl;
                wait_for_signal(msgid, CtoA);
                diff++;
            }

            now = std::chrono::high_resolution_clock::now();
            elapsed = now - lastImg;
            elapsed_d = elapsed.count();
            while( elapsed_d < DURATION )
            {
                now = std::chrono::high_resolution_clock::now();
                elapsed = now - lastImg;
                elapsed_d = elapsed.count();
            }
            lastImg = std::chrono::high_resolution_clock::now();

            cap.read(image);

            pixelPtr = (uint8_t*)image.data;

            for(int i = 0; i < image.rows; i++)
            {
                for(int j = 0; j < image.cols; j++)
                {
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0]; // B
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1]; // G
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2]; // R

                }
            }
            send_signal(msgid,AtoB,'Z');
            diff--;
            if(check_if_exit(msgid,CLOSE_A))
            {
                break;
            }
        }

    }
    return 0;
}
