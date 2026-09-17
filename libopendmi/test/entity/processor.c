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
#include <opendmi/test/logger.h>

#include <opendmi/entity/processor.h>

static void test_processor_status_name(void **pstate);
static void test_processor_decode_status(void **pstate);
static void test_processor_decode_version(void **pstate);
static void test_processor_decode_incomplete(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// Processor information structure (SMBIOS 2.0) with strings
static const uint8_t test_processor_data[] = {
    4, 0x1A, 0x00, 0x10,                            // Header
    0x01, 0x03, 0x01, 0x02,                         // Socket, type, family, vendor
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Processor ID
    0x03, 0x8C,                                     // Version, voltage
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // Clock and speeds
    0x00, 0x01,                                     // Status, upgrade
    'C', 'P', 'U', '0', 0,
    'V', 'e', 'n', 'd', 'o', 'r', 0,
    'M', 'o', 'd', 'e', 'l', 0,
    0
};

// Offset of the status field
static const size_t test_status_offset = 0x18;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_processor_status_name),
        cmocka_unit_test(test_processor_decode_status),
        cmocka_unit_test(test_processor_decode_version),
        cmocka_unit_test(test_processor_decode_incomplete)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_processor_status_name(void **pstate)
{
    dmi_unused(pstate);

    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_UNKNOWN));
    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_ENABLED));
    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_DISABLED_BY_USER));
    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_DISABLED_BY_FW));
    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_IDLE));
    assert_non_null(dmi_processor_status_name(DMI_PROCESSOR_STATUS_OTHER));

    // Reserved values
    assert_null(dmi_processor_status_name(0x05));
    assert_null(dmi_processor_status_name(0x06));
    assert_null(dmi_processor_status_name(__DMI_PROCESSOR_STATUS_COUNT));
}

static void test_processor_decode_status(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        uint8_t                value;
        bool                   is_populated;
        dmi_processor_status_t status;
    } cases[] = {
        { 0x00, false, DMI_PROCESSOR_STATUS_UNKNOWN          },
        { 0x41, true,  DMI_PROCESSOR_STATUS_ENABLED          },
        { 0x42, true,  DMI_PROCESSOR_STATUS_DISABLED_BY_USER },
        { 0x43, true,  DMI_PROCESSOR_STATUS_DISABLED_BY_FW   },
        { 0x04, false, DMI_PROCESSOR_STATUS_IDLE             },
        { 0x47, true,  DMI_PROCESSOR_STATUS_OTHER            },
        // Reserved bits are ignored
        { 0xB9, false, DMI_PROCESSOR_STATUS_ENABLED          }
    };

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    for (size_t i = 0; i < countof(cases); i++) {
        uint8_t data[sizeof(test_processor_data)];

        memcpy(data, test_processor_data, sizeof(data));
        data[test_status_offset] = cases[i].value;

        dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
        assert_non_null(info);
        assert_int_equal(info->is_populated, cases[i].is_populated);
        assert_int_equal(info->status, cases[i].status);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static void test_processor_decode_version(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    dmi_entity_t *entity = dmi_entity_create(context, test_processor_data, sizeof(test_processor_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    assert_non_null(info);
    assert_string_equal(info->socket_designation, "CPU0");
    assert_string_equal(info->vendor, "Vendor");
    assert_string_equal(info->version, "Model");

    // Cache handles are not set in SMBIOS 2.0 structures
    assert_int_equal(entity->level, DMI_VERSION(2, 0, 0));
    assert_int_equal(info->l1_cache_handle, DMI_HANDLE_INVALID);
    assert_int_equal(info->l2_cache_handle, DMI_HANDLE_INVALID);
    assert_int_equal(info->l3_cache_handle, DMI_HANDLE_INVALID);

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}

static void test_processor_decode_incomplete(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // SMBIOS 2.0 structure followed by the first two cache handles only
    uint8_t data[sizeof(test_processor_data) + 4];
    size_t length = test_processor_data[1];

    memcpy(data, test_processor_data, length);
    memcpy(data + length, (const uint8_t[]){ 0x10, 0x00, 0x11, 0x00 }, 4);
    memcpy(data + length + 4, test_processor_data + length, sizeof(test_processor_data) - length);
    data[1] = (uint8_t)(length + 4);

    dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    assert_non_null(info);
    assert_string_equal(info->version, "Model");

    // Completely present cache handles are decoded
    assert_int_equal(entity->level, DMI_VERSION(2, 1, 0));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);
    assert_int_equal(info->l1_cache_handle, 0x0010);
    assert_int_equal(info->l2_cache_handle, 0x0011);
    assert_int_equal(info->l3_cache_handle, DMI_HANDLE_INVALID);

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}
