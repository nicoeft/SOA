# 1 "taskswitch.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "taskswitch.S"
# 1 "include/asm.h" 1
# 2 "taskswitch.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
            pushl %ebp;
            movl %esp,%ebp;

            pushl %esi
            pushl %edi
            pushl %ebx

            movl 8(%ebp),%eax;
            pushl %eax;

            call inner_task_switch

            addl $4,%esp;

            popl %ebx
            popl %edi
            popl %esi

            popl %ebp
            ret;

.globl inner_task_switch; .type inner_task_switch, @function; .align 0; inner_task_switch:
            pushl %ebp;
            movl %esp,%ebp;


            call current;
            movl %ebp,4(%eax);

            movl 8(%ebp),%ebx;
            movl 4(%ebx),%edi;
            leal tss, %ecx;
            movl %ebx,%eax;
            addl 0x1000,%eax;
            movl %eax, 4(%ecx);


            movl 8(%ebx),%ecx;
            pushl %ecx;
            call set_cr3;
            addl $4,%esp;

            movl %edi,%esp;

            popl %ebp;
            ret;
