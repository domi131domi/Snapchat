#include <iostream>
#include <stdio.h>

using namespace std;


int main( int argc, char** argv ) {

    //podpiecie sie do kolejki
    key_t key = ftok(KEYQ, 65);

    int msgid = msgget(key, 0666);
    if(msgid == -1)
    {
        perror("procesA: nie udalo sie podpiac do kolejki");
        exit(1);
    }

    return 0;
}
