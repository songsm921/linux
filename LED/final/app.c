#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int fd;
    int val;
    int menu = -1;
    unsigned char read_result;
    char status[9];
    status[8] = '\0';
    int i;
    int tmp;
    fd = open("/dev/LED", O_RDWR);
    if(fd < 0)
    {
        printf("Unable to open file\n");
        printf("%d",errno);
        
        return 0;
    }
    while(menu != 3){
        printf("-----------------------------------\n");
        printf("\t\t menu \t\t \n");
        printf("-----------------------------------\n");
        printf("Select your op by choosing number : \n");
        printf("1. Read current LED status\n");
        printf("2. Assign LED value in decimal\n");
        printf("3. Terminate \n");
        printf("Enter your op: ");
        scanf("%d",&menu);
        switch(menu){
            case 1: 
                if((tmp = read(fd,&read_result,sizeof(char)) ) == -1)
                {
                    printf("Read size does not match with intended size!\n ");
                    printf("tmp = %d, read_result = %d",tmp,(int)read_result);
                    return -1;
                }
                
        if((int)read_result == 0){
            printf("Current LED is OFF\n");
        }
        else{
            for(i = 7; i>=0;i--){
                if((int)read_result%2)
                    status[i] = 'O';
                else
                    status[i] = 'X';
               read_result >>= 1;
            }
            printf("%s",status);
            printf(" Current LED is ON\n");
            }
            break;
            case 2:
                printf("input number : ");
                scanf("%d",&val);
                if(write(fd, &val, sizeof(char)) == -1)
                {
                    printf("Write size does not match with your intended! \n");
                    return -2;
                }
                break;
            default:
                break;
            }
    }
    close(fd);
    
    return 0;
}
