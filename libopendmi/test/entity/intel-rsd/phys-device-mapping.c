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
#include <opendmi/module.h>
#include <opendmi/module/intel-rsd.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel-rsd/phys-device-mapping.h>

static int test_rsd_phys_device_mapping_setup(void **pstate);
static int test_rsd_phys_device_mapping_teardown(void **pstate);

static void test_rsd_phys_device_mapping_decode(void **pstate);
static void test_rsd_phys_device_mapping_decode_unknown(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_phys_device_mapping_decode, test_rsd_phys_device_mapping_setup, test_rsd_phys_device_mapping_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_phys_device_mapping_decode_unknown, test_rsd_phys_device_mapping_setup, test_rsd_phys_device_mapping_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_phys_device_mapping_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel-rsd"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_phys_device_mapping_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_phys_device_mapping_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Memory devices, with a trailing byte
    static const uint8_t data[] = {
        200, 0x0F, 0x00, 0xC8,          // Header
        0x03, 0x00,                     // Device type and reserved byte
        0x20, 0x00, 0x00, 0x01,         // Handle, CPU socket and DIMM slot
        0x21, 0x00, 0x01, 0x02,         // Handle, CPU socket and DIMM slot
        0xFF,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));

    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(dmi_entity_is_incomplete(entity));

    const dmi_intel_rsd_phys_device_mapping_t *info = dmi_entity_info(entity, DMI_TYPE(intel_rsd_phys_device_mapping));
    assert_non_null(info);

    assert_int_equal(info->device_type, DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY);
    assert_int_equal(info->device_count, 2);

    assert_int_equal(info->devices[0].handle, 0x0020);
    assert_int_equal(info->devices[0].type, DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY);
    assert_int_equal(info->devices[0].primary_number, 0);
    assert_int_equal(info->devices[0].secondary_number, 1);
    assert_false(info->devices[0].is_raw);

    assert_int_equal(info->devices[1].handle, 0x0021);
    assert_int_equal(info->devices[1].primary_number, 1);
    assert_int_equal(info->devices[1].secondary_number, 2);

    // Handles refer to the structures of the type the device type names, and
    // to any structure for the device types the specification leaves out
    const struct {
        dmi_intel_rsd_phys_device_type_t  device_type;
        const dmi_type_t                 *target;
    } test_targets[] = {
        { DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PROCESSOR, DMI_TYPE(processor)     },
        { DMI_INTEL_RSD_PHYS_DEVICE_TYPE_PCIE_SLOT, DMI_TYPE(system_slots)  },
        { DMI_INTEL_RSD_PHYS_DEVICE_TYPE_MEMORY,    DMI_TYPE(memory_device) },
        { DMI_INTEL_RSD_PHYS_DEVICE_TYPE_INVALID,   DMI_TYPE_ANY            }
    };

    const dmi_attribute_t *devices = entity->spec->attributes;
    while ((devices->params.code != nullptr) and (strcmp(devices->params.code, "devices") != 0))
        devices++;

    assert_non_null(devices->params.code);

    const dmi_attribute_t *handle = devices->params.attrs;
    assert_string_equal(handle->params.code, "handle");

    for (size_t i = 0; i < countof(test_targets); i++) {
        dmi_intel_rsd_phys_device_t device = { .type = test_targets[i].device_type };

        const dmi_attribute_t *resolved = dmi_attribute_resolve(handle, &device);
        assert_non_null(resolved);
        assert_int_equal(resolved->type, DMI_ATTRIBUTE_TYPE_HANDLE);
        assert_string_equal(resolved->params.code, "handle");

        if (test_targets[i].target == DMI_TYPE_ANY) {
            assert_null(resolved->params.targets);
        } else {
            assert_non_null(resolved->params.targets);
            assert_ptr_equal(resolved->params.targets[0], test_targets[i].target);
            assert_null(resolved->params.targets[1]);
        }
    }

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_rsd_phys_device_mapping_decode_unknown(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Location data of unknown device types is kept as stored
    static const uint8_t data[] = {
        200, 0x0A, 0x00, 0xC8,
        0x07, 0x00,
        0x30, 0x00, 0xAA, 0xBB,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));

    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(dmi_entity_is_incomplete(entity));

    const dmi_intel_rsd_phys_device_mapping_t *info = dmi_entity_info(entity, DMI_TYPE(intel_rsd_phys_device_mapping));
    assert_non_null(info);

    assert_int_equal(info->device_count, 1);
    assert_true(info->devices[0].is_raw);
    assert_int_equal(info->devices[0].data.length, 2);
    assert_memory_equal(info->devices[0].data.data, data + 8, 2);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}
