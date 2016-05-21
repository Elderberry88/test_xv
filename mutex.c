#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "file.h"
#include "spinlock.h"
#include "mutex.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"

int
flock_acquire(struct file *f)
{
  struct mutex *m;
  struct mutex *free_mutex = 0;

  if (f->type != FD_INODE)
  {
    return -1;
  }

  acquire(&mtable.lock);
try_acquire:
  for(m = mtable.mutex; m < mtable.mutex + NMUTEX; m++)
  {
    if(m->file == f) /* file is already locked by us */
    {
      release(&mtable.lock);
      return 1;
    }

    if (m->file->ip == f->ip)
    {
      sleep(f->ip, &mtable.lock);
      if (proc->killed)
      {
        release(&mtable.lock);
        return -1;
      }

      goto try_acquire;
    }

    if (!free_mutex && m->file == 0)
    {
      free_mutex = m;
    }
  }

  if (free_mutex == 0) /* no free mutex */
  {
    release(&mtable.lock);
    return -2;
  }

  free_mutex->file = f;

  release(&mtable.lock);
  return 0;
}

int
flock_release(struct file *f)
{
  struct mutex *m;

  if (f->type != FD_INODE)
  {
    return -1;
  }

  acquire(&mtable.lock);
  for(m = mtable.mutex; m < mtable.mutex + NMUTEX; m++)
  {
    if(m->file == f) /* file is locked by us */
    {
      m->file = 0;
      wakeup(f->ip);
      release(&mtable.lock);
      return 0;
    }
  }

  release(&mtable.lock);
  /* file is not locked */
  return -2;
}

struct sem*
k_sem_alloc(const char *name, int init, int max_val)
{
  struct sem *s;

  acquire(&stable.lock);
  for(s = stable.sem; s < stable.sem + NSEM; s++){
    acquire(&s->lock);
    if(s->ref == 0){
      s->ref = 1;
      safestrcpy(s->name, name, 6);
      s->value = init;
      s->max_val = max_val;
      release(&s->lock);
      release(&stable.lock);
      return s;
    }

    release(&s->lock);
  }

  release(&stable.lock);
  return 0;
}

struct sem*
k_sem_find(const char *name)
{
  struct sem *s;
  acquire(&stable.lock);
  for(s = stable.sem; s < stable.sem + NSEM; s++){
    acquire(&s->lock);
    if(s->ref > 0 && strncmp(s->name, name, sizeof(s->name)) == 0)
    {
      release(&s->lock);
      release(&stable.lock);
      return s;
    }

    release(&s->lock);
  }

  release(&stable.lock);
  return 0;
}

struct sem*
k_sem_dup(struct sem *s)
{
  acquire(&s->lock);
  if(s->ref < 1)
    panic("sem_dup");
  s->ref++;
  release(&s->lock);
  return s;
}

// Close semaphore s.  (Decrement ref count, close when reaches 0.)
void
k_sem_close(struct sem *s)
{
  acquire(&s->lock);
  if(s->ref < 1)
    panic("semclose");
  if(--s->ref > 0){
    release(&s->lock);
    return;
  }

  s->name[0] = '\0';
  s->value = 0;
  s->max_val = 0;
  release(&s->lock);
}

int
k_sem_trywait(struct sem *s)
{
  acquire(&s->lock);
  if (s->value == 0)
  {
    release(&s->lock);
    return -1;
  }

  s->value--;
  release(&s->lock);
  return 0;
}

int
k_sem_wait(struct sem *s)
{
  acquire(&s->lock);
  while (s->value == 0)
  {
    sleep(s->name, &s->lock);
    if (proc->killed)
    {
      release(&s->lock);
      return -1;
    }
  }

  s->value--;
  release(&s->lock);
  return 0;
}

int
k_sem_signal(struct sem *s)
{
  acquire(&s->lock);
  if (s->value == s->max_val)
  {
    release(&s->lock);
    return -1;
  }

  s->value++;
  wakeup(s->name);
  release(&s->lock);
  return 0;
}

/* vim: set tabstop=2 shiftwidth=2 expandtab: */
