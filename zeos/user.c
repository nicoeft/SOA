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
	fork();
	fork();
	int count=0;
	while(1)
	{
		if(count%5000000==0){
			pid = getpid();
			itoa(pid,c);
			char pidBuffer[]="PID es: ";
			write(1,pidBuffer,strlen(pidBuffer));
			write(1,c,strlen(c));
			write(1,"        \n",10);
			if(pid%2!=0) exit();
				struct stats st;
				int retu = get_stats(6,&st);
			if(retu == 0){
				itoa(st.user_ticks,c);
				write(1,"USER_TICKS:",12);
				write(1,c,strlen(c));
				write(1,"\n",1);
			}else write(1,"ERROR_STATS\n",13);
		}
		count++;
	}
}
