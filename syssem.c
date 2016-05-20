//
// Semaphore-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into mutex.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "mutex.h"

// Fetch the nth word-sized system call argument as a semaphore descriptor
// and return both the descriptor and the corresponding struct semaphore.
static int
argsd(int n, int *psd, struct sem **ps)
{
  int sd;
  struct sem *s;

  if(argint(n, &sd) < 0)
    return -1;
  if(sd < 0 || sd >= NOSEM || (s=proc->osem[sd]) == 0)
    return -1;
  if(psd)
    *psd = sd;
  if(ps)
    *ps = s;
  return 0;
}

// Allocate a semaphore descriptor for the given semaphore.
// Takes over semaphore reference from caller on success.
static int
sdalloc(struct sem *s)
{
  int sd;

  for(sd = 0; sd < NOSEM; sd++){
    if(proc->osem[sd] == 0){
      proc->osem[sd] = s;
      return sd;
    }
  }
  return -1;
}

int
sys_sem_open(void)
{
  char *name;
  int sd, create, init, max_val;
  struct sem *s;

  if(argstr(0, &name) < 0
      || argint(1, &create) < 0
      || argint(2, &init) < 0
      || argint(3, &max_val) < 0)
    return -1;

  if (strlen(name) > sizeof(s->name) -1)
    return -1;

  if (init > max_val)
    return -1;

  if (init < 0 || max_val < 0)
    return -1;

  s = k_sem_find(name);
  if (!s)
  {
    if (!create)
      return -1;
    else
    {
      s = k_sem_alloc();
      if (!s) /* No free semaphores */
        return -1;

      safestrcpy(s->name, name, 6);
      s->value = init;
      s->max_val = max_val;
      sd = sdalloc(s);
      if (sd == -1)
      {
        k_sem_close(s);
        return -1;
      }
    }
  }

  return sd;
}

int
sys_sem_close(void)
{
  int sd;
  struct sem *s;

  if (argsd(0, &sd, &s) < 0)
    return -1;

  proc->osem[sd] = 0;
  k_sem_close(s);
  return 0;
}

int
sys_sem_wait(void)
{
  int sd;
  struct sem *s;

  if (argsd(0, &sd, &s) < 0)
    return -1;

  return k_sem_wait(s);
}

int
sys_sem_trywait(void)
{
  int sd;
  struct sem *s;

  if (argsd(0, &sd, &s) < 0)
    return -1;

  return k_sem_trywait(s);
}

int
sys_sem_signal(void)
{
  int sd;
  struct sem *s;

  if (argsd(0, &sd, &s) < 0)
    return -1;

  return k_sem_signal(s);
}

/* vim: set tabstop=2 shiftwidth=2 expandtab: */
