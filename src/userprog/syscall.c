#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"

/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"

#define DEBUG_SYSCALL(format, ...) printf(format "\n", ##__VA_ARGS__)

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:
   
   int sys_read_arg_count = argc[ SYS_READ ];
   
   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
  /* basic calls */
  0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 
  /* not implemented */
  2, 1,    1, 1, 2, 1, 1,
  /* extended */
  0
};

static void
syscall_handler (struct intr_frame *f) 
{
  int32_t* esp = (int32_t*)f->esp;
  /*
   Any parameters passed to syscall will be above ESP, see illustration below.
   Number of arguments each syscall expects is in argc[] array above.

   syscall number is pointed to by ESP. 
   --------------------------------------
  		PHYS_BASE
   		.................
   		<param>
		<param>
   ESP --->  	<syscall_number>
  
   --------------------------------------
  */
  int32_t syscall_nr = *esp;

  uint32_t eax = (uint32_t)f->eax;
  DEBUG_SYSCALL("# Argument passed to syscall: %i\n\n", eax);
//  printf("Return %i\n\n\n", f->eax);

  switch (syscall_nr)
  {
    case SYS_HALT:
	DEBUG_SYSCALL("# Halt received, force shutdown...\n");
	power_off();
    break;
    case SYS_EXIT:
	DEBUG_SYSCALL("# Exitting thread from syscall...\n");
	thread_exit();
    break;
    default:
    {
      DEBUG_SYSCALL ("Executed an unknown system call (nr: %i)!\n", syscall_nr);
      DEBUG_SYSCALL ("Stack top + 0: %d\n", esp[0]);
      DEBUG_SYSCALL ("Stack top + 1: %d\n", esp[1]);

      thread_exit ();
    }
  }
}
