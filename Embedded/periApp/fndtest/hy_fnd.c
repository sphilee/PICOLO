#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
/*
		a		
	f		b
		g		
	e		c	
		d		dp


*/
#define FND_DRIVER_NAME		"/dev/cnfnd"

#define MAX_FND_NUM		6

#define  DOT_OR_DATA	0x80

const unsigned short segNum[10] =
{
	0x3F, // 0
	0x06,
	0x5B,
	0x4F,
	0x66,
	0x6D,
	0x7D,
	0x27,
	0x7F,
	0x6F  // 9
};
const unsigned short segSelMask[MAX_FND_NUM] = 
{
	0xFE00,
	0xFD00,
	0xFB00,
	0xF700,
	0xEF00,
	0xDF00
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

#define ONE_SEG_DISPLAY_TIME_USEC	1000
// return 1 => exit  , 0 => success
int fndDisp(int driverfile, int num , int dotflag,int durationSec)
{
	int cSelCounter,loopCounter;
	int temp , totalCount, i ;
	unsigned short wdata;
	int dotEnable[MAX_FND_NUM];
	int fndChar[MAX_FND_NUM];

	for (i = 0; i < MAX_FND_NUM ; i++ )
	{
		dotEnable[i] = dotflag & (0x1 << i);  
	}
	// if 6 fnd
	temp = num % 1000000;
	fndChar[0]= temp /100000;

	temp = num % 100000;
	fndChar[1]= temp /10000;

	temp = num % 10000;
	fndChar[2] = temp /1000;

	temp = num %1000;
	fndChar[3] = temp /100;

	temp = num %100;
	fndChar[4] = temp /10;

	fndChar[5] = num %10;

	totalCount = durationSec*(1000000 / ONE_SEG_DISPLAY_TIME_USEC);
	printf("totalcounter: %d\n",totalCount);
	cSelCounter = 0;
	loopCounter = 0;
	while(1)
	{
		wdata = segNum[fndChar[cSelCounter]]  | segSelMask[cSelCounter] ;
		if (dotEnable[cSelCounter])
			wdata |= DOT_OR_DATA;	

		write(driverfile,&wdata,2);

		cSelCounter++;
		if ( cSelCounter >= MAX_FND_NUM )
			cSelCounter = 0;

		usleep(ONE_SEG_DISPLAY_TIME_USEC);

		loopCounter++;
		if ( loopCounter > totalCount )
			break;
		
		if (kbhit())
		{
			if ( getchar() == (int)'q')
			{
		
				wdata= 0;
				write(driverfile,&wdata,2);
				printf("Exit fndtest\n");
				return 0;
			}

		}
	}

	wdata= 0;
	write(driverfile,&wdata,2);

	return 1;
}

#define MODE_STATIC_DIS		0
#define MODE_TIME_DIS		1
#define MODE_COUNT_DIS		2
int fnd(int weight)
{
	int fd;
	
	// open  driver 
	fd = open(FND_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
		return 1;
	}
	changemode(1);
		
	fndDisp(fd, weight, 0, 1);

	changemode(0);
	close(fd);
	
	return 0;
}
