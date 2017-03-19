CC=arm-none-linux-gnueabi-gcc

all: main

main: main.o bitmap.o buzzertest.o dipswtest.o ledtest.o mledtest.o cledtest.o fndtest.o oledtest.o tlcdtest.o thread_ardu.o
	$(CC) -o final.cgi main.o bitmap.o buzzertest.o dipswtest.o ledtest.o mledtest.o cledtest.o fndtest.o oledtest.o tlcdtest.o thread_ardu.o -pthread

clean:
	rm final.cgi *.o 
