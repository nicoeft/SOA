#include <asm.h>

ENTRY(task_switch)
            pushl %ebp;      //Dynamic Link
            movl %esp,%ebp;  //puntero al bloque de activacion
            
            pushl %esi
            pushl %edi
            pushl %ebx
            
            movl 8(%ebp),%eax;  // 
            pushl %eax;            //parametre new per inner task switch
            
            call inner_task_switch
            
            ;add %esp,4 
            movl %ebp,%esp; //borrar parametre new de inner task switch

            
            popl %ebx
            popl %edi
            popl %esi
            
            popl %ebp
            ret;
            
ENTRY(inner_task_switch)
            pushl %ebp;      //Dynamic Link
            movl %esp,%ebp;  //puntero al bloque de activacion
            
            movl 8(%ebp),%ebx;     //ebx adressa de NEW 
            movl 4(%ebx),%edi;    // edi=KernelESP
            leal tss, %ecx;
            movl %edi, 4(%ecx);     //Posar kernekESP de NEW a tss 
            
            ;//addl %ecx, %ebx,8 per carregar el dir_pages_basAdr fem
            movl 8(%ebx),%ecx;
            pushl %ecx;  //crida a set_cr3 amb paramatre page_table_entry de NEW
            call set_cr3;
            ;addl %esp,4 ;                      
            movl %ebp,%esp; //borra paramatre
            
            call current;                   //ara a eax hi ha &pcb actual
            movl %ebp,4(%eax);    //current->kernelesp=ebp
            
            movl %edi,%ebp;        //ebp=new->kernelesp
            
            popl %ebp;
            ret;