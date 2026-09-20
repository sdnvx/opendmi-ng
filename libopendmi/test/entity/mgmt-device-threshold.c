//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/mgmt-device-threshold.h>

static void test_mgmt_device_threshold_units(void **pstate);
static void test_mgmt_device_threshold_ambiguous(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

// Management device threshold data structure (0x10 bytes) without strings
static const uint8_t test_data[] = {
    36, 0x10, 0x25, 0x00,               // Header
    0x52, 0x03, 0x84, 0x03,             // Lower and upper non-critical
    0x20, 0x03, 0xB6, 0x03,             // Lower and upper critical
    0x00, 0x80, 0x00, 0x80,             // Lower and upper non-recoverable
    0x00, 0x00
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_mgmt_device_threshold_units),
        cmocka_unit_test(test_mgmt_device_threshold_ambiguous)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static const char *test_format(dmi_entity_t *entity)
{
    static char buffer[64];

    const dmi_attribute_t *attr = entity->spec->attributes;
    const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
    assert_non_null(variant);

    const void *value = dmi_member_ptr(entity->info, variant->value, dmi_data_t);
    char *text = dmi_attribute_format(dmi_entity_context(entity), variant, value, true);
    assert_non_null(text);

    snprintf(buffer, sizeof(buffer), "%s%s%s", text,
             (variant->params.unit != nullptr) ? " " : "",
             (variant->params.unit != nullptr) ? variant->params.unit : "");
    free(text);

    return buffer;
}

static void test_mgmt_device_threshold_units(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    static const struct {
        dmi_type_t  type;
        const char *expected;
    } test_cases[] = {
        { DMI_TYPE(VOLTAGE_PROBE),     "850 mV"   },
        { DMI_TYPE(TEMPERATURE_PROBE), "85.0 °C"  },
        { DMI_TYPE(CURRENT_PROBE),     "850 mA"   },
        { DMI_TYPE(COOLING_DEVICE),    "850 rpm"  }
    };

    // Values are shown as stored, until the component is known
    dmi_entity_t *entity = dmi_entity_create(context, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_string_equal(test_format(entity), "850");
    dmi_entity_destroy(entity);

    for (size_t i = 0; i < countof(test_cases); i++) {
        entity = dmi_entity_create(context, test_data, sizeof(test_data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        dmi_mgmt_device_threshold_set_component(entity, test_cases[i].type);
        assert_string_equal(test_format(entity), test_cases[i].expected);

        // Unknown values are recognized in all units
        const dmi_attribute_t *attr = &entity->spec->attributes[4];
        const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
        assert_true(dmi_attribute_is_unknown(variant, dmi_member_ptr(entity->info, variant->value, dmi_data_t)));

        dmi_entity_destroy(entity);
    }

    dmi_destroy(context);
}

static void test_mgmt_device_threshold_ambiguous(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = dmi_create(0);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    dmi_entity_t *entity = dmi_entity_create(context, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_mgmt_device_threshold_t *info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));
    assert_non_null(info);

    // Thresholds shared by components of the same type
    dmi_mgmt_device_threshold_set_component(entity, DMI_TYPE(VOLTAGE_PROBE));
    dmi_mgmt_device_threshold_set_component(entity, DMI_TYPE(VOLTAGE_PROBE));
    assert_int_equal(info->component_type, DMI_TYPE(VOLTAGE_PROBE));
    assert_false(info->is_ambiguous);

    // Thresholds shared by components of different types have no units
    dmi_mgmt_device_threshold_set_component(entity, DMI_TYPE(TEMPERATURE_PROBE));
    dmi_mgmt_device_threshold_set_component(entity, DMI_TYPE(VOLTAGE_PROBE));
    assert_int_equal(info->component_type, DMI_TYPE_INVALID);
    assert_true(info->is_ambiguous);
    assert_string_equal(test_format(entity), "850");

    dmi_entity_destroy(entity);
    dmi_destroy(context);
}
