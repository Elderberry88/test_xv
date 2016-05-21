// shop_input - create input file for shop.c to read
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_LINES 9
char* buf[MAX_LINES] = 
  {
	"Avi Gali 20 0",
	"Beni addidas 40 0 assics 43 0 addidas 44 1",
	"Gadi Vans 39 0 assics 42 0 addidas 41 0 shoe1 40 0 shoe2 40 1",
	"Dani assics 40 0 XYZ 42 0 addidas 41 1",
	"Hagar Prada 30 0 Prada 31 0 Prada 32 0",
	"Vered AAA 20 0 BBB 30 0 CCC 40 1",
	"Zeev  Sandal 40 1",
	"Hava Teva 35 1",
	"Tova Naot 36 1"
  };

void help()
{  
  printf(2, "shop_input: create customer input file for shop program\n"
             "usage:   shop_input file-name Number-of-lines\n"
			 "   e.g.  - the following command:\n"
			 "         shop_input f1 5\n"
			 "   creates a file named f1, with 5 lines (5 customers)\n"
			 "  Note: at most a file with 9 lines is created\n"
		  );
}

void
my_cat(int fd, int n)
{
  int i;
  // for (i=0; i<n && i < MAX_LINES; i++)
  for (i=0; i<n ; i++)
  {
    printf(1, "line: %s\n", buf[i]);
    printf(fd, "%s\n", buf[i]);
  }
}

int
main(int argc, char *argv[])
{
  int fd=0, n;

  
  if(argc != 3){
    help();
    exit();
  }

  if((fd = open(argv[1], O_CREATE | O_RDWR )) < 0){
    printf(1, "cat: cannot open %s\n", argv[1]);
    exit();
  }

  n = atoi(argv[2]);
  my_cat(fd, n);
  close(fd);
  exit();
}

