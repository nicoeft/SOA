/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

enum state_t { ST_RUN, ST_READY, ST_BLOCKED };



struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  unsigned long * kernelEsp;
  page_table_entry * dir_pages_baseAddr;
  struct list_head list;
  int quantum;
  enum state_t state;
  struct stats p_stats;
  
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union protected_tasks[NR_TASKS+2];
extern union task_union *task; /* Vector de tasques */
extern struct task_struct *idle_task;
extern struct task_struct *task1;
extern struct list_head freequeue;  //Mirar si hacer extern en el .h
extern struct list_head readyqueue;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */

void init_freequeue(void);
  
void init_readyqueue(void);

void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void task_switch(union task_union*t);

void inner_task_switch(union task_union *nw);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

void update_in_stats();

void interrupt_out_update_stats();

void ready_out_update_stats(struct task_struct * t);


#endif  /* __SCHED_H__ */
