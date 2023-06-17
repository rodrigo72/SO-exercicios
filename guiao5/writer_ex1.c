#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIFO_PATH "myfifo"

int main (void) {

    int fd = open(FIFO_PATH, O_WRONLY, 0240);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buf[50];
    int bytes_read = 0;
    while((bytes_read = read(0, &buf, 50)) > 0) {
        write(fd, &buf, 50);
    }

    close(fd);
}