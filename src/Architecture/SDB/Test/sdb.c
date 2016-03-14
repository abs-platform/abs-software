#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <mcs.h>
#include <sdb.h>
#include <abs_test.h>

#define SDB_PATH "../sdb"

int pid = -1;
pthread_t sender;
bool sender_on = false;
pthread_t receiver;
bool receiver_on = false;

void sdb_test_end(void);
void *sdb_test_thread(void *arg);

static void signal_handler(int sig)
{
    if(sig == SIGSEGV) {
        sdb_test_end();
        abs_test_fail_and_exit("Segmentation fault\n");
    }
}

static void sdb_test_init(void)
{
    int id_send, id_recv;

    pid = fork();

    if(pid < 0) {
        abs_test_fail_and_exit("SDB Fork\n");
    } else if(pid == 0) {
        execl(SDB_PATH, SDB_PATH, NULL);
        abs_test_printf("Could not exec\n");
        exit(1);
    }

    sleep(2);
    if(waitpid(pid, NULL, WNOHANG) != 0) {
        abs_test_fail_and_exit("SDB Execution\n");
    }

    id_send = 1;
    id_recv = 2;
    if(pthread_create(&sender, NULL, sdb_test_thread, &id_send) < 0) {
        sdb_test_end();
    } else {
        sender_on = true;
    }

    if(pthread_create(&receiver, NULL, sdb_test_thread, &id_recv) < 0) {
        sdb_test_end();
    } else {
        receiver_on = true;
    }

    pthread_join(sender, NULL);
    sender_on = false;
    pthread_join(receiver, NULL);
    receiver_on = false;

    sdb_test_end();
    abs_test_end();
}

void sdb_test_end(void)
{
    if(sender_on) {
        pthread_cancel(sender);
        pthread_join(sender, NULL);
        sender_on = false;
    }

    if(receiver_on) {
        pthread_cancel(receiver);
        pthread_join(receiver, NULL);
        receiver_on = false;
    }

    if(pid != -1) {
        kill(pid, SIGINT);
    }
}

void *sdb_test_thread(void *arg)
{
    int fd;
    int id;
    int id_other;
    char name_other[5];
    char buffer[100];
    char welcome[] = "appX:app";
    struct sockaddr_in addr;
    MCSPacket *pkt;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    id = *(int *)arg;
    welcome[3] = id + '0';

    if(id == 1) {
        id_other = 2;
    } else {
        id_other = 1;
    }
    sprintf(name_other, "app%d", id_other);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SDB_SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* SDB connection */
    sprintf(buffer, "%s: SDB Connection", name_other);
    abs_test_printf("Setting connection for %s\n", name_other);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        abs_test_printf("Could not create the socket\n");
        goto error;
    }

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        abs_test_printf("Could not connect to the socket\n");
        goto error;
    }

    abs_test_add_result(PASS, buffer);

    /* Handshaking */
    sprintf(buffer, "%s: Handshake", name_other);
    /* Remember NULL character! */
    pkt = mcs_create_packet(MCS_MESSAGE_SDB_HANDSHAKE, 0, NULL,
                            strlen(welcome) + 1, (unsigned char *)welcome);
    if(pkt == NULL) {
        abs_test_printf("Packet generated is NULL\n");
        goto error;
    }

    if(mcs_write_command(pkt, fd) != 0) {
        abs_test_printf("Could not send handshake packet\n");
        goto error_free;
    }

    mcs_free(pkt);

    pkt = mcs_read_command(fd, fd);
    if(pkt == NULL) {
        abs_test_printf("Could not read response packet\n");
        goto error;
    } else if(pkt->type != MCS_TYPE_OK) {
        if(pkt->type == MCS_TYPE_ERR) {
            abs_test_printf("Wrong response packet. Error %d\n",
                                            mcs_err_code_from_command(pkt));
        } else {
            abs_test_printf("Wrong reponse packet. Type: %hhd\n", pkt->type);
        }
        goto error_free;
    }

    abs_test_add_result(PASS, buffer);

    /* Sending TEST message */
    sprintf(buffer, "%s: Test packet", name_other);
    /* app1 will send the test packet, whereas app2 will receive it */
    if(id == 1) {
        pkt = mcs_create_packet_with_dest(MCS_MESSAGE_TEST, "app2",
                                                    0, NULL, 0, NULL);
        if(mcs_write_command(pkt, fd) != 0) {
            abs_test_printf("Error sending TEST packet\n");
            goto error_free;
        }

        mcs_free(pkt);

        pkt = mcs_read_command(fd, fd);

        if(pkt == NULL) {
            abs_test_printf("Could not read response packet\n");
            goto error;
        } else if(pkt->type != MCS_TYPE_OK) {
            if(pkt->type == MCS_TYPE_ERR) {
                abs_test_printf("Wrong response packet. Error %d\n",
                                            mcs_err_code_from_command(pkt));
            } else {
                abs_test_printf("Wrong reponse packet. Type: %hhd\n",
                                                                pkt->type);
            }
            goto error_free;
        }
    } else {
        pkt = mcs_read_command(fd, fd);

        if(pkt == NULL) {
            abs_test_printf("Could not read TEST packet\n");
            goto error;
        } else if(mcs_command(pkt) != MCS_MESSAGE_TEST) {
            abs_test_printf("Packet is not the TEST packet\n");
            mcs_write_command_and_free(mcs_err_packet(pkt, EUNDEF), fd);
            goto error_free;
        } else {
            mcs_write_command_and_free(mcs_ok_packet(pkt), fd);
        }
    }

    mcs_free(pkt);

    abs_test_add_result(PASS, buffer);

    pthread_exit(NULL);

error_free:
    mcs_free(pkt);
error:
    abs_test_add_result(FAIL, buffer);
    pthread_exit(NULL);
}

int main()
{
    abs_test_init("sdb");

    signal(SIGSEGV, signal_handler); /* Overwrite the signal handler */
    signal(SIGINT, signal_handler);

    sdb_test_init();
    return 0;
}
