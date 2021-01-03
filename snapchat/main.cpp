#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h> 

using namespace cv;
using namespace std;

struct mesg_buffer {
    long mesg_type;
    char data;
} message;

int main( int argc, char** argv ) {

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

    return 0;
}



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
