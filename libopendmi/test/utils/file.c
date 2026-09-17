//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/utils/file.h>
#include <opendmi/test/logger.h>

static int test_file_setup(void **pstate);
static int test_file_teardown(void **pstate);

static void test_file_write_read(void **pstate);
static void test_file_seek(void **pstate);
static void test_file_stat(void **pstate);
static void test_file_lock(void **pstate);
static void test_file_get(void **pstate);
static void test_file_get_partial(void **pstate);
static void test_file_get_empty(void **pstate);
static void test_file_get_errors(void **pstate);

static void test_file_create(const char *path, const dmi_data_t *data, size_t size);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// Relative to test working directory
static const char *test_path       = "file-test.bin";
static const char *test_empty_path = "file-test-empty.bin";

static const dmi_data_t test_data[] = "0123456789ABCDEF";

typedef struct test_file_state
{
    dmi_context_t *context;
    FILE          *stream;
    int            fd;
} test_file_state_t;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_file_write_read, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_seek, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_stat, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_lock, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_get, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_get_partial, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_get_empty, test_file_setup, test_file_teardown),
        cmocka_unit_test_setup_teardown(test_file_get_errors, test_file_setup, test_file_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_file_setup(void **pstate)
{
    test_file_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    *pstate = state;

    state->context = dmi_create(0);
    if (state->context == nullptr)
        return -1;

    dmi_set_logger(state->context, &test_logger);

    // Stream is opened in binary mode, and its descriptor is used by tests
    state->stream = fopen(test_path, "w+b");
    if (state->stream == nullptr)
        return -1;

    state->fd = fileno(state->stream);

    return 0;
}

static int test_file_teardown(void **pstate)
{
    test_file_state_t *state = *pstate;

    if (state != nullptr) {
        if (state->stream != nullptr)
            fclose(state->stream);

        dmi_destroy(state->context);
        free(state);
    }

    remove(test_path);
    remove(test_empty_path);

    *pstate = nullptr;

    return 0;
}

static void test_file_write_read(void **pstate)
{
    const test_file_state_t *state = *pstate;
    dmi_data_t buffer[sizeof(test_data)] = {};

    assert_int_equal(dmi_file_write(state->fd, test_data, sizeof(test_data)), sizeof(test_data));

    // Read from offset
    assert_int_equal(dmi_file_read(state->fd, buffer, 10, 4), 4);
    assert_memory_equal(buffer, "ABCD", 4);

    // Read from current position
    assert_int_equal(dmi_file_read(state->fd, buffer, -1, 2), 2);
    assert_memory_equal(buffer, "EF", 2);

    // Short read at the end of file
    assert_int_equal(dmi_file_read(state->fd, buffer, 12, sizeof(buffer)), sizeof(test_data) - 12);
    assert_memory_equal(buffer, "CDEF", 5);

    // Read beyond the end of file
    assert_int_equal(dmi_file_read(state->fd, buffer, 100, sizeof(buffer)), 0);

    // Zero-sized operations
    assert_int_equal(dmi_file_read(state->fd, buffer, 0, 0), 0);
    assert_int_equal(dmi_file_write(state->fd, test_data, 0), 0);
}

static void test_file_seek(void **pstate)
{
    const test_file_state_t *state = *pstate;

    assert_int_equal(dmi_file_write(state->fd, test_data, sizeof(test_data)), sizeof(test_data));
    assert_int_equal(dmi_file_tell(state->fd), sizeof(test_data));

    assert_int_equal(dmi_file_seek(state->fd, 4, SEEK_SET), 4);
    assert_int_equal(dmi_file_tell(state->fd), 4);

    assert_int_equal(dmi_file_seek(state->fd, 2, SEEK_CUR), 6);
    assert_int_equal(dmi_file_seek(state->fd, -1, SEEK_END), sizeof(test_data) - 1);

    // Negative position is an error
    assert_true(dmi_file_seek(state->fd, -1, SEEK_SET) < 0);
}

static void test_file_stat(void **pstate)
{
    const test_file_state_t *state = *pstate;
    dmi_file_stat_t st;

    assert_int_equal(dmi_file_write(state->fd, test_data, sizeof(test_data)), sizeof(test_data));

    assert_int_equal(dmi_file_stat(state->fd, &st), 0);
    assert_true(S_ISREG(st.st_mode));
    assert_int_equal(st.st_size, sizeof(test_data));
}

static void test_file_lock(void **pstate)
{
    const test_file_state_t *state = *pstate;

    assert_int_equal(dmi_file_write(state->fd, test_data, sizeof(test_data)), sizeof(test_data));
    assert_int_equal(dmi_file_seek(state->fd, 0, SEEK_SET), 0);

    assert_true(dmi_file_lock(state->fd, sizeof(test_data)));
    assert_true(dmi_file_unlock(state->fd, sizeof(test_data)));
}

static void test_file_get(void **pstate)
{
    test_file_state_t *state = *pstate;

    fclose(state->stream);
    state->stream = nullptr;

    test_file_create(test_path, test_data, sizeof(test_data));

    // Whole file
    size_t length = 0;
    dmi_data_t *data = dmi_file_get(state->context, test_path, -1, &length);
    assert_non_null(data);
    assert_int_equal(length, sizeof(test_data));
    assert_memory_equal(data, test_data, sizeof(test_data));
    dmi_free(data);

    // Region from offset
    length = 4;
    data = dmi_file_get(state->context, test_path, 10, &length);
    assert_non_null(data);
    assert_int_equal(length, 4);
    assert_memory_equal(data, "ABCD", 4);
    dmi_free(data);
}

static void test_file_get_partial(void **pstate)
{
    test_file_state_t *state = *pstate;

    fclose(state->stream);
    state->stream = nullptr;

    test_file_create(test_path, test_data, sizeof(test_data));

    // Requested region exceeds the end of file
    size_t length = 8;
    dmi_data_t *data = dmi_file_get(state->context, test_path, 12, &length);
    assert_non_null(data);
    assert_int_equal(length, sizeof(test_data) - 12);
    assert_memory_equal(data, "CDEF", 5);
    dmi_free(data);
}

static void test_file_get_empty(void **pstate)
{
    test_file_state_t *state = *pstate;

    test_file_create(test_empty_path, test_data, 0);

    // Empty file is not an error
    size_t length = 0;
    dmi_error_clear(state->context);
    dmi_data_t *data = dmi_file_get(state->context, test_empty_path, -1, &length);
    assert_non_null(data);
    assert_int_equal(length, 0);
    assert_null(dmi_error_peek_last(state->context));
    dmi_free(data);
}

static void test_file_get_errors(void **pstate)
{
    test_file_state_t *state = *pstate;
    size_t length = 0;

    // Missing file
    dmi_error_clear(state->context);
    assert_null(dmi_file_get(state->context, "file-test-missing.bin", -1, &length));
    assert_int_equal(dmi_error_peek_last(state->context)->reason, DMI_ERROR_FILE_OPEN);

    // Invalid arguments
    dmi_error_clear(state->context);
    assert_null(dmi_file_get(state->context, nullptr, -1, &length));
    assert_int_equal(dmi_error_peek_last(state->context)->reason, DMI_ERROR_NULL_ARGUMENT);

    dmi_error_clear(state->context);
    assert_null(dmi_file_get(state->context, test_path, -1, nullptr));
    assert_int_equal(dmi_error_peek_last(state->context)->reason, DMI_ERROR_NULL_ARGUMENT);

    assert_null(dmi_file_get(nullptr, test_path, -1, &length));
}

static void test_file_create(const char *path, const dmi_data_t *data, size_t size)
{
    FILE *stream = fopen(path, "wb");

    if (stream != nullptr) {
        size_t written = fwrite(data, 1, size, stream);
        int rv = fclose(stream);

        assert_int_equal(written, size);
        assert_int_equal(rv, 0);
    } else {
        fail_msg("Unable to create file %s", path);
    }
}
