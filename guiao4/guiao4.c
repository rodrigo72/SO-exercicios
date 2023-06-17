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

void pai_to_filho (int n) {
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {

        close(p[1]); // fechar extremo de escrita no processo filho

        int recebido;
        read(p[0], &recebido, sizeof(int));
        printf("Sou o filho e recebi: %d\n", recebido);

        close(p[0]); // fechar extremo de leitura no processo filho
        _exit(0);
    } else {

        close(p[0]); // fechar extremo de leitura no processo pai

        int enviado = n;
        // sleep(5);
        write(p[1], &enviado, sizeof(int));

        close(p[1]); // fechar extremo de escrita no processo pai
        
        int status;
        wait(&status);
    }
}

void filho_to_pai (int n) {
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        close(p[0]);

        int enviado = n;
        write(p[1], &enviado, sizeof(int));

        close(p[1]);
        _exit(0);
    } else {
        close(p[1]);
        
        wait(NULL);

        int recebido;
        read(p[0], &recebido, sizeof(int));
        printf("Sou o pai e recebi: %d\n", recebido);

        close(p[0]);
    }
}

void filho_to_pai_array (int n[], int size) {
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        close(p[0]);

        for (int i = 0; i < size; i++) {
            write(p[1], &n[i], sizeof(int));
            usleep(250000);
        }

        close(p[1]);
        _exit(0);
    } else {
        close(p[1]);
        int recebido;
        int bytes_read = 0;

        while ((bytes_read = read(p[0], &recebido, sizeof(int))) > 0) {
            printf("Sou o pai e recebi: %d\n", recebido);
        }
        
        close(p[0]);
    }
}

int procura_ocorr (int matrix[ROWS][COLUMNS], int value, int vetor[]) {

    int p[2];
    pipe(p);

    for (int i = 0; i < ROWS; i++) {
        int pid = fork();
        if (pid == 0) {
            
            close(p[0]); // extremo de leitura

            for (int j = 0; j < COLUMNS; j++) {
                if (matrix[i][j] == value)
                    write(p[1], &matrix[i][j], sizeof(int));
            }

            close(p[1]);
            _exit(0);
        }
    }

    close(p[1]); // extremo de escrita

    int bytes_read;
    int recebido;
    int i = 0;

    while ((bytes_read = read(p[0], &recebido, sizeof(int))) > 0) {
        vetor[i] = recebido;
        i++;
    }

    close(p[0]);
    return i;
}

int main (void) {

    pai_to_filho(3);
    filho_to_pai(5);
    printf("\n");

    int n[] = {1, 2, 3, 4, 5};
    filho_to_pai_array(n, 5);
    printf("\n");

    srand(time(NULL));
	int matrix[ROWS][COLUMNS];
	printf("Generating random matrix...\n");
	for(size_t i = 0; i < ROWS; i++) {
		for(size_t j = 0; j < COLUMNS; j++) {
			matrix[i][j] = rand() % COLUMNS;
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("Random matrix generated.\n");

	int num = rand() % COLUMNS;
    int vetor[100];

    int k = procura_ocorr(matrix, num , vetor);    
    for (int i = 0; i < k; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

    return 0;
}