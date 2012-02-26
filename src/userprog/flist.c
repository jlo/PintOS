#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"
//#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/thread.h"

// Contains all open files, system-wide
static struct map open_inode_table;

static bool inited = false;

// an open file for a process
struct process_open_file_entry
{
  int fd;
  struct file *file;
};

//TODO: When should this be called?
bool flist_init(void)
{
  map_init(&open_inode_table);
  //map_init(&process_open_files);
  map_insert(&open_inode_table, "A");
  map_insert(&open_inode_table, "A");
  inited = true;

  return true;
}

void echo(void)
{
  //TODO: how do I solve this more neatly?
  if(!inited) {
    flist_init();
  }

  printf("#Found: %s \n", map_find(&open_inode_table, 0));
}

// TODO: dont forget, when usinbg this function. check if already opened
int flist_add_file(struct file *file, const int process_id)
{
  //TODO: how do I solve this more neatly?
  if(!inited) {
    flist_init();
  }

  // store open file in open_inode_table and return file descriptor
  int fd = (int)map_insert(&open_inode_table, file->inode);
      
  struct process_open_file_entry *pofe;

  // allocate memory for entry in open_file_table
  pofe = (struct process_open_file_entry*)malloc(sizeof(struct process_open_file_entry));

  pofe->fd = fd;
  pofe->file = file;
  
  struct thread* cur_thread = thread_current();  
  map_insert(&(cur_thread->process_open_files), pofe);
  
  return fd;
}


//thread_tid();
