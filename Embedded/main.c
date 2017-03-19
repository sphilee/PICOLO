/*--------------------------------------------------------------

	MAIN

	*algorithm
	1. receive the RGB value from server
	2. send the RGB and CMYK value to Embedded board and Arduino
	3. execute all of them

--------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

// DEFINE ------------------------------------------------------
#define	BMP_IMAGE_NAME		"color_list.bmp"
#define OLED_IMAGE_NAME		"oled_image.img"
#define THREAD_NUM				5
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

// FUNCTION DECLARATION----------------------------------------
int 	 bitmap(char* bmp_image_name);	// bitmap (static)
int 	 dipswitch(int* weight);				// dip/push sw (static)
int		 color_led(unsigned int vR, unsigned int vG, unsigned int vB);  // Full Color LED (static)
int 	 oled(char* oled_image_name);		// OLED (static)
int		 fnd(int weight);								// 7-segment (static)
void*  text_lcd(void* rgb_arr);				// Text LCD (dynamic)
void*	 matrix_led();								  // Dot Matrix-MLED (dynamic)
void*	 buzzer();										  // Buzzer (dynamic)
void*	 bus_led();										  // Bus LED (dynamic)
void*  thread_ardu();									// communication with arduino

// GLOBAL VARIABLES -------------------------------------------
pthread_mutex_t mtx;
int level;

int main() {

	printf("Content-type: text/html\n\n");
	printf("<html>\n");
	printf("<head><title>Server</title></head>\n");
	printf("<body>\n<center>This is server page</center>\n");
	printf("<br></br>\n");

	//1. wait for receiving RGB value from server
	char* BtnVal;
	char* token;
	char* rgb_str;
	char* cmyk_str;

	unsigned int rgb_color[3] = { 0 };
	unsigned int 	vR, vG, vB;

	double cmyk_color[4] = { 0.0 };
	double vC, vM, vY, vK;

	int cnt = 0;
	int err = 0;
	
  token = (char*)malloc(sizeof(char));
  rgb_str = (char*)malloc(sizeof(char));
  cmyk_str = (char*)malloc(sizeof(char));

	BtnVal=(char*)getenv("QUERY_STRING");

	rgb_str = strtok(BtnVal, "&");
	cmyk_str = strtok(NULL, "&");
	
	// get RGB value
	token = strtok(rgb_str, "=");
	while((token=strtok(NULL, "+"))!=NULL) 
		rgb_color[cnt++] = (unsigned int)atoi(token);

	vR = rgb_color[0]; vG = rgb_color[1]; vB = rgb_color[2];
	printf(" RGB value: %03d %03d %03d\n", vR, vG, vB);
	printf("<br></br>\n");

	cnt = 0;
  // get CMYK value
  token = strtok(cmyk_str, "=");
  while((token=strtok(NULL, "+"))!=NULL) 
    cmyk_color[cnt++] = (double)atof(token);
	
	vC = cmyk_color[0]; vM = cmyk_color[1]; vY = cmyk_color[2]; vK = cmyk_color[3];
	printf("CMYK value: %6.4f %6.4f %6.4f\n", vC, vM, vY, vK);
	printf("<br></br>\n");

	// bitmap (static)
	char* bmp_image_name = BMP_IMAGE_NAME;

	err = bitmap(bmp_image_name);
	if(err != 0)
		perror("Error: bitmap\n");
	
	// dip/push sw (static)
	int startAr = 0;
	int weight = 0;

	while((startAr=dipswitch(&weight))==0);
	fprintf(stdout, "<br></br>\nSuccess dip/switch!!\n");

	// Full Color LED (static)
	err = color_led(vR, vG, vB);
	if(err != 0)
		perror("Error: colorled\n");
	printf("<br></br>Success Full Color LED!!\n");

	// OLED (static)
	char* oled_image_name = OLED_IMAGE_NAME;

	err = oled(oled_image_name);
	if(err != 0)
		perror("Error: oled\n");
	printf("<br></br>Success OLED!!\n");

	// 7-segment (static)
	err = fnd(weight);
	if(err != 0)
		perror("Error: fnd\n");
	printf("<br></br>Success 7-Segment!!\n");

	//===== THREAD =====
	pthread_t pth[THREAD_NUM];
	level = 0;

	// Text LCD (dynamic)
	unsigned int rgb_arr[3];
	rgb_arr[0] = vR;
	rgb_arr[1] = vG;
	rgb_arr[2] = vB;

	err = pthread_create(&pth[0], NULL, text_lcd, (void*)rgb_arr);
	if(err != 0)
		perror("Error: textlcd\n");

	// Dot Matrix-MLED (dynamic)
	err = pthread_create(&pth[1], NULL, matrix_led, NULL);
	if(err != 0)
		perror("Error: matrix_led\n");

	// Buzzer (dynamic)
	err = pthread_create(&pth[2], NULL, buzzer, NULL);
	if(err != 0)
		perror("Error: buzzer\n");

	// Bus LED (dynamic)
	err = pthread_create(&pth[3], NULL, bus_led, NULL);
	if(err != 0)
		perror("Error: bus_led\n");

	// Arduino (dynamic)
	err = pthread_create(&pth[4], NULL, thread_ardu, NULL);
	if(err != 0)
		perror("Error: thread_ardu\n");

/*
	// Arduino (main code)
	int ardu_fd, ardu_res;
  struct termios oldtio,newtio;
  char ardu_buf[2];
  char ch_and = '&';

  ardu_fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
  if (ardu_fd <0) {perror(MODEMDEVICE); exit(-1); }
        
  tcgetattr(ardu_fd,&oldtio); // save current port settings 
        
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
        
  // set input mode (non-canonical, no echo,...) 
  newtio.c_lflag = 0;
        
  newtio.c_cc[VTIME]    = 0;   // inter-character timer unused 
  newtio.c_cc[VMIN]     = 1;   // blocking read until 5 chars received 

	tcflush(ardu_fd, TCIFLUSH);
	tcsetattr(ardu_fd, TCSANOW, &newtio);

	while(1) {
		memset(ardu_buf, 0, 2);

	  ardu_res = read(ardu_fd,ardu_buf,1);   // returns after 5 chars have been input 
		if(ardu_buf[0] < '0' || ardu_buf[0] > '9') {
			pthread_mutex_unlock(&mtx);		
			continue;
		}
		printf("ardu_result: %s / %d\n", ardu_buf, strlen(ardu_buf));
		write(ardu_fd,&ch_and,1);
	
		level = ardu_buf[0];
		if(ardu_buf[0]=='6')
			break;
	}
	tcsetattr(ardu_fd,TCSANOW,&oldtio);
*/
	pthread_exit(NULL);

	return 0;
}
