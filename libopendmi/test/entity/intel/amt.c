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
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/intel.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/amt.h>

static int test_amt_setup(void **pstate);
static int test_amt_teardown(void **pstate);

static void test_amt_decode(void **pstate);
static void test_amt_extended(void **pstate);
static void test_amt_signature(void **pstate);

static const char *test_nuvo_path = OPENDMI_TEST_DATA "/neousys/nuvo-7000-a2-cfl-s.bin";
static const char *test_t14_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t14-g3-21aj.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_amt_decode, test_amt_setup, test_amt_teardown),
        cmocka_unit_test_setup_teardown(test_amt_extended, test_amt_setup, test_amt_teardown),
        cmocka_unit_test_setup_teardown(test_amt_signature, test_amt_setup, test_amt_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_amt_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_amt_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_amt_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // AMT of a platform with the corporate firmware
    assert_true(dmi_load(context, test_nuvo_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_amt), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_amt_spec);

    const dmi_intel_amt_t *info = dmi_entity_info(entity, DMI_TYPE(intel_amt));
    assert_non_null(info);

    assert_memory_equal(info->signature.data, "$AMT", 4);
    assert_true(info->is_supported);
    assert_true(info->is_enabled);
    assert_true(info->is_ider_enabled);
    assert_true(info->is_sol_enabled);
    assert_true(info->is_network_enabled);
    assert_true(info->is_kvm_enabled);
    assert_int_equal(info->extended_data, 0xA5);
    assert_int_equal(info->oem_capabilities[0], 0xBF);
    assert_int_equal(info->oem_capabilities[1], 0x02);
    assert_int_equal(info->oem_capabilities[2], 0xC0);
    assert_int_equal(info->oem_capabilities[3], 0x00);
}

static void test_amt_extended(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Longer structure of later platforms holds more bytes at the end
    assert_true(dmi_load(context, test_t14_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_amt), false);
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x18);

    const dmi_intel_amt_t *info = dmi_entity_info(entity, DMI_TYPE(intel_amt));
    assert_non_null(info);
    assert_true(info->is_enabled);
    assert_int_equal(info->oem_capabilities[0], 0xEF);
}

static void test_amt_signature(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Structure of type 130 without the signature is not decoded
    uint8_t data[0x16] = {
        130, 0x14, 0x10, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_null(entity->spec);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}
