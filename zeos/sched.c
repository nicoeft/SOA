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
unsigned int QUANTUM = 500;
unsigned int newpid;
int quantum_remaining;
struct list_head freequeue;  //Mirar si hacer extern en el .h
struct list_head readyqueue;
extern struct list_head blocked;
struct task_struct *idle_task;
struct task_struct *task1;

 
union task_union protected_tasks[NR_TASKS+2]  //equivalente al vector de PCB sin que se puede leer ni escribir en primeras ni ultimas posiciones
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif



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
	int count=0;
	while(1)
	{
		if(count%5000000==0)printk("CPU_IDLE\n");
		count++;
	;
	}
}


void init_stats_pcb(struct task_struct *t){
	t->p_stats.user_ticks = 0;
	t->p_stats.system_ticks = 0;
	t->p_stats.blocked_ticks = 0;
	t->p_stats.ready_ticks = 0;
	t->p_stats.elapsed_total_ticks = get_ticks();
	t->p_stats.total_trans = 0;
	t->p_stats.remaining_ticks = get_ticks();
}

void init_idle (void)
{
	struct list_head *primerListHead = list_first(&freequeue);  //agafem el primer PCB de la freequeue
	list_del(primerListHead);   //eliminem el PCB de la freequeue
	idle_task = list_head_to_task_struct(primerListHead); //agafem adreça PCB
	idle_task->PID=0;
	allocate_DIR(idle_task);
	union task_union *idle_union = (union task_union *)idle_task;
	idle_union->stack[KERNEL_STACK_SIZE-1]=(unsigned int) &cpu_idle;
	idle_union->stack[KERNEL_STACK_SIZE-2]=0;
	idle_task->kernelEsp=&(idle_union->stack[KERNEL_STACK_SIZE-2]);
	init_stats_pcb(idle_task);
}

void init_task1(void)
{
	struct list_head *primerListHead = list_first(&freequeue);  //agafem el primer PCB de la freequeue
	list_del(primerListHead);   //eliminem el PCB de la freequeue
	task1 = list_head_to_task_struct(primerListHead); //agafem adreça PCB
	task1->PID=1;
	task1->quantum=QUANTUM;
	task1->state=ST_RUN;
	allocate_DIR(task1);
	set_user_pages(task1);
	union task_union *task1_union = (union task_union *)task1;
	tss.esp0 = KERNEL_ESP(task1_union);
	set_cr3(task1->dir_pages_baseAddr);
	init_stats_pcb(task1);	
}


void init_sched(){
	
	quantum_remaining = 1;
	//initialize pid for other tasks
	newpid=2;
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
		task[i].task.PID = -1;
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

void task_switch(union task_union *t);

void inner_task_switch(union task_union *t);

unsigned int get_new_pid(){
	return newpid++;
}

int get_quantum (struct task_struct *t){
	return t->quantum;
}
void set_quantum (struct task_struct *t, int new_quantum){
	t->quantum = new_quantum;
}

void sched_next_rr(){
	struct task_struct *next_process_task;
	if(list_empty(&readyqueue)){ //if there are no other processes we do IDLE
		next_process_task = idle_task;
	}else{
		struct list_head *primerListHead = list_first(&readyqueue); 
		list_del(primerListHead);
		next_process_task = list_head_to_task_struct(primerListHead); 
	}
	next_process_task->state = ST_RUN;
	quantum_remaining = get_quantum(next_process_task);
	update_in_stats();
	ready_out_update_stats(next_process_task);
	task_switch((union task_union *)next_process_task);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest){
	if(t->state!=ST_RUN){
		if(t->state==ST_READY)printk("ST_READY\n");
		 list_del(&t->list); //if not running it can be in blocked or ready queues
	}
	if(dest != NULL){ //new state is not running
			if(t != idle_task){
				list_add_tail(&t->list,dest); //never add idle to readyqueue
			}
			if(dest == &readyqueue){
				t->state=ST_READY;
			}
			else if(dest != &freequeue) t->state=ST_BLOCKED;
	}
	else t->state = ST_RUN;
}

int needs_sched_rr(){
	if(quantum_remaining <= 0){
		if(!list_empty(&readyqueue)) return 1;
		else quantum_remaining = get_quantum(current());
	}
	return 0;
}

void update_sched_data_rr(){
	quantum_remaining--;
}

void schedule(){
	update_sched_data_rr();
	if(needs_sched_rr()){
		update_process_state_rr(current(),&readyqueue); //Put the current process in ready
		sched_next_rr(); //execute the next process ready
	}
}

void update_in_stats(){
	current()->p_stats.user_ticks += get_ticks()-current()->p_stats.elapsed_total_ticks;
	current()->p_stats.elapsed_total_ticks = get_ticks();
}

void interrupt_out_update_stats(){
	current()->p_stats.system_ticks += get_ticks()-current()->p_stats.elapsed_total_ticks;
	current()->p_stats.elapsed_total_ticks = get_ticks();
}

void ready_out_update_stats(struct task_struct * t){
	t->p_stats.ready_ticks += get_ticks()-t->p_stats.elapsed_total_ticks;
	t->p_stats.elapsed_total_ticks = get_ticks();
}


