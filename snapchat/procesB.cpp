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
void drawMark(int x, int y, char color);

struct Option
{
        int val;
        mouse_pos pos;
};
std::vector<Option> options;
uint8_t drawing[W*H*CHANNELS];

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
   Option option;
   bool drawing_empty = true;

   while(true)
   {

       wait_for_signal(msgid, AtoB,'Z');
       wait_for_signal(msgid, CtoB,'Z');


       if(msgrcv(msgid, &mouse_msg, sizeof(mouse_msg), MOUSE_POS, IPC_NOWAIT) >= 0)
       {
            mouse.x = mouse_msg.x;
            mouse.y = mouse_msg.y;
           if(options.size() > 0)
           {
               options[options.size()-1].pos.x = mouse.x;
               options[options.size()-1].pos.y = mouse.y;
           }

       }
       if(msgrcv(msgid, &msg, sizeof(msg), OPTION, IPC_NOWAIT) >= 0)
       {
           option.val = (int) msg.data;
           if(option.val == 0)
           {
               options.clear();
               for(int e = 0; e < H*W*CHANNELS; e++)
               {
                   drawing[e] = 0;
               }
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
            mirror_line_x(str, opt.pos.x, opt.pos.y);
           else if(opt.val == 1)
            mirror_line_y(str, opt.pos.x, opt.pos.y);
           else if(opt.val == 3)
                black_white(str);
            else if(opt.val == 4)
                color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'R');
            else if(opt.val == 5)
                color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'G');
            else if(opt.val == 6)
                color_saturation(str,MAX_SKALA_KOLOR - (int)(opt.pos.y / (float)H * (float)(2*MAX_SKALA_KOLOR)),'B');
            else if(opt.val == 20)
            {
                drawMark(opt.pos.x, opt.pos.y, 'B');
                drawing_empty = false;
            }
            else if(opt.val == 21)
            {
                drawMark(opt.pos.x, opt.pos.y, 'G');
                drawing_empty = false;
            }
            else if(opt.val == 22)
            {
                drawMark(opt.pos.x, opt.pos.y, 'R');
                drawing_empty = false;
            }

            if(!drawing_empty)
            {
                for(int i = 0; i < H*W*CHANNELS; i++)
                    if(drawing[i] != 0)
                        str->picture[i] = drawing[i];

            }


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
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = less_than_255((int)(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0]) + precentage);
            else if (color == 'G')
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = less_than_255((int)(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1]) + precentage);  // G
            else if (color == 'R')
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = less_than_255((int)(str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2]) + precentage );
        }
    }
}

void drawMark(int x, int y, char color)
{
    if(color == 'B')
    {
        drawing[(y * W + x)*CHANNELS + 0] = 255;
        drawing[(y * W + x)*CHANNELS + 1] = 0;
        drawing[(y * W + x)*CHANNELS + 2] = 0;

        if(y < H && y > 0 && x < W && x)
        {
            drawing[(y * W + x)*CHANNELS + 0 +3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 +3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 -3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS] = 255;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS] = 255;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS-3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS-3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS+3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS+3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS+3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS+3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS-3] = 255;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS-3] = 0;

        }
    }
    if(color == 'G')
    {
        drawing[(y * W + x)*CHANNELS + 0] = 0;
        drawing[(y * W + x)*CHANNELS + 1] = 255;
        drawing[(y * W + x)*CHANNELS + 2] = 0;

        if(y < H && y > 0 && x < W && x)
        {
            drawing[(y * W + x)*CHANNELS + 0 +3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 +3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 -3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS] = 255;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS] = 255;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS-3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS-3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS+3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS+3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS+3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS+3] = 0;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS-3] = 255;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS-3] = 0;

        }
    }
    if(color == 'R')
    {
        drawing[(y * W + x)*CHANNELS + 0] = 0;
        drawing[(y * W + x)*CHANNELS + 1] = 0;
        drawing[(y * W + x)*CHANNELS + 2] = 255;

        if(y < H && y > 0 && x < W && x)
        {
            drawing[(y * W + x)*CHANNELS + 0 +3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +3] = 255;

            drawing[(y * W + x)*CHANNELS + 0 -3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -3] = 255;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS] = 255;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS] = 255;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS-3] = 255;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS+3] = 255;

            drawing[(y * W + x)*CHANNELS + 0 +W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 +W*CHANNELS+3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 +W*CHANNELS+3] = 255;

            drawing[(y * W + x)*CHANNELS + 0 -W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 1 -W*CHANNELS-3] = 0;
            drawing[(y * W + x)*CHANNELS + 2 -W*CHANNELS-3] = 255;

        }
    }
}

void set_image(memory* str, int x_pos, int y_pos)
{
    cv::Mat image;
    image.create(H,W,CV_8UC3);
    cv::Mat photo = imread("/mnt/d/01_STUDIA/SEM5/SCZR/projekt/repo/Snapchat/snapchat/photo.png", IMREAD_COLOR);    // 3 channels

    if(photo.empty())
    {
        std::cerr << "Could not read the image: " << std::endl;
        return;
    }

    uint8_t* pixelPtr = (uint8_t*)image.data;
    int photo_y = 0, photo_x = 0;
    uint8_t pixelR, pixelG, pixelB;
    for(int i = y_pos; i < y_pos + photo.rows; i++)
    {
        photo_x = 0;
        for( int j = x_pos; j < x_pos + photo.cols; j++)
        {
            pixelB = photo[photo_y*photo.cols*photo.channels() + photo_y*photo.channels() + 0];
            pixelG = photo[photo_y*photo.cols*photo.channels() + photo_y*photo.channels() + 1];
            pixelR = photo[photo_y*photo.cols*photo.channels() + photo_y*photo.channels() + 2];
            if( !(pixelB == 255 && pixelG == 255 && pixelR == 255)){
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0] = pixelB;
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1] = pixelG;  // G
                str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2] = pixelR;
            }
            ++photo_x;    
        }
        ++photo_y;
    }
}
