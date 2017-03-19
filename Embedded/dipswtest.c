/*-----------------------------------------------------------------

	DIPSWITCH

	1. we use switch button, and receive the value of that
	2. if the most left bit is on, we finish this function and send the value 		to main

-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DRIVER_DIPNAME		"/dev/cndipsw"
#define MAX_INT				32768
#define NUM_63				63
#define MAX_WEIGHT		60

int dipswitch(int* weight)
{	
	int fd;
	int retvalue;
	int firstbit = 0;
	int startAr = 0;

	// open driver
  fd = open(DRIVER_DIPNAME,O_RDWR);
  if ( fd < 0 )
  {    
    perror("driver  open error.\n");
    return;
  }    
	read(fd,&retvalue,4);

	/* use the Most Left Bit to send the message of starting arduino */
	firstbit=MAX_INT&retvalue;//1000 0000 0000 0000

	if(firstbit==MAX_INT)		// when MLB==1, set startAr=1
		startAr=1;	
	else	
		startAr=0;

	// we use only right 6bits
	*weight = NUM_63&retvalue;
	if(*weight>MAX_WEIGHT)
		*weight=MAX_WEIGHT;//maximum weight
	close(fd);	
	return startAr;
}
