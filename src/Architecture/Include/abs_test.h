#ifndef __ABS_TEST_H
#define __ABS_TEST_H

#include <stdarg.h>
#include <abs.h>

typedef enum ABSTestResult {
    PASS,   /* Passed */
    XFAIL,  /* Expected failure */
    SKIP,   /* Skipped */
    FAIL,   /* Failure */
} ABSTestResult;

enum_string(ABSTestResult, test_result_string)
    {PASS, "PASS"},
    {XFAIL, "XFAIL"},
    {SKIP, "SKIP"},
    {FAIL, "FAIL"},
};

void abs_test_init(const char *name);
void abs_test_end(void);
void abs_test_add_result(ABSTestResult res, const char *name);
void abs_test_fail_and_exit(const char *name);
void abs_test_printf(char *fmt, ...);

#endif
