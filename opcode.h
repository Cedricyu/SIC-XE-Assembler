#ifndef OP_H
#define OP_H
#include <string.h>
#include <stdlib.h>

#define D_NOTDR "NOTDR"
#define D_START "START"
#define D_END "END"
#define D_BASE "BASE"
#define D_BYTE "BYTE"
#define D_WORD "WORD"
#define D_RESB "RESB"
#define D_RESW "RESW"


#define REGISTER_A 0x00
#define REGISTER_X 0x01
#define REGISTER_L 0x02
#define REGISTER_B 0x03
#define REGISTER_S 0x04
#define REGISTER_T 0x05
#define REGISTER_F 0x06

// ASM register list
static const char regs_list[7] = {
    'A', 'X', 'L', 'B', 'S', 'T', 'F'
};

int find_register_no(const char *c);

// Define the instruction structure
struct Instruction {
    char mnemonic[6];
    int opcode;
};

// Function to search by op code number
const int searchByOpcode(const int opcode);
// Function to search by mnemonic
const int searchByMnemonic(char *mnemonic);

struct ObjectCode
{
    unsigned int format;
    unsigned int obj_code;
    unsigned int byte_length;
    unsigned int address;

    struct ObjectCode *next;
};

struct OBJECT_CODE_LIST
{
    unsigned int top;
    struct ObjectCode *head;
    struct ObjectCode *tail;
};

struct ObjectCode *createObjectCode(unsigned int address);

void init_op_code(struct ObjectCode *o, char *m);

void init_address_mode(struct ObjectCode *o,unsigned int am);

void init_registers(struct ObjectCode *o,int rg);

void init_disp(struct ObjectCode *o, int rg);

void init_disp4(struct ObjectCode *o, int rg);


int init_byte(struct ObjectCode *o, char c);

void init_hex_byte(struct ObjectCode *o, int b);

extern struct OBJECT_CODE_LIST ObjectCodeList;

void insert_object_list(struct ObjectCode *o);

int is_format_two(const char *op);

void print_code(struct ObjectCode *o);

void print_obj_code(struct ObjectCode *o_ptr);

typedef struct MRecord {
    unsigned int addr;          // Object number or starting address
    unsigned int m_type;         // Modification type or length
    struct MRecord* next;   // Pointer to the next MRecord
} MRecord;

extern MRecord *MRecordList;

// Function to create a new MRecord and add it to the linked list
MRecord *createMRecord(unsigned int objNo, unsigned int m_type);

void addMRecord(unsigned int objNo, unsigned int m_type);

#endif