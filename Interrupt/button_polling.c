#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(){
    int fd;
    int value;
    bool input = true;
    fd = open("/dev/poll", O_RDWR);
    if(fd<0){
        printf("Unable to open file\n");
        printf("%d",errno);
        return 0;
    }
    while(1)
    {   
        read(fd,&value,sizeof(int));
        if(value == 1)
        {
            write(fd,&input,sizeof(int));
            input = !input;
        }
        sleep(2);
    }
    close(fd);
    return 0;
}