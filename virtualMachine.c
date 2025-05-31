/* virtualMachine.c */
#include "virtualMachine.h"

int main(int argc, char *argv[]) {
    int8 size;
    VM *vm;
    Program *prog;

    size = (map(mov) + map(nop));
    prog = exampleprogram();
    printf("prog = %p\n", prog);

    vm = virtualMachine(prog, size);
    printf("vm = %p\n", vm);

    return 0;

}

VM *virtualMachine(Program *pr, int16 progSize) {
    VM *p; 
    Program *pp; //program pointer
    int16 size;

    assert((pr) && (progSize));
    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
	errno = ErrMem;
	return (VM *)0;
    }

    zero($8 p, size);
    pp = (Program *)malloc($i progSize);
    if (!pp) {
	free(p);
	errno = ErrMem;
	return (VM *)0;
    }
    copy(pp, pr, progSize);

    return p;
}

Program *exampleprogram() {
    Program *prog;
    Instruction *i1, *i2;
    Args *a1;
    int16 s1, s2, sa1; //sizes

    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc($i s2);
    assert(i1 && i2);
    zero($1 i1, s1);
    zero($1 i2, s2);

    i1->o = mov;
    sa1 = (s1-1);
    if (s1) {
	a1 = (Args *)malloc($i sa1);
	assert(a1);
	zero(a1, sa1);
    }
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
