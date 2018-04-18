#include <libc.h>

char buff[]="Init Task\n";

int pid;
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	write(1,buff,strlen(buff));
	int time=gettime();
	while(time<200){
		time=gettime();
	}
	char c[5];
	itoa(time,c);
	char buffer[]="El resultado del gettime es:";
	write(1,buffer,strlen(buffer));
	write(1,c,strlen(c));
	
	/*if(write(2,buff,strlen(buff))<0) perror();*/
	int retPid=fork();
	int count=0;
	while(1)
	{
		if(count%5000000==0){
			int pid = getpid();
			itoa(pid,c);
			char pidBuffer[]="PID es: ";
			write(1,pidBuffer,strlen(pidBuffer));
			write(1,c,strlen(c));
			write(1,"\n",1);
			//exit();
		}
		count++;
	}
}
