#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"

#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/thread.h"

// TODO: dont forget, when usinbg this function. check if already opened
int flist_add_file(struct file *file)
{
  
  struct thread* cur_thread = thread_current();  
  int fd = map_insert(&(cur_thread->process_open_files), file);
  
  return fd;
}

struct file* flist_get_process_file(const int* fd)
{
	struct thread* cur_thread = thread_current();  
	struct file* file = map_find(&(cur_thread->process_open_files), (key_t)fd);

	return file;

}

void flist_remove_process_file(const int* fd)
{
	struct file* file = flist_get_process_file(fd);
	if(file != NULL){
		file_close(file);

		struct thread* cur_thread = thread_current();  
		map_remove(&(cur_thread->process_open_files), fd);
	}  
}

void close_process_open_file(key_t k, value_t v, int aux)
{
	file_close(v);
}

void flist_close_process_files()
{
	struct thread* cur_thread = thread_current();
	map_for_each(&(cur_thread->process_open_files), close_process_open_file, 0);	
}


