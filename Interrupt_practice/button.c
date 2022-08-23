#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(){
    int fd;
    char value;
    fd = open("/dev/test", O_RDWR);
    if(fd<0){
        printf("Unable to open file\n");
        printf("%d",errno);
        return 0;
    }
    while(1)
    {
    }
    close(fd);
    return 0;
}