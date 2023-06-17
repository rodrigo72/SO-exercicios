#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>

int main (void) {

    int res = fork();
    if (res == 0) {
        // FILHO
        printf("Sou o filho e o meu pid é %d | Parent id: %d\n", getpid(), getppid());
        sleep(1);
        _exit(32); // numero entre 0 e 255 (1000 => devolve depois 232 devido a overflow)
    } else {
        // PAI
        int status;
        // wait(&status);
        waitpid(res, &status, 0);
        if (WIFEXITED(status)) {
            printf("-----------\n");
            printf("Sou o pai (%d) e o filho (%d) devolveu %d.\n", getpid(), res, WEXITSTATUS(status));
        } else {
            printf("Erro.\n");
        }

    }

    // sequencialmente
    for (int i = 1; i <= 10; i++) {
        int pid = fork();
        if (pid == 0) {
            printf("[FILHO] PID: %d; PID-pai: %d\n", getpid(), getppid());
            sleep(1);
            _exit(i);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                printf("[PAI] Filho devolveu: %d\n", WEXITSTATUS(status));
            } else {
                printf("Erro.\n");
            }
        }
    }

    // em concorrencia:
    for (int i = 0; i < 10; i++) {
        int res = fork();
        if (res == 0) {
            printf("[FILHO] PID: %d; PID-pai: %d\n", getpid(), getppid());
            sleep(1);
            _exit(i);
        }
    }

    int status;
    while (wait(&status) != -1) {
        if (WIFEXITED(status)) {
            printf("[PAI] Filho devolveu: %d\n", WEXITSTATUS(status));
        } else {
            printf("Erro.\n");
        }
    }

    return 0;
}