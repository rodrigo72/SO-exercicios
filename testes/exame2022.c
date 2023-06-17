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

#define MAX_PEDIDOS 5
#define LOGS_PATH "comando.log"

// executa um programa "filtro" sem argumentos, que processa o ficheiro de entradam,
// e escreve o output no ficheiro de saida
int comando (const char *filtro, const char *entrada, const char *saida) {

    int fd_entrada = open(entrada, O_RDONLY);
    if (fd_entrada == -1) {
        perror("open");
        return -1;
    }

    int fd_saida = open(saida, O_WRONLY);
    if (fd_saida == -1) {
        perror("open");
        return -1;
    }

    int err_pipe[2];
    if (pipe(err_pipe) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(fd_entrada, 0);
        close(fd_entrada);

        dup2(fd_saida, 1);
        close(fd_saida);

        close(err_pipe[0]);
        dup2(err_pipe[1], 2);
        close(err_pipe[1]);

        char *aux = strdup(filtro);
        execlp(aux, aux, NULL);
        _exit(1);
    } else {

        close(err_pipe[1]);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }

        if (!WIFEXITED(status)) {
            printf("Erro.\n");
        }

        char buf[1];
        int bytes_read = read(err_pipe[0], &buf, 1);
        close(err_pipe[0]);
        if (bytes_read == -1) {
            perror("read");
            return -1;
        }

        close(fd_entrada);
        close(fd_saida);
        
        return bytes_read > 0;
    }

    return -1;
}

int main (void) {

    unlink(PIPE_PATH);

    if (mkfifo(PIPE_PATH, 0666) == -1) {
        perror("mkdfifo");
        exit(1);
    }

    int fd_pipe_r = open(PIPE_PATH, O_RDONLY);
    if (fd_pipe_r == -1) {
        perror("open");
        exit(1);
    }

    int fd_pipe_w = open(PIPE_PATH, O_WRONLY);
    if (fd_pipe_w == -1) {
        perror("open");
        exit(1);
    }

    int fd_logs = open(LOGS_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd_logs == -1) {
        perror("open");
        exit(1);
    }

    Pedido pedidos[MAX_PEDIDOS];
    int child_pids[MAX_PEDIDOS];
    int acc_pedidos = 0, bytes_read = 0;
    for (int i = 0; i < MAX_PEDIDOS; i++) child_pids[i] = -1;

    while (1) {
        printf("\nWaiting ...\n");
        char log_lines[MAX_PEDIDOS][1024] = {0};

        Pedido pedido;
        bytes_read = read(fd_pipe_r, &pedido, sizeof (struct pedido));
        if (bytes_read == -1) {
            perror("read");
            continue;
        }

        pedidos[acc_pedidos] = pedido;
        printf("Pedido recebido.\n");

        if (acc_pedidos == MAX_PEDIDOS - 1) {
            printf("Limite máximo atingido.\n");
            for (int i = 0; i <= acc_pedidos; i++) {

                sprintf(log_lines[i], "%s; %s; %s\n", pedidos[i].filtro, pedidos[i].entrada, pedidos[i].saida);
                pid_t pid = fork();
                if (pid == 0) {
                    if (!comando(pedidos[i].filtro, pedidos[i].entrada, pedidos[i].saida)) {
                        write(fd_logs, log_lines[i], strlen(log_lines[i]));
                    }

                    close(fd_logs);
                    close(fd_pipe_r);
                    close(fd_pipe_w);
                    
                    _exit(0);
                } else if (pid > 0) {
                    child_pids[i] = pid;
                } else {
                    perror("fork");
                    continue;
                }
            }

            for (int i = 0; i < MAX_PEDIDOS; i++) {
                int status;
                if (waitpid(child_pids[i], &status, 0) == -1) {
                    perror("waitpid");
                } else if (!WIFEXITED(status)) { // devolveu 1 => erro
                    printf("The child process did not exit normally.\n");
                }
            }

            acc_pedidos = 0;
        } else {
            acc_pedidos++;
        }
    }

    close(fd_pipe_r);
    close(fd_pipe_w);
    close(fd_logs);

    unlink(PIPE_PATH);

    exit(0);
}