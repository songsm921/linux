#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void){
    int fd = open("/dev/LED",O_RDWR);
    int i = 0;
    char enter;
    if(fd < 0)
    {
        perror("open failed");
        exit(0);
    }
    while(i<10){
        scanf("%c",&enter);
        write(fd,&enter,sizeof(char));
        i++;
    }
    close(fd);
    return 0;
}