/* virtualMachine.c */
#include "virtualMachine.h"

int main(int argc, char *argv[]) {
    VM *vm;
    Program *prog;

    vm = virtualMachine();
    printf("vm = %p (size: %lu)\n", vm, sizeof(struct s_vm));

    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

    execute(vm);
    printf("ax = %.04hx\n", $i vm->c.r.ax);

    printhex($8 prog, (map(mov) + map(nop) + map(hlt)), ' ');

    return 0;

}

VM *virtualMachine() {
    VM *p; 
    int16 size;

    size = sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
	errno = ErrMem;
	return (VM *)0;
    }
    zero($8 p, size);

    return p;
}

Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i1, *i2, *i3;
    Args a1;
    int16 s1, s2, sa1; //sizes
    //sa1 = arugment size

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s2);
    i3 = (Instruction *)malloc($i map(hlt));

    assert(i1 && i2 && i3);
    zero($8 i1, s1);
    zero($8 i2, s2);
    zero($8 i3, map(hlt));

    i1->o = mov;
    sa1 = (s1-1);
    a1 = 0x0005;

    p = vm->m;
    copy($8 p, $8 i1, 1);
    p++;

    if (a1) {
        copy($8 p, $8 &a1, sa1);
        p += sa1;
    }

    i2->o = nop;
    copy($8 p, $8 i2, 1);
    p++;

    i3->o = hlt;
    copy($8 p, $8 i3, 1);

    vm->b = (s1+sa1+s2+map(hlt));
    vm->c.r.ip = (Reg)vm->m;
    vm->c.r.sp = (Reg)-1;

    free(i1);
    free(i2);
    free(i3);

    return (Program *)vm->m;
}

int8 map(Opcode o) {
    int8 n;
    IM *p;
    
    for (n=0, p=instrmap; n < IMs; n++, p++) {
        if (p->o == o) {
            return p->size;
        }
    }
    return 0; // Return 0 for unknown opcodes
}

// Move value into register ax
void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm->c.r.ax = (Reg)a1;
    return;
}

// Execute a single instruction based on its opcode
void execinstr(VM *vm, Instruction *i) {
    Args a1, a2;
    int16 size;

    a1 = a2 = 0;
    size = map(i->o);

    // Extract arguments based on instruction size
    switch (size) {
        case 1:
            break;
        case 2:
            a1 = i->a[0];
            break;
        case 3:
            a1 = i->a[0];
            a2 = i->a[1];
            break;
        default:
            segfault(vm);
            return;
    }

    // Execute appropriate instruction
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
}

// Move VM instruction pointer and execute instructions
void execute(VM *vm) {
    Program *pp;
    int32 brkaddr;
    Instruction *ip; // instruction pointer
    int16 size;

    size = 0;
    assert(vm && vm->m[0]); // Check if VM and memory are initialized
    printf("memory addr %p\n", vm->m);
    printf("break line %d\n", vm->b);
    brkaddr = (int32)(vm->m) + vm->b;
    pp = vm->m;

    do {
        vm->c.r.ip = (Reg)(int32)pp;
        ip = (Instruction *)pp;
        
        // Check memory bounds before accessing
        if ((int32)pp >= brkaddr) {
            segfault(vm);
        }
        
        size = map(ip->o);
        if (size == 0) {
            segfault(vm); // Invalid instruction
        }
        
        execinstr(vm, ip);
        pp += size; // Move to next instruction
        
        printf("pp = %p\n", pp);
        printf("brk = 0x%x\n", brkaddr);
    } while (*pp != hlt && (int32)pp < brkaddr);
}

void error(VM* vm, Errorcode e) {
    int8 exitcode;

    exitcode = -1;
    if (vm) {
	free(vm);
    }
    switch (e) {
	case ErrSegv:
	    fprintf(stderr, "%s\n", "VM Segmentation fault");
	    break;
	case SysHlt:
	    fprintf(stderr, "%s\n", "System halted");
	    exitcode = 0;
	    break;
	default:
	    break;
    }
    exit($i exitcode);
}
