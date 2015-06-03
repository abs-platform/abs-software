#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <abs.h>
#include <mcs.h>
#include <abs_test.h>

int pipe_fd[2];

void start_test(void)
{
    pipe2(pipe_fd, O_NONBLOCK);
}

void end_test(void)
{
    close(pipe_fd[0]);
    close(pipe_fd[1]);
}

bool mcs_cmp(MCSPacket *pkt_a, MCSPacket *pkt_b)
{
    int i;

    if(pkt_a == NULL || pkt_b == NULL) {
        abs_test_printf("Packets can't be null\n");
        return false;
    }

    if(pkt_a->type != pkt_b->type) {
        abs_test_printf("Types are different. a: %d, b: %d\n", pkt_a->type, pkt_b->type);
        return false;
    }

    if(pkt_a->cmd != pkt_b->cmd) {
        abs_test_printf("Commands are different. a: %d, b: %d\n",
                                            pkt_a->cmd, pkt_b->cmd);
        return false;
    }

    if(pkt_a->nargs != pkt_b->nargs) {
        abs_test_printf("Number of arguments is different. a: %d, b: %d\n",
                                            pkt_a->nargs, pkt_b->nargs);
        return false;
    }

    for(i = 0; i < pkt_a->nargs; ++i) {
        if(pkt_a->args[i] != pkt_b->args[i]) {
            abs_test_printf("Argument %d is different.", i);
            return false;
        }
    }

    if((pkt_a->dest == NULL && pkt_b->dest != NULL) ||
                            (pkt_a->dest != NULL && pkt_b->dest == NULL)) {
        abs_test_printf("Destination is different. a: %s, b: %s\n",
                                            pkt_a->dest, pkt_b->dest);
        return false;
    } else if(pkt_a->dest != NULL && strcmp(pkt_a->dest, pkt_b->dest) != 0) {
        abs_test_printf("Destination is different. a: %s, b: %s\n",
                                            pkt_a->dest, pkt_b->dest);
        return false;
    }

    if(pkt_a->data_size != pkt_b->data_size) {
        abs_test_printf("Data size is different. a: %d, b: %d\n",
                                        pkt_a->data_size, pkt_b->data_size);
        return false;
    }

    for(i = 0; i < pkt_a->data_size; ++i) {
        if(pkt_a->data[i] != pkt_b->data[i]) {
            abs_test_printf("Data element %d is different.\n", i);
            return false;
        }
    }

    return true;
}

void run_test(enum MCSType type)
{
    const MCSCommandOptionsCommon *cmd;
    MCSPacket *pkt_wr, *pkt_rd;
    unsigned char *args;
    unsigned char data[] = "Hello World!";
    int i, j;
    bool error = false;

    for(i = 0; i < mcs_command_list_size[type]; ++i) {
        start_test();

        if(type == MCS_TYPE_MESSAGE) {
            cmd = &mcs_command_message_list[i].cmd;
        } else if(type == MCS_TYPE_STATE) {
            cmd = &mcs_command_state_list[i].cmd;
        } else if(type == MCS_TYPE_PAYLOAD) {
            cmd = &mcs_command_payload_list[i].cmd;
        } else {
            cmd = NULL;
            abs_test_printf("mcs_command_list_size is incorrect for type %d\n",
                                                                        type);
            abs_test_fail_and_exit("MCS Type");
        }

        abs_test_printf("Testing command %s\n", cmd->name);

        if(cmd->nargs == 0) {
            args = NULL;
        } else {
            args = malloc(cmd->nargs);
            for(j = 0; j < cmd->nargs; ++j) {
                args[j] = j;
            }
        }

        if(type == MCS_TYPE_MESSAGE &&
                    mcs_command_message_list[i].destination != NULL &&
                    mcs_command_message_list[i].destination[0] == '@') {
            if(cmd->raw_data) {
                pkt_wr = mcs_create_packet_with_dest(
                        (MCSCommand)((int)(type << 16) | i), "test_dest",
                        cmd->nargs, args, strlen((char *)data), data);
            } else {
                pkt_wr = mcs_create_packet_with_dest(
                        (MCSCommand)((int)(type << 16) | i), "test_dest",
                        cmd->nargs, args, 0, NULL);
            }
        } else {
            if(cmd->raw_data) {
                pkt_wr = mcs_create_packet(
                                (MCSCommand)((int)(type << 16) | i),
                                cmd->nargs, args, strlen((char *)data), data);
            } else {
                pkt_wr = mcs_create_packet(
                                (MCSCommand)((int)(type << 16) | i),
                                cmd->nargs, args, 0, NULL);
            }
        }

        if(cmd->nargs != 0) {
            free(args);
        }

        if(pkt_wr != NULL) {
            if(mcs_write_command(pkt_wr, pipe_fd[1]) < 0) {
                abs_test_printf("mcs_write_command returned a wrong value\n");
                error = true;
            } else {
                pkt_rd = mcs_read_command(pipe_fd[0], pipe_fd[1]);
                error = !mcs_cmp(pkt_wr, pkt_rd);
                mcs_free(pkt_rd);
            }
        } else {
            error = true;
        }

        mcs_free(pkt_wr);
        end_test();

        if(error) {
            abs_test_add_result(FAIL, cmd->name);
        } else {
            abs_test_add_result(PASS, cmd->name);
        }
    }

}

void run_test_err(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    bool error = false;

    start_test();

    abs_test_printf("Testing command ERR\n");

    pkt_wr = mcs_err_packet(EUNDEF);

    if(pkt_wr != NULL) {
        if(mcs_write_command_and_free(pkt_wr, pipe_fd[1]) < 0) {
            abs_test_printf("mcs_write_command returned a wrong value\n");
            error = true;
        } else {
            pkt_rd = mcs_read_command(pipe_fd[0], pipe_fd[1]);
            error = !mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = true;
    }

    end_test();

    if(error) {
        abs_test_add_result(FAIL, "TYPE_ERR");
    } else {
        abs_test_add_result(PASS, "TYPE_ERR");
    }
}

void run_test_ok(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    bool error = false;

    start_test();

    abs_test_printf("Testing command OK\n");

    pkt_wr = mcs_ok_packet();

    if(pkt_wr != NULL) {
        if(mcs_write_command_and_free(pkt_wr, pipe_fd[1]) < 0) {
            abs_test_printf("mcs_write_command returned a wrong value\n");
            error = true;
        } else {
            pkt_rd = mcs_read_command(pipe_fd[0], pipe_fd[1]);
            error = !mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = EUNDEF;
    }

    end_test();

    if(error) {
        abs_test_add_result(FAIL, "TYPE_OK");
    } else {
        abs_test_add_result(PASS, "TYPE_OK");
    }
}

void run_test_ok_data(void)
{
    MCSPacket *pkt_wr, *pkt_rd;
    unsigned char data[] = "Hello World!";
    bool error = false;

    start_test();

    abs_test_printf("Testing command OK_DATA\n");

    pkt_wr = mcs_ok_packet_data(data, strlen((char *)data));

    if(pkt_wr != NULL) {
        if(mcs_write_command_and_free(pkt_wr, pipe_fd[1]) < 0) {
            abs_test_printf("mcs_write_command returned a wrong value\n");
            error = true;
        } else {
            pkt_rd = mcs_read_command(pipe_fd[0], pipe_fd[1]);
            error = !mcs_cmp(pkt_wr, pkt_rd);
            mcs_free(pkt_rd);
        }
    } else {
        error = true;
    }

    end_test();

    if(error) {
        abs_test_add_result(FAIL, "TYPE_OK_DATA");
    } else {
        abs_test_add_result(PASS, "TYPE_OK_DATA");
    }
}

int main()
{
    abs_test_init("mcs");

    run_test(MCS_TYPE_MESSAGE);
    run_test(MCS_TYPE_STATE);
    run_test(MCS_TYPE_PAYLOAD);
    run_test_err();
    run_test_ok();
    run_test_ok_data();

    abs_test_end();
    return 0;
}
