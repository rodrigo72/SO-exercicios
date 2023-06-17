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

    int result = unlink(FIFO_PATH);
    if (result == -1) {
        perror("unlink");
        return -1;
    }

    return 0;
}
