#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#define MAX_SIZE 33

int main()
{
    int fd = 0, ret = 0;
    char buff[MAX_SIZE] = "";

    fd = open("/dev/hung_chrdev", O_RDONLY);

    printf("Character devicle file descriptor :%d\n", fd);

    ret = read(fd, buff, 8);
    buff[ret] = '\0';

    printf("Random Integer: %s\nLength: %d bytes\n", buff, ret);
    close(fd);
    return 0;
}
