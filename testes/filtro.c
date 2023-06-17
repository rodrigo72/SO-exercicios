#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void) {
    int buf[128];
    int bytes_read = read(0, &buf, 128);
    if (bytes_read == -1) {
        write(1, "Erro\n", 5);
    } else {
        write(1, "Nice\n", 5);
    }

    return 0;
}