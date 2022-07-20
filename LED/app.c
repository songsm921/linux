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
    
    fd = open("/dev/LEDDriver", O_RDWR);
    if(fd < 0)
    {
        printf("Unable to open file\n");
        printf("%d",errno);
        
        return 0;
    }
    
    if(argc != 2)
    {
        printf("Only 1 parameter can inputted\n");
        printf("You input %d parameters\n", argc-1);
        
        return 0;
    }
    
    val = atoi(argv[1]);
    
    printf("input number : %d\n", val);
    
    write(fd, &val, sizeof(int));
    
    close(fd);
    
    return 0;
}