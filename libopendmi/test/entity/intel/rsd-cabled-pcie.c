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
#include <opendmi/module/intel.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-cabled-pcie.h>

static int test_rsd_cabled_pcie_setup(void **pstate);
static int test_rsd_cabled_pcie_teardown(void **pstate);

static void test_rsd_cabled_pcie_decode(void **pstate);
static void test_rsd_cabled_pcie_decode_short(void **pstate);
static void test_rsd_cabled_pcie_decode_truncated(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD cabled PCIe port information structure with two cable indices
// (0x0C bytes) and no strings
static const uint8_t test_data[] = {
    199, 0x0C, 0x00, 0x35,          // Header
    0x02, 0x00,                     // Slot ID
    0x08,                           // Link width
    0x02,                           // Cable index count
    0x00, 0x00,                     // Cable index 0, start lane 0
    0x01, 0x04,                     // Cable index 1, start lane 4
    0, 0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_cabled_pcie_decode,
                                        test_rsd_cabled_pcie_setup, test_rsd_cabled_pcie_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_cabled_pcie_decode_short,
                                        test_rsd_cabled_pcie_setup, test_rsd_cabled_pcie_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_cabled_pcie_decode_truncated,
                                        test_rsd_cabled_pcie_setup, test_rsd_cabled_pcie_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_cabled_pcie_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_cabled_pcie_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_cabled_pcie_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x0C);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_cabled_pcie_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_CABLED_PCIE));
    assert_non_null(info);

    assert_int_equal(info->pci_slot_id, 2);
    assert_int_equal(info->link_width, 8);
    assert_int_equal(info->port_count, 2);
    assert_non_null(info->ports);
    assert_int_equal(info->ports[0].index, 0);
    assert_int_equal(info->ports[0].start_lane, 0);
    assert_int_equal(info->ports[1].index, 1);
    assert_int_equal(info->ports[1].start_lane, 4);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Structure one byte shorter than its minimum length is rejected.
//
static void test_rsd_cabled_pcie_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[0x09 + 2] = {};
    memcpy(data, test_data, 0x09);
    data[1] = 0x09;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Cable indices the structure declares but does not hold are left out.
//
static void test_rsd_cabled_pcie_decode_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[sizeof(test_data)];
    memcpy(data, test_data, sizeof(data));
    data[0x07] = 3;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_cabled_pcie_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_CABLED_PCIE));
    assert_non_null(info);
    assert_int_equal(info->port_count, 2);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}
