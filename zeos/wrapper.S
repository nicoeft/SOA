#include <asm.h>
#include <errno.h>

ENTRY(write)
			movl 4(%ebp),%edx;
			movl 8(%ebp),%ecx;
			movl 12(%ebp),%ebx;
			movl $4,%eax;
			int $0x80;
			cmpl $0,%eax;
			jle notError; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError:   ret;
	
