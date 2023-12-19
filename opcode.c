#include "opcode.h"
#include <stdio.h>
#define INSTRUCTION_CNT sizeof(instructions) / sizeof(struct Instruction)

struct OBJECT_CODE_LIST ObjectCodeList = { .top = 0, .head = NULL, .tail = NULL };
 
static const struct Instruction instructions[] = {
    {"ADD", 0x18},
    {"ADDF", 0x58},
    {"AND", 0x40},
    {"COMP", 0x28},
    {"COMPF", 0x88},
    {"COMPR" ,0xA0},
    {"CLEAR", 0xB4},
    {"DIV", 0x24},
    {"DIVF", 0x64},
    {"J", 0x3C},
    {"JEQ", 0x30},
    {"JGT", 0x34},
    {"JLT", 0x38},
    {"JSUB", 0x48},
    {"LDA", 0x00},
    {"LDB", 0x68},
    {"LDCH", 0x50},
    {"LDF", 0x70},
    {"LDL", 0x08},
    {"LDS", 0x6C},
    {"LDT", 0x74},
    {"LDX", 0x04},
    {"LPS", 0xD0},
    {"MUL", 0x20},
    {"MULF", 0x60},
    {"OR", 0x44},
    {"RD", 0xD8},
    {"RSUB", 0x4C},
    {"SSK", 0xEC},
    {"STA", 0x0C},
    {"STB", 0x78},
    {"STCH", 0x54},
    {"STF", 0x80},
    {"STI", 0xD4},
    {"STL", 0x14},
    {"STS", 0x7C},
    {"STSW", 0xE8},
    {"STT", 0x84},
    {"STX", 0x10},
    {"SUB", 0x1C},
    {"SUBF", 0x5C},
    {"TD", 0xE0},
    {"TIX", 0x2C},
    {"TIXR", 0xB8},
    {"WD", 0xDC}
};

struct ObjectCode* createObjectCode(unsigned int address) {
    struct ObjectCode* obj = (struct ObjectCode*)malloc(sizeof(struct ObjectCode));
    if (obj != NULL) {
        obj->address = address;
    }
    return obj;
}

int find_register_no(const char *c){
    for (size_t i = 0; i < sizeof(regs_list)/sizeof(char); i++)
        if(*c == regs_list[i])
            return i;
    return -1;
}

void init_op_code(struct ObjectCode *o, char *m) {
    int idx = searchByMnemonic(m);
    if (idx == -1) {
        printf("\033[31m%s op code not found\033[0m\n", m);
        return;
    }
    o->obj_code = instructions[idx].opcode;
    if(m[0]=='+'){
        o->format = 4;
        return;
    }
    o->format = 3;
}

void init_address_mode(struct ObjectCode *o, unsigned am) {
    o->obj_code >>= 2; // Corrected: Right shift by 2 bits and assign back to o->obj_code
    if (am > 3) {
        printf("address mode out of bound\n");
        return;
    }
    o->obj_code <<= 2; // Corrected: Left shift by 2 bits and assign back to o->obj_code
    o->obj_code += am & 3;
}

void init_registers(struct ObjectCode *o, int rg) {
    
    o->obj_code <<= 4; // Corrected: Left shift by 4 bits and assign back to o->obj_code
    o->obj_code += rg & 15;
}

void init_disp(struct ObjectCode *o, int rg) {
    o->obj_code <<= 12; // Corrected: Left shift by 12 bits and assign back to o->obj_code
    o->obj_code += rg & 4095;
}

void init_disp4(struct ObjectCode *o, int rg) {
    o->obj_code <<= 20; // Corrected: Left shift by 12 bits and assign back to o->obj_code
    o->obj_code += rg & ((2 << 16) -1);
}

int init_byte(struct ObjectCode *o,char c){
    // printf("char %x\n", c);
    if (o->format >= 4)
        return 0;
    o->obj_code <<= 8;
    o->obj_code += c & 255;
    o->format += 1;
    return 1;
}
void init_hex_byte(struct ObjectCode *o, int b){
    o->obj_code <<= 4;
    o->obj_code += b;
    o->format += 1;
    return ;
}

const int searchByOpcode(const int opcode) {
    for (int i = 0; i < INSTRUCTION_CNT; i++) {
        if (instructions[i].opcode == opcode) {
            return i;
        }
    }
    return -1;
}

// Function to search by mnemonic
const int searchByMnemonic(char* mnemonic) {
    if(mnemonic[0]=='+')
        mnemonic += 1;
    for (int i = 0; i < INSTRUCTION_CNT; i++)
    {
        if (strcmp(instructions[i].mnemonic, mnemonic) == 0)
        {
            return i;
        }
    }
    return -1; // Not found
}

void insert_object_list(struct ObjectCode *o){
    if(o==NULL)
        return;
    o->next = NULL;
    if (ObjectCodeList.head == NULL){
        // printf("\033[31m head modified\033[0m\n");
        ObjectCodeList.head = o;
    }
    if (ObjectCodeList.tail== NULL)
        ObjectCodeList.tail = o;
    else
        ObjectCodeList.tail->next = o;

    ObjectCodeList.tail = o;
    ObjectCodeList.top += 1;
}

void print_object_list(){
    struct ObjectCode *o_ptr = ObjectCodeList.head;
    printf("\033[31m print object list :\033[0m\n");
    while (o_ptr)
    {
        printf("OBJECT CODE : [");
        size_t obj_count = o_ptr->byte_length;
        for (size_t i = 0; i < obj_count; i++)
        {
            print_code(o_ptr);
            if (i < obj_count - 1)
                o_ptr = o_ptr->next;
        }
        printf("]\n");
        o_ptr = o_ptr->next;
    }
}

int is_format_two(const char *op) {
    if (strcmp(op, "CLEAR") == 0) {
        return 1;
    }
    size_t len = strlen(op);
    if (len > 0 && op[len - 1] == 'R') {
        return 1;
    }
    return 0;
}

void print_code(struct ObjectCode *o){
    switch(o->format){
        case 1:
            printf("%02X", o->obj_code);
            break;
        case 2:
            printf("%04X", o->obj_code);
            break;
        case 3:
            printf("%06X", o->obj_code);
            break;
        case 4:
            printf("%08X", o->obj_code);
            break;
    }
    return;
}

void print_obj_code(struct ObjectCode *o_ptr){
    printf("OBJECT CODE : [\033[32m");
    size_t obj_count = o_ptr->byte_length;
    for (size_t i = 0; i < obj_count; i++)
    {
        print_code(o_ptr);
        if (i < obj_count - 1)
            o_ptr = o_ptr->next;
    }
    printf("\033[0m]\n");
}