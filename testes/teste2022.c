#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// servidor guarda mensagens dos clientes em diferentes ficheiros de texto
// nome do ficheiro = nome do autor
// linhas do ficheiro = uma mensagem por linha

// devolve um inteiro com o número de mensagens em que uma determinada palavra ocorre num determinado ficheiro
// utiliza os comandos grep e wc (grep palavra ficheiro | wc -l)

int messages (char *word, char *file) {

    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int p[2][2];
    for (int i = 0; i < 2; i++) {
        if (pipe(p[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    pid_t pid_1 = fork();
    if (pid_1 == 0) {
        close(p[0][0]);
        dup2 (p[0][1], 1);
        close(p[0][1]);

        close(p[1][0]);
        close(p[1][1]);

        char *args[] = { "grep", word, file, NULL };
        execvp(args[0], args);
        _exit(1);
    } else {
        pid_t pid_2 = fork();
        if (pid_2 == 0) {
            close(p[0][1]);
            dup2 (p[0][0], 0);
            close(p[0][0]);

            close(p[1][0]);
            dup2 (p[1][1], 1);
            close(p[1][1]);

            execlp("wc", "wc", "-l", NULL);
            _exit(1);
        } else {
            int status;
            if (waitpid(pid_1, &status, 0) == -1) {
                perror("waitpid");
                return -1;
            }
            close(p[0][0]);
            close(p[1][1]);
            close(p[0][1]); // necessário fechar o extremo de escrita do 
                            // primeiro pipe de modo a que o wc receba um EOF !
                            // assim a leitura não bloqueia
            char c;
            if (read(p[1][0], &c, sizeof(char)) == -1) {
                perror("read");
                return -1;
            }
            if (waitpid(pid_2, &status, 0) == -1) {
                perror("waitpid");
            }
            printf("result: %c\n", c);
            return c - '0';
        }
    }
    return -1;
}

int authors_who_used_word (char *word, int n, char *authors[n]) {
    int p[2];
    if (pipe(p) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t child_pids[n];
    for (int i = 0; i < n; i++) child_pids[i] = -1;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            close(p[0]);
            dup2 (p[1], 1);
            close(p[1]);

            int n = messages(word, authors[i]);
            write(1, &n, sizeof(int));
            _exit(0);
        } else if (pid > 0) {
            child_pids[i] = pid;
        } else {
            perror("fork");
            return -1;
        }
    }

    close(p[1]);
    int bytes_read = 0, result = -1, count = 0;
    for (int i = 0; i < n; i++) {
        int status;
        if (waitpid(child_pids[i], &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
        if (!WIFEXITED(status)) {
            perror("status");
        } else {
            int bytes_read = read(p[0], &result, sizeof(int));
            if (bytes_read > 0 && result > 0) 
                count++; 
        }
    }
    close(p[0]);
    return count;
}

int main (void) {
    char word[] = "palavra";
    char file[] = "joao.txt";
    char *authors[] = 
                    {
                        "artur.txt",
                        "joao.txt",
                        "maria.txt"
                    };
                    
    int n = authors_who_used_word(word, 3, authors);
    printf("%d\n", n);
    return 0;
}