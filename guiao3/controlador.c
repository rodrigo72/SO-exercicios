#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char const *argv[]) {

    // queria umar um MAP aqui mas C NAO TEM IMPLEMENTAÇAO DE MAP COMO ASSIMMMMM
    int end = 0;
    int programCount = argc - 1;
    int *execCount = malloc(sizeof(int) * programCount);
    memset(execCount, 0, sizeof(int) * programCount);
    int *programPids = malloc(sizeof(int) * programCount);
    memset(programPids, 0, sizeof(int) * programCount);

    while (!end) {
        for (int i = 1; i < argc; i++) {
            int pid = fork();
            if (pid == 0) {
                // char path[100];
                // strcat(path, "/bin/");
                // strcat(path, argv[i]);
                // printf("%s\n", path);
                execlp(argv[i], argv[i], NULL);
                _exit(EXIT_FAILURE);
            }
        }

        int status;
        pid_t childPID;
        int i = 0;

        end = 1;

        while ((childPID = wait(&status)) != -1) {
            if (WIFEXITED(status)) {
                printf("Child PID %d terminated with exit status: %d\n", childPID, WEXITSTATUS(status));
            } else {
                printf("Child PID %d terminated abnormally.\n", childPID);
                end = 0;
            }
            execCount[i]++;
            programPids[i] = childPID;
            i++;
        }

    }
    
    for (int i = 1; i < argc; i++) {
        printf("(%d) %s: %d\n", programPids[i - 1], argv[i], execCount[i - 1]);
    }

    free(execCount);

    return 0;
}