#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 10

int my_system (const char *string) {

    char **sep = malloc(sizeof(char *) * MAX_ARGS);
    memset(sep, 0, sizeof(char *) * MAX_ARGS);

    char *aux = strdup(string);

    for (int i = 0; aux != NULL && i < MAX_ARGS; i++) {
        char *arg = strsep(&aux, " ");
        sep[i] = malloc(sizeof(char) * strlen(arg));
        strncpy(sep[i], arg, strlen(arg));
    }

    int pid = fork(), r = 1;
    if (pid == 0) {
        int res = execvp(sep[0], sep);
        printf("Did not execute command (%d).\n", res); r = -1;
        _exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Returned: %d\n", WEXITSTATUS(status));
        } else {
            printf("Error.\n"); r = -1;
        }
    }

    for (int i = 0; i < MAX_ARGS; i++) free(sep[i]);
    free(sep);

    return r;
}

int main (void) {

    my_system("ls -a\0");

    return 0;
}