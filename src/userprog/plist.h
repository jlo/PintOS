#ifndef _PLIST_H_
#define _PLIST_H_

#include "userprog/map.h"

#include "threads/thread.h"



/* Place functions to handle a running process here (process list).
   
   plist.h : Your function declarations and documentation.
   plist.c : Your implementation.

   The following is strongly recommended:

   - A function that given process inforamtion (up to you to create)
     inserts this in a list of running processes and return an integer
     that can be used to find the information later on.

   - A function that given an integer (obtained from above function)
     FIND the process information in the list. Should return some
     failure code if no process matching the integer is in the list.
     Or, optionally, several functions to access any information of a
     particular process that you currently need.

   - A function that given an integer REMOVE the process information
     from the list. Should only remove the information when no process
     or thread need it anymore, but must guarantee it is always
     removed EVENTUALLY.
     
   - A function that print the entire content of the list in a nice,
     clean, readable format.
     
 */

int plist_add_process(struct map* process_list, int parent_pid, char* name);

void print_process(key_t k, value_t v, int aux UNUSED);
void plist_print_processes(struct map* process_list) ;


struct process* plist_find_process_by_pid(struct map* process_list, int pid);
void plist_remove_process(struct map* process_list, int pid);

bool remove(key_t k, value_t v, int removed_process_pid);

void plist_set_exit_status(struct map* process_list, int pid, int status);
int plist_get_exit_status_by_pid(struct map* process_list, int pid);

#endif

