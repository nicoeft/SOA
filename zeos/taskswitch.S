#include <asm.h>

ENTRY(task_switch)
            pushl %ebp;      //Dynamic Link
            movl %esp,%ebp;  //puntero al bloque de activacion
            
            pushl %esi
            pushl %edi
            pushl %ebx
            
            movl 8(%ebp),%eax;
            pushl %eax;   //parametre new per inner task switch
            
            call inner_task_switch
            
            addl $4,%esp; //borrar parametre new de inner task switch

            popl %ebx
            popl %edi
            popl %esi
            
            popl %ebp
            ret;
            
ENTRY(inner_task_switch)
            pushl %ebp;      //Dynamic Link
            movl %esp,%ebp;  //puntero al bloque de activacion
            
            
            call current;       //ara a eax hi ha &task_union actual
            movl %ebp,4(%eax);  //current->kernelesp=ebp
            
            movl 8(%ebp),%ebx;   //ebx adressa de NEW 
            movl 4(%ebx),%edi;   // edi=KernelESP
            leal tss, %ecx;      //%ecx=&tss
            movl %ebx,%eax;      
            addl 0x1000,%eax;    //&task_union+4096 es la system_stack
            movl %eax, 4(%ecx);     //Posar la system_stack de NEW a tss 
            
            
            movl 8(%ebx),%ecx; // carregar el dir_pages_basAdr 
            pushl %ecx;  //crida a set_cr3 amb paramatre page_table_entry de NEW
            call set_cr3;                    
            addl $4,%esp; //borra paramatre
            
            movl %edi,%esp;  //esp=new->kernelesp

            popl %ebp;
            ret;
