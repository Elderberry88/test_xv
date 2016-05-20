// file: flock_common.c

//==== includes ====
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

//==== defines ====
#define NUM_PROCS  1     
#define NUM_PRINTS 2
#define BURN_TIME  10
   
//==== prototypes ====
void burn_time();

//=================================================================
void update_file(int fd)
{
  int i;

  for (i=0; i< NUM_PRINTS; i++)
  {
    //----------------------------------
    // The critical section
    //----------------------------------
  
	// now lock... using flock
	flock_ex(fd);

	write(fd, "I ", 2) ;
	write(fd, "am ", 3) ;
	write(fd, "process ", 8) ;
	write(fd, "#", 1) ;
	printf(fd, "%d", getpid());
	write(fd, "\n", 1) ;
	int j;
	char c;
	for (j=0; j<NUM_PRINTS; j++) 
	{
		burn_time();
		printf(fd, "(process %d) ", getpid());
		c='A';
		for (c='A'; c<='Z'; c++) {
			write(fd, &c, 1);
			// burn_time();
		}
		write(fd, "\n", 1) ; // writes new-line
	}
  }
  //----------------------------------
  // END OF The critical section
  //----------------------------------
  // free lock
  flock_un(fd);
}

void burn_time() {
	double k;
	int j;
	for (j=0; j<BURN_TIME;j++)
		k=((double)j/k);
	sleep((getpid()%3 +1)*2);
}

void print_help(char *name) 
{
	printf(2,
	    "%s, invocation is:\n"
	    "%s <num> <file-name>\n"
		
	    "For example:\n"
	    "   %s 5 temp\n"
	    "The program creates 5 child processes and they write to file \'temp\'\n\n",
		name, name, name);
}
