#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int completed = 0;
int input = 0;
void* thread_func(void* arg){
    printf("Thread %d will be sleeping \n", (int)arg);
    while(input == 0){
        printf("waiting for input... %d\n",input);
        sleep(1);
    }
    completed = 1;
    printf("Thread %d has completed\n", (int)arg);
}

int main(void){
    pthread_t thread;
    int a = 1;
    int thread_id;
    thread_id = pthread_create(&thread, NULL, thread_func, (void*)a);
    if(thread_id < 0){
        perror("Thread creation failed");
        exit(0);
    }
    scanf("%d", &input);
    while(!completed){
        printf("Waiting for polling completion... %d\n",input);
        sleep(1);
    }
    printf("The input is %d\n", input);
    return 0;
}