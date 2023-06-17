#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main (void) {

    char path[] = "./input.txt";
    char palavra[] = "palavra";

    int p[2];
    pipe(p);

    int fd = open(path, O_RDONLY, 0400);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    int buffer_length = 1024;
    char buf[buffer_length];
    ssize_t total_bytes_read = 0;
    ssize_t bytes_read = 0;
    char ch;

    int end = 0;
    while (!end) {

        while (total_bytes_read < buffer_length - 2) {
            bytes_read = read(fd, &ch, 1);
            if (bytes_read == -1) {
                perror("read");
                return -1;
            }

            if (bytes_read == 0) {
                end = 1;
                break;
            }

            buf[total_bytes_read++] = ch;

            if (ch == '\n') {
                buf[total_bytes_read] = '\0';
                
                int pid = fork();
                if (pid == 0) {
                    close(p[0]);

                    if (strstr(buf, "palavra") != NULL) {
                        write(p[1], &buf, buffer_length);
                    }

                    close(p[1]);
                    _exit(0);
                }

                total_bytes_read = 0;
            }
        }
    }

    close(p[1]);

    while ((bytes_read = read(p[0], &buf, buffer_length)) > 0) {
        printf("%s\n", buf);
    }

    close(p[0]);

    int status;
    while (wait(&status) != -1);

} 