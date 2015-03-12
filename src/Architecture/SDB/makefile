CC = arm-linux-androideabi-gcc
CFLAGS = -Wall

all: main            
main: sdb_dummy.o usb_queue.o   
	${CC} ${CFLAGS} sdb_dummy.o usb_queue.o -o sdb_dummy
usb_queue.o: usb_queue.c usb_queue.h

clean:
	rm -f main
