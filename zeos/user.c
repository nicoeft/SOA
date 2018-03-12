#include <libc.h>

char buff[24]="\nWrite Working\n";

int pid;
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	write(1,buff,strlen(buff));
	int time=gettime();
	char timeChar[5];
	itoa(time,timeChar);
	char buffer[]="El resultado del gettime es:";
	write(1,buffer,strlen(buffer));
	write(1,timeChar,strlen(timeChar));
	
  while(1) { }
}
