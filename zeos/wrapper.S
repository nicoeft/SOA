#include <asm.h>
#include <errno.h>

ENTRY(write)
			pushl %ebx;
			movl 8(%ebp),%ebx; //en ebp+4 esta @ret
			movl 12(%ebp),%ecx;
			movl 16(%ebp),%edx;
			movl $4,%eax;
			int $0x80;
			popl %ebx;
			cmpl $0,%eax;
			jge notError; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError:   ret;
	
