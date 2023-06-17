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

#define FIFO_PATH "myfifo"
#define INTERNAL_FIFO_PATH "internal_fifo"
#define LOG_PATH "logs.txt"

int main (void) {

    // if needed
    unlink(FIFO_PATH);

    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    unlink(INTERNAL_FIFO_PATH);

    if (mkfifo(INTERNAL_FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    int fd_logs = open(LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (fd_logs == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // FIFO (READ)
    int fd_fifo_r = open(FIFO_PATH, O_RDONLY);
    if (fd_fifo_r == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // FIFO (WRITE)
    // assim, o server não entra em loop quando não existem clientes
    int fd_fifo_w = open(FIFO_PATH, O_WRONLY);
    if (fd_fifo_w == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int bytes_read = 0;

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    while (1) {
        HEADER header;
        bytes_read = read(fd_fifo_r, &header, sizeof(HEADER));
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        if (header.type == NEW) {

            printf("Recieved header with request type NEW.\n");

            int pid = fork();
            if (pid == 0) {
                char *fifo_name = malloc(sizeof(char) * 64);
                sprintf(fifo_name, "tmp/%d.fifo", header.pid);

                int private_fifo_fd = open(fifo_name, O_RDONLY);
                if (private_fifo_fd == -1) {
                    perror("[private_fifo_fd] open");
                    _exit(EXIT_FAILURE);
                }

                char send[] = "Connected to private fifo.\n";
                write(0, &send, sizeof(send));

                HEADER_PRIVATE header_private;
                while ((bytes_read = read(private_fifo_fd, &header_private, sizeof(HEADER_PRIVATE))) > 0) {
                    if (header_private.type == STRING) {
                        char string[header_private.size];
                        bytes_read = read(private_fifo_fd, &string, header_private.size);
                        if (bytes_read == -1) break;

                        lock.l_type = F_WRLCK;
                        fcntl(fd_logs, F_SETLKW, &lock);

                        write(fd_logs, &string, header_private.size);

                        lock.l_type = F_UNLCK;
                        fcntl(fd_logs, F_SETLKW, &lock);

                        write(0, "Wrote to logs.\n", 16);
                    }
                } 

                _exit(0);
            }
        }

    }

    close(fd_fifo_r);
    close(fd_fifo_w);

    if (unlink(FIFO_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}