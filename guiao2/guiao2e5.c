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
#define COLUMNS 10000

int main (void) {

    srand(time(NULL));
	int matrix[ROWS][COLUMNS];
	printf("Generating random matrix...\n");
	for(size_t i = 0; i < ROWS; i++)
		for(size_t j = 0; j < COLUMNS; j++)
			matrix[i][j] = rand() % COLUMNS;
    printf("Random matrix generated.\n");

	int num = rand() % COLUMNS;

    for (size_t i = 0; i < ROWS; i++) {
        int res = fork();
        if (res == 0) {
            for (size_t j = 0; j < COLUMNS; j++)
                if (matrix[i][j] == num)
                    _exit(i+1);
            _exit(0);
        }
    }

    int status, pid;
    while ((pid = wait(&status)) != -1) {
        if (WIFEXITED(status)) {
            printf("[PAI] Filho (%d) devolveu: %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("Erro.\n");
        }
    }

    return 0;
}