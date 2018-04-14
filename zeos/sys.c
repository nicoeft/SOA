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
	//Get a free task_struct for the process. If no space an error will be returned.
    if(list_empty(&freequeue)) return -ENOSPC; 
	//first we see if we can get all the frames,if enough we can get a new task_struct
	//Get frames for user(data+stack), if not enough, free them and return an error.
	
	int frames[NUM_PAG_DATA];
	for(int i=0;i<NUM_PAG_DATA;i++){
		frames[i] = alloc_frame(); //returns a free frame and marks it as used or -1 if no more
		if(frames[i] < 0){
			for(;i>=0;i--){
			free_frame(frames[i]);
			}
			return -ENOMEM;
		}
	}
	struct list_head *primerListHead = list_first(&freequeue);
	list_del(primerListHead);
	union task_union *parent_union = (union task_union *)current();
	union task_union *child_union = (union task_union *) list_head_to_task_struct(primerListHead);
	//Inherit system data: copy the parent’s task_union to the child
	copy_data(parent_union,child_union,sizeof(union task_union));
	//Initialize dir_pages_baseAddr with a new directory to store the process address space
	allocate_DIR(&child_union->task);
	//Inherit user data:
		/*Create new address space: Access page table of the child process through the 
		directory field in the task_struct to initialize it*/
		page_table_entry *child_PT = get_PT(&child_union->task);
		/*Page table entries for system (code+data) and user code can be a copy of the parents tp entries.
		  We'll have the entries of the parent and child pointing to the same frame*/
		for(int i=0;i<NUM_PAG_KERNEL+NUM_PAG_CODE;i++){
			set_ss_pag(child_PT,i,i); //Addresses of the system space are absolute (same logical/physical)
		}
		// we need the entries for user data+stack to point to new allocated pages(frames)*/
		
	//Copy the user data+stack pages from the parent process to the child process:
	page_table_entry * parent_PT = get_PT(current());
	//Use temporal free entries on the page table of the parent(only for the copy):
		//Search the first free entry of the parent PT
	int first_free_entry = -1;
	for(int i=PAG_LOG_INIT_DATA+NUM_PAG_DATA;i<TOTAL_PAGES;i++){
		if(!parent_PT[i].entry){
			first_free_entry = i;
			break;
		}
	}
	//check if we found a free entry
	if(first_free_entry<0) return -ENOMEM;
	for(int i=0;i<NUM_PAG_DATA;i++){
		set_ss_pag(child_PT,i+PAG_LOG_INIT_DATA,frames[i]);//entries point to allocated frames
		set_ss_pag(parent_PT,first_free_entry,frames[i]);//temporaly map child frames to copy
		//We can now copy  parent data into child cuz we have the child frame in the parent TP
		copy_data((void *)((PAG_LOG_INIT_DATA+i)*PAGE_SIZE),(void *)(first_free_entry*PAGE_SIZE), PAGE_SIZE);
		//Free temporal entry used in the page table,we don't want the parent to know child user data
		del_ss_pag(parent_PT,first_free_entry);
	}
	//Flush TLB to really disable the parent process to access child pages
	set_cr3(current()->dir_pages_baseAddr);
	child_union->task.PID=get_new_pid(); //Assign a new PID
	//Initialize the fields of the task_struct that are not common to the child
	child_union->task.kernelEsp = &child_union->stack[KERNEL_STACK_SIZE-19]; //Correct esp
	//Prepare the child stack with a content that emulates the result of a call to task_switch.Like the idle
	child_union->stack[KERNEL_STACK_SIZE-19]=0; //when it returns from a task_switch will pop ebp
	child_union->stack[KERNEL_STACK_SIZE-18]=&ret_from_fork; // and then return(here the magic happens, the fork() in child returns 0)
	list_add(&child_union->task.list,&readyqueue);
  return child_union->task.PID;
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
 if((buffer == NULL) || size<0) return -EINVAL;
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
