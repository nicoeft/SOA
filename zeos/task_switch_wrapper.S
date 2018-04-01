#include <asm.h>
#include <errno.h>

ENTRY(task_switch)
    pushl %ebp;
    movl %esp,%ebp;
    pushl %ebx;
    pushl %edi;
    pushl %esi
    call inner_task_switch;
    popl %esi;
    popl %edi;
    popl %ebx;
    iret;

