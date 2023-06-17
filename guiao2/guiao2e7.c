#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <time.h>

#define ROWS 10
#define COLUMNS 10

#define FILENAME "file_matrix"

int change_matrix_value (int linha, int coluna, int new_value) {
    int fd = open(FILENAME, O_RDWR, 0644);
    if (fd == -1) return -1;

    if (linha > 0 && coluna > 0 && linha <= ROWS && coluna <= COLUMNS) {
        off_t x = lseek(fd, ((linha - 1) * COLUMNS + (coluna - 1)) * sizeof(int) , SEEK_SET);
        write(fd, &new_value, sizeof(int));
        printf("Value in (%d, %d) changed to %d\n", linha, coluna, new_value);
    }
    close(fd);
    return 0;
}

void print_matrix_from_file () {
    int fd = open(FILENAME, O_RDWR, 0644);
    if (fd == -1) return;

    int bytes_read = 0, number, coluna = 0;
    while ((bytes_read = read(fd, &number, sizeof(int))) > 0) {
        if (coluna < COLUMNS - 1) {
            printf("%d ", number);
            coluna++;
        } else {
            printf("%d\n", number);
            coluna = 0;
        }
    }
}

int main (int argc, char const *argv[]) {

    int fd = open(FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (lseek(fd, 0, SEEK_END) <= 1) {

        srand(time(NULL));
        int matrix[ROWS][COLUMNS];
        printf("Generating random matrix...\n");
        for(size_t i = 0; i < ROWS; i++)
            for(size_t j = 0; j < COLUMNS; j++)
                matrix[i][j] = rand() % COLUMNS;
        printf("Random matrix generated.\n");

        write(fd, &matrix, sizeof(matrix));

        printf("Matrix file created.\n\n");
    } else {
        printf("Matrix file found.\n\n");
    }

    close(fd);

    print_matrix_from_file();
    printf("\n");
    change_matrix_value(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    printf("\n");
    print_matrix_from_file();

    return 0;
}