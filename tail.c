#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char **argv)
{
  int fd, num, offset, i;
  struct stat st;
  char *path, c[2];

  if (argc != 3)
  {
    printf(1, "Usage: %s <file> <num>\n", argv[0]);
    exit();
  }

  path = argv[1];
  num = atoi(argv[2]);

  if((fd = open(argv[1], 0)) < 0) {
    printf(2, "%s: cannot open %s\n", argv[0], path);
    exit();
  }

  if(fstat(fd, &st) < 0) {
    printf(2, "%s: cannot stat %s\n", argv[0], path);
    close(fd);
    exit();
  }

  if ((offset = lseek(fd, st.size - num, '\0')) < 0) {
    printf(2, "%s: cannot seek %s\n", argv[0], path);
    close(fd);
    exit();
  }

  c[1] = '\0';
  for (i = offset; i < st.size; i++) {
    read(fd, &c, 1);
    printf(1, "%s", c);
  }

  printf(1, "\n");

	exit();
}
/* vim: set tabstop=2 shiftwidth=2 expandtab: */
