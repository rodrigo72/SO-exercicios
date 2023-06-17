#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100

int main (void) {

    int cmds_count = 0;

    char ***commands = malloc(MAX_COMMANDS * sizeof(char **));

    for (int i = 0; i < MAX_COMMANDS; i++) {
        commands[i] = malloc(sizeof(char *) * MAX_ARGS);
        for (int j = 0; j < MAX_ARGS; j++) {
            commands[i][j] = malloc(sizeof(char) * MAX_ARG_LENGTH);
            commands[i][j] = NULL;
        }
    }
    
    char *pipeline = strdup("echo \"This is a sample sentence. Another sentence follows.\" | tr ' ' '\n' | sort | uniq -c");

    for (int i = 0; pipeline != NULL && i < MAX_COMMANDS; i++) {
        char *cmd = strsep(&pipeline, "|");
        int len = strlen(cmd);
        if (cmd == NULL || len == 0) break;

        int k = 0;
        for (int j = 0; j < len && j < MAX_ARGS; j++) {

            while (cmd[k] && cmd[k] == ' ') k++;

            if (cmd[k] == '"') {
                int k2 = k + 1;
                while (k2 < len && cmd[k2] != '"') k2++;
                commands[i][j] = strndup(cmd + k + 1, k2 - k - 1);
                k = k2 + 1;
            } else if (cmd[k] == '\'') {
                int k2 = k + 1;
                while (k2 < len && cmd[k2] != '\'') k2++;
                commands[i][j] = strndup(cmd + k + 1, k2 - k - 1);
                k = k2 + 1;
            } else if (cmd[k] != '\0'){
                int k2 = k + 1;
                while (k2 < len && cmd[k2] != ' ') k2++;
                commands[i][j] = strndup(cmd + k, k2 - k);
                k = k2;
            }
        }
        cmds_count++;
    }
    
    int original_stdin = dup(0);
    int original_stdout = dup(1);

    int child_pid = fork();
    if (child_pid == 0) {

        int pipes[cmds_count-1][2];
        for (int i = 0; i < cmds_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        int *child_pids = malloc(sizeof(int) * cmds_count);
        for (int i = 0; i < cmds_count; i++) {
            child_pids[i] = -1;
        }

        for (int i = 0; i < cmds_count; i++) {
            int pid = fork();
            if (pid == 0) {
                if (i == 0) {
                    // first command => stdin = keyboard
                    close(pipes[0][0]);
                    dup2 (pipes[0][1], 1); // stdout = pipes[0][1]; stdin = keyboard
                    close(pipes[0][1]);
                } else if (cmds_count - 1 == i) {
                    // last command => stdout = screen
                    close(pipes[i-1][1]);
                    dup2 (pipes[i-1][0], 0); // stdin = extremo de leitura do pipe entre o comando anterior e o atual; stdout = screen
                    close(pipes[i-1][0]);
                } else {
                    // middle command => stdin e stdout = extremos de leitura "anterior" e de escrita "posterior" 
                    close(pipes[i-1][1]);
                    dup2 (pipes[i-1][0], 0); // stdin = extremo de leitura do pipe entre o último comando e o atual
                    close(pipes[i-1][0]);

                    close(pipes[i][0]);
                    dup2 (pipes[i][1], 1); // stdout = extremo de escrita do pipe do comando entre o comando atual e o próximo
                    close(pipes[i][1]);
                }

                // Close all pipe file descriptors in the [child process] (já não são necessários, pois já foram duplicados para o stdin e/ou stdout)
                for (int j = 0; j < cmds_count; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                if (execvp(commands[i][0], commands[i]) == -1) { // executar comando (utilizando a configuração do stdin/ stdout definida acima)
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            } else {
                child_pids[i] = pid;
            }
        }

        // Close all pipe file descriptors in the [parent process]
        for (int i = 0; i < cmds_count; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Wait for all childs
        for (int i = 0; i < cmds_count; i++) {
            int status;
            if (waitpid(child_pids[i], &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }

        exit(EXIT_SUCCESS);

    } else {

        int status;
        if (wait(&status) > 0 && WIFEXITED(status)) {

            dup2(original_stdin, STDIN_FILENO);
            dup2(original_stdout, STDOUT_FILENO);

            close(original_stdin);
            close(original_stdout);

            printf("\nEnd.\n");
        }
    }

    exit(EXIT_SUCCESS);
}