#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "usb_queue.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MYPORT 1111

int main()
{
    int sockfd;
    int clientfd;
    int bytes_read;
    char buf[100];
    int struct_size;
    struct sockaddr_in my_addr;
    struct sockaddr_in con_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    my_addr.sin_zero[8]='\0';

    bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));

    listen(sockfd,5);

    struct_size = sizeof(con_addr);
    clientfd = accept(sockfd, (struct sockaddr*)&con_addr, &struct_size);

    bytes_read = read(clientfd, buf, 100);
    buf[bytes_read] = '\0';
    printf("You received:%d is %s \n",clientfd, buf);

    close(sockfd);
    close(clientfd);

    /*

	int i,p;
	char *c;

	usb_queue q = usb_queue_init(20);
 		
	usb_queue_push(q, "B1", 2); 
	usb_queue_push(q, "B2", 2);
	usb_queue_push(q, "B3", 2);
	usb_queue_push(q, "B4", 2);
    usb_queue_push(q, "A1", 3);
    usb_queue_push(q, "A2", 3);
	usb_queue_push(q, "C2", 1);
	usb_queue_push(q, "C3", 1);
	usb_queue_push(q, "C4", 1);
	usb_queue_push(q, "A3", 3);
	usb_queue_push(q, "A4", 3);
	usb_queue_push(q, "A5", 3);

	while ((c = usb_queue_pop(q, &p)))
    	printf("%d: %s\n", p, c);
	*/
}


