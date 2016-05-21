#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void
assert(int v, const char * err_msg)
{
  if (!v)
  {
    printf(1, "ERROR: %s\n", err_msg);
  }
  else
  {
    printf(1, "OK   : %s\n", err_msg);
  }
}

void
test_bad_fd(void)
{
  assert(flock_ex(-1) == -1, "flock_ex() bad fd");
  assert(flock_un(-1) == -1, "flock_un() bad fd");
}

void
test_simple(void)
{
  int fd = open("cat", O_RDONLY);
  assert(flock_ex(fd) == 0, "flock_ex() simple");
  assert(flock_un(fd) == 0, "flock_un() simple");
  close(fd);
}

void
test_double_lock(void)
{
  int fd = open("cat", O_RDONLY);
  assert(flock_ex(fd) == 0, "flock_ex() double");
  assert(flock_ex(fd) == 1, "flock_ex() double");
  assert(flock_un(fd) == 0, "flock_un() double");
  close(fd);
}

void
test_auto_close(void)
{
  if (fork() == 0)
  {
    sleep(1);
    int fd = open("cat", O_RDONLY);
    assert(flock_ex(fd) == 0, "flock_ex() auto close [child]");
    close(fd);
    exit();
  }
  else
  {
    int fd = open("cat", O_RDONLY);
    assert(flock_ex(fd) == 0, "flock_ex() auto close [parent]");
    sleep(3);
    close(fd);
    wait();
  }
}

int
main(int argc, char *argv[])
{
  test_bad_fd();
  test_simple();
  test_double_lock();
  test_auto_close();
  exit();
}

/* vim: set tabstop=2 shiftwidth=2 expandtab: */
