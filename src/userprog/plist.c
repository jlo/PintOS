#include <stddef.h>
#include <stdlib.h>

#include "plist.h"

#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"

#include "filesys/file.h"

#include "filesys/inode.h"
#include "threads/synch.h"


static struct lock plist_lock;


void plist_init(void)
{
    lock_init(&plist_lock);
    
}

int plist_add_process(struct map* process_list, int parent_pid, char* name)
{
    lock_acquire(&plist_lock);
    struct process* p = (struct process*)malloc(sizeof(struct process));   
    int index = map_insert(process_list, (value_t)p);
    p->pid = index;    
    p->parent_pid = parent_pid;
    p->should_wait = false;
    
    // We need to allocate memory for name. Cannot just reference to argument
    // name since this refers to a memory address in thread struct, which
    // can be deleted anytime.
    p->name = (char*)malloc(16);    
    memcpy(p->name, name, 16);
    
    p->exit_status = 0;
    
    p->alive = 1;    
    
    p->has_exited = 0;
    // Since process was just started BY its parent, we can assume parent is alive.
    p->parent_alive = 1;
        
    sema_init(&p->wait_sema, 0);
        
    lock_release(&plist_lock);
    return p->pid;
}


void print_process(key_t k, value_t v, int aux UNUSED)
{
	struct process* p = (struct process*)v;
	if(p == NULL){
	  return;
	}
	printf("# %-20s %-20i %-20i %-20i %-20i %-20i\n", p->name, k, p->parent_pid, p->alive, p->parent_alive, p->exit_status);
}

void plist_print_processes(struct map* process_list) 
{

    lock_acquire(&plist_lock);
    printf ("# %-20s %-20s %-20s %-20s %-20s %-20s\n","Name","PID","ParentPID", "Alive", "Parent alive", "Exit status");
    map_for_each(process_list, print_process, 0);

    lock_release(&plist_lock);
}

struct process* plist_get_process(struct map* process_list, int pid)
{
    
    lock_acquire(&plist_lock);
    struct process* p = (struct process*)map_find( process_list, pid);
    if(p == NULL){
        printf("# Error! plist_find_process_by_pid(): Could not find process with PID %i\n", pid);
    }    
    
    lock_release(&plist_lock);
    return p;
}

struct process* internal_plist_find_process_by_pid(struct map* process_list, int pid)
{
    
    //lock_acquire(&plist_lock);
    struct process* p = (struct process*)map_find( process_list, pid);
    if(p == NULL){
        printf("# Error! plist_find_process_by_pid(): Could not find process with PID %i\n", pid);
    }    
    
    // lock_release(&plist_lock);
    return p;
}




bool remove(key_t k UNUSED, value_t v, int removed_process_pid)
{    
    struct process* p = (struct process*)v;
    if(p != NULL){
    
        if(p->parent_pid == removed_process_pid){
            p->parent_alive = 0;
        }
        
        if(p->alive == 0 && p->parent_alive == 0){
            // If both parent process and child process are dead, no need to 
            // keep it in list anymore. 
             
            // We should free all struct process memory here.
            free(p->name);
            return true;
        }
    }
    return false;
}



void plist_remove_process(struct map* process_list, int pid)
{  

    lock_acquire(&plist_lock);
    struct process* p = internal_plist_find_process_by_pid(process_list, pid);
    if(p != NULL){
        p->alive = 0;        
    }        
    map_remove_if(process_list, remove, pid);

    lock_release(&plist_lock);   
}

void plist_set_exit_status(struct map* process_list, int pid, int status)
{

    lock_acquire(&plist_lock);
    struct process* p = internal_plist_find_process_by_pid(process_list, pid);
    if(p != NULL){
        p->exit_status = status;       
    }    
    
    lock_release(&plist_lock);
}

int plist_get_exit_status_by_pid(struct map* process_list, int pid)
{

    lock_acquire(&plist_lock);
    struct process* p = internal_plist_find_process_by_pid(process_list, pid);
    if(p != NULL){      

    	lock_release(&plist_lock);  
        return p->exit_status;       
    }    
    
    lock_release(&plist_lock);
    return -1;
}

int plist_get_alive_status_by_pid(struct map* process_list, int pid)
{

    lock_acquire(&plist_lock);
    struct process* p = internal_plist_find_process_by_pid(process_list, pid);
    if(p != NULL){
        
    	lock_release(&plist_lock);
        return p->alive;       
    }   

    lock_release(&plist_lock);
    return 0;
}

void plist_force_remove_process(struct map* process_list, int pid)
{  

    lock_acquire(&plist_lock);
    struct process* p = internal_plist_find_process_by_pid(process_list, pid);
    if(p != NULL){
        p->alive = 0;      
	    p->parent_alive = 0;
    }    
    map_remove_if(process_list, remove, pid);    
    
    lock_release(&plist_lock);  
}

