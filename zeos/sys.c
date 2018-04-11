/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  
	struct list_head *primerListHead = list_first(&freequeue);
	if(primerListHead==NULL)return ENOSPC;
	list_del(primerListHead);
	struct task_struct *taskFork = list_head_to_task_struct(primerListHead);
	copy_data((union task_union *)current(),(union task_union *)taskFork,KERNEL_STACK_SIZE);
	allocate_DIR(taskFork);
	int i;
	int frames[NUM_PAG_DATA];
	page_table_entry * pt = get_PT(taskFork);
	for(i=0;i<NUM_PAG_DATA;i++){
		frames[i] = alloc_frame();
		if(frames[i] < 0){
			for(;i>=0;i--){
			free_frame(frames[i]);
			}
			return ENOMEM;
		}
		set_ss_pag(pt,i+NUM_PAG_KERNEL+NUM_PAG_CODE,frames[i]);
	}
	for(i=0;i<NUM_PAG_KERNEL+NUM_PAG_CODE;i++){
		set_ss_pag(pt,i,i);
	}
	page_table_entry * ptf = get_PT(current());
	for(i=0;i<NUM_PAG_DATA;i++){
		set_ss_pag(ptf,i+NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA,frames[i]);
		copy_data((i+NUM_PAG_KERNEL+NUM_PAG_CODE)*PAGE_SIZE,(i+NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA)*PAGE_SIZE,PAGE_SIZE);
		del_ss_pag(ptf,i+NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA);
	}
	set_cr3(current()->dir_pages_baseAddr);
	
	taskFork->PID=newpid;
	newpid++;
	taskFork->kernelEsp = ((union task_union *)taskFork)+(KERNEL_STACK_SIZE*4)-19*4;
	((union task_union *)taskFork)->stack[KERNEL_STACK_SIZE-18]=&ret_from_fork;
	list_add(taskFork->list,&readyqueue);
  return taskFork->PID;
}

int ret_from_fork(){
	return 0;
}

void sys_exit()
{  
}

int sys_write(int fd, char * buffer, int size){
 int resultCheck=check_fd(fd,ESCRIPTURA);
 if(resultCheck<0) return resultCheck;
 if((buffer == NULL) || size<0) return EINVAL;
 char sysBuffer[4096];
 int ret;
 while(size > 4096){
	 copy_from_user(buffer,sysBuffer,4096);
	 size -= 4096;
	 if(ret=sys_write_console(sysBuffer,4096)<0)return ret;
 } 
 copy_from_user(buffer,sysBuffer,size);
 return sys_write_console (sysBuffer,size);

}

int sys_gettime(){
	return zeos_ticks;
}
