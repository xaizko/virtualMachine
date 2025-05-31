/* virtualMachine.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <birchutils.h>

#define ErrMem 0x01
#define NoArgs {0x00, 0x00}

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

#define $8 (int8 *)
#define $6 (int16)
#define $2 (int32)
#define $4 (int64)
#define $c (char *)
#define $i (int)

typedef unsigned short int Reg;

struct s_registers {
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg sp; //Stack pointer
    Reg ip; //instruction pointer
};
typedef struct s_registers Registers;

enum e_opcode {
    mov = 0x01,
    nop = 0x02
};
typedef enum e_opcode Opcode;

struct s_cpu {
    Registers r;
};
typedef struct s_cpu CPU;

struct s_instrmap {
    Opcode o;
    int8 size;
};
typedef struct s_instrmap IM;

typedef int8 Args;

struct s_instruction {
    Opcode o;
    Args a[]; //argument 0-2 bytes
};
typedef struct s_instruction Instruction;

typedef int8 Program;

typedef int8 Memory[((unsigned int)(-1))];


struct s_vm {
    CPU c;
    Memory s;
    Program p;
};
typedef struct s_vm VM;

typedef Memory *Stack;

static Opcode opc;
static IM instrmap[] = {
    { mov, 0x03 },
    { nop, 0x01 }
};
#define IMs (sizeof(instrmap) / sizeof(struct s_instrmap))

//function declarations
int8 map(Opcode o);
Program exampleprogram(void);
VM *virtualMachine(*Program,int16);

int main(int,char**);



