#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int
main(int argc, char **argv)
{
  int fd, i, seek_num, write_count;
  char fill_char, write_char;
  char *path;

  if (argc != 6)
  {
    printf(1, "Usage: %s <file-name> <seek-num> <fill-char> <write-count> <write-char>\n", argv[0]);
    exit();
  }

  path = argv[1];
  seek_num = atoi(argv[2]);
  fill_char = argv[3][0];
  write_count = atoi(argv[4]);
  write_char = argv[5][0];

  if((fd = open(argv[1], O_CREATE|O_RDWR)) < 0) {
    printf(2, "%s: cannot open %s\n", argv[0], path);
    exit();
  }

  if (lseek(fd, seek_num, fill_char) < 0) {
    printf(2, "%s: cannot seek %s\n", argv[0], path);
    close(fd);
    exit();
  }

  for (i = 0; i < write_count; i++) {
    write(fd, &write_char, 1);
  }

  exit();
}
/* vim: set tabstop=2 shiftwidth=2 expandtab: */
