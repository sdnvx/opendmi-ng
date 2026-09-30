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
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/asf.h>

static int test_asf_setup(void **pstate);
static int test_asf_teardown(void **pstate);

static void test_asf_decode(void **pstate);
static void test_asf_vendors(void **pstate);
static void test_asf_extra(void **pstate);
static void test_asf_signature(void **pstate);

static const dmi_intel_asf_t *test_asf_info(dmi_context_t *context, const char *path, dmi_handle_t handle);

static const char *test_e6230_path = OPENDMI_TEST_DATA "/dell/latitude-e6230-1.bin";
static const char *test_t61p_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t61p-6460-6xg.bin";
static const char *test_framework_path = OPENDMI_TEST_DATA "/framework/laptop-13-12th-gen-intel.bin";
static const char *test_aspire_path = OPENDMI_TEST_DATA "/acer/aspire-3680.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_asf_decode, test_asf_setup, test_asf_teardown),
        cmocka_unit_test_setup_teardown(test_asf_vendors, test_asf_setup, test_asf_teardown),
        cmocka_unit_test_setup_teardown(test_asf_extra, test_asf_setup, test_asf_teardown),
        cmocka_unit_test_setup_teardown(test_asf_signature, test_asf_setup, test_asf_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_asf_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_asf_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_asf_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structure of the Intel reference code, kept by Dell at its own type
    const dmi_intel_asf_t *info = test_asf_info(context, test_e6230_path, 0xE01C);
    assert_int_equal(info->version, 1);
    assert_string_equal(info->name, "Intel_ASF");
    assert_string_equal(info->identifier, "Intel_ASF_001");
    assert_int_equal(info->parameter, 1);
    assert_false(info->has_extra);
}

static void test_asf_vendors(void **pstate)
{
    dmi_context_t *context = *pstate;

    // ThinkPad T61p is the only platform which gives the parameter as zero
    const dmi_intel_asf_t *info = test_asf_info(context, test_t61p_path, 0x0046);
    assert_string_equal(info->name, "Intel_ASF");
    assert_int_equal(info->parameter, 0);

    dmi_close(context);

    // Firmware of Insyde names the support of its own
    info = test_asf_info(context, test_framework_path, 0x002A);
    assert_string_equal(info->name, "Insyde_ASF_001");
    assert_string_equal(info->identifier, "Insyde_ASF_002");
    assert_int_equal(info->parameter, 1);
}

static void test_asf_extra(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Structures of 16 bytes hold 8 more bytes
    const dmi_intel_asf_t *info = test_asf_info(context, test_aspire_path, 0x001F);
    assert_string_equal(info->name, "Intel_ASF_001");
    assert_true(info->has_extra);
    assert_memory_equal(info->extra, "\x00\x00\x00\x01\x00\x00\x08\x01", 8);
}

static void test_asf_signature(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Structures other vendors give type 129 are not decoded, e.g. the ones
    // of Gigabyte
    uint8_t data[] = {
        129, 0x08, 0x5C, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_null(entity->spec);
    dmi_entity_destroy(entity);

    // ...nor the ones whose version matches while the first string does not
    data[0x04] = 0x01;

    entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_null(entity->spec);
    dmi_entity_destroy(entity);

    dmi_buffer_destroy(buffer);
}

static const dmi_intel_asf_t *test_asf_info(dmi_context_t *context, const char *path, dmi_handle_t handle)
{
    assert_true(dmi_load(context, path));

    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), handle, DMI_TYPE(intel_asf), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_asf_spec);

    const dmi_intel_asf_t *info = dmi_entity_info(entity, DMI_TYPE(intel_asf));
    assert_non_null(info);

    return info;
}
