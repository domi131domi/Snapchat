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
#include <fstream>
#include <thread>
#include <cstdlib>

using namespace std;

int mouseX = W/2, mouseY = H/2;
bool new_pos = false, new_option = false, working = true;
int option = 0;

void my_mouse_callback( int event, int x, int y, int flags, void* param ) {
    if(event==CV_EVENT_LBUTTONDOWN){
       mouseX = x;
       mouseY = y;
       new_pos = true;
   }
   if(option == 7 && event==CV_EVENT_LBUTTONDOWN)
   {
       option = 20;
       new_option = true;
   }
   if(option == 20 )
   {
       mouseX = x;
       mouseY = y;
       new_pos = true;
       if(event==CV_EVENT_LBUTTONUP)
       {
          option = 7;
          new_option = true;
       }
   }
   if(option == 8 && event==CV_EVENT_LBUTTONDOWN)
   {
       option = 21;
       new_option = true;
   }
   if(option == 21 )
   {
       mouseX = x;
       mouseY = y;
       new_pos = true;
       if(event==CV_EVENT_LBUTTONUP)
       {
          option = 8;
          new_option = true;
       }
   }
   if(option == 9 && event==CV_EVENT_LBUTTONDOWN)
   {
       option = 22;
       new_option = true;
   }
   if(option == 22 )
   {
       mouseX = x;
       mouseY = y;
       new_pos = true;
       if(event==CV_EVENT_LBUTTONUP)
       {
          option = 9;
          new_option = true;
       }
   }

}

void load_option()
{
    std::cout << "Wybierz opcje:" << std::endl;
    std::cout << "0. Brak filtra" << std::endl;
    std::cout << "1. Odbicie w poziomie" << std::endl;
    std::cout << "2. Odbicie w pionie" << std::endl;
    while(working)
    {
        std::cout << ">";
        std::cin >> option;
        new_option = true;
    }
}


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

   cv::Mat image;
   image.create(H,W,CV_8UC3);
   cv::namedWindow("Snapchat");
   setMouseCallback( "Snapchat", my_mouse_callback, &image );
   thread loadThread(load_option);

   uint8_t* pixelPtr = (uint8_t*)image.data;
   memory* str = (memory*) shmat(shmid,NULL,0);
   while(working)
   {
       //wyslanie pozycji myszy
       if(new_pos)
       {
           mouse_pos msg;
           msg.mesg_type = MOUSE_POS;
           msg.x = mouseX;
           msg.y = mouseY;
           msgsnd(msgid, &msg, sizeof(msg), 0);
           new_pos = false;
       }

      if(new_option)
      {
          msg_buffer msg;
          msg.mesg_type = OPTION;
          msg.data = option;
          msgsnd(msgid, &msg, sizeof(msg), 0);
          new_option = false;
      }

       send_signal(msgid,CtoB,'Z');
       if(check_if_exit(msgid,CLOSE_C))
       {
           working = false;
       }
       if(working)
       {
        wait_for_signal(msgid,BtoC,'Z');

       for(int i = 0; i < image.rows; i++)
       {
           for(int j = 0; j < image.cols; j++)
            {
                pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0];    // B
                pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1];    // G
                pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2];    // R

            }
        }
        cv::imshow("Snapchat", image);
        cv::waitKey(10);
        }
    }

    cv::destroyAllWindows();
    return 0;
}
