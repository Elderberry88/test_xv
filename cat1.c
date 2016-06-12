#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char **argv)
{
  int fd, offset, num, i;
  char *path, c[2];

  if (argc != 4)
  {
    printf(1, "Usage: %s <file> <offset> <num>\n", argv[0]);
    exit();
  }

  path = argv[1];
  offset = atoi(argv[2]);
  num = atoi(argv[3]);

  if((fd = open(argv[1], 0)) < 0) {
    printf(2, "%s: cannot open %s\n", argv[0], path);
    exit();
  }

  if (lseek(fd, offset, '\0') < offset) {
    printf(2, "%s: cannot seek %s:%d\n", argv[0], path, offset);
    close(fd);
    exit();
  }

  c[1] = '\0';
  for (i = 0; i < num; i++) {
    read(fd, &c, 1);
    printf(1, "%s", c);
  }

  printf(1, "\n");

	exit();
}
/* vim: set tabstop=2 shiftwidth=2 expandtab: */
