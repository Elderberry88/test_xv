// file: flock_good.c

//==== includes ====
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
// #include "flock_common.c"

//==== defines ====
#define NUM_PROCS  1
#define NUM_PRINTS 2
#define BURN_TIME  10

//==== prototypes ====
void do_child(char* file_name);
void update_file(int);
void print_help(char *);

/*====================================
 main
====================================*/
int
main(int argc, char *argv[])
{
  int child_pid;
  int i;

  if (argc != 3) {
    printf(2, "Wrong number of Arguments:\n\n");
	print_help("flock_good");
	exit();
  }
  int num_procs = atoi(argv[1]);
  char *file_name = argv[2];


  for (i=0; i< num_procs; i++) {
    child_pid = fork();
    switch (child_pid) {
    case -1:
      printf(2,"fork failed");
      exit();
    case 0:
      do_child(file_name);
      exit();
    default:
      break;
    }
  }

  /* wait for all children to finish running */
  for (i=0; i<num_procs; i++)
    wait();

  printf(1, "main: we're done\n");
  exit();
}

/*====================================
 do_child
====================================*/
void do_child(char* file_name)
{
  int fd;

  if ((fd = open(file_name, O_CREATE | O_RDWR | O_APPEND))==-1)
  {
    printf(2,"Open failed");
    exit();
  }

  update_file(fd);
  close(fd);
}


