#include <stddef.h>
#include <stdlib.h>

#include "flist.h"
#include "map.h"
//#include "thread/thread.h"

// Contains all open files, system-wide
static struct map open_inode_table;

// processes opened files
static struct map open_file_tables;

static bool inited = false;

// an open file for a process
struct open_file_table
{
  int process_id;
  rw_pos read_write_pos;
};


//TODO: When should this be called?
bool flist_init(void)
{
  map_init(&open_inode_table);
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

// TODO: dont forget, when using this function. check if already opened
int flist_add_file(struct file *file, const int process_id)
{
  //TODO: how do I solve this more neatly?
  if(!inited) {
    flist_init();
  }

  map_insert(&open_inode_table, file->inode);
  map_find(&open_inode_table, 0);
  //../../userprog/flist.c:49: varning: implicit declaration of function `malloc' ?????????????
  /*struct open_file_table *opf = (struct open_file_table*)malloc(sizeof(struct open_file_table));

  //TODO: do I need to malloc?
  int* pidPtr = (int*)malloc(sizeof(int));
  pidPtr = process_id;
  opf->process_id = pidPtr;

  opf->read_write_pos = 0;

  map_insert(&open_file_tables,opf);*/
  return 1;
}


//thread_tid();
