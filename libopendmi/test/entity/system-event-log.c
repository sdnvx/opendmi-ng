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
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/system-event-log.h>

static void test_system_event_log_access_address(void **pstate);
static void test_system_event_log_descriptors(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_system_event_log_access_address),
        cmocka_unit_test(test_system_event_log_descriptors)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_system_event_log_access_address(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t               method;
        dmi_attribute_type_t  type;
    } test_cases[] = {
        { DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_16BIT, DMI_ATTRIBUTE_TYPE_STRUCT  },
        { DMI_SYSTEM_LOG_ACCESS_METHOD_MMAP_32BIT_ADDR,  DMI_ATTRIBUTE_TYPE_ADDRESS },
        { DMI_SYSTEM_LOG_ACCESS_METHOD_DATA_FUNCTIONS,   DMI_ATTRIBUTE_TYPE_INTEGER },
        { 0x80,                                          DMI_ATTRIBUTE_TYPE_INTEGER }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        // System event log structure (SMBIOS 2.0) without strings
        const uint8_t data[] = {
            15, 0x14, 0x00, 0x40,               // Header
            0x00, 0x10,                         // Log area length
            0x00, 0x00, 0x10, 0x00,             // Header and data start offsets
            test_cases[i].method, 0x01,         // Access method, status
            0x12, 0x34, 0x56, 0x78,             // Change token
            0x6A, 0x04, 0x6C, 0x04,             // Access method address
            0x00, 0x00
        };

        dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_system_event_log_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
        assert_non_null(info);

        // Other SMBIOS 2.0 fields
        assert_int_equal(info->area_length, 0x1000);
        assert_int_equal(info->header_offset, 0x0000);
        assert_int_equal(info->data_offset, 0x0010);
        assert_int_equal(info->change_token, 0x78563412);
        assert_int_equal(info->descriptor_count, 0);

        // Address is decoded in all forms, and shown according to the method
        assert_int_equal(info->access_address, 0x046C046A);
        assert_int_equal(info->access_ports.index_port, 0x046A);
        assert_int_equal(info->access_ports.data_port, 0x046C);
        assert_int_equal(info->access_gpnv_handle, 0x046A);

        const dmi_attribute_t *attr = entity->spec->attributes;
        while ((attr->params.code != nullptr) and (strcmp(attr->params.code, "access-address") != 0))
            attr++;

        assert_non_null(attr->params.code);

        const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
        assert_non_null(variant);
        assert_int_equal(variant->type, test_cases[i].type);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static void test_system_event_log_descriptors(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t length;
        uint8_t count;
        uint8_t descriptor_length;
        size_t  decoded;
        bool    incomplete;
    } test_cases[] = {
        { 0x1B, 2, 2, 2, false },
        // Longer descriptors are allowed for future extensions
        { 0x1D, 2, 3, 2, false },
        // Descriptors of unknown format are not decoded
        { 0x17, 2, 1, 0, false },
        // Only completely present descriptors are decoded
        { 0x1A, 2, 2, 1, true  }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        // System event log structure (SMBIOS 2.1) with type descriptors
        uint8_t data[0x20] = {
            15, test_cases[i].length, 0x00, 0x40,       // Header
            0x00, 0x10, 0x00, 0x00, 0x10, 0x00,         // Area length, header and data offsets
            DMI_SYSTEM_LOG_ACCESS_METHOD_DATA_FUNCTIONS, 0x01,
            0x00, 0x00, 0x00, 0x00,                     // Change token
            0x00, 0x00, 0x00, 0x00,                     // Access method address
            DMI_SYSTEM_LOG_HEADER_FMT_TYPE_1,           // Header format
            test_cases[i].count, test_cases[i].descriptor_length
        };

        // Descriptors of POST error with POST results bitmap and system boot
        // without data
        size_t length = 0x17;
        const uint8_t descriptors[][2] = {
            { DMI_EVENT_LOG_TYPE_POST_ERROR, DMI_EVENT_LOG_DATA_FORMAT_POST_RESULTS },
            { DMI_EVENT_LOG_TYPE_SYSTEM_BOOT, DMI_EVENT_LOG_DATA_FORMAT_NONE }
        };

        for (size_t j = 0; (j < countof(descriptors)) and (length < test_cases[i].length); j++) {
            memcpy(data + length, descriptors[j], 2);
            length += test_cases[i].descriptor_length;
        }

        data[test_cases[i].length]     = 0;
        data[test_cases[i].length + 1] = 0;

        dmi_entity_t *entity = dmi_entity_create(context, data, test_cases[i].length + 2);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));
        assert_int_equal(entity->level, dmi_version(2, 1, 0));
        assert_int_equal((entity->state & DMI_ENTITY_STATE_INCOMPLETE) != 0, test_cases[i].incomplete);

        const dmi_system_event_log_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_EVENT_LOG));
        assert_non_null(info);
        assert_int_equal(info->header_format, DMI_SYSTEM_LOG_HEADER_FMT_TYPE_1);
        assert_int_equal(info->descriptor_count, test_cases[i].decoded);

        for (size_t j = 0; j < info->descriptor_count; j++) {
            assert_int_equal(info->descriptors[j].type, descriptors[j][0]);
            assert_int_equal(info->descriptors[j].data_format, descriptors[j][1]);
        }

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

