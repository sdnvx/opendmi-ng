//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/error.h>

static int test_error_setup(void **pstate);
static int test_error_teardown(void **pstate);

static void test_error_message(void **pstate);
static void test_error_message_unknown(void **pstate);
static void test_error_raise_null_context(void **pstate);
static void test_error_raise(void **pstate);
static void test_error_raise_ex(void **pstate);
static void test_error_peek_first_null_context(void **pstate);
static void test_error_peek_first_empty(void **pstate);
static void test_error_peek_first(void **pstate);
static void test_error_peek_last_null_context(void **pstate);
static void test_error_peek_last_empty(void **pstate);
static void test_error_peek_last(void **pstate);
static void test_error_get_first_null_context(void **pstate);
static void test_error_get_first_empty(void **pstate);
static void test_error_get_first(void **pstate);
static void test_error_get_last_null_context(void **pstate);
static void test_error_get_last_empty(void **pstate);
static void test_error_get_last(void **pstate);
static void test_error_overflow_fifo(void **pstate);
static void test_error_overflow_lifo(void **pstate);
static void test_error_wraparound(void **pstate);
static void test_error_clear_null_context(void **pstate);
static void test_error_clear(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_error_message),
        cmocka_unit_test(test_error_message_unknown),
        cmocka_unit_test(test_error_raise_null_context),
        cmocka_unit_test(test_error_raise),
        cmocka_unit_test(test_error_raise_ex),
        cmocka_unit_test(test_error_peek_first_null_context),
        cmocka_unit_test(test_error_peek_first_empty),
        cmocka_unit_test(test_error_peek_first),
        cmocka_unit_test(test_error_peek_last_null_context),
        cmocka_unit_test(test_error_peek_last_empty),
        cmocka_unit_test(test_error_peek_last),
        cmocka_unit_test(test_error_get_first_null_context),
        cmocka_unit_test(test_error_get_first_empty),
        cmocka_unit_test(test_error_get_first),
        cmocka_unit_test(test_error_get_last_null_context),
        cmocka_unit_test(test_error_get_last_empty),
        cmocka_unit_test(test_error_get_last),
        cmocka_unit_test(test_error_overflow_fifo),
        cmocka_unit_test(test_error_overflow_lifo),
        cmocka_unit_test(test_error_wraparound),
        cmocka_unit_test(test_error_clear_null_context),
        cmocka_unit_test(test_error_clear)
    };

    return cmocka_run_group_tests(tests, test_error_setup, test_error_teardown);
}

static int test_error_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = dmi_cast(*pstate, context);

    return 0;
}

static int test_error_teardown(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    dmi_destroy(context);

    return 0;
}

static void test_error_message(void **pstate)
{
    dmi_unused(pstate);

    for (size_t i = 0; i < __DMI_ERROR_COUNT; i++) {
        const char *result = dmi_error_message(i);
        assert_non_null(result);
    }
}

static void test_error_message_unknown(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_error_message((dmi_error_code_t)-1), "Unknown error");
    assert_string_equal(dmi_error_message(__DMI_ERROR_COUNT), "Unknown error");
    assert_string_equal(dmi_error_message((dmi_error_code_t)__DMI_ERROR_COUNT + 1), "Unknown error");
}

static void test_error_raise_null_context(void **pstate)
{
    dmi_unused(pstate);

    assert_false(dmi_error_raise(nullptr, DMI_ERROR_INVALID_ARGUMENT));
}

static void test_error_raise(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    int line = __LINE__ + 1;
    assert_true(dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT));

    error = dmi_error_peek_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);
    assert_non_null(error->file);
    assert_string_equal(error->file, __FILE__);
    assert_non_null(error->function);
    assert_string_equal(error->function, __func__);
    assert_int_equal(error->line, line);
    assert_null(error->message);

    dmi_error_clear(context);
}

static void test_error_raise_ex(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    int line = __LINE__ + 1;
    assert_true(dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "path: %s", "/dev/mem"));

    error = dmi_error_peek_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_FILE_OPEN);
    assert_non_null(error->file);
    assert_string_equal(error->file, __FILE__);
    assert_non_null(error->function);
    assert_string_equal(error->function, __func__);
    assert_int_equal(error->line, line);

    assert_non_null(error->message);
    assert_string_equal(error->message, "path: /dev/mem");

    dmi_error_clear(context);
}

static void test_error_peek_first_null_context(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_peek_first(nullptr));
}

static void test_error_peek_first_empty(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);
    assert_null(dmi_error_peek_first(context));
}

static void test_error_peek_first(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
    dmi_error_raise(context, DMI_ERROR_INVALID_STATE);

    error = dmi_error_peek_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);

    // peek is non-destructive: second call returns the same entry
    error = dmi_error_peek_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);

    dmi_error_clear(context);
}

static void test_error_peek_last_null_context(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_peek_last(nullptr));
}

static void test_error_peek_last_empty(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);
    assert_null(dmi_error_peek_last(context));
}

static void test_error_peek_last(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
    dmi_error_raise(context, DMI_ERROR_INVALID_STATE);

    error = dmi_error_peek_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    // peek is non-destructive: second call returns the same entry
    error = dmi_error_peek_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    dmi_error_clear(context);
}

static void test_error_get_first_null_context(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_get_first(nullptr));
}

static void test_error_get_first_empty(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);
    assert_null(dmi_error_get_first(context));
}

static void test_error_get_first(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
    dmi_error_raise(context, DMI_ERROR_INVALID_STATE);
    dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);

    // Errors are dequeued in FIFO order (oldest first)
    error = dmi_error_get_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);

    error = dmi_error_get_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    error = dmi_error_get_first(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_OUT_OF_MEMORY);

    // Queue is now empty
    assert_null(dmi_error_get_first(context));
}

static void test_error_get_last_null_context(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_error_get_last(nullptr));
}

static void test_error_get_last_empty(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);
    assert_null(dmi_error_get_last(context));
}

static void test_error_get_last(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const dmi_error_t *error;

    dmi_error_clear(context);

    dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
    dmi_error_raise(context, DMI_ERROR_INVALID_STATE);
    dmi_error_raise(context, DMI_ERROR_OUT_OF_MEMORY);

    // Errors are dequeued in LIFO order (newest first)
    error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_OUT_OF_MEMORY);

    error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ARGUMENT);

    // Queue is now empty
    assert_null(dmi_error_get_last(context));
}

static void raise_numbered_errors(dmi_context_t *context, int first, int count)
{
    for (int i = first; i < first + count; i++)
        assert_true(dmi_error_raise_ex(context, DMI_ERROR_INTERNAL, "%d", i));
}

static void assert_error_number(const dmi_error_t *error, int expected)
{
    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%d", expected);

    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INTERNAL);
    assert_non_null(error->message);
    assert_string_equal(error->message, buffer);
}

static void test_error_overflow_fifo(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);
    const int extra = 5;

    dmi_error_clear(context);

    // Oldest errors are dropped when the queue is full
    raise_numbered_errors(context, 0, DMI_ERROR_MAX_DEPTH + extra);
    assert_int_equal(context->error_queue.count, DMI_ERROR_MAX_DEPTH);

    assert_error_number(dmi_error_peek_first(context), extra);
    assert_error_number(dmi_error_peek_last(context), DMI_ERROR_MAX_DEPTH + extra - 1);

    for (int i = 0; i < DMI_ERROR_MAX_DEPTH; i++)
        assert_error_number(dmi_error_get_first(context), extra + i);

    assert_null(dmi_error_get_first(context));

    dmi_error_clear(context);
}

static void test_error_overflow_lifo(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);

    // Filling the queue exactly must not drop anything
    raise_numbered_errors(context, 0, DMI_ERROR_MAX_DEPTH);
    assert_int_equal(context->error_queue.count, DMI_ERROR_MAX_DEPTH);
    assert_error_number(dmi_error_peek_first(context), 0);

    // One more error drops the oldest one
    raise_numbered_errors(context, DMI_ERROR_MAX_DEPTH, 1);

    for (int i = DMI_ERROR_MAX_DEPTH; i > 0; i--)
        assert_error_number(dmi_error_get_last(context), i);

    assert_null(dmi_error_get_last(context));

    dmi_error_clear(context);
}

static void test_error_wraparound(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_clear(context);

    // Move the head of the queue, so that new errors wrap around the buffer
    raise_numbered_errors(context, 0, 10);
    for (int i = 0; i < 8; i++)
        assert_error_number(dmi_error_get_first(context), i);

    raise_numbered_errors(context, 10, DMI_ERROR_MAX_DEPTH);
    assert_int_equal(context->error_queue.count, DMI_ERROR_MAX_DEPTH);

    assert_error_number(dmi_error_peek_first(context), 10);
    assert_error_number(dmi_error_peek_last(context), DMI_ERROR_MAX_DEPTH + 9);

    for (int i = 0; i < DMI_ERROR_MAX_DEPTH; i++)
        assert_error_number(dmi_error_get_first(context), 10 + i);

    assert_null(dmi_error_get_first(context));

    dmi_error_clear(context);
}

static void test_error_clear_null_context(void **pstate)
{
    dmi_unused(pstate);

    // Must not crash on NULL context
    dmi_error_clear(nullptr);
}

static void test_error_clear(void **pstate)
{
    dmi_context_t *context = dmi_cast(context, *pstate);

    dmi_error_raise(context, DMI_ERROR_INVALID_ARGUMENT);
    dmi_error_raise_ex(context, DMI_ERROR_FILE_OPEN, "path: %s", "/dev/mem");

    dmi_error_clear(context);

    assert_null(dmi_error_peek_first(context));
    assert_null(dmi_error_peek_last(context));
    assert_null(dmi_error_get_first(context));
    assert_null(dmi_error_get_last(context));
}
