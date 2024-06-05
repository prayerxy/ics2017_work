#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  //and r/m16,imm8 符号扩展，在decode已经扩展完毕
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  //zf=0 溢出of=0
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sext(&t0, &id_dest->val, id_dest->width);
  rtl_sar(&t0, &t0, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(not);
}

make_EHelper(rol){
   
  rtl_li(&t0,31);
  rtl_and(&id_src->val,&t0,&id_src->val);
  rtl_li(&t1,(8*id_dest->width)-id_src->val);
  rtl_shl(&t2,&id_dest->val,&id_src->val);
  rtl_shr(&t3,&id_dest->val,&t1);
  rtl_or(&t2,&t2,&t3);

	if(id_src->val==1){
	  rtl_msb(&t3,&t2,id_dest->width);
	  rtl_get_CF(&t1);
	  rtl_xor(&t1,&t1,&t3);
	  rtl_set_OF(&t1);
	}

	operand_write(id_dest,&t2);
	print_asm_template2(rol);
};

make_EHelper(ror){
  rtl_li(&t0,31);
  rtl_and(&id_src->val,&t0,&id_src->val);
  rtl_li(&t1,(8*id_dest->width)-id_src->val);
  rtl_shr(&t2,&id_dest->val,&id_src->val);
  rtl_shl(&t3,&id_dest->val,&t1);
  rtl_or(&t2,&t2,&t3);

	if(id_src->val==1){
	  rtl_msb(&t3,&t2,id_dest->width);
	  rtl_shl(&t1,&t2,&id_src->val);
	  rtl_msb(&t1,&t1,id_dest->width);
	  rtl_xor(&t1,&t1,&t3);
	  rtl_set_OF(&t1);
	}

	operand_write(id_dest,&t2);
		print_asm_template2(ror);
};

make_EHelper(shld) 
{
  // PA5
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  rtl_li(&t2,id_src2->width);
  rtl_shli(&t2,&t2,3); 
  rtl_subi(&t2,&t2,id_src->val);
  rtl_shr(&t2,&id_src2->val,&t2);
  rtl_or(&t0,&t0,&t2);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template3(shld);
}

make_EHelper(shrd) 
{
  // PA5
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  rtl_li(&t2,id_src2->width);
  rtl_shli(&t2,&t2,3); 
  rtl_subi(&t2,&t2,id_src->val);
  rtl_shl(&t2,&id_src2->val,&t2);
  rtl_or(&t0,&t0,&t2);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template3(shrd);
}