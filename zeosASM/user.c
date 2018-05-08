#include <libc.h>

char buff[256];

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

void printStats(void){
    struct stats st;
    char buffPid[10];
    get_stats(getpid(), &st);
		write(1, "\nPID: ", strlen("\nPID: "));
		itoa(getpid(), buffPid);
		write(1,buffPid,strlen(buffPid));
		
		write(1," User: ", strlen(" User: "));
		itoa(st.user_ticks,buff);
		write(1,buff,strlen(buff));

		write(1," System: ", strlen(" System: "));
		itoa(st.system_ticks,buff);
		write(1,buff,strlen(buff));

		write(1," Blocked: ", strlen(" Blocked: "));
		itoa(st.blocked_ticks,buff);
    write(1,buff,strlen(buff));

    write(1," Ready: ", strlen(" Ready: "));
		itoa(st.ready_ticks,buff);
    write(1,buff,strlen(buff));

    write(1," Total: ", strlen(" Total: "));
		itoa(st.elapsed_total_ticks,buff);
    write(1,buff,strlen(buff));
}



int __attribute__ ((__section__(".text.main")))
  main(void)
{
  set_sched_policy(1); // 0-RR 1-FCFS
  int retPid = fork();
  if(retPid !=0) fork();
 /*  CPU 
  for (int num=30; num<35; num++){
	  fibonacci(num);
    printStats(); 
  }
  exit();
  */
  /* I/O 
  for(int i=0;i<5;i++){
    read(0,&buff, 500);
    printStats();
  }
  exit();*/
  /*  Both*/
    for (int num=30; num<35; num++){
    if(retPid == 0) fibonacci(num);
    else read(0,&buff, 500);
    printStats(); 
  }
  exit();
  
    
}
