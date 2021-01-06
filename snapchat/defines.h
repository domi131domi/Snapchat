#define PERMS 0666
#define KEYQ "ID_KOLEJKA2"
#define KEYM "ID_PAMIEC2"
#define W 1280
#define H 720
#define CHANNELS 3

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>

struct msg_buffer
{
    long mesg_type;
    char data;
};

struct memory
{
    //int n;
    //cv::Mat picture;
    uint8_t picture[W*H*CHANNELS];
};
