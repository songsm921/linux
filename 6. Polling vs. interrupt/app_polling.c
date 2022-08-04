#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#define BASE_ADDRESS 0xA0000000
#define BASE_ADDRESS_2 0xA0002000
#define LEN 8
static int condition = 0;
void* thread_func(void* arg){
    int res;
    do
    {
        sleep(1);
        read((int)arg,&res,sizeof(int));
    } while (res == 0);
    printf("Loop Escaped!\n");
    condition = 1;
}
int main(void){
    pthread_t thread;
    int tmp = 1;
    int thread_id;
    clock_t start1,end1,start2,end2;

    /*1st Step : Memcpy to BRAM*/
    int fd = open("/dev/mem", O_RDWR);
    int fd_2  = open("/dev/LED", O_RDWR);
    int fd_3 = open("/dev/mem",O_RDWR);
    char input;
    void* addr = mmap(0,LEN,PROT_READ | PROT_WRITE, MAP_SHARED, fd, BASE_ADDRESS);
    void* addr_2 = mmap(0,LEN,PROT_READ | PROT_WRITE, MAP_SHARED, fd_2, BASE_ADDRESS_2);
    if(addr == MAP_FAILED || addr_2 == MAP_FAILED){
        perror("mmap failed");
        close(fd);
        exit(0);
    }
    start1 = clock();
    memset(addr,0,LEN);
    end1 = clock();
    printf("1st step elapsed time is %ld\n",(end1-start1));
    /*2nd Step : Waiting User input...*/
    scanf("%c",&input);
    start2 = clock();
    /*3rd Step : Divide two thread*/
    thread_id = pthread_create(&thread, NULL, thread_func, (void*)fd_2);
    if(thread_id < 0){
        perror("Thread creation failed");
        munmap(addr,LEN);
        close(fd);
        exit(0);
    }
    write(fd_2,&tmp,sizeof(int));
    while(condition == 0)
    {
        printf("Waiting for polling completion...\n");
    }
    printf("Status is complete!\n");
    //memset(addr,0,LEN);
    memcpy(addr_2,addr,LEN);
    end2 = clock();
    printf("All process is completed!\n");
    printf("2nd step elapsed time is %ld\n",(end2-start2));
    munmap(addr,LEN);
    munmap(addr_2,LEN);
    close(fd);
    close(fd_2);
    close(fd_3);
    return 0;
}