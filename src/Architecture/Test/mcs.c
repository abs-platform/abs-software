#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include <mcs.h>

int pipe_fd[2];

void signal_handler(int sig)
{
    printf("Segmentation fault\n");
    exit(0);
}

void start_test(void)
{
    pipe2(pipe_fd, O_NONBLOCK);
}

void end_test(void)
{
    close(pipe_fd[0]);
    close(pipe_fd[1]);
}

int mcs_cmp(MCSPacket *pkt_a, MCSPacket *pkt_b)
{
    int i;

    if(pkt_a == NULL || pkt_b == NULL) {
        printf("Packets should not be null\n");
        return 1;
    }

    if(pkt_a->type != pkt_b->type) {
        printf("Types are different. a: %d, b: %d\n", pkt_a->type, pkt_b->type);
        return 1;
    }

    if(pkt_a->cmd != pkt_b->cmd) {
        printf("Commands are different. a: %d, b: %d\n",
                                            pkt_a->cmd, pkt_b->cmd);
        return 1;
    }

    if(pkt_a->nargs != pkt_b->nargs) {
        printf("Number of arguments is different. a: %d, b: %d\n",
                                            pkt_a->nargs, pkt_b->nargs);
        return 1;
    }

    for(i = 0; i < pkt_a->nargs; ++i) {
        if(pkt_a->args[i] != pkt_b->args[i]) {
            printf("Argument %d is different.", i);
            return 1;
        }
    }

    if(pkt_a->data_size != pkt_b->data_size) {
        printf("Data size is different. a: %d, b: %d\n",
                                            pkt_a->data_size, pkt_b->data_size);
        return 1;
    }

    for(i = 0; i < pkt_a->data_size; ++i) {
        if(pkt_a->data[i] != pkt_b->data[i]) {
            printf("Data element %d is different.", i);
            return 1;
        }
    }

    return 0;
}

void run_test(enum MCSType type)
{
    const MCSCommandOptionsCommon *cmd;
    MCSPacket *pkt_wr, *pkt_rd;
    unsigned char *args;
    unsigned char data[] = "Hello World!";
    int i, j;
    int error = 0;

    for(i = 0; !error && i < mcs_command_list_size[type]; ++i) {
        start_test();

        if(type == MCS_TYPE_MESSAGE) {
            cmd = &mcs_command_message_list[i].cmd;
        } else if(type == MCS_TYPE_STATE) {
            cmd = &mcs_command_state_list[i].cmd;
        } else if(type == MCS_TYPE_PAYLOAD) {
            cmd = &mcs_command_payload_list[i].cmd;
        }

        printf("Testing command %s:\n ", cmd->name);

        if(cmd->nargs == 0) {
            args = NULL;
        } else {
            args = malloc(cmd->nargs);
            for(j = 0; j < cmd->nargs; ++j) {
                args[j] = j;
            }
        }

        printf("Creating... ");
        if(cmd->raw_data) {
            pkt_wr = mcs_create_packet((MCSCommand)((int)(type << 16) | i),
                            cmd->nargs, args, strlen((char *)data), data);
        } else {
            pkt_wr = mcs_create_packet((MCSCommand)((int)(type << 16) | i),
                            cmd->nargs, args, 0, NULL);
        }

        if(cmd->nargs != 0) {
            free(args);
        }

        if(pkt_wr != NULL) {
            printf("Writing... ");
            if(mcs_write_command_and_free(pipe_fd[1], pkt_wr) < 0) {
                printf("mcs_write_command returned a wrong value\n");
                error = 1;
            } else {
                printf("Reading... ");
                pkt_rd = mcs_read_command(pipe_fd[0]);
                printf("Checking... ");
                error = mcs_cmp(pkt_wr, pkt_rd);
                mcs_free(pkt_rd);
            }
        } else {
            error = 1;
        }

        if(!error) {
            printf("OK!\n");
        }

        end_test();
    }

    if(error) {
        exit(1);
    }
}

void run_test_err(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    int error = 0;

    start_test();

    printf("Testing command ERR:\n ");

    printf("Creating... ");

    pkt_wr = mcs_err_packet(12);

    if(pkt_wr != NULL) {
        printf("Writing... ");
        if(mcs_write_command_and_free(pipe_fd[1], pkt_wr) < 0) {
            printf("mcs_write_command returned a wrong value\n");
            error = 1;
        } else {
            printf("Reading... ");
            pkt_rd = mcs_read_command(pipe_fd[0]);
            printf("Checking... ");
            error = mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = 1;
    }

    if(!error) {
        printf("OK!\n");
    }

    end_test();
}

void run_test_ok(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    int error = 0;

    start_test();

    printf("Testing command OK:\n ");

    printf("Creating... ");

    pkt_wr = mcs_ok_packet();

    if(pkt_wr != NULL) {
        printf("Writing... ");
        if(mcs_write_command_and_free(pipe_fd[1], pkt_wr) < 0) {
            printf("mcs_write_command returned a wrong value\n");
            error = 1;
        } else {
            printf("Reading... ");
            pkt_rd = mcs_read_command(pipe_fd[0]);
            printf("Checking... ");
            error = mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = 1;
    }

    if(!error) {
        printf("OK!\n");
    }

    end_test();
}

void run_test_ok_data(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    unsigned char data[] = "Hello World!";
    int error = 0;

    start_test();

    printf("Testing command OK_DATA:\n ");

    printf("Creating... ");

    pkt_wr = mcs_ok_packet_data(data, strlen((char *)data));

    if(pkt_wr != NULL) {
        printf("Writing... ");
        if(mcs_write_command_and_free(pipe_fd[1], pkt_wr) < 0) {
            printf("mcs_write_command returned a wrong value\n");
            error = 1;
        } else {
            printf("Reading... ");
            pkt_rd = mcs_read_command(pipe_fd[0]);
            printf("Checking... ");
            error = mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = 1;
    }

    if(!error) {
        printf("OK!\n");
    }

    end_test();
}

int main(int argc, char **argv)
{
    signal(SIGSEGV, signal_handler);
    run_test(MCS_TYPE_MESSAGE);
    run_test(MCS_TYPE_STATE);
    run_test(MCS_TYPE_PAYLOAD);
    run_test_err();
    run_test_ok_data();
    run_test_ok();

    return 0;
}
