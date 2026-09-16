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
#include <opendmi/test/logger.h>

#include <opendmi/entity/firmware-inventory.h>

static void test_firmware_ident_format_name(void **pstate);
static void test_firmware_inventory_decode_ident_format(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_firmware_ident_format_name),
        cmocka_unit_test(test_firmware_inventory_decode_ident_format)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_firmware_ident_format_name(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_firmware_ident_format_name(0x00), "Free form");
    assert_string_equal(dmi_firmware_ident_format_name(0x01), "GUID");
    assert_string_equal(dmi_firmware_ident_format_name(0x02), "Reserved");
    assert_string_equal(dmi_firmware_ident_format_name(0x7F), "Reserved");
    assert_string_equal(dmi_firmware_ident_format_name(0x80), "Vendor/OEM-specific");
    assert_string_equal(dmi_firmware_ident_format_name(0xFF), "Vendor/OEM-specific");
    assert_null(dmi_firmware_ident_format_name(0x100));
}

static void test_firmware_inventory_decode_ident_format(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t ident_format;
        const char *ident;
    } test_data[] = {
        { 0x00, "35EQP72B" },
        { 0x01, "1624a9df-5e13-47fc-874a-df3aff143089" }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        // SMBIOS 3.5 firmware inventory information structure
        uint8_t data[128] = {
            45, 0x18, 0x00, 0x10,               // Header
            0x01, 0x00, 0x00,                   // Name, version, version format
            0x02, test_data[i].ident_format,    // Identifier, identifier format
            0x00, 0x00, 0x00,                   // Release date, vendor, lowest version
            0xFF, 0xFF, 0xFF, 0xFF,             // Image size
            0xFF, 0xFF, 0xFF, 0xFF,
            0x00, 0x00,                         // Characteristics
            0x04,                               // State
            0x00                                // Number of associated components
        };

        size_t length = 0x18;
        const char *strings[] = { "BIOS", test_data[i].ident };

        for (size_t j = 0; j < countof(strings); j++) {
            size_t size = strlen(strings[j]) + 1;

            memcpy(data + length, strings[j], size);
            length += size;
        }

        data[length++] = 0;

        dmi_entity_t *entity = dmi_entity_create(context, data, length);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_firmware_inventory_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
        assert_non_null(info);

        assert_string_equal(info->name, "BIOS");
        assert_string_equal(info->ident, test_data[i].ident);
        assert_int_equal(info->ident_format, test_data[i].ident_format);
        assert_int_equal(info->state, DMI_FIRMWARE_INVENTORY_STATE_ENABLED);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}
