#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sdb.h>
#include "sdb_private.h"

void *sdb_director_thread()
{
    int fd, con_fd;
    int retry;
    socklen_t size;
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SDB_SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf_dbg("Can't open socket\n");
        goto error;
    }

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf_dbg("Can't bind socket\n");
        goto error;
    }

    if (listen(fd, 20) < 0) {
        printf_dbg("Can't listen to socket\n");
        goto error;
    }

    retry = 5;

    while(1) {
        size = sizeof(client_addr);
        con_fd = accept(fd, (struct sockaddr *)&client_addr, &size);
        if (con_fd < 0) {
            retry--;
            printf_dbg("Error accepting connections\n");
            if (retry != 0) {
                continue;
            } else {
                goto error;
            }
        }

        retry = 5;

        printf("Connection accepted\n");

        sdb_module_init(con_fd, con_fd);
    }

error:
    pthread_exit(NULL);
}
