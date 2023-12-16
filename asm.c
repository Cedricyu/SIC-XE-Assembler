#include "opcode.h"
#include "sym_node.h"
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>



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

int isInteger(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0; // Not an integer
    }
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return 0; // Not an integer
        }
        str++;
    }
    return 1; // It's an integer
}

// #define DEBUG

int start_loc = 0;
int locctr = 0;

int not_empty = 1;
int pg_loc = 0;
int base = 0;

char label[MAX_LABEL_LENGTH];
char op[MAX_LABEL_LENGTH];
char operand1[MAX_LABEL_LENGTH];
char operand2[MAX_LABEL_LENGTH];


int hasLabel(const char *line) {
    char *c_ptr = line;
    MOVE_TO_NEXT_WORD(c_ptr);
    char *start = c_ptr;
    MOVE_TO_NEXT_SPACE(c_ptr);
    c_ptr -= 1;
    char op_search[MAX_LABEL_LENGTH];
    strncpy(op_search, start, STR_LEN(start, c_ptr));
    if(searchByMnemonic(op_search) == -1)
        return 1;
    return 0;
}

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


void ADD_LOCTR();

int readline(){
    char line[MAX_INPUT_LENGTH];
    line[0] = '\0';
    label[0] = '\0';
    op[0] = '\0';
    operand1[0] = '\0';
    operand2[0] = '\0';

    if(fgets(line, MAX_INPUT_LENGTH, stdin)==NULL)
        return 0;

    line[strcspn(line, "\n")] = '\0';

#ifdef DEBUG
    printf("line len = %ld\n", strlen(line));
    printf("input = %s\n", line);
#endif

    CHECK_NOT_EMPTY(line);

    char *cursur = line;

    if(hasLabel(line)){
        char *start = cursur;
        MOVE_TO_NEXT_SPACE(cursur)
        char *end = cursur - 1;
    #ifdef DEBUG
        printf("start = %s\n",start);
        printf("end = %s\n",end);
    #endif
        strncpy(label, start, STR_LEN(start, end));
        label[STR_LEN(start, end)] = '\0';
    }

    MOVE_TO_NEXT_WORD(cursur)

    char *start = cursur;
    MOVE_TO_NEXT_SPACE(cursur)
    char *end = cursur - 1;
#ifdef DEBUG
    printf("start = %s\n",start);
    printf("end = %s\n",end);
#endif
    strncpy(op, start, STR_LEN(start, end));
    op[STR_LEN(start, end)] = '\0';

    if(CMP(op,"RSUB"))
        return 1;

    MOVE_TO_NEXT_WORD(cursur)

    start = cursur;
    MOVE_BASED_ON_CHAR(start, cursur);
    end = cursur - 1;
#ifdef DEBUG
    printf("start = %s\n",start);
    printf("end = %s\n",end);
    printf("len = %ld\n", STR_LEN(start, end));
#endif
    strncpy(operand1, start, STR_LEN(start, end));
    operand1[STR_LEN(start, end)] = '\0';
    char *c_ptr;
    if ((c_ptr = strchr(cursur, ',')) != NULL)
    {
        c_ptr++;
        MOVE_TO_NEXT_WORD(c_ptr);
        start = c_ptr;
        MOVE_TO_NEXT_SPACE(c_ptr);
        end = c_ptr - 1;
        strncpy(operand2, start, STR_LEN(start, end));
        operand2[STR_LEN(start, end)] = '\0';
    }
    return 1;
}

#define TEST_RES(x) do { \
    if (x) { \
        x = 0; \
    } else { \
        x = 1; \
    } \
} while (0)

// #define ADD_DEBUG

// Macro to decrease locctr by 3

void ADD_LOCTR()
{
    
    if (CMP(op, D_START))
    {
        // Set start_loc based on the operand1
        start_loc = strtol(operand1, NULL, 16);
    }
    // Print the line information
    printf("[%06X] Read a line: label=[\033[33m%s\033[0m], op=[\033[33m%s\033[0m], operand1=[\033[33m%s\033[0m] operand2=[\033[33m%s\033[0m].\n",
           locctr + start_loc, label, op, operand1, operand2);
    pg_loc = locctr + start_loc;
    // Process specific directives

    if (CMP(op, D_START))
    {
      
    }
    else if(CMP(op, D_BASE)) {

    }
    else if(CMP(op, "CLEAR")||CMP(op, "COMPR")||CMP(op, "TIXR")){
        locctr += 2;
    }
    else if (CMP(op, D_RESB))
    {
        locctr += strtol(operand1, NULL, 10);
    }
    else if(CMP(op, D_RESW)){
        locctr += 3*strtol(operand1, NULL, 10);
    }
    else if (CMP(op, D_BYTE) && operand1[0] != 'X')
    {
        // Process the operand1 as a string and increment locctr accordingly
        char *end = operand1;
        char *start = operand1 + 2;
        MOVE_TO_STRING_END(end);
        end -= 2;
    #ifdef ADD_DEBUG
        printf("start = %s\n", start);
        printf("end = %s\n", end);
        printf("len = %ld\n", STR_LEN(start, end));
    #endif

        // Increment locctr based on the length of the string
        locctr += (int)(STR_LEN(start, end));
    }
    else if (CMP(op, D_BYTE) && operand1[0] == 'X')
    {
        // Increment locctr for X'0F' by 1
        locctr += 1;
    }
    else
    {
        // Increment locctr based on the operation
        locctr += (op[0] == '+') ? 4 : 3;
    }
}

void write_line(int fd,struct ObjectCode *obj){
    char outputLine[MAX_INPUT_LENGTH];
    outputLine[0] = '\0';
    char str[7];
    snprintf(str,sizeof(str),"%06X",pg_loc);
    strcat(outputLine,str);
    strcat(outputLine,"\t");
    strcat(outputLine,label);
    strcat(outputLine,"\t");
    strcat(outputLine,op);
    strcat(outputLine,"\t");
    strcat(outputLine,operand1);
    if(obj){
        strcat(outputLine,"\t");
        char obj_str[9];
        snprintf(obj_str,sizeof(obj_str),"%06X",obj->obj_code);
        strcat(outputLine,obj_str);
    }
    strcat(outputLine,"\n");
    ssize_t bytes_written = write(fd, outputLine, strlen(outputLine));

    if (bytes_written == -1) {
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }
    
}

void pass_one() {
    //
    //  pass one
    //
    const char *filename = "location.txt";
    int fd;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (readline()){
        if(LINE_NOT_EMPTY){
            ADD_LOCTR();
            if(findSymbol(label)){
                printf("%s exist in symbol table\n", label);
            }
            else if(label[0]!='\0'){
                printf("insert %s into symbol table\n", label);
                insertSymbol(label,pg_loc);
            }
            write_line(fd,NULL);
        }
    }

    printSymbolTable();

    close(fd);
}
// #define PASS_TWO_DEBUG

#define INIT_OBJECT_CODE(obj) \
    do { \
        (obj)->format = 0; \
        (obj)->obj_code = 0; \
        (obj)->byte_length = 1; \
    } while(0)

#define IS_INTEGER_WITH_HASH(operand1) (operand1[0] == '#' && isInteger(operand1 + 1))
#define OPERAND_NOT_EMPTY(operand2) (operand2[0] != '\0')

void pass_two() {
    //
    //  pass two
    //
    const char *filename = "objcode.txt";
    int fd;
    start_loc = 0;
    locctr = 0;
    pg_loc = 0;
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
 
    while (readline())
    {
        if(LINE_NOT_EMPTY){
            ADD_LOCTR();
            // set base register
            if (CMP(op, D_BASE))
                base = findSymbol(operand1);
            ///
            struct ObjectCode *obj = NULL;  /// initialize object node
            if (!(CMP(op, D_START) || CMP(op, D_RESW) || CMP(op, D_RESB) || CMP(op, D_WORD) || CMP(op, D_BASE) || CMP(op, D_END)))
            {
            obj = malloc(sizeof(struct ObjectCode));
            INIT_OBJECT_CODE(obj);

            if(CMP(op,"BYTE")){
                char bytes[MAX_LABEL_LENGTH];
                char *start = operand1 + 2;
                char *end = operand1 + 2;
                
                if (operand1[0] == 'C')
                {
                    EXTRACT_operand1(end);
                    strncpy(bytes, start, STR_LEN(start, end));
                    bytes[STR_LEN(start, end)] = '\0';
                    obj->byte_length = (STR_LEN(start, end)) / 4+ 1;
                    for (; start <= end; start++)
                    {
                        while (!init_byte(obj,*start)){
                            insert_object_list(obj);
                            obj = malloc(sizeof(struct ObjectCode));
                            INIT_OBJECT_CODE(obj);
                        }
                    #ifdef PASS_TWO_DEBUG
                        printf("OBJECT CODE : [%06X]\n", obj->obj_code);
                    #endif
                    }
                }
                else if (operand1[0] == 'X')
                {
                    EXTRACT_operand1(end);
                    strncpy(bytes, start, STR_LEN(start, end));
                    bytes[STR_LEN(start, end)] = '\0';
                    int byte = strtol(bytes, NULL, 16);
                    init_hex_byte(obj, byte);
                }
                #ifdef PASS_TWO_DEBUG
                    printf("extract string %s\n", bytes);
                #endif
            }
            else{

                init_op_code(obj,op);
                if (CMP(op, "CLEAR") || CMP(op, "COMPR") || CMP(op, "TIXR")) {
                    // Find the register numbers for operand1 and operand2
                    size_t reg1 = find_register_no(operand1);
                    size_t reg2 = find_register_no(operand2);

                    if (reg1 == -1) {
                        printf("\033[31m Register for operand1 not found \033[0m\n");
                        init_registers(obj, 0);  // Default to register 0
                    } else {
                        init_registers(obj, reg1);
                    }

                    if (reg2 == -1) {
                        printf("\033[31m Register for operand2 not found \033[0m\n");
                        init_registers(obj, 0);  // Default to register 0
                    } else {
                        init_registers(obj, reg2);
                    }
                }
                else{
                // n=0, i=0 ： SIC machine
                // n=0, i=1 ： Immediate
                // n=1, i=0 ： Indirect(間接)
                // n=1, i=1 ： Simple

                // Immediate addressing modes：有#的
                // Indirect addressing：有@的
                // Extended instruction format：有+的
                size_t addr_mode;
                size_t registers = 0;

                char first = operand1[0];
                if (first == '#')
                {
                    addr_mode = 1;
                }
                else if(first == '@'){
                    addr_mode = 2;
                }
                else{
                    addr_mode = 3;
                }

                init_address_mode(obj, addr_mode);
                /*
                    set registers
                */
                size_t base_relative = 0;
                if ( abs(findSymbol(operand1) - pg_loc - 3) > 4096 ){
                    registers = 4;
                    base_relative = 1;
                }
                else
                    registers = 2;

                if(OPERAND_NOT_EMPTY(operand2)){
                    registers += 8;
                }
                else if(obj->format==4){
                    registers = 1;
                }
                else if(IS_INTEGER_WITH_HASH(operand1)){
                    registers = 0;
                }

            #ifdef PASS_TWO_DEBUG
                printf("operand1  = %s\n",operand1);
            #endif

                init_registers(obj, registers);

                if(obj->format == 3){
                    int s_v = findSymbol(operand1);
                    // printf("s_v [%d] pg_loc [%d] disp [%03X]\n",s_v,pg_loc,s_v - pg_loc);
                    if (s_v){
                        if(base_relative)
                            init_disp(obj, s_v - base);
                        else
                            init_disp(obj, s_v - pg_loc - 3);
                    }
                    else if (operand1[0] == '#' && isInteger(operand1+1)){
                        size_t immediate_addr = strtol(operand1+1, NULL, 10);
                        init_disp(obj, immediate_addr);
                    }
                    else if (operand1[0] == '\0')
                        init_disp(obj, 0);
                    else
                        printf("\033[31m Symbol not found\033[0m\n");
                }
                else if(obj->format == 4){
                    int s_v = findSymbol(operand1);
                    // printf("s_v [%d] pg_loc [%d] disp [%03X]\n",s_v,pg_loc,s_v - pg_loc);
                    if (s_v)
                        init_disp4(obj, s_v);
                    else if (operand1[0] == '#' && isInteger(operand1+1)){
                        size_t immediate_addr = strtol(operand1+1, NULL, 10);
                        init_disp4(obj, immediate_addr);
                    }
                    else if (operand1[0] == '\0')
                        init_disp4(obj, 0);
                    else
                        printf("\033[31m Symbol not found\033[0m\n");
                }
                }
            }
            printf("OBJECT CODE : [\033[32m%06X\033[0m]\n", obj->obj_code);
            insert_object_list(obj);
            }
            write_line(fd, obj);
        }
    }

    close(fd);
}



