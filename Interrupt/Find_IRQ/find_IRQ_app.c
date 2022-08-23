#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
int main(void){
    int fd = open("/dev/timer",O_RDWR);
    if(fd < 0){
        perror("open failed");
        exit(0);
    }
    while(1){
        int res;
        int val;
        scanf("%d",&val);
        res = write(fd,&val,sizeof(int));
        if(res<0)
        {
            printf("NO.\n");
        }
        else
        {
            printf("OK. %d \n",val);
        }
    }    
    close(fd);
    return 0;
}