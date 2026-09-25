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
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/system-boot.h>

static void test_system_boot_status(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_system_boot_status)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_system_boot_status(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t length;
        uint8_t status;
        size_t  data_length;
        bool    has_data;
    } test_cases[] = {
        { 0x14, DMI_BOOT_STATUS_NO_ERRORS_DETECTED, 9, false },
        { 0x14, 0x80,                                9, true  },
        { 0x14, 0xC5,                                9, true  },
        { 0x0B, DMI_BOOT_STATUS_OS_FAILED_TO_LOAD,   0, false }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        // System boot information structure with status data, which is
        // longer than the minimum
        uint8_t data[0x14 + 2] = {
            32, test_cases[i].length, 0x00, 0x20,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // Reserved
            test_cases[i].status,
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
        };

        data[test_cases[i].length]     = 0;
        data[test_cases[i].length + 1] = 0;

        dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

        dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, test_cases[i].length + 2);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_system_boot_t *info = dmi_entity_info(entity, DMI_TYPE(SYSTEM_BOOT));
        assert_non_null(info);
        assert_int_equal(info->status, test_cases[i].status);
        assert_int_equal(info->status_data.length, test_cases[i].data_length);
        assert_int_equal(info->has_status_data, test_cases[i].has_data);

        if (test_cases[i].data_length > 0) {
            assert_ptr_equal(info->status_data.data,
                             dmi_buffer_at(entity->buffer, entity->offset, entity->total_length) + 0x0B);
            assert_int_equal(info->status_data.data[8], 0x09);
        }

        // Status data is shown only for vendor and product status codes
        const dmi_attribute_t *attr = entity->spec->attributes;
        while ((attr->params.code != nullptr) and (strcmp(attr->params.code, "status-data") != 0))
            attr++;

        assert_non_null(attr->params.code);
        assert_int_equal(dmi_attribute_resolve(attr, entity->info) != nullptr, test_cases[i].has_data);

        dmi_entity_destroy(entity);

        dmi_buffer_destroy(entity_buffer);
    }

    dmi_destroy(context);
}
