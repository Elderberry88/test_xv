
struct mutex {
  struct file *file;
};

struct {
  struct spinlock lock;
  struct mutex mutex[NMUTEX];
} mtable;

struct sem {
  struct spinlock lock; // single semaphore lock
  char name[6];
  int ref;
  int value;
  int max_val;
};

struct {
  struct spinlock lock; //global semaphore lock
  struct sem sem[NSEM];
} stable;

void k_sem_close(struct sem *s);
int k_sem_wait(struct sem *s);
int k_sem_trywait(struct sem *s);
int k_sem_signal(struct sem *s);
struct sem *k_sem_dup(struct sem *s);
struct sem* k_sem_find(char *name);
struct sem* k_sem_alloc(void);

int flock_acquire(struct file *f);
int flock_release(struct file *f);

