#ifndef ASM_H
#define ASM_H
#include "opcode.h"
#include "sym_node.h"
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>



#define MAX_INPUT_LENGTH 200
#define MAX_LABEL_LENGTH 40
#define STR_LEN(start,end) ((char*)end - (char*)start + 1 )
#define MOVE_TO_NEXT_WORD(cursor) \
    while (*cursor == ' ' || *cursor == '\t') {      \
        cursor++;                  \
    }

#define MOVE_TO_NEXT_SPACE(cursor) \
    while (*cursor != ' ' && *cursor != '\t' && *cursor != '\0' && *cursor != ',') {  \
        cursor++;                     \
    }

#define MOVE_TO_STRING_END(cursor)     \
    cursor += 2;                       \
    while (*cursor && *cursor != '\'') \
    {                                  \
        cursor++;                      \
    }                                  \
    cursor += 1;

#define IS_LINE_EMPTY(line) ({ \
    size_t len = strlen(line); \
    int i; \
    for (i = 0; i < len; i++) { \
        if (!isspace((unsigned char)(line)[i])) { \
            break; \
        } \
    } \
    i == len; \
})

#define CHECK_NOT_EMPTY(line) \
    do { \
        if (IS_LINE_EMPTY(line)) { \
            not_empty = 0; \
            return 1; \
        } \
        not_empty = 1; \
    } while (0)

#define EXTRACT_operand1(cursor) \
    while (*cursor != '\'')     \
    {                           \
        cursor++;               \
    }                           \
    cursor--;

int isInteger(const char *str);
// #define DEBUG

#define MOVE_BASED_ON_CHAR(start, cursor) \
    do { \
        if ((*start == 'C' || *start == 'X') && *(start+1) == '\'' ) { \
            MOVE_TO_STRING_END(cursor); \
        } else { \
            MOVE_TO_NEXT_SPACE(cursor); \
        } \
    } while (0)

#define LINE_NOT_EMPTY not_empty

#define CMP(x,y) (strcmp(x,y)==0)

#define TEST_RES(x) do { \
    if (x) { \
        x = 0; \
    } else { \
        x = 1; \
    } \
} while (0)

#define INIT_OBJECT_CODE(obj) \
    do { \
        (obj)->format = 0; \
        (obj)->obj_code = 0; \
        (obj)->byte_length = 1; \
    } while(0)

#define IS_INTEGER_WITH_HASH(operand1) (operand1[0] == '#' && isInteger(operand1 + 1))
#define OPERAND_NOT_EMPTY(operand2) (operand2[0] != '\0')


#endif
