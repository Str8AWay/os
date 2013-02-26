#include <kernel.h>
#include <proc.h>

/* This main program will be created and run as a process by nulluser. */

int main(int argc, char **argv) 
{
  int i = 0;
  kprintf("Hello XINU World!\r\n");

  for (i = 0; i < 10; i++)
    {
      kprintf("This is process %d\r\n", currpid);

      /* Uncomment the resched() line below to see cooperative scheduling. */
      resched();
    }

  return 0;
}
