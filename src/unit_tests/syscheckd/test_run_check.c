/*
 * Copyright (C) 2015-2019, Wazuh Inc.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "../syscheckd/syscheck.h"

#ifdef TEST_WINAGENT
void set_priority_windows_thread();
#endif

struct state {
    unsigned int sleep_seconds;
} state;

/* redefinitons/wrapping */

int __wrap__minfo(const char * file, int line, const char * func, const char *msg, ...)
{
    check_expected(msg);
    return 1;
}

void __wrap__mdebug1(const char * file, int line, const char * func, const char *msg, ...) {
    char formatted_msg[OS_MAXSTR];
    va_list args;

    va_start(args, msg);
    vsnprintf(formatted_msg, OS_MAXSTR, msg, args);
    va_end(args);

    check_expected(formatted_msg);
}

void __wrap__merror(const char * file, int line, const char * func, const char *msg, ...) {
    char formatted_msg[OS_MAXSTR];
    va_list args;

    va_start(args, msg);
    vsnprintf(formatted_msg, OS_MAXSTR, msg, args);
    va_end(args);

    check_expected(formatted_msg);
}

#ifdef TEST_AGENT
char *_read_file(const char *high_name, const char *low_name, const char *defines_file) __attribute__((nonnull(3)));

int __wrap_getDefine_Int(const char *high_name, const char *low_name, int min, int max) {
    int ret;
    char *value;
    char *pt;

    /* Try to read from the local define file */
    value = _read_file(high_name, low_name, "./internal_options.conf");
    if (!value) {
        merror_exit(DEF_NOT_FOUND, high_name, low_name);
    }

    pt = value;
    while (*pt != '\0') {
        if (!isdigit((int)*pt)) {
            merror_exit(INV_DEF, high_name, low_name, value);
        }
        pt++;
    }

    ret = atoi(value);
    if ((ret < min) || (ret > max)) {
        merror_exit(INV_DEF, high_name, low_name, value);
    }

    /* Clear memory */
    free(value);

    return (ret);
}

int __wrap_isChroot() {
    return 1;
}
#endif

int __wrap_audit_set_db_consistency() {
    return 1;
}

unsigned int __wrap_sleep(unsigned int seconds) {
    state.sleep_seconds += seconds;
    return 0;
}

int __wrap_SendMSG(int queue, const char *message, const char *locmsg, char loc) {
    (void) queue;
    (void) message;
    (void) locmsg;
    (void) loc;
    return 0;
}

/* Setup */

static int setup(void ** state) {
    (void) state;
    syscheck.max_eps = 100;
    syscheck.sync_max_eps = 10;
    return 0;
}

#ifdef TEST_WINAGENT
int __wrap_realtime_adddir(const char *dir, int whodata) {
    return 1;
}
#endif

/* teardown */

static int free_syscheck(void **state)
{
    (void) state;
    Free_Syscheck(&syscheck);
    return 0;
}

/* tests */

void test_log_realtime_status(void **state)
{
    (void) state;

    log_realtime_status(2);

    expect_string(__wrap__minfo, msg, FIM_REALTIME_STARTED);
    log_realtime_status(1);
    log_realtime_status(1);

    expect_string(__wrap__minfo, msg, FIM_REALTIME_PAUSED);
    log_realtime_status(2);
    log_realtime_status(2);

    expect_string(__wrap__minfo, msg, FIM_REALTIME_RESUMED);
    log_realtime_status(1);
}


void test_fim_whodata_initialize(void **state)
{
    (void) state;
    int ret;

    expect_string(__wrap__mdebug1, formatted_msg, "(6287): Reading configuration file: 'test_syscheck.conf'");
    expect_string(__wrap__mdebug1, formatted_msg, "Found nodiff regex node ^file");
    expect_string(__wrap__mdebug1, formatted_msg, "Found nodiff regex node ^file OK?");
    expect_string(__wrap__mdebug1, formatted_msg, "Found nodiff regex size 0");

    #if defined(TEST_AGENT) || defined(TEST_WINAGENT)
    expect_string(__wrap__mdebug1, formatted_msg, "(6208): Reading Client Configuration [test_syscheck.conf]");
    #endif

    #ifdef TEST_WINAGENT
    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_LOWEST);
    will_return(wrap_SetThreadPriority, true);

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '10'");
    #else
    expect_string(__wrap__mdebug1, formatted_msg, "(6227): Directory added for real time monitoring: '/etc'");
    expect_string(__wrap__mdebug1, formatted_msg, "(6227): Directory added for real time monitoring: '/usr/bin'");
    expect_string(__wrap__mdebug1, formatted_msg, "(6227): Directory added for real time monitoring: '/usr/sbin'");
    #endif

    Read_Syscheck_Config("test_syscheck.conf");

    ret = fim_whodata_initialize();

    assert_int_equal(ret, 0);
}

#ifdef TEST_WINAGENT
void test_set_priority_windows_thread_highest(void **state) {
    syscheck.process_priority = -10;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '-10'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_HIGHEST);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_above_normal(void **state) {
    syscheck.process_priority = -8;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '-8'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_ABOVE_NORMAL);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_normal(void **state) {
    syscheck.process_priority = 0;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '0'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_NORMAL);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_below_normal(void **state) {
    syscheck.process_priority = 2;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '2'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_BELOW_NORMAL);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_lowest(void **state) {
    syscheck.process_priority = 7;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '7'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_LOWEST);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_idle(void **state) {
    syscheck.process_priority = 20;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '20'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_IDLE);
    will_return(wrap_SetThreadPriority, true);

    set_priority_windows_thread();
}

void test_set_priority_windows_thread_error(void **state) {
    syscheck.process_priority = 10;

    expect_string(__wrap__mdebug1, formatted_msg, "(6320): Setting process priority to: '10'");

    will_return(wrap_GetCurrentThread, (HANDLE)123456);

    expect_value(wrap_SetThreadPriority, hThread, (HANDLE)123456);
    expect_value(wrap_SetThreadPriority, nPriority, THREAD_PRIORITY_LOWEST);
    will_return(wrap_SetThreadPriority, false);

    will_return(wrap_GetLastError, 2345);

    expect_string(__wrap__merror, formatted_msg, "Can't set thread priority: 2345");

    set_priority_windows_thread();
}

#endif
void test_fim_send_sync_msg_10_eps(void ** _state) {
    (void) _state;
    syscheck.sync_max_eps = 10;

    // We must not sleep the first 9 times

    state.sleep_seconds = 0;

    for (int i = 1; i < syscheck.sync_max_eps; i++) {
        fim_send_sync_msg("");
        assert_int_equal(state.sleep_seconds, 0);
    }

    // After 10 times, sleep one second

    fim_send_sync_msg("");
    assert_int_equal(state.sleep_seconds, 1);
}

void test_fim_send_sync_msg_0_eps(void ** _state) {
    (void) _state;
    syscheck.sync_max_eps = 0;

    // We must not sleep

    state.sleep_seconds = 0;

    fim_send_sync_msg("");
    assert_int_equal(state.sleep_seconds, 0);
}

void test_send_syscheck_msg_10_eps(void ** _state) {
    (void) _state;
    syscheck.max_eps = 10;

    // We must not sleep the first 9 times

    state.sleep_seconds = 0;

    for (int i = 1; i < syscheck.max_eps; i++) {
        send_syscheck_msg("");
        assert_int_equal(state.sleep_seconds, 0);
    }

    // After 10 times, sleep one second

    send_syscheck_msg("");
    assert_int_equal(state.sleep_seconds, 1);
}

void test_send_syscheck_msg_0_eps(void ** _state) {
    (void) _state;
    syscheck.max_eps = 0;

    // We must not sleep

    state.sleep_seconds = 0;

    send_syscheck_msg("");
    assert_int_equal(state.sleep_seconds, 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_log_realtime_status),
        cmocka_unit_test_teardown(test_fim_whodata_initialize, free_syscheck),

        #ifdef TEST_WINAGENT
        cmocka_unit_test(test_set_priority_windows_thread_highest),
        cmocka_unit_test(test_set_priority_windows_thread_above_normal),
        cmocka_unit_test(test_set_priority_windows_thread_normal),
        cmocka_unit_test(test_set_priority_windows_thread_below_normal),
        cmocka_unit_test(test_set_priority_windows_thread_lowest),
        cmocka_unit_test(test_set_priority_windows_thread_idle),
        cmocka_unit_test(test_set_priority_windows_thread_error),
        #endif

        cmocka_unit_test(test_fim_send_sync_msg_10_eps),
        cmocka_unit_test(test_fim_send_sync_msg_0_eps),
        cmocka_unit_test(test_send_syscheck_msg_10_eps),
        cmocka_unit_test(test_send_syscheck_msg_0_eps),
    };

    return cmocka_run_group_tests(tests, setup, NULL);
}
