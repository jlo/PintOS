#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"
//#include "filesys/file.h"
#include "filesys/inode.h"
//#include "thread/thread.h"

// Contains all open files, system-wide
static struct map open_inode_table;

// processes opened files
static struct map process_open_files;

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
  map_init(&process_open_files);
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
  int fd;

  fd = (int)map_insert(&open_inode_table, file->inode);

  printf("# fd: %d inode: %i", fd);//,*( file->inode)->removed);
  //*(file->inode)->removed = true;
  
  struct process_open_file_entry *pofe;

  // TODO: dont forget to free this
  // ../../userprog/flist.c:65: varning: implicit declaration of function `malloc' 
  // Why do this happen?? Something with that we dont include right lib with malloc?
  pofe = (struct process_open_file_entry*)malloc(sizeof(struct process_open_file_entry));

  //  printf("# h1i %08x \n", &(*file));

  pofe->fd = fd;
  pofe->file = file;
  map_insert(&process_open_files, pofe);
  
  //struct process_open_file_entry* s = (struct process_open_file_entry*)map_find(&process_open_files, 0);
  //printf("# h2i %08x \n", &(*s->file));
  
  return (int)fd;
}


//thread_tid();
