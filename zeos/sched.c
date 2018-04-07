/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
struct list_head freequeue;  //Mirar si hacer extern en el .h

struct list_head readyqueue;

struct task_struct * idle_task;
 
union task_union protected_tasks[NR_TASKS+2]  //equivalente al vector de PCB sin que se puede leer ni escribir en primeras ni ultimas posiciones
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *primerListHead = list_first(&freequeue);  //agafem el primer PCB de la freequeue
	list_del(primerListHead);   //eliminem el PCB de la freequeue
	idle_task = list_head_to_task_struct(primerListHead); //agafem adreça PCB
	idle_task->PID=0;
	allocate_DIR(idle_task);
	union task_union *idle_union = (union task_union *)idle_task;
	idle_union->stack[1023]=&cpu_idle;
	idle_union->stack[1022]=0;
	idle_task->kernelEsp=&(idle_union->stack[1022]);

	

}

void init_task1(void)
{
	struct list_head *primerListHead = list_first(&freequeue);  //agafem el primer PCB de la freequeue
	list_del(primerListHead);   //eliminem el PCB de la freequeue
	struct task_struct *task1 = list_head_to_task_struct(primerListHead); //agafem adreça PCB
	task1->PID=1;
	allocate_DIR(task1);
	set_user_pages(task1);
	union task_union *task1_union = (union task_union *)task1;
	tss.esp0 = &(task1_union->stack[KERNEL_STACK_SIZE-1]);
	set_cr3(task1->dir_pages_baseAddr);
	
}


void init_sched(){
    /* Initialize freequeue (sched.c)*/
  init_freequeue();
  
  /* Initialize readyqueue (sched.c)*/
  init_readyqueue();
  
}

void init_readyqueue(){
	INIT_LIST_HEAD(&readyqueue);
}

void init_freequeue(){
	INIT_LIST_HEAD(&freequeue);
	int i;
	for(i=0;i<NR_TASKS;i++){
		list_add(&task[i].task.list,&freequeue);
	}
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000); //mask 12 bits(4KB)
}

void task_switch(union task_union*t);

void inner_task_switch(union task_union *nw);