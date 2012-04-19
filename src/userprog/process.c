#include <debug.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "userprog/gdt.h" /* SEL_* constants */
#include "userprog/process.h"
#include "userprog/load.h"
#include "userprog/pagedir.h" /* pagedir_activate etc. */
#include "userprog/tss.h" /* tss_update */
#include "filesys/file.h"
#include "threads/flags.h" /* FLAG_* constants */
#include "threads/thread.h"
#include "threads/vaddr.h" /* PHYS_BASE */
#include "threads/interrupt.h" /* if_ */

/* Headers not yet used that you may need for various reasons. */
#include "threads/synch.h"
#include "threads/malloc.h"
#include "lib/kernel/list.h"

#include "userprog/flist.h"
#include "userprog/plist.h"

#include "userprog/map.h"

/* HACK defines code you must remove and implement in a proper way */
#define HACK


#define PLIST_DEBUG(...) printf(__VA_ARGS__)

// #define debug  

struct map process_list;


/* This function is called at boot time (threads/init.c) to initialize
* the process subsystem. */
void process_init(void)
{
    map_init(&process_list);
}

/* This function is currently never called. As thread_exit does not
* have an exit status parameter, this could be used to handle that
* instead. Note however that all cleanup after a process must be done
* in process_cleanup, and that process_cleanup are already called
* from thread_exit - do not call cleanup twice! */
void process_exit(int status)
{
    plist_set_exit_status(&process_list, thread_current()->pid, status);
}

/* Print a list of all running processes. The list shall include all
* relevant debug information in a clean, readable format. */
void process_print_list()
{
    plist_print_processes(&process_list); 
}


struct parameters_to_start_process
{
  char* command_line;
  int proc_id;
  struct semaphore sema;
};

static void
start_process(struct parameters_to_start_process* parameters) NO_RETURN;

/* Starts a new proccess by creating a new thread to run it. The
process is loaded from the file specified in the COMMAND_LINE and
started with the arguments on the COMMAND_LINE. The new thread may
be scheduled (and may even exit) before process_execute() returns.
Returns the new process's thread id, or TID_ERROR if the thread
cannot be created. */
int
process_execute (const char *command_line)
{
  char debug_name[64];
  int command_line_size = strlen(command_line) + 1;
  tid_t thread_id = -1;
  int process_id = -1;

  /* The code here prepare the arguments to pass to start_process. */

  /* WARNING WARNING WARNING
*
* The `arguments' struct is a LOCAL variable. But a pointer to this
* variable is arranged (by thread_create below) to be passed as
* argument to start_process, when it start execution in the new
* thread. The `arguments' memory becomes invalid as soon as we
* return from process_execute. This creates a dependency between
* the thread running process_execute ant the thread running
* start_process. We MAY NOT return from process_execute until
* start_process is finished using the arguments.
*/
  struct parameters_to_start_process arguments;
  sema_init(&arguments.sema, 0); 
  
  arguments.proc_id = thread_current()->pid;

  debug("%s#%d: process_execute(\"%s\") ENTERED\n",
        thread_current()->name,
        thread_current()->tid,
        command_line);

  /* The new process, running in a new thread created below, need to
* know the command line that started it. Since the command line
* normally comes from the `parent' process, it must be copied from
* the memory space of that process to kernel memory to be
* accessible in the new process. (A process may only access it's
* own memory.) You are welcome to figure out a cleaner way to to
* this, that avoids copying the entire command line.
*/
  arguments.command_line = malloc(command_line_size);
  strlcpy(arguments.command_line, command_line, command_line_size);


  strlcpy_first_word (debug_name, command_line, 64);

  /* This creates a new thread to load and execute the new process.
	See threads/thread.c for more details. */
  thread_id = thread_create (debug_name, PRI_DEFAULT,
                             (thread_func*)start_process, &arguments);


  if(thread_id != -1){
     // Thread was successfully created and this means that we will run start_process()
     // which is critical, cant accept interrupts. Therefore lock the semaphore.
     sema_down(&arguments.sema);
  }
  else {
  	arguments.proc_id = -1;
  } 
  
  /* The task at hand is to create a new PROCESS. Above code created a
new THREAD. The process is created in the new thread, by the
start_process function. In this function you shall return:

-1 if either the THREAD could not be started (determined here)
-1 if the PROCESS cold not be started (determined in start_process)
PID if all activities lead to successful creationg of a new process

In the current solution the process received same id as the
thread. That is all-right, but other solutions may be more
practical.

Your problems are:
- How to transfer relevant information between the thraeds
- Avoiding to return before you know a process was created
- Avoiding to return before it is safe (see warnings above)

Assume that the ready queue contain the kernel threads IDLE and
OTHER when thread_create is called above. The thread PARENT is
executing. Then make sure you understand:

- Which of the threads are on the ready queue immediately after
the return of thread_create (assuming no thread switches had
time to occur)?

- Assume one thread switch occur after thread create. Which
threads are then on the ready queue, and which thread is
executing? (Two correct answers!)

- Assume another thread switch, back to PARENT thread, to
occur. Is it GUARANTEED that the function start_process have
executed to it's final line, or at all?

- Is it POSSIBLE that the newly created thread happen to be
scheduled and execute it's process to completion before
thread_create even return?

Note that some events may be UNLIKELY but still possible, or
LIKELY but not GUARANTEED. An implementation that depends on
likely or unlikely events is not acceptable. All code must be
GUARANTEED to work in any sequence of (the most unlikely or
likely) events.
*/
  process_id = arguments.proc_id;

  /* Read comments and warnings above and below. The following line
* will avoid bad stuff by turning off before they happen... THIS IS
* NOT A SOLUTION. You will fix it. */
 // power_off(); 

  /* WARNING WARNING WARNING
*
* As with the `arguments' struct variable above this PRESUMES that
* the thread running start_process will not use the memory anymore!
*/
  free(arguments.command_line);

  debug("%s#%d: process_execute(\"%s\") RETURNS %d\n",
        thread_current()->name,
        thread_current()->tid,
        command_line, process_id);

  return process_id;
}

/* Replace calls to STACK_DEBUG with calls to printf. All such calls * easily removed later by replacing with nothing. */
#define STACK_DEBUG(...) 
// printf(__VA_ARGS__)

/* "struct main_args" represent the stack as it must look when * entering main. The only issue: argv must point somewhere...
*
* The members of this structure is is directly related to The * arguments of main in a C-program, of course.
*
* int main(int argc, char* argv[]);
*
* (char** argv is a more generic form of char* argv[])
*
* The function pointer is normally handled by the compiler
* automatically, and determine the address at where a function shall * continue when it returns. The main function does not use it, as The * operating system arrange for the program to stop execution when * main is finished.
*/
struct main_args{
  /* Hint: When try to interpret C-declarations, read from right to * left! It is often easier to get the correct interpretation,
* altough it does not always work. */

  /* Variable "ret" that stores address (*ret) to a function taking not * parameters (void) and returning nothing. */
  void (*ret)(void);

  /* Just a normal integer. */
  int argc;

  /* Variable "argv" that stores address to an address storing char.
* That is: argv is a pointer to char*
*/
  char** argv;
}; //argc++;


/** CHANGED **/
void* setup_main_stack(const char* command_line, void* stack_top)
{
  /* Variable "esp" stores an address, and at the memory loaction
* pointed out by that address a "struct main_args" is found.
* That is: "esp" is a pointer to "struct main_args" */
  struct main_args* esp;
  int argc;
  int total_size;
  int line_size = 0;
  /* "cmd_line_on_stack" and "ptr_save" are variables that each store
* one address, and at that address (the first) char (of a possible
* sequence) can be found. */
  char* cmd_line_on_stack;

  char* lennart = malloc(strlen(command_line)+1);
  strlcpy(lennart, command_line, strlen(command_line) + 1);
  /* calculate the bytes needed to store the command_line */


  /* calculate how many words the command_line contain */
  char *token;
  char **save_ptr;
  argc = 0;
  for (token = strtok_r(lennart, " ", &save_ptr); token != NULL;
       token = strtok_r(NULL, " ", &save_ptr)) {
    line_size += strlen(token) + 1;

    STACK_DEBUG ("TOKEN: '%s'\n", token);
    argc++;
  }
  STACK_DEBUG("# line_size = %d\n", line_size);
  strlcpy(lennart, command_line, strlen(command_line) + 1);

  /* round up to make it even divisible by 4 */
  line_size = (line_size + 4 - 1) / 4 * 4;
  STACK_DEBUG("# line_size (aligned) = %d\n", line_size);

  // this will be arguments to main

  STACK_DEBUG("# argc = %d\n", argc);

  /* calculate the size needed on our simulated stack */

  // antal tecken, pekare till varje ord + en terminator pekare,
  // en pekare som pekar på argv[0], antal argument, esp
  total_size = sizeof(char) * line_size + sizeof(char*) * (argc + 1) + sizeof(char**) +
    + sizeof(int) + sizeof(int);

  STACK_DEBUG("# total_size = %d\n", total_size);

  /* calculate where the final stack top will be located */

  esp = (struct main_args*)((unsigned)stack_top - total_size);

  /* setup return address and argument count */
  esp->ret = NULL;
  esp->argc = argc;
  /* calculate where in the memory the argv array starts */
  esp->argv = (char**)((unsigned)esp + sizeof(int) * 3);
  /* calculate where in the memory the words is stored */
  cmd_line_on_stack = (char*)((unsigned)stack_top - line_size);

  /* copy the command_line to where it should be in the stack */
  STACK_DEBUG("\n %s\n", lennart);

  //pekar på argv[0]
  char** cur_argv_pointer = esp->argv;

  for (token = strtok_r(lennart, " ", &save_ptr); token != NULL;
       token = strtok_r(NULL, " ", &save_ptr)) {
    // Copies the C string pointed by source into the array pointed by destination,
    // including the terminating null character.
    strlcpy(cmd_line_on_stack, token, strlen(token) + 1);

    *cur_argv_pointer++ = cmd_line_on_stack;


    cmd_line_on_stack += (strlen(token) + 1) * sizeof(char);

    STACK_DEBUG ("TOKEN: '%s'\n", token);
  }

  /* build argv array and insert null-characters after each word */

  return esp; /* the new stack top */
}

/* A thread function that loads a user process and starts it
running. */
/* CHANGED */
static void
start_process (struct parameters_to_start_process* parameters)
{
  /* The last argument passed to thread_create is received here... */
  struct intr_frame if_;
  bool success;
  
  // We need to save parent pid before defaulting current process pid to ERROR.
  int parent_pid = parameters->proc_id;
  
  // Initialize process ID to ERROR and only if we could start process
  // correctly we set it to a non-error value.
  parameters->proc_id = -1;

  char file_name[64];
  strlcpy_first_word (file_name, parameters->command_line, 64);

  debug("%s#%d: start_process(\"%s\") ENTERED\n",
        thread_current()->name,
        thread_current()->tid,
        parameters->command_line);

  /* Initialize interrupt frame and load executable. */
  memset (&if_, 0, sizeof if_);
  if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
  if_.cs = SEL_UCSEG;
  if_.eflags = FLAG_IF | FLAG_MBS;

  success = load (file_name, &if_.eip, &if_.esp);

  debug("%s#%d: start_process(...): load returned %d\n",
        thread_current()->name,
        thread_current()->tid,
        success);

    

  if (success)
    {
      /* We managed to load the new program to a process, and have
	allocated memory for a process stack. The stack top is in
	if_.esp, now we must prepare and place the arguments to main on
	the stack. */

      /* A temporary solution is to modify the stack pointer to
	"pretend" the arguments are present on the stack. A normal
	C-function expects the stack to contain, in order, the return
	address, the first argument, the second argument etc. */

      // Set process ID to index in process_list. Thread ID is not equal to process ID!. 
      parameters->proc_id = plist_add_process(&process_list, parent_pid, thread_current()->name);
      
      //HACK if_.esp -= 12; /* Unacceptable solution. */
      if_.esp = setup_main_stack(parameters->command_line, if_.esp);


      /* The stack and stack pointer should be setup correct just before
the process start, so this is the place to dump stack content
for debug purposes. Disable the dump when it works. */

      // dump_stack ( PHYS_BASE + 15, PHYS_BASE - if_.esp + 16 );

    }
    thread_current()->pid = parameters->proc_id;
    
    
  //PLIST_DEBUG("\tStarted process (Name: %s, PID: %i, Parent PID: %i)\n", thread_current()->name, parameters->proc_id, parent_pid);

  //plist_print_processes(&process_list);

  debug("%s#%d: start_process(\"%s\") DONE\n",
        thread_current()->name,
        thread_current()->tid,
        parameters->command_line);

  // We have now executed our critical section, unlock semaphore.
  sema_up(&parameters->sema); 

  /* If load fail, quit. Load may fail for several reasons.
Some simple examples:
- File doeas not exist
- File do not contain a valid program
- Not enough memory
*/
  if ( ! success )
    {
      thread_exit ();
    }

  /* Start the user process by simulating a return from an interrupt,
implemented by intr_exit (in threads/intr-stubs.S). Because
intr_exit takes all of its arguments on the stack in the form of
a `struct intr_frame', we just point the stack pointer (%esp) to
our stack frame and jump to it. */
  asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (&if_) : "memory");
  NOT_REACHED ();
}

/* Wait for process `child_id' to die and then return its exit
status. If it was terminated by the kernel (i.e. killed due to an
exception), return -1. If `child_id' is invalid or if it was not a
child of the calling process, or if process_wait() has already been
successfully called for the given `child_id', return -1
immediately, without waiting.

This function will be implemented last, after a communication
mechanism between parent and child is established. */
int
process_wait (int child_id)
{
  int status = -1;
  struct thread *cur = thread_current ();

  debug("%s#%d: process_wait(%d) ENTERED\n",
        cur->name, cur->tid, child_id);
  /* Yes! You need to do something good here ! */
  debug("%s#%d: process_wait(%d) RETURNS %d\n",
        cur->name, cur->tid, child_id, status);

  return status;
}

/* Free the current process's resources. This function is called
automatically from thread_exit() to make sure cleanup of any
process resources is always done. That is correct behaviour. But
know that thread_exit() is called at many places inside the kernel,
mostly in case of some unrecoverable error in a thread.

In such case it may happen that some data is not yet available, or
initialized. You must make sure that nay data needed IS available
or initialized to something sane, or else that any such situation
is detected.
*/

void
process_cleanup (void)
{
  struct thread *cur = thread_current ();
  uint32_t *pd = cur->pagedir;
  int status = plist_get_exit_status_by_pid(&process_list, cur->pid);

  debug("%s#%d: process_cleanup() ENTERED\n", cur->name, cur->tid);

  /* Later tests DEPEND on this output to work correct. You will have
* to find the actual exit status in your process list. It is
* important to do this printf BEFORE you tell the parent process
* that you exit. (Since the parent may be the main() function,
* that may sometimes poweroff as soon as process_wait() returns,
* possibly before the prontf is completed.)
*/
  printf("%s: exit(%d)\n", thread_name(), status);
  
  plist_remove_process(&process_list, cur->pid);
  flist_close_process_files();
  /* Destroy the current process's page directory and switch back
to the kernel-only page directory. */
  if (pd != NULL)
    {
      /* Correct ordering here is crucial. We must set
cur->pagedir to NULL before switching page directories,
so that a timer interrupt can't switch back to the
process page directory. We must activate the base page
directory before destroying the process's page
directory, or our active page directory will be one
that's been freed (and cleared). */
      cur->pagedir = NULL;
      pagedir_activate (NULL);
      pagedir_destroy (pd);
    }
  debug("%s#%d: process_cleanup() DONE with status %d\n",
        cur->name, cur->tid, status);
}

/* Sets up the CPU for running user code in the current
thread.
This function is called on every context switch. */
void
process_activate (void)
{
  struct thread *t = thread_current ();

  /* Activate thread's page tables. */
  pagedir_activate (t->pagedir);

  /* Set thread's kernel stack for use in processing
interrupts. */
  tss_update ();
}	
