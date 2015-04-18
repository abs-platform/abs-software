#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <abs_test.h>

FILE *abs_test_log_fd = NULL;

static void signal_handler()
{
    abs_test_printf("Segmentation fault\n");
    abs_test_add_result(FAIL, NULL);
}

void abs_test_init(const char *name)
{
    char log_name[256];

    signal(SIGSEGV, signal_handler);

    sprintf(log_name, "%s.abs.test", name);
    abs_test_log_fd = fopen(log_name, "w");
    if(abs_test_log_fd == NULL) {
        printf("Could not create the log file. Exiting\n");
        exit(1);
    }
}

void abs_test_end(void)
{
    fclose(abs_test_log_fd);

    exit(0);
}

void abs_test_add_result(ABSTestResult res, const char *name)
{
    const char *in_name = NULL;

    if(name == NULL) {
        strcpy((char *)in_name, "");
    } else {
        in_name = name;
    }

    fprintf(abs_test_log_fd, "%s: %s\n", test_result_string[res].str, in_name);
}

void abs_test_fail_and_exit(const char *name)
{
    abs_test_add_result(FAIL, name);
    abs_test_end();
}

void abs_test_printf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(abs_test_log_fd, "# ");
    vfprintf(abs_test_log_fd, fmt, args);
    va_end(args);
}
