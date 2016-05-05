#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

#include <mcs.h>
#include <sdb.h>
#include <abs_test.h>

#define SDB_PATH "../../../SDB/sdb"

int pid = -1;

pthread_cond_t callback_cond;
pthread_mutex_t callback_lock;

void sdb_test_end(void);
void *sdb_test_thread(void *arg);

static void signal_handler(int sig)
{
    if(sig == SIGSEGV) {
        sdb_test_end();
        abs_test_fail_and_exit("Segmentation fault\n");
    }
}

void callback_test(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    *pkt_out = mcs_ok_packet(pkt_in);
    mcs_free(pkt_in);

    abs_test_add_result(PASS, "Call callback");
}

static void sdb_test_init(void)
{
    int ret;
    bool fail_callback;
    char name[] = "app";
    MCSPacket *pkt;
    SDBPendingPacket *p_pkt;

    pid = fork();

    if(pid < 0) {
        abs_test_fail_and_exit("SDB Fork");
    } else if(pid == 0) {
        execl(SDB_PATH, SDB_PATH, NULL);
        abs_test_printf("Could not exec\n");
        exit(1);
    }

    sleep(2);
    if(waitpid(pid, NULL, WNOHANG) != 0) {
        abs_test_fail_and_exit("SDB Execution");
    }

    /* Register callback */
    ret = sdb_register_callback(MCS_MESSAGE_TEST, callback_test);
    if(ret != 0) {
        fail_callback = true;
        abs_test_add_result(FAIL, "Register callback");
    } else {
        fail_callback = false;
        abs_test_add_result(PASS, "Register callback");
    }

    /* Connecting to SDB */
    ret = sdb_connect(name, SDB_GROUP_APP);
    if(ret != 0) {
        abs_test_add_result(FAIL, "Connect to SDB");
        goto error;
    }

    abs_test_add_result(PASS, "Connect to SDB");

    /* Send sync */
    ret = sdb_send_sync_and_free(mcs_create_packet_with_dest(MCS_MESSAGE_TEST,
                                        name, 0, NULL, 0, NULL), &pkt);
    if(ret != 0) {
        abs_test_printf("Function returned error\n");
        abs_test_add_result(FAIL, "Send sync");
    } else if(pkt->type != MCS_TYPE_OK) {
        if(pkt->type == MCS_TYPE_ERR) {
            if(fail_callback) {
                abs_test_add_result(XFAIL, "Send sync");
            } else {
                abs_test_printf("Wrong response packet. Error %d\n",
                                            mcs_err_code_from_command(pkt));
                abs_test_add_result(FAIL, "Send sync");
            }
        } else {
            abs_test_printf("Wrong reponse packet. Type: %hhd\n", pkt->type);
            abs_test_add_result(FAIL, "Send sync");
        }
    } else {
        abs_test_add_result(PASS, "Send sync");
    }

    mcs_free(pkt);

    /* Send async */
    ret = sdb_send_async_and_free(mcs_create_packet_with_dest(MCS_MESSAGE_TEST,
                                        name, 0, NULL, 0, NULL), &p_pkt);
    if(ret != 0) {
        abs_test_printf("Function returned error\n");
        abs_test_add_result(FAIL, "Send async");
    } else {
        abs_test_add_result(PASS, "Send async");

        /* Check async */
        sleep(1);
        ret = sdb_check_async(p_pkt);

        if(ret == 0) {
            abs_test_printf("1 second and no answer\n");
            sleep(1);

            ret = sdb_check_async(p_pkt);
            if(ret != 1) {
                abs_test_printf("2 seconds and no answer\n");
                abs_test_add_result(FAIL, "Check async");
            }
        } else if(ret < 0) {
            abs_test_printf("Function returned error\n");
            abs_test_add_result(FAIL, "Check async");
        }

        if(ret == 1) {
            abs_test_add_result(PASS, "Check async");

            /* Wait async */
            ret = sdb_wait_async(p_pkt);

            if(ret != 0) {
                abs_test_printf("Function returned error\n");
                abs_test_add_result(FAIL, "Wait async");
            } else if(p_pkt->pp_pkt->type != MCS_TYPE_OK) {
                if(p_pkt->pp_pkt->type == MCS_TYPE_ERR) {
                    if(fail_callback) {
                        abs_test_add_result(XFAIL, "Wait async");
                    } else {
                        abs_test_printf("Wrong response packet. Error %d\n",
                                    mcs_err_code_from_command(p_pkt->pp_pkt));
                        abs_test_add_result(FAIL, "Wait async");
                    }
                } else {
                    abs_test_printf("Wrong reponse packet. Type: %hhd\n",
                                                        p_pkt->pp_pkt->type);
                    abs_test_add_result(FAIL, "Wait async");
                }
            } else {
                abs_test_add_result(PASS, "Wait async");
            }

            mcs_free(p_pkt->pp_pkt);
        }

        sdb_pending_packet_free(p_pkt);
    }

    /* Disconnect */
    sdb_disconnect();

error:
    sdb_test_end();
    abs_test_end();
}

void sdb_test_end(void)
{
    if(pid != -1) {
        kill(pid, SIGINT);
    }
}

int main()
{
    abs_test_init("sdb_libs");

    signal(SIGSEGV, signal_handler); /* Overwrite the signal handler */
    signal(SIGINT, signal_handler);

    sdb_test_init();
    return 0;
}
