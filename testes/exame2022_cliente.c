#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

int main (void) {

    int fd_pipe = open(PIPE_PATH, O_WRONLY);
    if (fd_pipe == -1) {
        perror("open");
        exit(1);
    }

    Pedido p;
    strcpy(p.filtro, "./filtro");
    strcpy(p.entrada, "entrada.txt");
    strcpy(p.saida, "saida.txt");

    int written_bytes = write(fd_pipe, &p, sizeof(struct pedido));
    if (written_bytes == -1) {
        perror("write");
        exit(1);
    }

    close(fd_pipe);
    exit(0);
}