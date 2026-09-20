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
#include <opendmi/test/logger.h>

#include <opendmi/entity/processor.h>

static void test_processor_status_name(void **pstate);
static void test_processor_decode_status(void **pstate);
static void test_processor_decode_voltage(void **pstate);
static void test_processor_decode_version(void **pstate);
static void test_processor_decode_incomplete(void **pstate);
static void test_processor_decode_id(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

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

// Offsets of the voltage and status fields
static const size_t test_voltage_offset = 0x11;
static const size_t test_status_offset  = 0x18;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_processor_status_name),
        cmocka_unit_test(test_processor_decode_status),
        cmocka_unit_test(test_processor_decode_voltage),
        cmocka_unit_test(test_processor_decode_version),
        cmocka_unit_test(test_processor_decode_incomplete),
        cmocka_unit_test(test_processor_decode_id)
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

static void test_processor_decode_voltage(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        uint8_t value;
        uint8_t voltage;
        uint8_t supported_voltages;
    } cases[] = {
        // Current voltage
        { 0x8C, 12, 0x00 },
        { 0xFF, 127, 0x00 },
        { 0x80, 0, 0x00 },
        // Supported voltages in legacy mode
        { 0x00, 0, 0x00 },
        { 0x01, 0, 0x01 },
        { 0x06, 0, 0x06 },
        // Reserved bits are ignored
        { 0x7A, 0, 0x02 }
    };

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    for (size_t i = 0; i < countof(cases); i++) {
        uint8_t data[sizeof(test_processor_data)];

        memcpy(data, test_processor_data, sizeof(data));
        data[test_voltage_offset] = cases[i].value;

        dmi_entity_t *entity = dmi_entity_create(context, data, sizeof(data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
        assert_non_null(info);
        assert_int_equal(info->voltage, cases[i].voltage);
        assert_int_equal(info->supported_voltages.__value, cases[i].supported_voltages);

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

static void test_processor_decode_id(void **pstate)
{
    dmi_unused(pstate);

    // Signature of Alder Lake (family 6, model 151, stepping 5) and Zen 5
    // (family 26, model 68, stepping 0) processors, and feature flags
    static const uint8_t intel_id[] = { 0x75, 0x06, 0x09, 0x00, 0xFF, 0xFB, 0xEB, 0xBF };
    static const uint8_t amd_id[]   = { 0x40, 0x0F, 0xB4, 0x00, 0xFF, 0xFB, 0x8B, 0x17 };

    // Bobcat processor ID with swapped words, as reported by some firmware
    static const uint8_t swapped_id[] = { 0xFF, 0xFB, 0x8B, 0x17, 0x20, 0x0F, 0x50, 0x00 };

    // Arm MIDR of Cortex-A72 r0p3, and SoC ID with JEP106 bank 4 and code
    // 0x3B, and revision 2
    static const uint8_t midr_id[]   = { 0x83, 0xD0, 0x0F, 0x41, 0x00, 0x00, 0x00, 0x00 };
    static const uint8_t soc_id[]    = { 0x34, 0x12, 0x3B, 0x04, 0x02, 0x00, 0x00, 0x00 };

    static const struct {
        uint16_t                   family;
        uint16_t                   features;
        const char                *vendor;
        const char                *version;
        const uint8_t             *id;
        dmi_processor_id_format_t  format;
        unsigned int               x86_family;
        unsigned int               x86_model;
        unsigned int               x86_stepping;
    } cases[] = {
        { DMI_PROCESSOR_FAMILY_INTEL_CORE_I5, 0, "Intel", "Core", intel_id,  DMI_PROCESSOR_ID_FORMAT_X86, 6,  151, 5 },
        { DMI_PROCESSOR_FAMILY_AMD_ZEN,       0, "AMD",   "Ryzen", amd_id,   DMI_PROCESSOR_ID_FORMAT_X86, 26, 68,  0 },
        { DMI_PROCESSOR_FAMILY_AMD_C,         0, "AMD",   "C-60", swapped_id, DMI_PROCESSOR_ID_FORMAT_X86, 20, 2, 0 },

        // Other and unknown families are recognized by vendor
        { DMI_PROCESSOR_FAMILY_OTHER,   0, "GenuineIntel", "CPU", intel_id, DMI_PROCESSOR_ID_FORMAT_X86, 6, 151, 5 },
        { DMI_PROCESSOR_FAMILY_UNKNOWN, 0, "Vendor", "AMD Athlon", amd_id,  DMI_PROCESSOR_ID_FORMAT_X86, 26, 68, 0 },
        { DMI_PROCESSOR_FAMILY_OTHER,   0, "Vendor", "Model", intel_id,     DMI_PROCESSOR_ID_FORMAT_RAW, 0, 0, 0 },
        { DMI_PROCESSOR_FAMILY_OTHER,   0, "Intelligent", "CPU", intel_id,  DMI_PROCESSOR_ID_FORMAT_RAW, 0, 0, 0 },

        // Apple M1 and M2 families were Cyrix M1 and M2 ones before
        { DMI_PROCESSOR_FAMILY_APPLE_M2, 0, "AMD", "Athlon", amd_id,     DMI_PROCESSOR_ID_FORMAT_X86,  26, 68, 0 },
        { DMI_PROCESSOR_FAMILY_APPLE_M1, 0, "Apple", "M1", midr_id,      DMI_PROCESSOR_ID_FORMAT_MIDR, 0, 0, 0 },

        // Arm64 processors report SoC ID, if it is supported
        { DMI_PROCESSOR_FAMILY_ARM_V8, 0x0000, "ARM", "A72", midr_id, DMI_PROCESSOR_ID_FORMAT_MIDR,   0, 0, 0 },
        { DMI_PROCESSOR_FAMILY_ARM_V8, 0x0200, "ARM", "A72", soc_id,  DMI_PROCESSOR_ID_FORMAT_SOC_ID, 0, 0, 0 },
        { DMI_PROCESSOR_FAMILY_ARM_V7, 0x0200, "ARM", "A15", midr_id, DMI_PROCESSOR_ID_FORMAT_MIDR,   0, 0, 0 }
    };

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    for (size_t i = 0; i < countof(cases); i++) {
        // SMBIOS 2.6 processor structure with characteristics and extended
        // family
        uint8_t data[0x2A + 64] = {
            4, 0x2A, 0x00, 0x10,
            0x00, 0x03, 0xFE, 0x01,                     // Socket, type, family, vendor
        };

        memcpy(data + 0x08, cases[i].id, 8);
        data[0x10] = 0x02;                              // Version
        memset(data + 0x1A, 0xFF, 6);                   // Cache handles
        data[0x26] = cases[i].features & 0xFFu;
        data[0x27] = cases[i].features >> 8;
        data[0x28] = cases[i].family & 0xFFu;
        data[0x29] = cases[i].family >> 8;

        size_t length = 0x2A;
        const char *strings[] = { cases[i].vendor, cases[i].version };

        for (size_t j = 0; j < countof(strings); j++) {
            size_t size = strlen(strings[j]) + 1;

            memcpy(data + length, strings[j], size);
            length += size;
        }

        data[length++] = 0;

        dmi_entity_t *entity = dmi_entity_create(context, data, length);
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
        assert_non_null(info);

        // Raw processor ID is kept as stored
        assert_int_equal(info->id.length, 8);
        assert_memory_equal(info->id.data, cases[i].id, 8);

        if (info->id_format != cases[i].format)
            fail_msg("Case %zu: format %d instead of %d", i, info->id_format, cases[i].format);

        if (cases[i].format == DMI_PROCESSOR_ID_FORMAT_X86) {
            assert_int_equal(info->x86_id.family, cases[i].x86_family);
            assert_int_equal(info->x86_id.model, cases[i].x86_model);
            assert_int_equal(info->x86_id.stepping, cases[i].x86_stepping);
            assert_true(info->x86_id.features & 0x01u);
        } else if (cases[i].format == DMI_PROCESSOR_ID_FORMAT_MIDR) {
            assert_int_equal(info->arm_id.implementer, 0x41);
            assert_int_equal(info->arm_id.variant, 0x0);
            assert_int_equal(info->arm_id.architecture, 0xF);
            assert_int_equal(info->arm_id.part_number, 0xD08);
            assert_int_equal(info->arm_id.revision, 0x3);
        } else if (cases[i].format == DMI_PROCESSOR_ID_FORMAT_SOC_ID) {
            assert_int_equal(info->soc_id.jep106_bank, 0x04);
            assert_int_equal(info->soc_id.jep106_id, 0x3B);
            assert_int_equal(info->soc_id.soc_id, 0x1234);
            assert_int_equal(info->soc_id.soc_revision, 2);
        }

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

