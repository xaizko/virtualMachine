/* virtualMachine.c */
#include "virtualMachine.h"

int main(int argc, char *argv[]) {
    VM *vm;
    Program *prog;
    int16 total_size;

    vm = virtualMachine();
    printf("vm = %p (size: %lu)\n", vm, sizeof(struct s_vm));

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

    execute(vm);
    printf("ax = %.04hx\n", $i vm->c.r.ax);

    // Calculate the total size of all instructions
    total_size = map(mov) + map(nop) + map(hlt);
    // Add the size of the argument for the mov instruction
    total_size += sizeof(Args) - 1; 
    printf("Printing %d bytes of opcodes: ", total_size);
    printhex($8 prog, total_size, ' ');

    return 0;

}

VM *virtualMachine() {
    VM *p; 
    
    p = (VM *)malloc(sizeof(struct s_vm));
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }
    // Initialize the memory to zero
    memset(p, 0, sizeof(struct s_vm));

    return p;
}

Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i1, *i2, *i3;
    Args a1;
    int16 s1, s2, s3, sa1; //sizes
    //sa1 = argument size

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);
    s3 = map(hlt);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s2);
    i3 = (Instruction *)malloc($i s3);

    assert(i1 && i2 && i3);
    zero($8 i1, s1);
    zero($8 i2, s2);
    zero($8 i3, s3);

    i1->o = mov;
    sa1 = sizeof(Args); 
    a1 = 0x0005; // Value to set in ax register

    p = vm->m;
    // Copy the mov opcode 
    copy($8 p, $8 i1, 1);
    p++;

    // Copy the argument value 
    copy($8 p, $8 &a1, sa1);
    p += sa1;

    // Copy the nop opcode (
    i2->o = nop;
    copy($8 p, $8 i2, 1);
    p++;

    // Copy the hlt opcode 
    i3->o = hlt;
    copy($8 p, $8 i3, 1);

    // Total memory used: 1 (mov) + 2 (args) + 1 (nop) + 1 (hlt) = 5 bytes
    vm->b = 5;
    vm->c.r.ip = (Reg)vm->m;
    vm->c.r.sp = (Reg)-1;

    free(i1);
    free(i2);
    free(i3);

    return (Program *)vm->m; // Return the start of memory
}

int8 map(Opcode o) {
    int8 n;
    IM *p;
    
    for (n = 0; n < IMs; n++) {
        p = &instrmap[n];
        if (p->o == o) {
            return p->size;
        }
    }
    return 0; // Return 0 for unknown opcodes
}

void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    // Store the argument value in the ax register
    vm->c.r.ax = (Reg)a1;
    return;
}

void execinstr(VM *vm, Instruction *i) {
    Args a1, a2;
    int16 size;
    int8 *ptr;

    a1 = a2 = 0;
    size = map(i->o);
    
    ptr = (int8 *)i;
    ptr++; // Move past opcode
    
    switch (size) {
        case 1:
            break;
        case 2:
            // For 2-byte instructions, extract 1 byte argument
            a1 = (Args)(*ptr);
            break;
        case 3:
            // For 3-byte instructions, extract 2 bytes of arguments
            a1 = *((Args *)ptr);
            break;
        default:
            segfault(vm);
            return;
    }

    switch (i->o) {
        case mov:
            __mov(vm, i->o, a1, a2);
            break;
        case nop:
            break;
        case hlt:
            error(vm, SysHlt);
            break;
        default:
            segfault(vm);
    }
    
    return;
}

void execute(VM *vm) {
    int8 *pp;     
    int32 brkaddr;
    Opcode current_opcode;
    int16 size;

    assert(vm && vm->m[0]); 
    
    brkaddr = (int32)(vm->m) + vm->b;
    pp = vm->m;

    while ((int32)pp < brkaddr) {
        vm->c.r.ip = (Reg)(int32)pp;
        
        current_opcode = (Opcode)(*pp);
        
        if (current_opcode == mov) {
            // For MOV instruction, read opcode + argument 
            Args arg;
            memcpy(&arg, pp + 1, sizeof(Args));  //copy 2 bytes for the argument
            __mov(vm, current_opcode, arg, 0);
            size = 1 + sizeof(Args); // 1 byte opcode + 2 bytes argument
        } else if (current_opcode == nop) {
            // NOP is just 1 byte
            size = 1;
        } else if (current_opcode == hlt) {
            // HLT instruction - terminate execution
            printf("HLT instruction encountered\n");
            error(vm, SysHlt);
            return;
        } else {
            // Unknown opcode
            printf("Unknown opcode: 0x%02x (decimal: %d) at address %p\n", current_opcode, current_opcode, pp);
            segfault(vm);
            return;
        }
        
        pp += size; // Move to next instruction
    }
    
    // If we get here, we've executed all instructions without a HLT
    printf("Reached end of program memory without HLT\n");
}

void error(VM* vm, Errorcode e) {
    int8 exitcode;

    exitcode = -1;
    switch (e) {
        case ErrSegv:
            fprintf(stderr, "%s\n", "VM Segmentation fault");
            if (vm) {
                free(vm);
            }
            exit($i exitcode);
            break;
        case SysHlt:
            fprintf(stderr, "%s\n", "System halted");
            // Don't exit for HLT, just return to allow the program to continue
            return;
            break;
        case ErrMem:
            fprintf(stderr, "%s\n", "Memory allocation error");
            if (vm) {
                free(vm);
            }
            exit($i exitcode);
            break;
        default:
            fprintf(stderr, "%s\n", "Unknown error");
            if (vm) {
                free(vm);
            }
            exit($i exitcode);
            break;
    }
}
