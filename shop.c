// file: shop.c

//==== includes ====
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NAME_SEM_FITTING "fit"
#define NAME_SEM_STORE "store"
#define NAME_SEM_PAY "pay"


#define VERBOSE 0
#define DEB_LINE 0

#define DEB_FUNCTION {if (VERBOSE) \
         printf(1, "Line=%d Func=%s\n", __LINE__, __FUNCTION__); }
#define LINE(msg) {if (DEB_LINE) \
         printf(1, "Line=%d *** %s\n", __LINE__, msg); }

#define BUF_SIZE 80
#undef isblank

#define stdout 1
#define stderr 2


//-------------------------------------------
// Time delays - All numbers in tenth-of-second
#define TRY_TIME (2 + (getpid() % 2))
// #define TRY_TIME 1
#define STORE_GET_TIME 1
#define STORE_RETURN_TIME 1
#define PAY_TIME 1
#define DELAY_FACTOR 1 //multiply the delay for all

//---------------------------------
// *** SEMAPHORES
#define SEM_FITTING 3
#define SEM_STORE 0
#define SEM_PAY 2

//---------------------------------
// semaphores as global variables
int sem_fitting;
int sem_store;
int sem_pay;

//---------------------------------
struct try_n_buy
{
	char name[10];
	char model[10];
	int size;
	int buy;
};

void debug_print_try(struct try_n_buy try)
{
  printf(1, "%s name=%s model=%s size=%d buy=%d\n",
         __FUNCTION__, try.name, try.model, try.size, try.buy);
}
//---------------------------------

void launch_shopper(char *buf);
int read_next_try(char* buf, struct try_n_buy* try);
void announce_shopper(char* name);
void enter_fitting(char* name);
void exit_fitting(char* name);
void go_try(struct try_n_buy try);
void get_from_store(struct try_n_buy try);
void return_to_store(struct try_n_buy try);
void go_pay(struct try_n_buy try);
void tenth_sleep(int n);
void open_all_sem(void);
void perror(char* str);
int isblank(char);
int is_blank_buffer(char* str);
char* read_word(char *dest, char *src,  int max);
int my_getline(char *buf, int max, int fd);

//===========================================================


//---------- MAIN ----------------
int main(int argc, char* argv[])
{
	int pid;
	int fd;
	char buf[80];
	int ret;
	// unsigned short char_count=80; // this is ignored actually
	int proc_count = 0;
	// int status;

DEB_FUNCTION
	if (argc != 2) {
		printf(stderr, "Wrong number of Arguments: Missing file Name\n\n");
		exit();
	}

	printf(stdout, "Shoe-Store, Tzvi Melamed id 01234567\n");

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		perror("failed to open file") ;
		exit() ;
	}
	open_all_sem();

	while ((ret = my_getline(buf, BUF_SIZE, fd)) > 0) {
		printf(stdout, "Shopper: just read: %s", buf);
		proc_count++;
		pid=fork();
		if (pid<0)
		{
			perror("Failed to fork a child process");
			exit() ;
		}
		if (pid==0)
		{
			launch_shopper(buf);
			exit();
		}
	}
	// wait for all children
	while(proc_count > 0)
	{
		//printf(stdout, "parent waiting (%d)\n",proc_count);
		wait();
		proc_count--;
	}
	printf (stdout, "last customer left... closing the shop.\nbyebye\n");
	// return 0;  WE MUST NOT RETURN FROM MAIN!!
	exit();
}

/*
=================================================================
  	launch_shopper
-------------------------------------------------------------------
Each line has the following format:
NAME [model size take/dont-take]+
      \------- single try ----/ ^ plus sign designates possibly many tries
where: 1: take, 0: dont-take
e.g.
Avi assics 40 0 assics 42 0 addidas 41 1

Shopping process:

a) go to fitting room  (3 fitting rooms)
b) for each "try": go_try()
	1) get shoe from storage room
	2) try it (print a message)
	3) if dont-take, return to storage room
c) if take/buy: go to cashier (pay)
*/
//=================================================================
void launch_shopper(char *buf)
{
DEB_FUNCTION
    printf(1, "launch_shopper buffer is: %s\n", buf);
	//char name[10]={0};
	struct try_n_buy try;

	//read name
	read_word(try.name, buf, 10);
printf(1, "%d %s name=%s@@@buf=%s@@@\n", __LINE__, __FUNCTION__, try.name, buf);

	announce_shopper(try.name);
	// int i=0;
	// while (isblank(buf[i])) i++; // ignore trailig blanks.
	// while (!isblank(buf[i])) i++; // ignore NAME
	// while (isblank(buf[i])) i++; // ignore blanks

	enter_fitting(try.name);
	while(read_next_try(buf, &try))
	{
debug_print_try(try);

	 	go_try(try);
	 	if(try.buy) break;
	}
	exit_fitting(try.name);
	if (try.buy) go_pay(try);

	// close the semaphores
	sem_close(sem_fitting);
	sem_close(sem_store);
	sem_close(sem_pay);
}

//=================================================================
int read_next_try(char* buf, struct try_n_buy* try)
{
	char tmp_str[10] = {0};

	if (is_blank_buffer(buf))
	  return 0;
	// ret = sscanf(buf, "%s %d %d", try->model, &try->size, &try->buy);
	read_word(try->model, buf, 10);
	read_word(tmp_str, buf, 5);
	try->size = atoi(tmp_str);
	read_word(tmp_str, buf, 5);
	try->buy = atoi(tmp_str);

// Porting to XV6 - we already "reduced" the buffer.
// Also, we ommit sanity checks (assuming the input file is OK).
/*
	if (ret != 3)
	{
		return 0; // failed to read 3 fields
	}

	//advance buf to ignore the fields we just scanned
	while (!isblank(buf[i])) i++; // ignore model
	while (isblank(buf[i])) i++; // ignore blanks.
	while (!isblank(buf[i])) i++; // ignore size field
	while (isblank(buf[i])) i++; // ignore blanks.
	while (!isblank(buf[i]))i++; // ignore buy field
	while (isblank(buf[i])) i++; // ignore trailig blanks.
	// copy the rest of the string
	while(buf[i]!=0)
		buf[k++]=buf[i++];
	buf[k]=0;
*/
	return 1; // true, i.e. successful
}

//=================================================================
void announce_shopper(char* name)
{
DEB_FUNCTION
	printf(stdout, "(%d %s) Shopper starting\n", getpid(), name);
}

//=================================================================
void enter_fitting(char* name)
{
DEB_FUNCTION
	sem_wait(sem_fitting);
	printf(stdout, "(%d %s) entered fitting room\n", getpid(), name);
}


//=================================================================
void exit_fitting(char* name)
{
DEB_FUNCTION
	printf(stdout, "(%d %s) exited fitting room\n", getpid(), name);
	sem_signal(sem_fitting);
}
//=================================================================
void go_try(struct try_n_buy try)
{
DEB_FUNCTION
	get_from_store(try);
	printf(stdout, "(%d %s) trying model %s size %d\n", getpid(), try.name, try.model, try.size);
	tenth_sleep(TRY_TIME);
	if (try.buy == 0) return_to_store(try);
}

//=================================================================
void get_from_store(struct try_n_buy try)
{
DEB_FUNCTION
	int ret;
	if ((ret = sem_trywait(sem_store)) < 0)
	{
		printf(stdout, "(%d %s) sem_trywait for Store failed... going to wait...\n",
				getpid(), try.name);
		sem_wait(sem_store);
	}
	printf(stdout, "(%d %s) Store: get model %s size %d\n", getpid(), try.name, try.model, try.size);
	tenth_sleep(STORE_GET_TIME);
	printf(stdout, "(%d %s) Store: Get completed.\n", getpid(), try.name);
	sem_signal(sem_store);
}


//=================================================================
void return_to_store(struct try_n_buy try)
{
DEB_FUNCTION
	int ret;
	if ((ret = sem_trywait(sem_store)) < 0)
	{
		printf(stdout, "(%d %s) sem_trywait for Store failed... going to wait...\n",
				getpid(), try.name);
		sem_wait(sem_store);
	}
	printf(stdout, "(%d %s) Store: Returning model %s size %d\n", getpid(), try.name, try.model, try.size);
	tenth_sleep(STORE_RETURN_TIME);
	printf(stdout, "(%d %s) Store: Return completed.\n", getpid(), try.name);
	sem_signal(sem_store);
}

//=================================================================
void go_pay(struct try_n_buy try)
{
	sem_wait(sem_pay);
	printf(stdout, "(%d %s) Cashier: Start payment...\n", getpid(), try.name);
	tenth_sleep(PAY_TIME);
	printf(stdout, "(%d %s) Cashier: Done  payment...\n", getpid(), try.name);
	sem_signal(sem_pay);
}

//=================================================================
void tenth_sleep(int n)
{
DEB_FUNCTION
	// return usleep(n*100000 * DELAY_FACTOR);
	sleep(n);
}

//=================================================================
void perror(char* str)
{
  printf(stderr, "%s\n", str);
}

//=================================================================
void open_all_sem(void)
{
/*	if (sem_unlink("/sem_fitting")==0)*/
/*		fprintf(stderr, "successul unlink of /sem_fitting\n");*/
	sem_fitting = sem_open(NAME_SEM_FITTING, 1, SEM_FITTING, SEM_FITTING);
	if (sem_fitting < 0)
	{
		perror("failed to open semaphore /sem_fitting\n");
		exit();
	}

/*	if (sem_unlink("/sem_pay")==0)*/
/*		fprintf(stderr, "successul unlink of /sem_pay\n");*/
	sem_pay = sem_open(NAME_SEM_PAY, 1, SEM_PAY, SEM_PAY);
	if (sem_pay < 0)
	{
		perror("failed to open semaphore /sem_pay\n");
		exit();
	}

/*	if (sem_unlink("/sem_store")==0)*/
/*		fprintf(stderr, "successul unlink of /sem_store\n");*/
	sem_store = sem_open(NAME_SEM_STORE, 1, SEM_STORE, 3);
	if (sem_store < 0)
	{
		perror("failed to open semaphore /sem_store\n");
		exit();
	}
}


//==================================================
int isblank(char c)
{
  return (c == ' ' || c =='\t' || c == '\n' || c == '\r');
}

//==================================================
int is_blank_buffer(char* str)
{
  int i;
  for (i=0; str[i] != 0; i++)
    if (!isblank(str[i]))
	  return 0;
  return 1;
}

//==================================================
char*
read_word(char *dest, char *src,  int max)
{
 int i, j, k;

  for(k=0; isblank(src[k]) && src[k]!=0 ; k++)
    ; // empty body

  for(j=0, i=k; j < max && src[i]!=0; j++, i++ ){
    if(isblank(src[i]))
	  break;
    dest[j] = src[i];
  }

  dest[j] = '\0';

  for(i=0;  src[i+j+k]!=0 ;  i++)
    src[i]=src[i+j+k];
  src[i]=0;

  return dest;
}

//==================================
int
my_getline(char *buf, int max, int fd)
{
DEB_FUNCTION
  int tmp_fd, ret;
  tmp_fd = dup(0);
  close(0);
  dup(fd);
  ret = getline(buf, max);
  close(0);
  dup(tmp_fd);
  close(tmp_fd);
  return ret;
}






