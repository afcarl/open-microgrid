#include "contiki.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Microgrid loop process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  int a;
  int b;

  printf("Please enter two numbers\n");
  //scanf("%d %d", &a, &b);
  a=2; b=123;
  printf("Their sum is %d.", a+b);
    
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
