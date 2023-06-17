#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CTRL + D, para ver os resultados do comando wc
int main (void) {

    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        close(p[1]);

        dup2(p[0], 0);

        execlp("wc", "wc", NULL);

        close(p[0]);
        _exit(1);
    } else {
        close(p[0]); // fechar extremo de leitura
        
        char buf[50];
        int bytes_read = 0;
        while ((bytes_read = read(0, &buf, 50)) > 0) {
            write(p[1], &buf, bytes_read);
        }

        close(p[1]);
        wait(NULL);
    }

    return 0;
}