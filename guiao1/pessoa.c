#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h>  /* O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILENAME "file_pessoas"

typedef struct Person {
    char name[200];
    char age;
} Person;

int new_person (const char *name, int age) {

    // S_IWUSR == 0200 (write permission for the owner)
    // S_IRUSR == 0400 (read permission for the owner)
    // (S_IRUSR | S_IWUSR) == 0600
    
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd == -1) {
        perror("[new_person] error opening 'FILENAME'");
        close(fd);
        return -1;
    }

    Person person;
    person.age = age;
    strcpy(person.name, name);

    write(fd, &person, sizeof(struct Person));

    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Registo %d\n", pos / sizeof(struct Person));

    close(fd);
    return pos / sizeof(struct Person);
}

int person_change_age (const char *name, int new_age) {

    // 6: read and write permissions for the owner
    int fd = open(FILENAME, O_RDWR, 0640); // 4: read permissions for the group
    if (fd == -1) {
        perror("[person_change_age] error opening 'FILENAME'");
        close(fd);
        return -1;
    }

    Person person;
    ssize_t bytes_read = 0;
    while ((bytes_read = read(fd, &person, sizeof(struct Person))) > 0) {
        if (!strcmp(name, person.name)) {
            person.age = new_age;
            off_t pos = lseek(fd, - sizeof(struct Person), SEEK_CUR);
            write(fd, &person, sizeof(struct Person));
        }
    }
}

int print_person (int n) {

    int fd = open(FILENAME, O_RDONLY, 0440);
    if (fd == -1) {
        perror("[print_person] error opening 'FILENAME'");
        close(fd);
        return -1;
    }

    Person person;
    lseek(fd, (n - 1) * sizeof(struct Person), SEEK_SET);
    read(fd, &person, sizeof(struct Person));

    printf("Name: %s | Age: %d\n", person.name, person.age);

    close(fd);
}

int main (int argc, char const *argv[]) {

    if (argv[1] && argv[2]) {
        if (argv[3] && argv[1][1] == 'i') {
            new_person(argv[2], atoi(argv[3]));
        } else if (argv[3] && argv[1][1] == 'u') {
            person_change_age(argv[2], atoi(argv[3]));
        } else if (argv[1][1] == 'p') {
            print_person(atoi(argv[2]));
        }
    }

    return 0;
}
