#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define THE_DEVICE "/dev/usb_accessory"

int main(int argc, char const *argv[])
{

	int fd = open(THE_DEVICE, O_RDWR);

	/*
     * Basic I/O
	 *
	 *	0	0	1	0	1	1	0	1 <- read digital pin (57) x2D
	 *	0	0	0	0	0	1	0	1 <- digital pin nº 2  (5) x05
	 *	0	0	0	0	0	0	1	0 <- end & packet = 1  (2) x02
	 *
	 */

	/*
     * Basic Event
	 *
	 *	0	1	1	0	0	1	0	1 <- set event prescaler 2	(101)	0x65
	 *	0	0	0	0	0	0	1	1 <- act 1: digitalWrite	  (3) 	0x03
	 *	0	0	0	1	0	0	0	1 <- opt 1: pin  number		  (8)	0x11
	 *	0	0	0	0	0	1	0	0 <- end & packet = 2  		  (2) 	0x04
	 *
	 */

	//digitalWrite(2,HIGH)

	char packet1[] = "\x2D\x5\x2";

	//digitalWrite(2,LOW)

	char packet2[] = "\x29\x5\x2";

	//Event pre=2 toggle(8)

	char packet3[] = "\x65\x3\x11\x4";

	/* create an event */

	write(fd,&packet3,sizeof(packet3));	

	sleep(5);

	/* create an event */

	write(fd,&packet3,sizeof(packet3));	

	sleep(5);

	while(1) {

	/* blink a led */

	write(fd,&packet1,sizeof(packet1));	

	printf("Packet 1 sended\n");

	sleep(1);

	write(fd,&packet2,sizeof(packet2));	

	printf("Packet 2 sended\n");

	sleep(1);

	}

	return 0;
}

