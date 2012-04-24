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
#include "userprog/flist.h"



#define DEBUG_SYSCALL(format, ...) // printf(format "\n", ##__VA_ARGS__)

static void syscall_handler (struct intr_frame *);

int SYS_READ_handler(int32_t* esp);
int SYS_WRITE_handler(int32_t* esp);
int SYS_CLOSE_handler(int32_t* esp);
int SYS_REMOVE_handler(int32_t* esp);


int SYS_SEEK_handler(int32_t* esp);
int SYS_FILESIZE_handler(int32_t* esp);
int SYS_TELL_handler(int32_t* esp);


int SYS_EXEC_handler(int32_t* esp);


char* get_system_call_name(int32_t syscall_number);

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


	DEBUG_SYSCALL(" # SYS_READ fd: %i ", fd);

  // Default to error...
  int retVal = -1;
  if(fd == STDIN_FILENO){
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
  else if(fd > 1){

	// A file descriptor has been used.
	struct file* file = flist_get_process_file(fd);
	if(file != NULL){
		retVal = file_read(file, buffer, len);
		//printf("\n\nFile with fd %i found, retVal: %i, len: %i\n", fd, retVal, len);
	}  else{
		//printf("\nCouldnt find file with FD %i\n", fd);

	}
	
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
  } else if(fd > 1){

	// A file descriptor has been used.
	struct file* file = flist_get_process_file(fd);
	if(file != NULL){
		retVal = file_write(file, buffer, len);
		//printf("\n\nFile with fd %i found, retVal: %i, len: %i\n", fd, retVal, len);
	}  else{
		//printf("\nCouldnt find file with FD %i\n", fd);

	}
	
  }

  return retVal;
}



int SYS_REMOVE_handler(int32_t* esp)
{
  // Default to error...
  int retVal = -1;

  char *name = (char*)*(esp + 1);
  /*if(fd > 1){

	/// A file descriptor has been used.
	//flist_remove_process_file(fd);	
  }*/
  retVal = filesys_remove(name);

  return retVal;
}


int SYS_CLOSE_handler(int32_t* esp)
{
  // Default to error...
  int retVal = -1;

  int fd = *(esp + 1);
  if(fd > 1){

	// A file descriptor has been used.
	flist_remove_process_file(fd);
	
  }

  return retVal;
}

int SYS_SEEK_handler(int32_t* esp)
{
	
	int fd = *(esp + 1);
	int position = *(esp + 2);

	//printf("# SYS_SEEK FD %i, POS: %i\n\n", fd, position);

	if(fd > 1){

		// A file descriptor has been used.
		struct file* file = flist_get_process_file(fd);
		if(file != NULL && position >= 0 && position <= file_length(file)){
			// File is considered OPEN if it is process open file table.
			file_seek(file, (off_t)position);
		}  
	}
	return 0;
}


int SYS_TELL_handler(int32_t* esp)
{
	int fd = *(esp + 1);
	int retVal = 0;
	if(fd > 1){

		// A file descriptor has been used.
		struct file* file = flist_get_process_file(fd);
		if(file != NULL){
			// File is considered OPEN if it is process open file table.
			retVal = file_tell(file);
		} 
	
	}
	return retVal;
}

int SYS_FILESIZE_handler(int32_t* esp)
{
	int fd = *(esp + 1);
	int retVal = 0;
	if(fd > 1){

		// A file descriptor has been used.
		struct file* file = flist_get_process_file(fd);
		if(file != NULL){
			// File is considered OPEN if it is process open file table.
			retVal = file_length(file);
		} 	
	}
	return retVal;
}


int SYS_EXEC_handler(int32_t* esp)
{
	
    char *command_line = (char*)*(esp + 1);
	int retVal = 0;
	
    retVal = process_execute(command_line);	
	return retVal;
}



char* get_system_call_name(int32_t syscall_number)
{
  char* system_calls[SYS_NUMBER_OF_CALLS];
  int i = 0;
  for(i = 0; i < SYS_NUMBER_OF_CALLS; i++){
    //	system_calls[i] = "Syscall " + (char)syscall_number;
  }

  system_calls[SYS_HALT] = "SYS_HALT";
  system_calls[SYS_EXIT] = "SYS_EXIT";

  system_calls[SYS_EXEC] = "SYS_EXEC";

  
  system_calls[SYS_CREATE] = "SYS_CREATE";
  system_calls[SYS_REMOVE] = "SYS_REMOVE";
  system_calls[SYS_OPEN] = "SYS_OPEN";
  system_calls[SYS_FILESIZE] = "SYS_FILESIZE";
  system_calls[SYS_REMOVE] = "SYS_REMOVE";
  system_calls[SYS_READ] = "SYS_READ";
  system_calls[SYS_WRITE] = "SYS_WRITE";
  system_calls[SYS_WAIT] = "SYS_WAIT";
  system_calls[SYS_SEEK] = "SYS_SEEK";
  system_calls[SYS_TELL] = "SYS_TELL";
  system_calls[SYS_CLOSE] = "SYS_CLOSE";
  system_calls[SYS_PLIST] = "SYS_PLIST";


  // TODO: Add more syscall names whenever they are implemented

  if(syscall_number > 0 && syscall_number < SYS_NUMBER_OF_CALLS){
    return system_calls[syscall_number];
  }
  return "Unknown syscall";
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
  
    case SYS_HALT:
      power_off();
      break;
    case SYS_EXEC:
    {
      f->eax = SYS_EXEC_handler(esp);
    }
    break;
  
    case SYS_WAIT:
    {
    
      int child_pid = *(esp + 1);
      f->eax = process_wait (child_pid);
      break; 
    }
  
    case SYS_EXIT:
    {
      int exit_status = *(esp + 1);
      // printf("SYS_EXIT: exit status for PID %d: %d\n", thread_current()->pid, exit_status);
      process_exit(exit_status);
      thread_exit();
      break;
    }
    case SYS_PLIST:
        process_print_list();
    break;
    case SYS_CREATE:
      {
        //TODO: should filesys_init in filesys.c be called first, or does the
        //system handle this?

        bool success = false;

        char *name = (char*)*(esp + 1); //TODO: cast to (char*)? Why?
        unsigned initial_size = *(esp + 2);

        success = filesys_create(name, initial_size);

        if(success) {
          DEBUG_SYSCALL("#SYS_CREATE - File with name: %s created. \n", name);

        } else {
          DEBUG_SYSCALL("#SYS_CREATE - filesys_create failed: file named \'%s\' already exists  or internal memory allocation failed \n", name);
        }

        f->eax = success;
        break;
      }
    case SYS_OPEN:
      {
        char *name = (char*)*(esp + 1);
        struct file *file;
        file = filesys_open(name);


	int retVal = -1;
        if(file != NULL) {
          DEBUG_SYSCALL("# SYS_OPEN - File with name: '%s' created. \n", name);

	  int fd = flist_add_file(file);
	  retVal = fd;
        } else {
          DEBUG_SYSCALL("# SYS_OPEN - filesys_open failed: no file named \'%s\' exists or internal memory allocation failed \n", name);
          retVal = -1;

        }
	f->eax = retVal;
        break;
      }
    case SYS_READ:
      {
        int retVal = SYS_READ_handler(esp);

        f->eax = retVal;
        break;
      }
    case SYS_CLOSE:
	{
	int retVal = SYS_CLOSE_handler(esp);
	f->eax = retVal;
	
	break;
	}
    case SYS_REMOVE:
	{
		int retVal = SYS_REMOVE_handler(esp);
		f->eax = retVal;
		break;
	}
    case SYS_WRITE:
      {
        int retVal = SYS_WRITE_handler(esp);
        f->eax = retVal;
        break;
      }
    case SYS_SEEK:
	{
	f->eax = SYS_SEEK_handler(esp);
	break;
	}
    case SYS_TELL:

	f->eax = SYS_TELL_handler(esp);
	break;
	

    case SYS_FILESIZE:
	f->eax = SYS_FILESIZE_handler(esp);
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
