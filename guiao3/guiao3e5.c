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
        sep[i] = malloc(sizeof(char) * (strlen(arg) + 1));
        strncpy(sep[i], arg, strlen(arg));
    }

    free(aux);

    int pid = fork(), r = 1;
    if (pid == 0) {
        int res = execvp(sep[0], sep);
        printf("Did not execute command (%d).\n", res); r = -1;
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            printf("Error.\n"); r = -1;
        }
    }

    for (int i = 0; i < MAX_ARGS; i++) free(sep[i]);
    free(sep);

    return r;
}

int main (void) {

    char buf[10];
    int bytes_read = 0;
    char *acc = NULL;
    
    while ((bytes_read = read(0, buf, 10)) > 0) {
        
        if (acc == NULL) {
            acc = malloc(sizeof(char) * (bytes_read + 1));
            strncpy(acc, buf, bytes_read);
            acc[bytes_read] = '\0';
        } else {
            int len = strlen(acc);
            acc = realloc(acc, (len +  bytes_read +  1) * sizeof(char));
            strncat(acc, buf, bytes_read);
            acc[len + bytes_read] = '\0';
        }

        if (buf[bytes_read - 1] == '\n') {
            acc[strlen(acc)-1] = '\0';
            printf("[%s]\n", acc);
            
            if (buf[bytes_read - 2] == '&') {
                acc[strlen(acc)-2] = '\0';
                int pid = fork();
                if (pid == 0) {
                    my_system(acc);
                    _exit(0);
                }
            } else {
                int pid = fork();
                if (pid == 0) {
                    my_system(acc);
                    _exit(0);
                } else {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }

            free(acc);
            acc = NULL;
        }
    }
}