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

#include <opendmi/entity/intel-rsd/memory-device.h>

static int test_rsd_memory_device_setup(void **pstate);
static int test_rsd_memory_device_teardown(void **pstate);

static void test_rsd_memory_device_decode(void **pstate);
static void test_rsd_memory_device_decode_short(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD memory device extended information structure (0x0F bytes)
// followed by strings
static const uint8_t test_data[] = {
    197, 0x0F, 0x00, 0x34,          // Header
    0x11, 0x00,                     // Memory device handle
    0x01,                           // Memory type (NVDIMM-N)
    0x03,                           // Memory media (proprietary)
    0x01, 0x02,                     // Firmware revision, firmware API version
    0x98, 0x3A, 0x00, 0x00,         // Maximum TDP (15000 mW)
    0xA0,                           // SMBus address
    '0', '1', '.', '0', '2', 0,
    '1', '.', '0', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_memory_device_decode,
                                        test_rsd_memory_device_setup, test_rsd_memory_device_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_memory_device_decode_short,
                                        test_rsd_memory_device_setup, test_rsd_memory_device_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_memory_device_setup(void **pstate)
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

static int test_rsd_memory_device_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_memory_device_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x0F);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_memory_device_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_MEMORY_DEVICE));
    assert_non_null(info);

    assert_int_equal(info->device_handle, 0x0011);
    assert_int_equal(info->memory_type, DMI_INTEL_RSD_MEMORY_TYPE_NVDIMM_N);
    assert_int_equal(info->memory_media, DMI_INTEL_RSD_MEMORY_MEDIA_PROPRIETARY);
    assert_string_equal(info->firmware_revision, "01.02");
    assert_string_equal(info->firmware_api_version, "1.0");
    assert_int_equal(info->maximum_tdp, 15000);
    assert_int_equal(info->smbus_address, 0xA0);

    // Handle refers to a memory device structure only
    const dmi_attribute_t *attr = entity->spec->attributes;
    while ((attr->params.code != nullptr) and (strcmp(attr->params.code, "device-handle") != 0))
        attr++;

    assert_non_null(attr->params.targets);
    assert_int_equal(attr->params.targets[0], DMI_TYPE_MEMORY_DEVICE);
    assert_int_equal(attr->params.targets[1], DMI_TYPE_INVALID);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Structure one byte shorter than its minimum length is rejected.
//
static void test_rsd_memory_device_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[0x0E + 2] = {};
    memcpy(data, test_data, 0x0E);
    data[1] = 0x0E;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}
