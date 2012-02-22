#include <stdlib.h>
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



#define DEBUG_SYSCALL(format, ...) //printf(format "\n", ##__VA_ARGS__)

static void syscall_handler (struct intr_frame *);

int SYS_READ_handler(int32_t* esp);
int SYS_WRITE_handler(int32_t* esp);

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

int SYS_READ_handler(int32_t* esp)
{

  int fd = *(esp + 1);
  char* buffer = (char*)*(esp + 2);
  int len = *(esp + 3);

  // Default to error...
  int retVal = -1;
  if(fd ==  STDIN_FILENO){
    int writtenCharacters = 0;
    while(writtenCharacters < len){
      uint8_t c = input_getc ();

      // Replace all \r with \n because output
      // will be weird looking otherwise
      if(c == '\r'){
        c = '\n';
      }

      *(buffer + writtenCharacters) = c;

      // Print typed character on screen
      putbuf(&c, 1);
      writtenCharacters++;
    }
    // We read data, set return value to bytes read.
    retVal = writtenCharacters;
  }
  return retVal;
}

int SYS_WRITE_handler(int32_t* esp)
{
  // Default to error...
  int retVal = -1;

  int fd = *(esp + 1);
  char* buffer = (char*)*(esp + 2);
  int len = *(esp + 3);

  if(fd == STDOUT_FILENO){

    putbuf(buffer, len);
    // Since we wrote data, set return value to bytes written.
    retVal = len;
  }

  return retVal;
}

char* get_system_call_name(int32_t syscall_number)
{
  char* system_calls[SYS_NUMBER_OF_CALLS];

  int i = 0;
  for(i = 0; i < SYS_NUMBER_OF_CALLS; i++){
    system_calls[i] = "Unknown system call!";
  }

  system_calls[SYS_HALT] = "SYS_HALT";
  system_calls[SYS_EXIT] = "SYS_EXIT";
  system_calls[SYS_EXEC] = "SYS_EXEC";
  system_calls[SYS_WAIT] = "SYS_WAIT";
  system_calls[SYS_WAIT] = "SYS_READ";
  system_calls[SYS_WRITE] = "SYS_WRITE";

  // TODO: Add more syscall names whenever they are implemented

  if(syscall_number > 0 && syscall_number < SYS_NUMBER_OF_CALLS){
    return system_calls[syscall_number];
  }
  return 'E';
}

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
    ESP --->    <syscall_number>

    --------------------------------------
  */
  int32_t syscall_nr = *esp;
  DEBUG_SYSCALL("# SYSCALL received = %s\n", get_system_call_name(syscall_nr));
  switch (syscall_nr)
    {
      //SYS_CREATE,                 /* Create a file. */
      //SYS_OPEN,                   /* Open a file. */
      //SYS_CLOSE,                  /* Close a file. */
      //SYS_REMOVE,                 /* Delete a file. */


    case SYS_HALT:
      DEBUG_SYSCALL("# Halt received, force shutdown...\n");
      power_off();
      break;
    case SYS_EXIT:
      DEBUG_SYSCALL("# Exitting thread from syscall...\n");
      thread_exit();
      break;
    case SYS_CREATE:
      {
	//TODO: should filesys_init in filesys.c be called first, or does the
	//system handle this?

        DEBUG_SYSCALL("# RECEIVED SYS_CREATE \n");
        bool success;

        char *name = (char*)*(esp + 1); //TODO: cast to (char*)? Why?
        unsigned initial_size = *(esp + 2);

        success = filesys_create(name, initial_size);

        if(success) {
	  DEBUG_SYSCALL("#SYS_CREATE - File with name: %s created. \n", name);
          
        } else {
	  DEBUG_SYSCALL("#SYS_CREATE - filesys_create failed: file named NAME already exists  or internal memory allocation failed \n");
	}

	f->eax = success;
        break;
      }
    case SYS_READ:
      {
        DEBUG_SYSCALL("# RECEIVED SYS_READ \n");
        int retVal = SYS_READ_handler(esp);
        f->eax = retVal;
        break;
      }
    case SYS_WRITE:
      {
        // Write to screen plz
        //DEBUG_SYSCALL("SYS_WRITE received, FD=%i, buf=%s, len=%i", fd, buffer, len);
        int retVal = SYS_WRITE_handler(esp);
        f->eax = retVal;
        break;
      }
    default:
      {
        DEBUG_SYSCALL ("Executed an unknown system call (nr: %i)!\n", syscall_nr);
        DEBUG_SYSCALL ("Stack top + 0: %d\n", esp[0]);
        DEBUG_SYSCALL ("Stack top + 1: %d\n", esp[1]);

        thread_exit ();
      }
    }
}
