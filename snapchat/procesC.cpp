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
    std::cout << "3. Obraz bialo-czarny" << std::endl;
    std::cout << "4. Zwiekszenie barwy czerwonej" << std::endl;
    std::cout << "5. Zwiekszenie barwy zielonej" << std::endl;
    std::cout << "6. Zwiekszenie barwy niebieskiej" << std::endl;
    std::cout << "7. Mazak niebieski" << std::endl;
    std::cout << "8. Mazak zielony" << std::endl;
    std::cout << "9. Mazak czerwony" << std::endl;
    std::cout << "10. Dodaj obrazek: smile" << std::endl;
    std::cout << "11. Dodaj obrazek: kleks" << std::endl;
    std::cout << "12. Dodaj obrazek: mask" << std::endl;
    std::cout << "13. Dodaj obrazek: glasses" << std::endl;
    while(working)
    {
        std::cout << ">";
        std::cin >> option;
        new_option = true;
    }
}

uint8_t temp[H*W*CHANNELS];

cpu_set_t  mask;
int diff = 0;
std::chrono::duration<double> elapsed;
double elapsed_d = DURATION;
auto now = std::chrono::high_resolution_clock::now();
auto lastImg = std::chrono::high_resolution_clock::now();

int main( int argc, char** argv ) {

    CPU_ZERO(&mask);
    CPU_SET(11, &mask);
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

   cv::Mat image;
   image.create(H,W,CV_8UC3);
   cv::namedWindow("Snapchat");
   setMouseCallback( "Snapchat", my_mouse_callback, &image );
   thread loadThread(load_option);

   uint8_t* pixelPtr = (uint8_t*)image.data;
   memory* str = (memory*) shmat(shmid,NULL,0);
   std::ofstream file("WynikNormal_C.txt");
   int block = 0;
   wait_for_signal(msgid, BtoC);
   diff++;
   while(working)
   {
       auto start = std::chrono::high_resolution_clock::now();
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


       if(working)
       {
        if(check_if_exit(msgid, BtoC)){
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

       if(diff > 0)
       {

       for(int i = 0; i < image.rows; i++)
       {
           for(int j = 0; j < image.cols; j++)
            {
                //pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 0];    // B
                //pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 1];    // G
                //pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[i*image.cols*CHANNELS + j*CHANNELS + 2];    // R
                temp[i*image.cols*CHANNELS + j*CHANNELS + 0] = str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 0];
                temp[i*image.cols*CHANNELS + j*CHANNELS + 1] = str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 1];
                temp[i*image.cols*CHANNELS + j*CHANNELS + 2] = str->picture[block][i*image.cols*CHANNELS + j*CHANNELS + 2];
            }
        }
        block++;
        if(block >= BLOCK_SIZE)
            block = 0;
        diff--;
        send_signal(msgid,CtoA,'Z');
        }
        if(check_if_exit(msgid,CLOSE_C))
        {
            working = false;
        }
            for(int i = 0; i < image.rows; i++)
            {
                for(int j = 0; j < image.cols; j++)
                 {
                     pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 0] = temp[i*image.cols*CHANNELS + j*CHANNELS + 0];    // B
                     pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 1] = temp[i*image.cols*CHANNELS + j*CHANNELS + 1];    // G
                     pixelPtr[i*image.cols*CHANNELS + j*CHANNELS + 2] = temp[i*image.cols*CHANNELS + j*CHANNELS + 2];    // R
                 }
             }


             elapsed_d = 0;
             while( elapsed_d < DURATION )
             {
                 cv::imshow("Snapchat", image);
                 cv::waitKey(1);
                 now = std::chrono::high_resolution_clock::now();
                 elapsed = now - lastImg;
                 elapsed_d = elapsed.count();
             }
             lastImg = std::chrono::high_resolution_clock::now();
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        file << elapsed.count() << std::endl;
    }

    cv::destroyAllWindows();
    return 0;
}
