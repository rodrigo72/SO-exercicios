#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

    int p[2][2];
    for (int i = 0; i < 2; i++) {
        if (pipe(p[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }

    char **newArgv = malloc((argc - 1) * sizeof(char *));
    for (int i = 1; i < argc; i++) {
        newArgv[i - 1] = strdup(argv[i]);
    }

    int mem[10];
    for (int i = 0; i < 10; i++) {

        pid_t pid_1 = fork();
        if (pid_1 == 0) {
            execvp(newArgv[0], newArgv);
            perror("execvp");
            _exit(1);
        }

        int status;
        if (waitpid(pid_1, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        printf("Program executed.\n");

        if (!WIFEXITED(status)) {
            printf("Child returned 1 (error).\n");
            return 1;
        }

        pid_t pid_2 = fork();
        if (pid_2 == 0) {
            close(p[0][0]);
            dup2 (p[0][1], 1);
            close(p[0][1]);

            close(p[1][1]);
            close(p[1][0]);

            char path[100];
            sprintf(path, "/proc/%d/maps", pid_1);
            execlp("grep", "grep", "VmPeak", path, NULL);
            _exit(1);
        }

        pid_t pid_3 = fork();
        if (pid_3 == 0) {
            close(p[0][1]);
            dup2 (p[0][0], 0); // stdin = extremo de escrita do pipe anterior
            close(p[0][0]);

            close(p[1][0]);
            dup2 (p[1][1], 1);
            close(p[1][1]);

            execlp("cut", "cut", "-d", " ", "-f4", NULL);
            _exit(1);
        }

        waitpid(pid_3, &status, 0);

        close(p[0][0]);
        close(p[0][1]);
        close(p[1][1]);

        char buf[64];
        int bytes_read = read(p[1][0], &buf, 64);
        close(p[1][0]);
        if (bytes_read == -1) {
            perror("read");
            return 1;
        }

        mem[i] = atoi(buf);
    }

    int min, max, avg;
    avg = max = min = mem[0]; 

    for (int i = 1; i < 10; i++) {
        if (mem[i] < min) min = mem[i];
        if (mem[i] > max) max = mem[i];
        avg += mem[i];
    }
    avg = avg / 10;

    printf("Average: %d\n", avg);
    printf("Min: %d\n", min);
    printf("Max: %d\n", max);

    return 0;
}