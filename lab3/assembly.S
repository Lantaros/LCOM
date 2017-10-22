//Keyboard MACROS
#include "i8042.h"

.global _ass_handler

.text

_ass_handler:
xor %eax, %eax

Ciclo:
inb $KBD_STAT_REG, %al
test $OUT_BUF_FULL, %al

jz Ciclo

//PARITY and TIMEOUT tests
test $PARITY_ERR, %al
test $TIMEOUT_ERR, %al
jnz FIM

inb $OUT_BUF, %al //read information from output buffer

FIM:
ret


