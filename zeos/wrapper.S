#include <asm.h>
#include <errno.h>

ENTRY(write)
			pushl %ebp;      //Dynamic Link
			movl %esp,%ebp;  //puntero al bloque de activacion
			
			pushl %ebx;  // ebx,edi,esi se guardan si se modifican
			movl 8(%ebp),%ebx; //en ebp+4 esta @ret
			movl 12(%ebp),%ecx;
			movl 16(%ebp),%edx;
			movl $4,%eax;
			int $0x80;
			popl %ebx;
			popl %ebp; //Deshacer dynamic link
			movl %esp,%ebp; //no hace falta no hay variables locales
			cmpl $0,%eax;
			jge notError; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError:   
			ret;
			
ENTRY(gettime)
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

ENTRY(getpid)
		pushl %ebp;      
		movl %esp,%ebp;
		movl $20,%eax;
		int $0x80;
		cmpl $0,%eax;
			jge notError2; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError2:   
			popl %ebp;
			ret;

ENTRY(fork)
		pushl %ebp;      
		movl %esp,%ebp;
		movl $2,%eax;
		int $0x80;
		cmpl $0,%eax;
			jge notError3; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError3:   
			popl %ebp;
			ret;


ENTRY(exit)
		pushl %ebp;      
		movl %esp,%ebp;
		movl $1,%eax;
		int $0x80;
		popl %ebp;
		ret;
		
ENTRY(get_stats)
		pushl %ebp;      
		movl %esp,%ebp;
		movl 8(%ebp),%ebx; //primer param (pid)
		movl 12(%ebp),%ecx; //segon param (struct stats *st)
		movl $35,%eax;
		int $0x80;
		cmpl $0,%eax;
			jge notError4; 
			neg %eax;
			movl %eax,errno;
			movl $-1,%eax;
notError4:   
			popl %ebp;
			ret;
