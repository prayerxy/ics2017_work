#include "cpu/exec.h"



make_EHelper(operand_size);
make_EHelper(mov);
make_EHelper(inv);
make_EHelper(nemu_trap);



//special.c
make_EHelper(nop);


//arith.c
make_EHelper(add);
make_EHelper(sub);
make_EHelper(xor);


//data-mov.c
make_EHelper(pop);
make_EHelper(push);
make_EHelper(leave);



//control.c
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(jmp_rm);
make_EHelper(ret);
make_EHelper(call_rm);

