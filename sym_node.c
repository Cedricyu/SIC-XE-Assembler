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
    printf("Symbol Table:\n");
    printf("|---------|---------|\n");
    printf("| %-7s | %-7s |\n", "Label", "Value");
    printf("|---------|---------|\n");

    struct SymbolEntry *current = SYMBOL_TABLE.head;

    while (current != NULL) {
        printf("| %-7s | %07X |\n", current->l, current->v);
        current = current->next;
    }
    printf("|---------|---------|\n");

}