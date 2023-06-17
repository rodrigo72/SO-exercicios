#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_PATH "text.txt"
#define ERROS_PATH "erros.txt"
#define OUTPUT_PATH "saida.txt"

int main (void) {

    // dup2 : cria uma cópia de f1 no f2, devolve f2

    int fd_input = open(INPUT_PATH, O_RDONLY);
    if (fd_input == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int fd_errors = open(ERROS_PATH, O_WRONLY | O_CREAT | O_APPEND);
    if (fd_errors == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int fd_output = open(OUTPUT_PATH, O_WRONLY | O_CREAT | O_APPEND);
    if (fd_output == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int stdin;
    if ((stdin = dup2(fd_input, 0)) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    int stderror;
    if ((stderror = dup2(fd_errors, 2)) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    int stdout;
    if ((stdout = dup2(fd_output, 1)) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(fd_input);
    close(fd_errors);
    close(fd_output);

    char msg[] = "Terminei.\n";
    write(stdout, &msg, sizeof(msg) - 1);

    int pid = fork();
    if (pid == 0) {
        execlp("wc", "wc", NULL);
        write(stdout, "Correu mal.\n", 12);
        _exit(1);
    } else {
        int status;
        wait(&status);
    }

    return 0;
}