#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"

#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/thread.h"

#include "threads/synch.h"

static struct lock flist_lock;


void flist_init()
{
	lock_init(&flist_lock);
}

// TODO: dont forget, when usinbg this function. check if already opened
int flist_add_file(struct file *file)
{
  lock_acquire(&flist_lock);
  struct thread* cur_thread = thread_current();  
  int fd = map_insert(&(cur_thread->process_open_files), (value_t)file);
  
  lock_release(&flist_lock);
  return fd;
}

struct file* flist_get_process_file(const int fd)
{

  lock_acquire(&flist_lock);
	struct thread* cur_thread = thread_current();  
	struct file* file = map_find(&(cur_thread->process_open_files), (key_t)fd);

  lock_release(&flist_lock);
   return file;

}

struct file* internal_flist_get_process_file(const int fd)
{
	struct thread* cur_thread = thread_current();  
	struct file* file = map_find(&(cur_thread->process_open_files), (key_t)fd);

	return file;

}


void flist_remove_process_file(const int* fd)
{

  lock_acquire(&flist_lock);
	struct file* file = internal_flist_get_process_file(fd);//flist_get_process_file(fd);
	if(file != NULL){
		file_close(file);

		struct thread* cur_thread = thread_current();  
		map_remove(&(cur_thread->process_open_files), fd);
	}  

  lock_release(&flist_lock);
}

void close_process_open_file(key_t k UNUSED, value_t v, int aux UNUSED)
{
	file_close(v);
}



void flist_close_process_files()
{

  lock_acquire(&flist_lock);
	struct thread* cur_thread = thread_current();
	map_for_each(&(cur_thread->process_open_files), close_process_open_file, 0);

  lock_release(&flist_lock);	
}


