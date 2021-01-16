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
#include <algorithm>
#include <vector>
#include <fstream>
#include <pthread.h>
#include <sched.h>

using namespace std;
void mirror_line(memory*, int, int,int);
void mirror_line_y(memory*, int, int,int);
void mirror_line_x(memory*, int, int,int);
void black_white(memory* str,int);
void color_saturation (memory* str, int precentage, char color,int);
void drawMark(int x, int y, char color,int);
void set_image(memory* , int , int, int,int );

struct Option
{
        int val;
        mouse_pos pos;
};

struct Draws
{
    char color;
    mouse_pos pos;

    void drawMark(memory* str,int block)
    {
        if(color == 'B')
        {
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0] = 255;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1] = 0;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2] = 0;

            if(pos.y < H && pos.y > 0 && pos.x < W && pos.x)
            {
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS-3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS-3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS+3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS+3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS+3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS+3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS-3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS-3] = 0;

            }
        }
        if(color == 'G')
        {
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0] = 0;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1] = 255;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2] = 0;

            if(pos.y < H && pos.y > 0 && pos.x < W && pos.x)
            {
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS-3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS-3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS+3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS+3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS+3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS+3] = 0;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS-3] = 255;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS-3] = 0;

            }
        }
        if(color == 'R')
        {
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0] = 0;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1] = 0;
            str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2] = 255;

            if(pos.y < H && pos.y > 0 && pos.x < W && pos.x)
            {
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +3] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -3] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS-3] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS+3] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 +W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 +W*CHANNELS+3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 +W*CHANNELS+3] = 255;

                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 0 -W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 1 -W*CHANNELS-3] = 0;
                str->picture[block][(pos.y * W + pos.x)*CHANNELS + 2 -W*CHANNELS-3] = 255;

            }
        }
    }
}; //Draws

std::vector<Draws> drawing;
std::vector<Option> options;
//uint8_t drawing[W*H*CHANNELS];

cpu_set_t  mask;

int main( int argc, char** argv ) {

    CPU_ZERO(&mask);
    CPU_SET(9, &mask);
    int result = sched_setaffinity(0, sizeof(mask), &mask);

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
   Option option;
   bool drawing_empty = true;

   std::ofstream file("WynikNormal_B.txt");

   while(true)
   {
       for(int block=0; block < BLOCK_SIZE; block++)
       {
       auto start = std::chrono::high_resolution_clock::now();
       wait_for_signal(msgid, AtoB);

       if(msgrcv(msgid, &mouse_msg, sizeof(mouse_msg), MOUSE_POS, IPC_NOWAIT) >= 0)
       {
            mouse.x = mouse_msg.x;
            mouse.y = mouse_msg.y;
           if(options.size() > 0)
           {
               options[options.size()-1].pos.x = mouse.x;
               options[options.size()-1].pos.y = mouse.y;

           if(options[options.size()-1].val == 20)
           {
               Draws draws;
               draws.color = 'B';
               draws.pos.x = mouse.x;
               draws.pos.y = mouse.y;
               drawing.push_back(draws);
               drawing_empty = false;
           }
           else if(options[options.size()-1].val == 21)
           {
               Draws draws;
               draws.color = 'G';
               draws.pos.x = mouse.x;
               draws.pos.y = mouse.y;
               drawing.push_back(draws);
               drawing_empty = false;
           }
           else if(options[options.size()-1].val == 22)
           {
               Draws draws;
               draws.color = 'R';
               draws.pos.x = mouse.x;
               draws.pos.y = mouse.y;
               drawing.push_back(draws);
               drawing_empty = false;
           }
           }
       }

       if(msgrcv(msgid, &msg, sizeof(msg), OPTION, IPC_NOWAIT) >= 0)
       {
           option.val = (int) msg.data;
           if(option.val == 0)
           {
               options.clear();
               drawing.clear();

               drawing_empty = true;
           }
            else
            {
                option.pos.x = W/2;
                option.pos.y = H/2;
                options.push_back(option);
            }


       }


           for(Option opt : options)
           {
               if(opt.val == 2)
                mirror_line_x(str, opt.pos.x, opt.pos.y,block);
               else if(opt.val == 1)
                mirror_line_y(str, opt.pos.x, opt.pos.y,block);
               else if(opt.val == 3)
                    black_white(str,block);
                else if(opt.val == 4)
                    color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'R',block);
                else if(opt.val == 5)
                    color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'G',block);
                else if(opt.val == 6)
                    color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'B',block);
                else if(opt.val == 10)
                    set_image(str, opt.pos.x, opt.pos.y, 0,block);
                else if(opt.val == 11)
                    set_image(str, opt.pos.x, opt.pos.y, 1,block);
                else if(opt.val == 12)
                    set_image(str, opt.pos.x, opt.pos.y, 2,block);
                else if(opt.val == 13)
                    set_image(str, opt.pos.x, opt.pos.y, 3,block);


                if(!drawing_empty)
                {

                    for( Draws d : drawing )
                    {
                        d.drawMark(str,block);
                    }

                }
            }

           send_signal(msgid,BtoC,'Z');
           //std::cout << "Przygotowalem" << block << std::endl;
           if(check_if_exit(msgid,CLOSE_ALL))
           {
               send_signal(msgid,CLOSE_A,'Z');
               send_signal(msgid,CLOSE_C,'Z');
               break;
           }

        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        file << elapsed.count() << std::endl;
        }
       }
    return 0;
}

void mirror_line_y(memory* str, int x_pos, int y_pos,int block)
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
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[block][new_y*image.cols*CHANNELS + j*CHANNELS + 0] ;
                 str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[block][new_y*image.cols*CHANNELS + j*CHANNELS + 1]; // G
                 str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[block][new_y*image.cols*CHANNELS + j*CHANNELS + 2];
            }

        }
    }
}


void mirror_line_x(memory* str, int x_pos, int y_pos, int block)
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
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[block][i*image.cols*CHANNELS + new_x*CHANNELS + 0];
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[block][i*image.cols*CHANNELS + new_x*CHANNELS + 1];  // G
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[block][i*image.cols*CHANNELS + new_x*CHANNELS + 2];
            }
        }

    }
}

void black_white(memory* str, int block)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;
    uint8_t grey;

    for(int i = 0; i < image.rows; i++)
    {
        for(int j = 0; j < image.cols; j++)
        {
            grey = 0.11 * str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] + 0.59 * str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] + 0.3 * str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2];
            str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = grey;
            str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = grey;
            str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = grey;
        }
    }
}


uint8_t less_than_255 ( int color ){
    if (color > 255)
    {
        return 255;
    }
    else if(color < 0)
    {
        return 0;
    }
    else
    {
        return color;
    }
}

void color_saturation (memory* str, int precentage, char color,int block)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);

    uint8_t* pixelPtr = (uint8_t*)image.data;

    for(int i = 0; i < image.rows; i++)
    {
        for( int j = 0; j < image.cols; j++)
        {
            if(color == 'B')
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = less_than_255((int)(str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0]) + precentage);
            else if (color == 'G')
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = less_than_255((int)(str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1]) + precentage);  // G
            else if (color == 'R')
                str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = less_than_255((int)(str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2]) + precentage );
        }
    }
}
void set_image(memory* str, int x_pos, int y_pos, int type,int block)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);
    cv::Mat photo;
    if(type == 0)
    {
        photo = imread("smile.png", cv::IMREAD_COLOR);
    }
    else if(type == 1)
    {
        photo = imread("kleks.png", cv::IMREAD_COLOR);
    }
    else if(type == 2)
    {
        photo = imread("mask.png", cv::IMREAD_COLOR);
    }
    else
    {
        photo = imread("glasses.png", cv::IMREAD_COLOR);
    }

    if(photo.empty())
    {
        std::cerr << "Could not read the image: " << std::endl;
        return;
    }
    x_pos = x_pos-photo.cols/2;
    y_pos = y_pos-photo.rows/2;
    int chann = photo.channels();
    uint8_t* pixelPtr = (uint8_t*)photo.data;
    int photo_y = 0, photo_x = 0;
    uint8_t pixelR, pixelG, pixelB;
    for(int i = y_pos; i < y_pos + photo.rows; i++)
    {
        photo_x = 0;
        for( int j = x_pos; j < x_pos + photo.cols; j++)
        {
            if(i >= 0 && i < H && j >= 0 && j < W)
            {
                pixelB = pixelPtr[photo_y*photo.cols*chann + photo_x*chann + 0];
                pixelG = pixelPtr[photo_y*photo.cols*chann + photo_x*chann + 1];
                pixelR = pixelPtr[photo_y*photo.cols*chann + photo_x*chann + 2];
                if( !(pixelB == 255 && pixelG == 255 && pixelR == 255)){
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0] = pixelB;
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1] = pixelG;  // G
                    str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2] = pixelR;
                }
            }
            ++photo_x;
        }
        ++photo_y;

    }
}
