#include <libc.h>

char buff[24];

int pid;

 int fibonacci(int n)
{
  if (n>2)
    return fibonacci(n-1) + fibonacci(n-2);
  else if (n==2)
    return 1;
  else if (n==1)       
    return 1;
  else if (n==0)
    return 0;
}



int __attribute__ ((__section__(".text.main")))
  main(void)
{
  int retPid = fork();
 /*  CPU */
  int num;
  char cpid[4];
  char cnum[10];
  for (num=0; num<5; num++){
	  itoa(getpid(),cpid);
	  if(retPid ==0)
	  write(1,"Proceso Hijo",12);
	  else  write(1,"Proceso padre",13);
	  write(1,&cpid,4);
	  write(1," ",1);
	  itoa(fibonacci(num),cnum);
	  write(1,&cnum,10);
	  write(1,"\n",1);
  }
   exit();
  
  /*  I/O
  
  
  */
  
  /*  Both

  */
    
}
