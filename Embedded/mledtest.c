/* --------------------------------------------------------------------

		Matrix LED

		We count until the program exit

---------------------------------------------------------------------*/
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
#include <termios.h>
#include <pthread.h>


#define MATRIX_DRIVER_NAME		"/dev/cnmled"
#define MAX_COLUMN_NUM	5
// 0 ~ 9
const unsigned short NumData[10][MAX_COLUMN_NUM]=
{
	{0xfe00,0xfd7F,0xfb41,0xf77F,0xef00}, // 0
	{0xfe00,0xfd42,0xfb7F,0xf740,0xef00}, // 1
	{0xfe00,0xfd79,0xfb49,0xf74F,0xef00}, // 2
	{0xfe00,0xfd49,0xfb49,0xf77F,0xef00}, // 3
	{0xfe00,0xfd0F,0xfb08,0xf77F,0xef00}, // 4
	{0xfe00,0xfd4F,0xfb49,0xf779,0xef00}, // 5
	{0xfe00,0xfd7F,0xfb49,0xf779,0xef00}, // 6
	{0xfe00,0xfd07,0xfb01,0xf77F,0xef00}, // 7
	{0xfe00,0xfd7F,0xfb49,0xf77F,0xef00}, // 8
	{0xfe00,0xfd4F,0xfb49,0xf77F,0xef00}  // 9
};

static struct termios oldt, newt;
void changemode(int dir)
{
	if( dir == 1)
	{
		tcgetattr(STDIN_FILENO , &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO );
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	}
	else
	{
		tcsetattr(STDIN_FILENO , TCSANOW, &oldt);

	}
}


int kbhit(void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO , &rdfs);

	select(STDIN_FILENO + 1 , &rdfs , NULL, NULL, &tv);

	return FD_ISSET(STDIN_FILENO , &rdfs);
}



#define ONE_LINE_TIME_U 	1000
// exit return => 0 , success return => 1
int displayDotLed(int driverfile , int num ,int timeS)
{
	int cSelCounter,loopCounter;
	int highChar , lowChar;
	int temp , totalCount ;
	unsigned short wdata[2];

	temp = num % 100;

	highChar = temp / 10;
	lowChar = temp % 10;

	
	totalCount = timeS*(1000000 / ONE_LINE_TIME_U);
	printf("totalcounter: %d\n",totalCount);
	cSelCounter = 0;
	loopCounter = 0;
	while(1)
	{
		// high byte display 
		wdata[0] = NumData[highChar][cSelCounter];

		// low byte display 
		wdata[1] = NumData[lowChar][cSelCounter];

		write(driverfile,(unsigned char*)wdata,4);

		cSelCounter++;
		if ( cSelCounter >= (MAX_COLUMN_NUM-1))
			cSelCounter = 1;

		usleep(ONE_LINE_TIME_U);

		loopCounter++;
		if ( loopCounter > totalCount )
			break;
		
		if (kbhit())
		{
			if ( getchar() == (int)'q')
			{
		
				wdata[0]= 0;
				wdata[1]= 0;
				write(driverfile,(unsigned char*)wdata,4);
				printf("Exit mledtest\n");
				return 0;
			}

		}

	}
	wdata[0]= 0;
	wdata[1]= 0;
	write(driverfile,(unsigned char*)wdata,4);

	return 1;
}

extern pthread_mutex_t mtx;
extern int level;

void* matrix_led()
{
	int durationTime ,Num ;
	
	int fd;
	int counterFlag = 1;
	int counter;

	durationTime = 1;

	Num = 0;

	changemode(1);
	// open  driver 
	fd = open(MATRIX_DRIVER_NAME,O_RDWR);
	if (fd  < 0) {
		perror("driver  open error.\n");
		exit(1);
	}
	// when the level is 6, break the loop
	while(level!=6) {
		pthread_mutex_lock(&mtx);
		if(!displayDotLed(fd , Num++ ,durationTime))
			break;
		pthread_mutex_unlock(&mtx);
	}
	changemode(0);
	close(fd);
	
	return;
}
