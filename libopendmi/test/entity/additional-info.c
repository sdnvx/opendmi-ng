//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/additional-info.h>

static int test_additional_info_setup(void **pstate);
static int test_additional_info_teardown(void **pstate);

static void test_additional_info_decode(void **pstate);
static void test_additional_info_long_value(void **pstate);
static void test_additional_info_short_entry(void **pstate);
static void test_additional_info_truncated_value(void **pstate);

static dmi_entity_t *test_additional_info_create(
        dmi_buffer_t *buffer, dmi_data_t *data, size_t count,
        const dmi_data_t *entries, size_t length);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Structure header, entry count and terminating string set are added to the
// entries by test_additional_info_create()
#define TEST_ADDITIONAL_INFO_OVERHEAD (sizeof(dmi_header_t) + 1 + 2)

// Value longer than the fixed buffer used by the decoder before
#define TEST_LONG_VALUE_LENGTH 40

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_additional_info_decode),
        cmocka_unit_test(test_additional_info_long_value),
        cmocka_unit_test(test_additional_info_short_entry),
        cmocka_unit_test(test_additional_info_truncated_value)
    };

    return cmocka_run_group_tests(tests, test_additional_info_setup, test_additional_info_teardown);
}

static int test_additional_info_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_additional_info_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_additional_info_decode(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Slot type with unchanged value, and a word value without string
    static const dmi_data_t entries[] = {
        0x06, 0x1E, 0x00, 0x05, 0x00, 0xAA,
        0x07, 0x00, 0x00, 0x05, 0x00, 0xDC, 0x05
    };

    dmi_data_t data[sizeof(entries) + TEST_ADDITIONAL_INFO_OVERHEAD];
    dmi_entity_t *entity = test_additional_info_create(entity_buffer, data, 2, entries, sizeof(entries));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_additional_info_t *info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
    assert_non_null(info);
    assert_int_equal(info->entry_count, 2);

    const dmi_additional_info_entry_t *entry = &info->entries[0];
    assert_int_equal(entry->ref_handle, 0x001E);
    assert_int_equal(entry->ref_offset, 0x05);
    assert_null(entry->string);
    assert_int_equal(entry->value.length, 1);
    assert_int_equal(entry->value.data[0], 0xAA);

    // Value is referenced in the data of the structure, which the buffer of
    // the entity holds
    entry = &info->entries[1];
    assert_int_equal(entry->ref_handle, 0x0000);
    assert_int_equal(entry->value.length, 2);
    assert_ptr_equal(entry->value.data,
                     dmi_buffer_at(entity->buffer, entity->offset, entity->total_length) +
                         sizeof(dmi_header_t) + 1 + 6 + DMI_ADDITIONAL_INFO_ENTRY_HEADER);
    assert_int_equal(entry->value.data[0], 0xDC);
    assert_int_equal(entry->value.data[1], 0x05);

    dmi_entity_destroy(entity);
}

static void test_additional_info_long_value(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Value length is not limited
    dmi_data_t entries[DMI_ADDITIONAL_INFO_ENTRY_HEADER + TEST_LONG_VALUE_LENGTH] = {
        DMI_ADDITIONAL_INFO_ENTRY_HEADER + TEST_LONG_VALUE_LENGTH, 0x1C, 0x00, 0x00, 0x00
    };

    for (size_t i = 0; i < TEST_LONG_VALUE_LENGTH; i++)
        entries[DMI_ADDITIONAL_INFO_ENTRY_HEADER + i] = (dmi_data_t)i;

    dmi_data_t data[sizeof(entries) + TEST_ADDITIONAL_INFO_OVERHEAD];
    dmi_entity_t *entity = test_additional_info_create(entity_buffer, data, 1, entries, sizeof(entries));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_additional_info_t *info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
    assert_non_null(info);
    assert_int_equal(info->entries[0].value.length, TEST_LONG_VALUE_LENGTH);
    assert_memory_equal(info->entries[0].value.data, entries + DMI_ADDITIONAL_INFO_ENTRY_HEADER,
                        TEST_LONG_VALUE_LENGTH);

    dmi_entity_destroy(entity);
}

static void test_additional_info_short_entry(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Entry shorter than its header, and entry without value
    static const dmi_data_t lengths[] = { 0x00, 0x03, DMI_ADDITIONAL_INFO_ENTRY_HEADER };

    for (size_t i = 0; i < countof(lengths); i++) {
        const dmi_data_t entries[] = { lengths[i], 0x1E, 0x00, 0x05, 0x00, 0xAA };

        dmi_data_t data[sizeof(entries) + TEST_ADDITIONAL_INFO_OVERHEAD];
        dmi_entity_t *entity = test_additional_info_create(entity_buffer, data, 1, entries, sizeof(entries));
        assert_non_null(entity);

        bool decoded = dmi_entity_decode(entity);
        dmi_entity_destroy(entity);

        if (decoded)
            fail_msg("Entry length %u: decoded", lengths[i]);
    }
}

static void test_additional_info_truncated_value(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_buffer_t  *entity_buffer = dmi_buffer_create(context);

    // Value is shorter than specified by entry length, the rest is kept
    static const dmi_data_t entries[] = { 0x09, 0x1E, 0x00, 0x05, 0x00, 0xAA, 0xBB };

    dmi_data_t data[sizeof(entries) + TEST_ADDITIONAL_INFO_OVERHEAD];
    dmi_entity_t *entity = test_additional_info_create(entity_buffer, data, 1, entries, sizeof(entries));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_additional_info_t *info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));
    assert_non_null(info);
    assert_int_equal(info->entries[0].value.length, 2);
    assert_int_equal(info->entries[0].value.data[1], 0xBB);

    dmi_entity_destroy(entity);
}

static dmi_entity_t *test_additional_info_create(
        dmi_buffer_t *buffer, dmi_data_t *data, size_t count,
        const dmi_data_t *entries, size_t length)
{
    const size_t body_length = sizeof(dmi_header_t) + 1 + length;

    const dmi_data_t header[] = { 40, (dmi_data_t)body_length, 0x00, 0x30, (dmi_data_t)count };

    memcpy(data, header, sizeof(header));
    memcpy(data + sizeof(header), entries, length);

    // Empty string set
    data[body_length]     = 0;
    data[body_length + 1] = 0;

    return dmi_test_entity_create(buffer, data, body_length + 2);
}
