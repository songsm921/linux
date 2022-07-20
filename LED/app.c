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
    int menu;
    char read_result;
    char status[8];
    int i;
    
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
                read(fd,&read_result,sizeof(char));
                int res = (int)read_result;
        if(!res){
            printf("Current LED is OFF\n");
        }
        else{
            for(i = 7; i>=0;i--){
                if(res%2)
                    status[i] = 'O';
                else
                    status[i] = 'X';
                res = res / 2;
            }
            for(i = 0 ; i<8;i++)
                printf("%c",status[i]);
            printf(" Current LED is ON\n");
            }
            break;
            case 2:
                printf("input number : ");
                scanf("%d",&val);
                write(fd, &val, sizeof(int));
                break;
            default:
                break;
            }
    }
    close(fd);
    
    return 0;
}
