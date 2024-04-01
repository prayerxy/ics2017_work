#include "cpu/exec.h"



make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);


//dummy包含的6条指令
make_EHelper(mov);
make_EHelper(call);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(pop);
make_EHelper(ret);
