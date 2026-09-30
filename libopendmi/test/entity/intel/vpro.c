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

#include <opendmi/entity/intel/vpro.h>

static int test_vpro_setup(void **pstate);
static int test_vpro_teardown(void **pstate);

static void test_vpro_decode(void **pstate);
static void test_vpro_signature(void **pstate);
static void test_vpro_shared_type(void **pstate);

static const char *test_asrock_path = OPENDMI_TEST_DATA "/asrock/b460-pro4.bin";
static const char *test_x280_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x280-20kf.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_vpro_decode, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_signature, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_shared_type, test_vpro_setup, test_vpro_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_vpro_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_vpro_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_vpro_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_asrock_path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_vpro), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_vpro_spec);
    assert_string_equal(dmi_entity_name(entity), "Intel vPro information");

    const dmi_intel_vpro_t *info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
    assert_non_null(info);

    // Versions match the ones of the firmware version information
    assert_int_equal(info->mebx_version.major, 10);
    assert_int_equal(info->mebx_version.minor, 0);
    assert_int_equal(info->mebx_version.hotfix, 0);
    assert_int_equal(info->mebx_version.build, 1);

    assert_int_equal(info->me_version.major, 14);
    assert_int_equal(info->me_version.minor, 5);
    assert_int_equal(info->me_version.hotfix, 12);
    assert_int_equal(info->me_version.build, 1111);

    // LPC bridge of Intel B460 at 0:1F.0, and Intel I219-V at 0:1F.6
    assert_int_equal(info->lpc_devfn, 0x00F8);
    assert_int_equal(info->lpc_device_id, 0xA3C8);
    assert_int_equal(info->gbe_devfn, 0x00FE);
    assert_int_equal(info->gbe_device_id, 0x0D55);

    assert_int_equal(info->flags_1, 0x35);
    assert_int_equal(info->flags_2, 0x01);
    assert_int_equal(info->flags_3, 0x26);

    assert_int_equal(info->signature.length, 4);
    assert_memory_equal(info->signature.data, "vPro", 4);
}

static void test_vpro_signature(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_intel_module));

    // Structure of type 131 without the signature is not decoded
    uint8_t data[0x42] = {
        131, 0x40, 0x10, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_null(entity->spec);
    dmi_entity_destroy(entity);

    // ...while the one with it is
    memcpy(data + 0x38, "vPro", 4);

    entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_ptr_equal(entity->spec, &dmi_intel_vpro_spec);
    dmi_entity_destroy(entity);

    dmi_buffer_destroy(buffer);
}

static void test_vpro_shared_type(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Lenovo gives type 131 to a structure of its own in the same table
    assert_true(dmi_load(context, test_x280_path));

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, dmi_get_registry(context), nullptr);

    size_t vpro  = 0;
    size_t other = 0;

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (entity->type_id != DMI_TYPE_ID(INTEL_VPRO))
            continue;

        if (entity->spec == &dmi_intel_vpro_spec) {
            vpro++;
            assert_non_null(dmi_entity_info(entity, DMI_TYPE(intel_vpro)));
        } else {
            other++;
            assert_string_equal(dmi_entity_string(entity, 1), "TVT-Enablement");
        }
    }

    assert_int_equal(vpro, 1);
    assert_int_equal(other, 1);
}
