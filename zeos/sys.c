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
	update_in_stats();
	return current()->PID;
	interrupt_out_update_stats();
}

int ret_from_fork(){
	return 0;
}

int sys_fork()
{	
	update_in_stats();
	//Get a free task_struct for the process. If no space an error will be returned.
    if(list_empty(&freequeue)) return -ENOSPC; 
	//first we see if we can get all the frames,if enough we can get a new task_struct
	//Get frames for user(data+stack), if not enough, free them and return an error.
	
	int frames[NUM_PAG_DATA];
	for(int i=0;i<NUM_PAG_DATA;i++){
		frames[i] = alloc_frame(); //returns a free frame and marks it as used or -1 if no more
		if(frames[i] < 0){
			//if error, free all the frames
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
	page_table_entry * parent_PT = get_PT(&parent_union->task);
	/*Page table entries for system (code+data) and user code can be a copy of the parents tp entries.
	  We'll have the entries of the parent and child pointing to the same frame*/
	for(int i=0;i<NUM_PAG_KERNEL+NUM_PAG_CODE;i++){
		set_ss_pag(child_PT,i,get_frame(parent_PT,i));
	}
	/*we need the entries for user data+stack to point to new allocated pages(frames)
	Copy the user data+stack pages from the parent process to the child process:
	Use temporal free entries on the page table of the parent(only for the copy):
	
	Search the first free entry of the parent PT 
	(maybe we could assume there are free entries at LOG_INIT_DATA+NUM_PAG_DATA cause all processes are the same)*/
	
	int first_free_entry = -1;
	for(int i=PAG_LOG_INIT_DATA+NUM_PAG_DATA;i<TOTAL_PAGES;i++){
		if(!parent_PT[i].entry){
			first_free_entry = i;
			break;
		}
	}


	//check if we found a free entry and we have enough (we could use only one and flush every time)
	if(first_free_entry<0 && first_free_entry+20 < TOTAL_PAGES  ) return -ENOMEM;
	
	for(int i=0;i<NUM_PAG_DATA;i++){
		set_ss_pag(child_PT,i+PAG_LOG_INIT_DATA,frames[i]);//entries point to allocated frames
		set_ss_pag(parent_PT,first_free_entry,frames[i]);//temporaly map child frames to copy
		//We can now copy  parent data into child cuz we have the child frame in the parent TP
		copy_data((void *)((PAG_LOG_INIT_DATA+i)*PAGE_SIZE),(void *)(first_free_entry*PAGE_SIZE), PAGE_SIZE);
		//Free temporal entry used in the page table,we don't want the parent to know child user data
		del_ss_pag(parent_PT,first_free_entry);
		first_free_entry++;
	}
	//Flush TLB to really disable the parent process to access child pages
	set_cr3(current()->dir_pages_baseAddr);
	child_union->task.PID=get_new_pid(); //Assign a new PID
	//Initialize the fields of the task_struct that are not common to the child
	child_union->task.kernelEsp = &child_union->stack[KERNEL_STACK_SIZE-19]; //Correct esp
	//Prepare the child stack with a content that emulates the result of a call to task_switch.Like the idle
	child_union->stack[KERNEL_STACK_SIZE-19]=0; //when it returns from a task_switch will pop ebp
	child_union->stack[KERNEL_STACK_SIZE-18]=(unsigned int)&ret_from_fork; // and then return(here the magic happens, the fork() in child returns 0)
	init_stats_pcb(&child_union->task);
	child_union->task.state = ST_READY;
	list_add_tail(&child_union->task.list,&readyqueue);
	interrupt_out_update_stats();
return child_union->task.PID;
}



void sys_exit()
{ 
	update_in_stats();
	free_user_pages(current()); 
	current()->PID = -1;
	update_process_state_rr(current(),&freequeue); //free PCB
	sched_next_rr();
	interrupt_out_update_stats();
}

int sys_write(int fd, char * buffer, int size){
 update_in_stats();
  int ret;
 ret=check_fd(fd,ESCRIPTURA);
 if(ret<0) return ret;
 if(size<0) return -EINVAL;
 if(!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;
 char sysBuffer[1024];

 int remaining = size;
 while(remaining > 1024){
	copy_from_user(buffer,sysBuffer,1024);
	ret=sys_write_console(sysBuffer,1024);
	if(ret<0)return ret;
	remaining -= ret;
	buffer+=ret;
 }
 if(remaining >0){
	 copy_from_user(buffer,sysBuffer,remaining);
	 ret=sys_write_console(sysBuffer,remaining);
	 if(ret<0)return ret;
	  remaining -= ret;
 }
 
 interrupt_out_update_stats();
 return size-remaining;
 

}

int sys_gettime(){
	update_in_stats();
	return zeos_ticks;
	interrupt_out_update_stats();
}

int sys_get_stats(int pid,struct stats *st){
	if(st == NULL || pid <0) return -1;
	for(int i=0;i<NR_TASKS;i++){
		if(pid == task[i].task.PID){
			copy_to_user(&task[i].task.p_stats,st,sizeof(struct stats));
			return 0;
		}
	}
	return -1;
}
