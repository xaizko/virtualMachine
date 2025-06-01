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

#define NoErr 0x00
#define SysHlt 0x01
#define ErrMem 0x02
#define ErrSegv 0x04

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

#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sx ->c.r.sp
#define $ix ->c.r.ip

#define segfault(x) error((x), ErrSegv)

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
    nop = 0x02,
    hlt = 0x03
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

typedef int16 Args;

struct s_instruction {
    Opcode o;
    Args a[]; //argument 0-2 bytes
};
typedef struct s_instruction Instruction;

typedef int8 Program;

typedef int8 Memory[((int16)(-1))];

typedef unsigned char Errorcode;

struct s_vm {
    CPU c;
    Memory m;
    int16 b; //breakline
};
typedef struct s_vm VM;

typedef Memory *Stack;

static Opcode opc;
static IM instrmap[] = {
    { mov, 0x03 },
    { nop, 0x01 },
    { hlt, 0x01 }
};
#define IMs (sizeof(instrmap) / sizeof(struct s_instrmap))

//function declarations
int8 map(Opcode o);
Program *exampleprogram(VM*);
VM *virtualMachine(void);
void __mov(VM*, Opcode, Args, Args);
void execinstr(VM*, Instruction*);
void execute(VM*);
void error(VM*, Errorcode);

int main(int,char**);



