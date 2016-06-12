#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char **argv)
{
  int fd;
  struct stat st;

  if (argc != 2)
  {
    printf(1, "Usage: %s <file>\n", argv[0]);
    exit();
  }

  if((fd = open(argv[1], 0)) < 0){
    printf(2, "%s: cannot open %s\n", argv[0], argv[1]);
    exit();
  }

  if(fstat(fd, &st) < 0){
    printf(2, "%s: cannot stat %s\n", argv[0], argv[1]);
    close(fd);
    exit();
  }

  close(fd);

  printf(1, "size of file %s is: %d\n", argv[1], st.size);

	exit();
}
/* vim: set tabstop=2 shiftwidth=2 expandtab: */
