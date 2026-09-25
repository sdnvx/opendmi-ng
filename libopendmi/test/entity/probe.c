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
#include <opendmi/attribute.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/probe.h>
#include <opendmi/entity/voltage-probe.h>
#include <opendmi/entity/current-probe.h>
#include <opendmi/entity/temperature-probe.h>

typedef struct test_probe_value {
    const char *code;
    const char *value;
    dmi_unit_t  unit;
} test_probe_value_t;

static void test_probe_location_name(void **pstate);
static void test_probe_voltage_values(void **pstate);
static void test_probe_current_values(void **pstate);
static void test_probe_temperature_values(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_probe_location_name),
        cmocka_unit_test(test_probe_voltage_values),
        cmocka_unit_test(test_probe_current_values),
        cmocka_unit_test(test_probe_temperature_values)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_probe_location_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_probe_location_name(__DMI_PROBE_LOCATION_COUNT));

    for (int i = 0; i < __DMI_PROBE_LOCATION_COUNT; i++) {
        assert_non_null(dmi_probe_location_name(i));
    }
}

static void assert_probe_values(dmi_type_t type, const test_probe_value_t *expected)
{
    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    // Probe structure with the same raw values for all probe types
    static const uint8_t data[] = {
        0, 0x16, 0x00, 0x10,        // Header (type is set below)
        0x00, 0x63,                 // Description, location and status (OK, processor)
        0xE0, 0x2E,                 // Maximum value (12000)
        0x98, 0xFE,                 // Minimum value (-360)
        0x05, 0x00,                 // Resolution (5)
        0x80, 0x00,                 // Tolerance (128)
        0x32, 0x00,                 // Accuracy (50)
        0x00, 0x00, 0x00, 0x00,     // OEM-defined
        0x00, 0x80,                 // Nominal value (unknown)
        0x00, 0x00                  // String set terminator
    };

    uint8_t buffer[sizeof(data)];
    memcpy(buffer, data, sizeof(data));
    buffer[0] = (uint8_t)type;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, buffer, sizeof(buffer));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_non_null(entity->spec);

    const dmi_probe_t *info = dmi_entity_info(entity, type);
    assert_non_null(info);
    assert_int_equal(info->nominal_value, SHRT_MIN);

    for (const test_probe_value_t *item = expected; item->code != nullptr; item++) {
        const dmi_attribute_t *attr;

        for (attr = entity->spec->attributes; attr->type != DMI_ATTRIBUTE_TYPE_NONE; attr++) {
            if (strcmp(attr->params.code, item->code) == 0)
                break;
        }

        assert_int_not_equal(attr->type, DMI_ATTRIBUTE_TYPE_NONE);

        char *value = dmi_attribute_format(context, attr, (const char *)info + attr->value.offset, false);
        assert_non_null(value);
        assert_string_equal(value, item->value);
        free(value);

        assert_int_not_equal(attr->params.unit, DMI_UNIT_NONE);
        assert_int_equal(attr->params.unit, item->unit);
    }

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
    dmi_destroy(context);
}

static void test_probe_voltage_values(void **pstate)
{
    dmi_unused(pstate);

    assert_probe_values(DMI_TYPE(VOLTAGE_PROBE), (const test_probe_value_t[]){
        { "maximum-value", "12000", DMI_UNIT_MILLIVOLT },
        { "minimum-value", "-360",  DMI_UNIT_MILLIVOLT },
        { "resolution",    "0.5",   DMI_UNIT_MILLIVOLT },
        { "tolerance",     "128",   DMI_UNIT_MILLIVOLT },
        { "accuracy",      "0.5",   DMI_UNIT_PERCENT   },
        {}
    });
}

static void test_probe_current_values(void **pstate)
{
    dmi_unused(pstate);

    assert_probe_values(DMI_TYPE(CURRENT_PROBE), (const test_probe_value_t[]){
        { "maximum-value", "12000", DMI_UNIT_MILLIAMPERE },
        { "minimum-value", "-360",  DMI_UNIT_MILLIAMPERE },
        { "resolution",    "0.5",   DMI_UNIT_MILLIAMPERE },
        { "tolerance",     "128",   DMI_UNIT_MILLIAMPERE },
        { "accuracy",      "0.5",   DMI_UNIT_PERCENT     },
        {}
    });
}

static void test_probe_temperature_values(void **pstate)
{
    dmi_unused(pstate);

    assert_probe_values(DMI_TYPE(TEMPERATURE_PROBE), (const test_probe_value_t[]){
        { "maximum-value", "1200.0", DMI_UNIT_CELSIUS },
        { "minimum-value", "-36.0",  DMI_UNIT_CELSIUS },
        { "resolution",    "0.005",  DMI_UNIT_CELSIUS },
        { "tolerance",     "12.8",   DMI_UNIT_CELSIUS },
        { "accuracy",      "0.5",    DMI_UNIT_PERCENT },
        {}
    });
}
