#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DEFAUT 2

// exec
// v => takes an array of arguments
// e => array of environment variables 
// p => searches for executable file in the path
// l => takes arguments as separate elements 

// caminhos para imagens
void defeitos (int n, const char *imagens[n], int max) {

    int checked = 0;
    char detectAnonPath[] = "./detectAnon";
    pid_t child_pids[n];
    int child_status[n];

    for (int i = 0; i < n; i++) {
        child_pids[i] = -1;
        child_status[i] = 0;
    }

    for (int mult = 1; checked < n; mult++) {
        for (int i = checked; i < n && i < max * mult; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                char *args[] = { detectAnonPath, strdup(imagens[i]), NULL};
                if (execve(args[0], args, NULL) == -1) {
                    perror("execve");
                    _exit(EXIT_FAILURE);
                }
            } else if (pid > 0) {
                child_pids[checked] = pid;
                checked++;
            } else {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = max * (mult - 1); i < checked; i++) {
            int status;
            pid_t terminated_pid = waitpid(child_pids[i], &status, 0);
            if (terminated_pid == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            } else {
                child_status[i] = WIFEXITED(status);
            }
        }
    }
}

void conta (int n, const char *imagens[n]) {

    int count = 0, status;
    int original_stdin = dup(0);
    int original_stdout = dup(1);

    int p[2];
    if (pipe(p) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid_1 = fork();
    if (pid_1 == 0) {

        close(p[0]); // fechar extremo de leitura
        dup2 (p[1], 1);
        close(p[1]);

        defeitos(n, imagens, MAX_DEFAUT);
        _exit(EXIT_SUCCESS);
    }

    // melhor não usar um wait aqui

    int pid_2 = fork();
    if (pid_2 == 0) {

        close(p[1]); // fechar extremo de escrita
        dup2 (p[0], 0);
        close(p[0]);
        
        execlp("wc", "wc", "-l", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    close(p[0]);
    close(p[1]);

    dup2(original_stdin,  0);
    dup2(original_stdout, 1);

    close(original_stdin);
    close(original_stdout);

    wait(NULL);
    wait(NULL);
}

int main (void) {
    const char *imagens[] = 
                    {  
                        "imagem1.png", 
                        "imagem2.jpg", 
                        "imagem3.jpg", 
                        "imagem4.gif",
                        "imagem5.png",
                        "imagem6.jpg",
                        "imagem7.jpg",
                    };

    int n = sizeof(imagens) / sizeof(imagens[0]);
    conta(n, imagens);
}