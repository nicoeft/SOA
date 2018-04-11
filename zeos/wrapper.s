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
   pushl %ebp;
   movl %esp,%ebp;

   pushl %ebx;
   movl 8(%ebp),%ebx;
   movl 12(%ebp),%ecx;
   movl 16(%ebp),%edx;
   movl $4,%eax;
   int $0x80;
   popl %ebx;
   popl %ebp;
   movl %esp,%ebp;
   cmpl $0,%eax;
   jge notError;
   neg %eax;
   movl %eax,errno;
   movl $-1,%eax;
notError:
   ret;

.globl gettime; .type gettime, @function; .align 0; gettime:
  pushl %ebp;
  movl %esp,%ebp;
  movl $10,%eax;
  int $0x80;
  popl %ebp;
  cmpl $0,%eax;
   jge notError1;
   neg %eax;
   movl %eax,errno;
   movl $-1,%eax;
notError1:
   ret;
