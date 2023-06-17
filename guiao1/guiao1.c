#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>

int my_cp (const char *from_path, const char *to_path) {

    int fd_1 = open(from_path, O_RDONLY);
    if (fd_1 == -1) {
        perror("[my_cp] error opening 'from_path'");
        close(fd_1);
        return -1;
    }

    int fd_2 = open(to_path, O_WRONLY);
    if (fd_2 == -1) {
        perror("[my_cp] error opening 'to_path'");
        close(fd_1);
        close(fd_2);
        return -1;
    }

    ssize_t bytes_read = 0;
    char buf[20];

    while ((bytes_read = read(fd_1, buf, 20)) > 0) {
        write(fd_2, buf, bytes_read);
    }

    close(fd_1);
    close(fd_2);

    return 0;
}

int my_cat (const char *path) {

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("[my_cat] error opening 'path'");
        close(fd);
        return -1;
    }

    ssize_t bytes_read = 0;
    char buf[20];

    // 0 -> standard input
    // 1 -> standard output
    // 2 -> standard error

    while ((bytes_read = read(fd, buf, 20)) > 0) {
        write(1, buf, bytes_read);
    }

    close(fd);
    return 0;
}

// ssize_t : size of a buffer
//           or number of bytes that have been read or written

ssize_t my_readln (int fd, char *line, size_t size) {

    if (fd < 0) {
        perror("[my_readln] error opening 'fd'");
        return -1;
    }

    ssize_t i = 0;
    while (i < size) {
        ssize_t bytes_read = read(fd, &line[i], 1);
        if (bytes_read == -1) break;
        if (line[i] == '\n') {
            break;
        }
        i++;
    }

    return i;
}

ssize_t my_readln_2 (int fd, char *line, ssize_t size) {

    if (fd < 0) {
        perror("[my_readln] error opening 'fd'");
        return -1;
    }

    int next_pos   = 0;
    int read_bytes = 0;

    while (next_pos < size && read(fd, line + next_pos, 1) > 0) {
        read_bytes++;
        if (line[next_pos] == '\n') break;
        next_pos++;
    }

    return read_bytes;
}

int my_nl (const char *path) {

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("[my_nl] error opening 'path'");
        close(fd);
        return -1;
    }

    char line[40], result[10];
    int bytes_read = 0, count = 1;

    while ((bytes_read = my_readln_2(fd, line, 40)) > 0) {
        if (line[0] == '\n') {
            write(1, line, bytes_read);
        } else {
            sprintf(result, "     %d  ", count);
            write(1, result, 9);
            write(1, line, bytes_read);
            count++;
        }
    }

    close(fd);
    return 0;
}

ssize_t my_readln_stdin (char *line, ssize_t size) {

    ssize_t i = 0;
    ssize_t bytes_read = 0;
    char ch;

    while (i < size -1) {
        bytes_read = read(0, &ch, 1);
        if (bytes_read == -1) break;
        if (ch == '\n') break;
        line[i] = ch;
        i++;
    }

    line[i] = '\0';
    return i;
}

int my_nl_stdin () {
    
    int count = 1;
    char line[40], result[10];
    ssize_t bytes_read = 0;
    while ((bytes_read = my_readln_stdin(line, 40)) > 0) {
        if (line[0] == '\n' || line[0] == '\0') {
            write(1, line, bytes_read);
        } else {
            sprintf(result, "     %d  ", count);
            write(1, result, 9);
            write(1, line, bytes_read);
            write(1, "\n", 1);
            count++;
        }
    }

    return 0;
} 

int main (void) {

    my_cp("input.txt", "output.txt");
    my_cat("output.txt");
    printf("\n\n");

    int fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("[main] error opening 'fd'");
        close(fd);
        return -1;
    }

    char line[40];
    my_readln(fd, line, 40);
    printf("readln (1): %s", line);

    char line2[40];
    my_readln_2(fd, line2, 40); // irá ler a proxima linha, o file descriptor é o mesmo, portanto já existe um offset
    printf("readln (2): %s", line2);

    close(fd);
    printf("\n");

    my_nl("input.txt");
    printf("\n");

    char line3[40];
    my_nl_stdin();

    return 0;
}