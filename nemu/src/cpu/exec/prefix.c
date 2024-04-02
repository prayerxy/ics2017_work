#include "cpu/exec.h"

make_EHelper(real);

//前缀0x66 operand_size  表示宽度16位;否则是32位
make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  exec_real(eip);//越过前缀重新调用exec_real执行，取得真正的操作码
  decoding.is_operand_size_16 = false;//重置
}
