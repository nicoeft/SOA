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
			//movl %esp,%ebp; no hace falta no hay variables locales
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
		
