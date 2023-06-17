#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void) {

    int pid = fork();
    if (pid == 0) {
        execlp("ls", "ls", "-l", NULL);
        printf("Correu mal.\n");
        _exit(1);
    } else {
        int status;
        wait(&status);
        printf("Terminou: %d\n", WEXITSTATUS(status));
    }

    return 0;
}