/*---------------------------------------------------------------

		BUZZER

		1. receive the value of level from main process
		2. As the level, we make the noise

----------------------------------------------------------------*/
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

#define DRIVER_BUZZNAME		"/dev/cnbuzzer"
#define MAX_BUZZER_NUMBER		36

int buzzer_on;//0 or 1
int buzz_cnt;

extern pthread_mutex_t mtx;
extern int level;

void* buzzer()
{
	int fd;
	int buzzer_control = 1;//do 1 re 3 mi 5 fa 6 sol 8 ra 10 si 12
	int null_integer = 0;

	int temp = 0;

		// control led ??
		// do re mi fa~~
		// open  driver 
		fd = open(DRIVER_BUZZNAME,O_RDWR);
		if ( fd < 0 )
		{
			perror("driver (//dev//cnbuzzer) open error.\n");
			exit(1);
		}
	while(level!=6) {
		pthread_mutex_lock(&mtx);
		// temp is the value in buzzer functionnnn
		// if temp is equal to level, we make noise
		if(temp == level) {
			buzzer_on = 1;
			temp++;
		}
		if(buzzer_on==1)
		{ 
			printf("I'm in buzzer!!!\n");
			write(fd,&buzzer_control,4);//BUZZER~~~~~~
			usleep(500000);
			write(fd,&null_integer, 4); //NULL~~~~~~

			buzzer_on=0;
			if(buzzer_control==5)
				buzzer_control++;

			else if(buzzer_control==12)
				buzzer_control=1;

			else
				buzzer_control=buzzer_control+2;
			buzz_cnt++;
		}
		pthread_mutex_unlock(&mtx);
		usleep(100000);
	}
	close(fd);	
	return;
}
