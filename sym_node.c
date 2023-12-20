#include "sym_node.h"

struct SymbolTable SYMBOL_TABLE = INIT_SYMBOL_TABLE;

int insertSymbol(char s[7], int r){
    struct SymbolEntry *new_node = (struct SymbolEntry *)malloc(sizeof(struct SymbolEntry));

    if (!new_node) {
        return 1;
    }
    strncpy(new_node->l, s, 7);
    new_node->v = r;

    new_node->next = NULL;
    new_node->prev = NULL;

    if (!SYMBOL_TABLE.head) {
        SYMBOL_TABLE.head = new_node;
        SYMBOL_TABLE.tail = new_node;
    } else {
        SYMBOL_TABLE.tail->next = new_node;
        new_node->prev = SYMBOL_TABLE.tail;
        SYMBOL_TABLE.tail = new_node;
    }

    return 0;
}

int findSymbol(char *s) {
    if(s[0] == '#'||s[0] == '@')
        s += 1;
    struct SymbolEntry *seek = SYMBOL_TABLE.head;
    while (seek) {
        if (!strcmp(seek->l, s)) {
            return seek->v;
        }
        seek = seek->next;
    }
    return 0;
}

void printSymbolTable() {
    const char *filename = "out.log";
    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    // Save the current standard output file descriptor
    int saved_stdout = dup(fileno(stdout));

    // Redirect standard output to the file
    if (dup2(file, fileno(stdout)) == -1) {
        perror("Error redirecting stdout");
        close(file);
        exit(EXIT_FAILURE);
    }
    printf("Symbol\n");
    printf("%-10s%-10s%10s  %-10s%-10s%-10s%-10s\n", "name", "hex", "dec", "scope", "kind", "type", "description");
    struct SymbolEntry *current = SYMBOL_TABLE.head;

    while (current != NULL) {
        // Print each symbol entry to the redirected stdout
        printf("%-10s%010X%10d  %-10s%-10s%-10s%-10s\n",
               current->l, current->v, current->v, "local", "relative", "data", "label");
        current = current->next;
    }

    close(file);
    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);
}

