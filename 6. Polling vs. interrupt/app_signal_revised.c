#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#define BASE_ADDR_BRAM_1 0xA0010000
#define BASE_ADDR_BRAM_2 0xA0012000
#define LEN 8
void *bram_addr_1;
void *bram_addr_2;
void handler(){
    memcpy(bram_addr_1,bram_addr_2,LEN);
    printf("Signal Received\n");
}
int main(){
    pid_t pid;
    clock_t start_1,end_1,start_2,end_2;
    sigset_t set;
    struct sigaction act;
    int signum;
    char input;
    int fd, fd_2, fd_3;
    pid = getpid();
    printf("This process ID is %d\n",pid);
    fd = open("/dev/mem",O_RDWR);
    fd_2 = open("/dev/mem",O_RDWR);
    fd_3 = open("/dev/timer",O_RDWR);
    bram_addr_1 = mmap(0,LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd,BASE_ADDR_BRAM_1);
    bram_addr_2 = mmap(0,LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd_2,BASE_ADDR_BRAM_2);
    if(bram_addr_1 == MAP_FAILED || bram_addr_2 == MAP_FAILED){
        if(bram_addr_1 != MAP_FAILED){
            perror("Mapping Failed : BRAM_2\n");
            close(fd);
        }
        else if(bram_addr_2 != MAP_FAILED){
            perror("Mapping Failed : BRAM_1\n");
            close(fd_2);
        }
        else{
            perror("Mapping Failed : Both\n");
        }
        exit(1);
    }
    printf("Mapping Successful\n");
    start_1 = clock();
    memset(bram_addr_1,0,LEN);
    end_1 = clock();
    printf("First Phase takes %f seconds\n",(double)(end_1 - start_1)/CLOCKS_PER_SEC);
    sigemptyset(&set);
    sigaddset(&set,SIGUSR1);
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigaction(SIGUSR1,&act,NULL);
    scanf("%c",&input);
    start_2 = clock();
    write(fd_3,&pid,sizeof(pid));
    sigwait(&set,&signum);
    end_2 = clock();
    printf("Second Phase takes %f seconds\n",(double)(end_2 - start_2)/CLOCKS_PER_SEC);
    munmmap(bram_addr_1,LEN);
    munmmap(bram_addr_2,LEN);
    close(fd);
    close(fd_2);
    close(fd_3);
    return 0;
}