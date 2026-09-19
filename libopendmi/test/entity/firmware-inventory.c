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

#include <opendmi/entity/firmware-inventory.h>

static void test_firmware_ident_format_name(void **pstate);
static void test_firmware_inventory_decode_ident_format(void **pstate);
static void test_firmware_inventory_parse_version(void **pstate);
static void test_firmware_inventory_parse_ident(void **pstate);
static void test_firmware_inventory_variants(void **pstate);

static dmi_entity_t *test_create(
        dmi_context_t *context,
        uint8_t        version_format,
        const char    *version,
        uint8_t        ident_format,
        const char    *ident);
static const dmi_attribute_t *test_attribute(const dmi_entity_t *entity, const char *code);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_firmware_ident_format_name),
        cmocka_unit_test(test_firmware_inventory_decode_ident_format),
        cmocka_unit_test(test_firmware_inventory_parse_version),
        cmocka_unit_test(test_firmware_inventory_parse_ident),
        cmocka_unit_test(test_firmware_inventory_variants)
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

static void test_firmware_inventory_parse_version(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t     format;
        const char *version;
        uint8_t     parsed;
        uint32_t    major;
        uint32_t    minor;
        uint64_t    value;
    } test_data[] = {
        // Semantic version numbers are not limited to bytes
        { DMI_VERSION_FORMAT_SEMANTIC, "1.45",            DMI_VERSION_FORMAT_SEMANTIC, 1, 45, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "60020.6",         DMI_VERSION_FORMAT_SEMANTIC, 60020, 6, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "4294967295.0",    DMI_VERSION_FORMAT_SEMANTIC, UINT32_MAX, 0, 0 },

        // Strings not conforming to the format are free-form ones
        { DMI_VERSION_FORMAT_SEMANTIC, "ALDER107",        DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "A0006.4",         DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "1.45.3",          DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "1.",              DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_SEMANTIC, "4294967296.0",    DMI_VERSION_FORMAT_FREE, 0, 0, 0 },

        { DMI_VERSION_FORMAT_HEX_32,   "0x0001002d",      DMI_VERSION_FORMAT_HEX_32, 0, 0, 0x0001002D },
        { DMI_VERSION_FORMAT_HEX_32,   "0X2D",            DMI_VERSION_FORMAT_HEX_32, 0, 0, 0x2D },
        { DMI_VERSION_FORMAT_HEX_32,   "0x000000010000002d", DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_HEX_32,   "0001002d",        DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_HEX_32,   "0x",              DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { DMI_VERSION_FORMAT_HEX_64,   "0x000000010000002d", DMI_VERSION_FORMAT_HEX_64, 0, 0, 0x000000010000002D },
        { DMI_VERSION_FORMAT_HEX_64,   "0x0g",            DMI_VERSION_FORMAT_FREE, 0, 0, 0 },

        // Free form and unknown formats are not parsed
        { DMI_VERSION_FORMAT_FREE,     "1.45",            DMI_VERSION_FORMAT_FREE, 0, 0, 0 },
        { 0x80,                        "1.45",            DMI_VERSION_FORMAT_FREE, 0, 0, 0 }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_entity_t *entity = test_create(context, test_data[i].format, test_data[i].version, 0x00, nullptr);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_firmware_inventory_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
        assert_non_null(info);

        // Lowest version is parsed in the same format
        const dmi_firmware_version_t versions[] = { info->parsed_version, info->parsed_lowest_version };

        for (size_t j = 0; j < countof(versions); j++) {
            if (versions[j].format != test_data[i].parsed)
                fail_msg("Version %s: format %d", test_data[i].version, versions[j].format);

            if (test_data[i].parsed == DMI_VERSION_FORMAT_SEMANTIC) {
                assert_int_equal(versions[j].number.major, test_data[i].major);
                assert_int_equal(versions[j].number.minor, test_data[i].minor);
            } else if (test_data[i].parsed != DMI_VERSION_FORMAT_FREE) {
                assert_int_equal(versions[j].value, test_data[i].value);
            }
        }

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static void test_firmware_inventory_parse_ident(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const dmi_byte_t guid[16] = {
        0x16, 0x24, 0xA9, 0xDF, 0x5E, 0x13, 0x47, 0xFC,
        0x87, 0x4A, 0xDF, 0x3A, 0xFF, 0x14, 0x30, 0x89
    };

    static const struct {
        uint8_t     format;
        const char *ident;
        uint8_t     parsed;
    } test_data[] = {
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df-5e13-47fc-874a-df3aff143089",  DMI_FIRMWARE_IDENT_FORMAT_GUID },
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624A9DF-5E13-47FC-874A-DF3AFF143089",  DMI_FIRMWARE_IDENT_FORMAT_GUID },
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df5e1347fc874adf3aff143089",      DMI_FIRMWARE_IDENT_FORMAT_FREE },
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df-5e13-47fc-874a-df3aff14308",   DMI_FIRMWARE_IDENT_FORMAT_FREE },
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df-5e13-47fc-874a-df3aff1430899", DMI_FIRMWARE_IDENT_FORMAT_FREE },
        { DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df-5e13-47fc-874a-df3aff14308z",  DMI_FIRMWARE_IDENT_FORMAT_FREE },
        { DMI_FIRMWARE_IDENT_FORMAT_FREE, "1624a9df-5e13-47fc-874a-df3aff143089",  DMI_FIRMWARE_IDENT_FORMAT_FREE }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_entity_t *entity = test_create(context, DMI_VERSION_FORMAT_FREE, nullptr,
                                           test_data[i].format, test_data[i].ident);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_firmware_inventory_t *info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE_INVENTORY));
        assert_non_null(info);

        if (info->parsed_ident.format != test_data[i].parsed)
            fail_msg("Identifier %s: format %d", test_data[i].ident, info->parsed_ident.format);

        if (test_data[i].parsed == DMI_FIRMWARE_IDENT_FORMAT_GUID)
            assert_memory_equal(info->parsed_ident.guid.__value, guid, sizeof(guid));

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static void test_firmware_inventory_variants(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        uint8_t               format;
        const char           *version;
        dmi_attribute_type_t  type;
        const char           *expected;
    } test_data[] = {
        { DMI_VERSION_FORMAT_SEMANTIC, "1.45",       DMI_ATTRIBUTE_TYPE_STRUCT,  nullptr      },
        { DMI_VERSION_FORMAT_HEX_32,   "0x0001002d", DMI_ATTRIBUTE_TYPE_INTEGER, "0x1002D"    },
        { DMI_VERSION_FORMAT_SEMANTIC, "ALDER107",   DMI_ATTRIBUTE_TYPE_STRING,  "ALDER107"   }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_entity_t *entity = test_create(context, test_data[i].format, test_data[i].version,
                                           DMI_FIRMWARE_IDENT_FORMAT_GUID, "1624a9df-5e13-47fc-874a-df3aff143089");
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        // Variant is selected by the format the string conforms to
        const dmi_attribute_t *attr = test_attribute(entity, "version");
        assert_non_null(attr);

        const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
        assert_non_null(variant);
        assert_int_equal(variant->type, test_data[i].type);

        if (test_data[i].expected != nullptr) {
            const void *value = dmi_member_ptr(entity->info, variant->value, dmi_data_t);
            char *text = dmi_attribute_format(context, variant, value, true);
            bool valid = (text != nullptr) and (strcmp(text, test_data[i].expected) == 0);

            free(text);
            assert_true(valid);
        }

        // Identifier is shown as UUID
        attr = test_attribute(entity, "ident");
        assert_non_null(attr);

        variant = dmi_attribute_resolve(attr, entity->info);
        assert_non_null(variant);
        assert_int_equal(variant->type, DMI_ATTRIBUTE_TYPE_UUID);

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static dmi_entity_t *test_create(
        dmi_context_t *context,
        uint8_t        version_format,
        const char    *version,
        uint8_t        ident_format,
        const char    *ident)
{
    static uint8_t data[256];

    // SMBIOS 3.5 firmware inventory information structure, version is also
    // used as the lowest version
    const uint8_t header[] = {
        45, 0x18, 0x00, 0x10,                       // Header
        0x01, 0x02, version_format,                 // Name, version, version format
        0x03, ident_format,                         // Identifier, identifier format
        0x00, 0x00, 0x02,                           // Release date, vendor, lowest version
        0xFF, 0xFF, 0xFF, 0xFF,                     // Image size
        0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00,                                 // Characteristics
        0x04,                                       // State
        0x00                                        // Number of associated components
    };

    memset(data, 0, sizeof(data));
    memcpy(data, header, sizeof(header));

    size_t length = sizeof(header);
    const char *strings[] = { "BIOS", (version != nullptr) ? version : "-", (ident != nullptr) ? ident : "-" };

    for (size_t i = 0; i < countof(strings); i++) {
        size_t size = strlen(strings[i]) + 1;

        memcpy(data + length, strings[i], size);
        length += size;
    }

    data[length++] = 0;

    return dmi_entity_create(context, data, length);
}

static const dmi_attribute_t *test_attribute(const dmi_entity_t *entity, const char *code)
{
    for (const dmi_attribute_t *attr = entity->spec->attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return attr;
    }

    return nullptr;
}

