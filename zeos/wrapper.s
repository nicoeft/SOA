# 1 "wrapper.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "wrapper.S"
# 1 "include/asm.h" 1
# 2 "wrapper.S" 2
# 1 "include/errno.h" 1
# 3 "wrapper.S" 2

.globl write; .type write, @function; .align 0; write:
   movl $4(ebp),%edx;
   movl $8(ebp),%ecx;
   movl $12(ebp),%ebx;
   movl $4,%eax;
   int $0x80;
   cmpl %eax,$0;
   jle notError;
   neg %eax;
   movl %eax,errno;
   movl $-1,%eax;
notError: ret;
