//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/log.h>
#include <opendmi/test/logger.h>

static int test_entry_setup(void **pstate);
static int test_entry_teardown(void **pstate);

static void test_entry_decode_legacy(void **pstate);
static void test_entry_decode_legacy_checksum(void **pstate);
static void test_entry_decode_v21(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_entry_decode_legacy, test_entry_setup, test_entry_teardown),
        cmocka_unit_test_setup_teardown(test_entry_decode_legacy_checksum, test_entry_setup, test_entry_teardown),
        cmocka_unit_test_setup_teardown(test_entry_decode_v21, test_entry_setup, test_entry_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_entry_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_entry_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void set_checksum(uint8_t *data, size_t checksum_offset, size_t length)
{
    uint8_t sum = 0;

    data[checksum_offset] = 0;
    for (size_t i = 0; i < length; i++)
        sum += data[i];

    data[checksum_offset] = (uint8_t)(0x100 - sum);
}

static void fill_legacy_entry(uint8_t *data, uint8_t bcd_revision)
{
    static const uint8_t template[0x0F] = {
        '_', 'D', 'M', 'I', '_',        // Anchor
        0x00,                           // Checksum
        0x34, 0x12,                     // Table length
        0x00, 0x00, 0x0F, 0x00,         // Table address
        0x2A, 0x00,                     // Number of structures
        0x00                            // BCD revision
    };

    memcpy(data, template, sizeof(template));
    data[0x0E] = bcd_revision;
    set_checksum(data, 0x05, sizeof(template));
}

static void test_entry_decode_legacy(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const struct {
        uint8_t bcd_revision;
        dmi_version_t expected;
    } test_data[] = {
        { 0x20, DMI_VERSION(2, 0, 0) },
        { 0x21, DMI_VERSION(2, 1, 0) },
        { 0x23, DMI_VERSION(2, 3, 0) },
        { 0x31, DMI_VERSION(3, 1, 0) }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        uint8_t data[0x0F];

        fill_legacy_entry(data, test_data[i].bcd_revision);

        context->state.smbios_version = 0;
        assert_true(dmi_entry_decode(context, data, sizeof(data)));

        assert_int_equal(context->state.smbios_version, test_data[i].expected);
        assert_int_equal(context->state.address_size, sizeof(uint32_t));
        assert_int_equal(context->state.table_area_size, 0x1234);
        assert_int_equal(context->state.table_area_addr, 0x000F0000);
        assert_int_equal(context->state.entity_count, 42);
    }
}

static void test_entry_decode_legacy_checksum(void **pstate)
{
    dmi_context_t *context = *pstate;
    uint8_t data[0x0F];

    fill_legacy_entry(data, 0x21);
    data[0x05]++;

    assert_false(dmi_entry_decode(context, data, sizeof(data)));
}

static void test_entry_decode_v21(void **pstate)
{
    dmi_context_t *context = *pstate;
    uint8_t data[0x1F] = {
        '_', 'S', 'M', '_',             // Anchor
        0x00,                           // Checksum
        0x1F,                           // Entry point length
        0x02, 0x07,                     // SMBIOS version
        0x00, 0x01,                     // Maximum structure size
        0x00,                           // Entry point revision
        0x00, 0x00, 0x00, 0x00, 0x00    // Formatted area
    };

    // Intermediate entry point has its own (older) BCD revision, which must
    // not override SMBIOS version from the entry point
    fill_legacy_entry(data + 0x10, 0x26);
    set_checksum(data, 0x04, sizeof(data));

    assert_true(dmi_entry_decode(context, data, sizeof(data)));

    assert_int_equal(context->state.smbios_version, DMI_VERSION(2, 7, 0));
    assert_int_equal(context->state.entity_max_size, 0x100);
    assert_int_equal(context->state.table_area_size, 0x1234);
    assert_int_equal(context->state.entity_count, 42);
}
