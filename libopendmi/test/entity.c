//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

static int test_entity_setup(void **pstate);
static int test_entity_teardown(void **pstate);

static void test_entity_create_fits_exactly(void **pstate);
static void test_entity_create_no_strings(void **pstate);
static void test_entity_create_end_of_table(void **pstate);
static void test_entity_create_strings(void **pstate);
static void test_entity_create_leading_nul(void **pstate);
static void test_entity_create_truncated_header(void **pstate);
static void test_entity_create_truncated_body(void **pstate);
static void test_entity_create_missing_string_set(void **pstate);
static void test_entity_create_unterminated_string(void **pstate);
static void test_entity_create_unterminated_string_set(void **pstate);
static void test_entity_stop(void **pstate);
static void test_entity_incomplete(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_entity_create_fits_exactly),
        cmocka_unit_test(test_entity_create_no_strings),
        cmocka_unit_test(test_entity_create_end_of_table),
        cmocka_unit_test(test_entity_create_strings),
        cmocka_unit_test(test_entity_create_leading_nul),
        cmocka_unit_test(test_entity_create_truncated_header),
        cmocka_unit_test(test_entity_create_truncated_body),
        cmocka_unit_test(test_entity_create_missing_string_set),
        cmocka_unit_test(test_entity_create_unterminated_string),
        cmocka_unit_test(test_entity_create_unterminated_string_set),
        cmocka_unit_test(test_entity_stop),
        cmocka_unit_test(test_entity_incomplete)
    };

    return cmocka_run_group_tests(tests, test_entity_setup, test_entity_teardown);
}

static int test_entity_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_entity_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void assert_entity_truncated(dmi_context_t *context, const void *data, size_t max_length)
{
    dmi_error_clear(context);

    assert_null(dmi_entity_create(context, data, max_length));
    assert_non_null(dmi_error_peek_last(context));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ENTITY_TRUNCATED);
}

static void test_entity_create_fits_exactly(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Inactive structure with 1-byte body and two strings, no bytes after it
    static const uint8_t data[] = {
        126, 5, 0x34, 0x12, 0xAA,
        'A', 0, 'B', 'C', 0, 0
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    assert_int_equal(entity->handle, 0x1234);
    assert_int_equal(entity->body_length, 5);
    assert_int_equal(entity->extra_length, 6);
    assert_int_equal(entity->total_length, sizeof(data));
    assert_int_equal(entity->string_count, 2);
    assert_string_equal(dmi_entity_string(entity, 1), "A");
    assert_string_equal(dmi_entity_string(entity, 2), "BC");

    dmi_entity_destroy(entity);
}

static void test_entity_create_no_strings(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = { 126, 4, 0x01, 0x00, 0, 0 };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    assert_int_equal(entity->extra_length, 2);
    assert_int_equal(entity->total_length, sizeof(data));
    assert_int_equal(entity->string_count, 0);

    dmi_entity_destroy(entity);
}

static void test_entity_create_end_of_table(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Zero length is treated as end-of-table marker
    static const uint8_t data[] = { 0, 0, 0xFF, 0xFE, 0, 0 };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    assert_int_equal(entity->type, DMI_TYPE(END_OF_TABLE));
    assert_int_equal(entity->total_length, sizeof(data));

    dmi_entity_destroy(entity);
}

static void test_entity_create_strings(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Strings are trimmed, and empty string terminates the string set
    static const uint8_t data[] = {
        126, 4, 0x01, 0x00,
        ' ', 'A', ' ', 0, 'B', 'C', 0, 0, 'D', 0, 0
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    assert_int_equal(entity->string_count, 2);
    assert_int_equal(entity->total_length, 12);
    assert_string_equal(dmi_entity_string_ex(entity, 1, true), " A ");
    assert_string_equal(dmi_entity_string_ex(entity, 1, false), "A");
    assert_string_equal(dmi_entity_string_ex(entity, 2, true), "BC");
    assert_string_equal(dmi_entity_string_ex(entity, 2, false), "BC");
    assert_null(dmi_entity_string_ex(entity, 3, false));

    dmi_entity_destroy(entity);
}

static void test_entity_create_leading_nul(void **pstate)
{
    dmi_context_t *context = *pstate;

    // String set starting with NUL has no strings, but structure still
    // extends up to the first double NUL
    static const uint8_t data[] = { 126, 4, 0x01, 0x00, 0, 'A', 0, 0 };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    assert_int_equal(entity->string_count, 0);
    assert_int_equal(entity->total_length, sizeof(data));
    assert_null(dmi_entity_string_ex(entity, 1, false));

    dmi_entity_destroy(entity);

    // Terminator must still fit into available data
    assert_entity_truncated(context, data, sizeof(data) - 1);
}

static void test_entity_create_truncated_header(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = { 126, 4, 0x01, 0x00, 0, 0 };

    for (size_t length = 0; length < sizeof(data); length++)
        assert_entity_truncated(context, data, length);
}

static void test_entity_create_truncated_body(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Declared length exceeds available data. Bytes past the limit look like
    // a valid string set, and must not be accessed.
    static const uint8_t data[] = {
        126, 92, 0x02, 0x00, 0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC,
        0xDD, 0xEE, 0, 0
    };

    assert_entity_truncated(context, data, 18);
    assert_entity_truncated(context, data, sizeof(data));
}

static void test_entity_create_missing_string_set(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Body fits, but there is no room for string set terminator
    static const uint8_t data[] = { 126, 6, 0x01, 0x00, 0xAA, 0xBB, 0, 0 };

    assert_entity_truncated(context, data, 6);
    assert_entity_truncated(context, data, 7);
}

static void test_entity_create_unterminated_string(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = { 126, 4, 0x01, 0x00, 'A', 'B', 'C', 0, 0 };

    assert_entity_truncated(context, data, 6);
    assert_entity_truncated(context, data, 7);
}

static void test_entity_create_unterminated_string_set(void **pstate)
{
    dmi_context_t *context = *pstate;

    // String is terminated, but string set is not
    static const uint8_t data[] = { 126, 4, 0x01, 0x00, 'A', 0, 0 };

    assert_entity_truncated(context, data, 6);

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    dmi_entity_destroy(entity);
}

static void test_entity_stop(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        126, 6, 0x01, 0x00, 0x11, 0x22,
        0, 0
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    // All data has been read, but decoder knows more fields
    dmi_stream_initialize(&entity->stream, entity);
    assert_true(dmi_stream_skip(&entity->stream, sizeof(data) - 2));

    assert_true(dmi_entity_stop(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_PARTIAL);
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    dmi_entity_destroy(entity);
}

static void test_entity_incomplete(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const uint8_t data[] = {
        126, 6, 0x01, 0x00, 0x11, 0x22,
        0, 0
    };

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);

    // Remaining data is shorter than the next set of fields
    dmi_stream_initialize(&entity->stream, entity);
    assert_true(dmi_stream_skip(&entity->stream, sizeof(dmi_header_t) + 1));

    assert_true(dmi_entity_incomplete(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_false(entity->state & DMI_ENTITY_STATE_PARTIAL);

    dmi_entity_destroy(entity);
}
