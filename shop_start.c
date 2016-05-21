#include "types.h"
#include "stat.h"
#include "user.h"

// Start the store_semaphore to 1
int main()
{
	int rv = sem_open("store", 1, 1, 1);
	if (rv == -1)
	{
		printf(1, "failed to open semaphore 'store': No semaphores available\n");
	}
	else
	{
		sem_signal(rv);
	}

	exit();
}
