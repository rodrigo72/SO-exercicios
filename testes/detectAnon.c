#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINES 4
#define MAX_LINE_LENGTH 40

const char *lines[] = {
    "erro 1",
    "erro 2",
    "erro 3",
    "erro 4",
    "erro 5",
    "erro 6",
    "erro 7",
    "erro 8",
    "erro 9",
    "erro 10"
};

int string_hash (const char *str) {
    int hash = 0;
    int c;

    while ((c = *str++)) {
        hash = (hash * 31) + c;
    }

    return hash;
}

void getRandomLines (const char seed[]) {
    srand(string_hash(seed));
    int numLines = rand() % MAX_LINES + 1;
    for (int i = 0; i < numLines; i++) {
        int randomIndex = rand() % (sizeof(lines) / sizeof(lines[0]));
        printf("%s\n", lines[randomIndex]);
    }
    printf("\n");
}

int main (int argc, char const *argv[]) {

    if (argc > 1 && argv[1]) {
        getRandomLines(argv[1]);
    }

    return 0;
}