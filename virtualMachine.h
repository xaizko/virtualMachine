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
}
typedef struct s_registers Registers;

struct s_cpu {
    Registers r;
};
typedef struct s_cpu CPU;

struct s_instruction {
    Opcode o;
    Args a[]; //argument
};
typedef struct s_instruction Instruction;

typedef int8 Stack[-1];
typedef Instruction Program;

struct s_vm {
    CPU c;
    Stack s;
    Program *p;
};
typedef struct s_vm VM;

int main(int,char**);
