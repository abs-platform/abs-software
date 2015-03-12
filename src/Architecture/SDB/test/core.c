#include <stdio.h>
#include <string.h>    
#include <unistd.h>   
#include <ctype.h>
#include <fcntl.h> 
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

#include "usb_queue.h"
 
int main(int argc , char *argv[])
{

    int fd;
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];

    usb_queue_init();
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1111);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
     
    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {

        //*mcs_read_command(int fd)

        int i;
        for (i = 0; i < 5; i++)
        {
            if (i > 0) printf(":");
            printf("%02X", client_message[i]);      
        }
        printf("\n");

        usb_queue_push(client_message, 1);

        write(client_sock , client_message, sizeof(client_message));

 
        int val = (int) strtol(client_message, (char **)NULL, 10);

        printf("Value of %d and size %d\n",val,sizeof(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    int p,i; char *d;

    while ((d = usb_queue_pop(&p)))
        
        for (i = 0; i < 5; i++)
        {
            if (i > 0) printf(":");
            printf("%02X", d[i]);
        }

        printf("\n");
     
    return 0;
}
