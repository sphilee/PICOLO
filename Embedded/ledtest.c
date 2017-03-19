/*-----------------------------------------------------------------------

		Bus LED

		We turn on the LED as the level.
		 ex) level == 1 -> LED2 ON
		 ex) level == 3 -> LED6 ON

----------------------------------------------------------------------*/
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
#include <pthread.h>

#define LED_ON	1
#define LED_OFF	0
#define MAX_LED_NO		8
#define LED_DRIVER_NAME		"/dev/cnled"

extern pthread_mutex_t mtx;
extern int level;

void* bus_led()
{
	int wdata ,rdata,temp ;
	
	int fd;
	int num;
		
	// open driver
	fd = open(LED_DRIVER_NAME, O_RDWR);
	if(fd < 0) {
		perror("driver (//dev//cnled) open errir.\n");
		exit(0);
	}
	while(level!=6) {
		pthread_mutex_lock(&mtx);
		if(level == 0)
			continue;
		wdata = 0;
		// clear bus led
		write(fd, &wdata, 4);
		for(num = 1; num <= level*2; num++) {
			// control led 
	
			read(fd,&rdata,4);
			temp = 1;
	
			temp <<=(num-1);
			wdata = rdata | temp;
		}
		write(fd,&wdata,4);
		 
		pthread_mutex_unlock(&mtx);
		usleep(100000);
	}
	close(fd);
	
	return ;
}
