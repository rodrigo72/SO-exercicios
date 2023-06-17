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

    int result = mkfifo(FIFO_PATH, 0666);
    if (result == -1) {
        perror("mkfifo");
        return -1;
    }

    return 0;
}
