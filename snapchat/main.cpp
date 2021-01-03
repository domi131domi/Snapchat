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
#include "defines.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {

    //Tworzenie kolejki
    key_t key = ftok(KEYQ, 65);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1)
    {
        std::cout << "Nie udalo sie utworzyc kolejki" << std::endl;
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
        execv(args[0],args);

        exit(1);
    }
    else if(pid_C == -1)
    {
        std::cout << "Wystapil blad przy tworzeniu procesu C" << std::endl;
    }


    while (wait(NULL) > 0);

    //usuniecie kolejki
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}



/*
std::cout<<"Tworze dziecko:"<<std::endl;
char* prog = (char*)"./dziecko";
char *args[]={prog,NULL};

pid_t child_pid,wpid;
int status = 0;

child_pid = fork();

if(child_pid == 0)    //dziecko
{
    execv(args[0],args);

    exit(1);
}
else if(child_pid == -1)
{
    std::cout << "Wystapil blad tworzenia dziecka." << std::endl;
}
else
{
    std::cout << "siema jestem rodzicem" << std::endl;
}

cout<<"Czekam na dzieci"<<std::endl;
while (wait(NULL) > 0);
cout<<"Rodzic zwija"<<std::endl;
*/
/*    cv::Mat image;

VideoCapture cap(0);
if(!cap.isOpened())
return -1;

int key;
//exit aby wyjsc
while(key != 27)
{
    cap >> image;

    if( !image.data ) {
        std::cout <<  "Could not load camera view." << std::endl ;
        return -1;
    }

    cv::imshow("Snapchat", image);
    key = waitKey(10);
}

destroyAllWindows();
*/
