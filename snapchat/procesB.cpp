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
#include <algorithm>
#include <vector>

using namespace std;
void mirror_line(memory*, int, int);
void mirror_line_y(memory*, int, int);
void mirror_line_x(memory*, int, int);
void black_white(memory* str);
void color_saturation (memory* str, int precentage, char color);
std::vector<int> options;


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
   mouse_pos mouse, mouse_msg;
   mouse.x = W/2;
   mouse.y = H/2;
   int option,s;

   while(true)
   {

       wait_for_signal(msgid, AtoB,'Z');
       wait_for_signal(msgid, CtoB,'Z');


       if(msgrcv(msgid, &mouse_msg, sizeof(mouse_msg), MOUSE_POS, IPC_NOWAIT) >= 0)
       {
           std::cout << options.size() << std::endl;
           mouse.x = mouse_msg.x;
           mouse.y = mouse_msg.y;
       }
       if(msgrcv(msgid, &msg, sizeof(msg), OPTION, IPC_NOWAIT) >= 0)
       {
           option = (int) msg.data;
           if(option == 0)
                options.clear();
            else
                options.push_back(option);

           std::cout << std::endl;
           mouse.x = W/2;
           mouse.y = H/2;
       }

       for(int opt : options)
       {
           if(opt == 2)
            mirror_line_x(str, mouse.x, mouse.y);
           else if(opt == 1)
            mirror_line_y(str, mouse.x, mouse.y);
           else if(opt == 3)
            black_white(str);
            else if(opt == 4)
                color_saturation(str,200,'R');
       }

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

void mirror_line_y(memory* str, int x_pos, int y_pos)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;
    int new_y = 0;
    int i, end;

    if( y_pos >= image.rows/2 ){
        i = y_pos;
        end = image.rows;
    }
    else{
        i = 0;
        end = y_pos;
    }
    for( ; i < end; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            new_y = 2 * y_pos - i;

            if( new_y < image.rows && new_y >= 0 ){
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[new_y*image.cols*CHANNELS + j*CHANNELS + 0] ;
                 str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[new_y*image.cols*CHANNELS + j*CHANNELS + 1]; // G
                 str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[new_y*image.cols*CHANNELS + j*CHANNELS + 2];
            }

        }
    }
}


void mirror_line_x(memory* str, int x_pos, int y_pos)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;
    int new_x = 0;
    int start, end;

    if( x_pos >= image.cols/2 ){
        start = x_pos;
        end = image.cols;
    }
    else{
        start = 0;
        end = x_pos;
    }

    for(int i = 0; i < image.rows; i++)
    {
        for(int j=start ; j < end; j++)
        {
            new_x = 2 * x_pos - j;

            if( new_x < image.cols && new_x >= 0 ){
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[i*image.cols*CHANNELS + new_x*CHANNELS + 0];
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[i*image.cols*CHANNELS + new_x*CHANNELS + 1];  // G
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[i*image.cols*CHANNELS + new_x*CHANNELS + 2];
            }
        }

    }
}

void black_white(memory* str)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;
    uint8_t grey;

    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            grey = 0.11 * str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] + 0.59 * str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] + 0.3 * str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2];
            str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = grey;
            str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = grey;
            str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = grey;
        }
    }
}


uint8_t less_than_255 ( uint8_t color ){
    if (color > 255)
        return 255;
    else return color;
}

void color_saturation (memory* str, int precentage, char color)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;

    for(int i = 0; i < image.rows; i++)
    {
        for( int j = 0; j < image.cols; j++)
        {
            if(color == 'B')
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = less_than_255(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] * (float)precentage/100);
            else if (color == 'G')
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = less_than_255(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] * (float)precentage/100);  // G
            else if (color == 'R')
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = less_than_255((uint8_t)(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] + (float)precentage));

                //less_than_255((uint8_t)(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] + (float)precentage/100))
        }
    }
}
