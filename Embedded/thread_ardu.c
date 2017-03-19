/*-------------------------------------------------------------------

		ARDUINO COMMUNICATION

		1. we receive the level of progress
		2. after receiving, we send the message('&')
		3. repeat them until level == 6

--------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

extern pthread_mutex_t mtx;
extern int level;
   
void* thread_ardu() {

	int fd,c, res;
  struct termios oldtio,newtio;
  char buf[2];
  char ch_and = '&';

  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
  if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
  tcgetattr(fd,&oldtio); /* save current port settings */
        
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
        
  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
        
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &newtio);

	while(1) {
//		pthread_mutex_lock(&mtx);
		memset(buf, 0, 2);

	  res = read(fd,buf,1);   /* returns after 1 chars have been input */
		if(buf[0] < '0' || buf[0] > '9') {
			continue;
		}
		printf("result: %s / %d\n", buf, strlen(buf));
		write(fd,&ch_and,1);
		level = buf[0] - '0';	
		if(buf[0]=='6')
			break;
//		pthread_mutex_unlock(&mtx);
		usleep(100000);
	}
	tcsetattr(fd, TCSANOW, &oldtio);
	return;
}
/*
int main() {

	pthread_t pth;
	int err;

	level = 0;
	
	err = pthread_create(&pth, NULL, thread_ardu, NULL);
	if(err != 0)
		perror("pth error\n");
	
	pthread_exit(NULL);
	return 0;
}
*/
