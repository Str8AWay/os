#include <kernel.h>
#include <proc.h>

extern void testcases(void);

/*------------------------------------------------------------------------
 *  main  --  user main program
 *  This test program creates four processes of different priorities.
 *  It also tests argument passing with a "bigargs" process.
 *------------------------------------------------------------------------
 */
int main(int argc, char **argv)
{
  kprintf("Hello XINU World.\r\n");

  enable();
  testcases();

  return 0;
}
