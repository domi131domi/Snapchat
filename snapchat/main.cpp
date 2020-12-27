#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {

  cv::Mat image;

  VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

  //cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );


int key;
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
  return 0;
}

