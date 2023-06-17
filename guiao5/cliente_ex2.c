#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>
#include "utils.h"

#define FIFO_PATH "myfifo"
#define MAX_RETRIES 10
#define MAX_BACKOFF_TIME 1000 // in milliseconds

int main (void) {

    srand(time(NULL));

    int fd, retries = 0, backoffTime = 0;
    int bytes_read = 0;
    char buf[50];

    do {
        fd = open(FIFO_PATH, O_WRONLY, 0200);
        if (fd == -1) {
            perror("open");
            
            retries++;
            if (retries >= MAX_RETRIES) {
                printf("Failed to open file descriptor after maximum retries.\n");
                break;
            }

            backoffTime = (1 << retries) * 1000; // exponential backoff time (tipo RC)

            int jitter = rand() % (backoffTime / 2);
            backoffTime += jitter;

            if (backoffTime > MAX_BACKOFF_TIME) {
                backoffTime = MAX_BACKOFF_TIME;
            }

            printf("Retrying after %d milliseconds.\n", backoffTime);
            usleep(backoffTime * 1000);

        }
    } while (fd == -1);

    if (fd != -1) {

        int pid = getpid();
        char *fifo_name = malloc(sizeof(char) * 64);
        sprintf(fifo_name, "tmp/%d.fifo", pid);

        if (mkfifo(fifo_name, 0666) == -1) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }

        HEADER header;
        header.type = NEW;
        header.pid = pid;
        header.size = sizeof(HEADER_PRIVATE);

        write(fd, &header, sizeof(HEADER));
        printf("Sent header to main FIFO.\n");

        int private_fifo_fd = open(fifo_name, O_WRONLY);
        if (private_fifo_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        printf("Connected to private fifo.\n");

        int bytes_read = 0;
        char buf[100];
        while ((bytes_read = read(0, &buf, 100)) > 0) {
            
            HEADER_PRIVATE hp;
            hp.size = bytes_read;
            hp.type = STRING;

            buf[bytes_read] = '\0';
            char *str = malloc(sizeof(char) * (bytes_read + 1));
            strncpy(str, buf, bytes_read);

            printf("bytes read: %d\n", bytes_read);

            write(private_fifo_fd, &hp, sizeof(HEADER_PRIVATE)); // informaçao com tamanho da proxima string
            printf("Sent private header.\n");
            write(private_fifo_fd, str, bytes_read); // string
            printf("Sent string.\n");
        }

        close(fd);
    }

    exit(EXIT_SUCCESS);
}