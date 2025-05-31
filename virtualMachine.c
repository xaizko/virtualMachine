/* virtualMachine.c */
#include "virtualMachine.h"

int main(int argc, char *argv[]) {
    int8 size;
    VM *vm;
    Program prog;

    size = (map(mov) + map(nop));
    prog = exampleprogram();
    printf("prog = %p\n", prog);

    vm = virtualMachine(prog, size);
    printf("vm = %p\n", vm);

    return 0;

}

VM *virtualMachine(Program pr, int16 progSize) {
    VM *p; 
    Program pp; //program pointer
    int16 size;

    assert((pr) && (progSize));
    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
	errno = ErrMem;
	return (VM *)0;
    }

    zero($8 p, size);
    pp = (Program)malloc($i progSize);
    if (!pp) {
	free(p);
	errno = ErrMem;
	return (VM *)0;
    }
    copy($8 pp, $8 pr, progSize);

    return p;
}

Program exampleprogram() {
    Instruction i1, i2;
    int16 size;

    size = map(mov);
    i1 = (Instruction)malloc($i size);
    if (!i1) {
	errno = ErrMem;
	return (Program)0;
    }

    size = map(nop);
    i2 = (Instruction)malloc($i size);
    if (!i2) {
	errno = ErrMem;
	return (Program)0;
    }

    Program prog = { i1, i2 };

    return prog;
}

int8 map(Opcode o) {
    int8 n, ret;
    IM *p;

    for (n=IMs; p=instrmap, n; n--, p++) {
	if (p->o == o) {
	    ret = p->size;
	    break;
	}
    }
    return ret;
}
