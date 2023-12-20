#ifndef SYM_NODE
#define SYM_NODE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


struct SymbolEntry {
    char l[7];
    int v;
    struct SymbolEntry *next, *prev;
};

struct SymbolTable {
    struct SymbolEntry* head;
    struct SymbolEntry* tail;
};

extern struct SymbolTable SYMBOL_TABLE;

int insertSymbol(char s[7], int r);
int findSymbol(char *s);
void printSymbolTable();

#define INIT_SYMBOL_TABLE { .head = NULL, .tail = NULL }

#endif
