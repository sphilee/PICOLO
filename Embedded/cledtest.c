/*---------------------------------------------------------------------

		Color LED

		we make color from the value of RGB, received from main process

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

#define CLED_DRIVER_NAME		"/dev/cncled"

#define INDEX_LED		0
#define INDEX_REG_LED		1
#define INDEX_GREEN_LED		2
#define INDEX_BLUE_LED		3
#define INDEX_MAX			4

int color_led(unsigned int vR, unsigned int vG, unsigned int vB)
{
	unsigned short colorArray[INDEX_MAX];
	
	int fd;
	// asssign the RGB value(received from main) to local variable
	colorArray[INDEX_LED] = 0;
	colorArray[INDEX_REG_LED] =(unsigned short) vR;
	colorArray[INDEX_GREEN_LED] =(unsigned short) vG;
	colorArray[INDEX_BLUE_LED] =(unsigned short) vB;
	
	// open  driver 
	fd = open(CLED_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver  open error.\n");
		return 1;
	}

	printf("index(%d) r(%d) g(%d) b(%d)\n",colorArray[INDEX_LED],colorArray[INDEX_REG_LED],colorArray[INDEX_GREEN_LED],colorArray[INDEX_BLUE_LED]);
	write(fd,&colorArray,6);

	close(fd);
	return 0;
}
