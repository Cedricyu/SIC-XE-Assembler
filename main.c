#include <stdio.h>
#include "asm.h"
#include <fcntl.h>

int pass_one();
int pass_two();
void print_object_list();
void generateObjectFile();


int main(int argc, char *argv[]) {
    int fd = open(argv[1],O_RDONLY);
    close(0);
    dup(fd);
    close(fd);

    pass_one();
    rewind(stdin);
    pass_two();
    //print_object_list();
    const char* filename = "out.o";
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    close(1);
    dup(fd);
    close(fd);
    generateObjectFile();
}