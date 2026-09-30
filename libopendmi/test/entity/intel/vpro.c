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
static void test_vpro_legacy(void **pstate);
static void test_vpro_tpm(void **pstate);

static const dmi_intel_vpro_t *test_vpro_info(dmi_context_t *context, const char *path, dmi_entity_t **pentity);
static bool test_vpro_shown(const dmi_entity_t *entity, const char *code);

static const char *test_asrock_path = OPENDMI_TEST_DATA "/asrock/b460-pro4.bin";
static const char *test_x280_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x280-20kf.bin";
static const char *test_6930p_path = OPENDMI_TEST_DATA "/hp/elitebook-6930p-nn187ea.bin";
static const char *test_w510_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-w510-4319.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_vpro_decode, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_signature, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_shared_type, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_legacy, test_vpro_setup, test_vpro_teardown),
        cmocka_unit_test_setup_teardown(test_vpro_tpm, test_vpro_setup, test_vpro_teardown)
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

    assert_false(info->has_mch_capabilities);
    assert_true(test_vpro_shown(entity, "mebx-version"));
    assert_false(test_vpro_shown(entity, "mch-device-id"));

    // LPC bridge of Intel B460 at 0:1F.0, and Intel I219-V at 0:1F.6
    assert_int_equal(info->lpc_devfn, 0xF8);
    assert_int_equal(info->lpc_bus, 0x00);
    assert_int_equal(info->lpc_device_id, 0xA3C8);
    assert_int_equal(info->gbe_devfn, 0xFE);
    assert_int_equal(info->gbe_bus, 0x00);
    assert_int_equal(info->gbe_device_id, 0x0D55);

    // VMX enabled, TXT capable, VT-x capable and enabled
    assert_int_equal(info->cpu_capabilities.__value, 0x35);
    assert_true(info->cpu_capabilities.is_vmx_enabled);
    assert_false(info->cpu_capabilities.is_smx_enabled);
    assert_true(info->cpu_capabilities.is_txt_capable);
    assert_false(info->cpu_capabilities.is_txt_enabled);
    assert_true(info->cpu_capabilities.is_vtx_capable);
    assert_true(info->cpu_capabilities.is_vtx_enabled);

    // Management Engine enabled, with no AMT
    assert_int_equal(info->me_capabilities.__value, 0x01);
    assert_true(info->me_capabilities.is_me_enabled);
    assert_false(info->me_capabilities.is_amt_supported);

    // VT-d and TXT configurable in setup, extensions of the Virtual Appliance
    assert_int_equal(info->bios_capabilities.__value, 0x26);
    assert_false(info->bios_capabilities.is_vtx_configurable);
    assert_true(info->bios_capabilities.is_vtd_configurable);
    assert_true(info->bios_capabilities.is_txt_configurable);
    assert_true(info->bios_capabilities.is_va_supported);
    assert_int_equal(info->va_version, 0);

    assert_false(info->tpm_capabilities.is_tpm_present);

    // Host bridge is given by the firmware of HP laptops only
    assert_false(info->has_host_bridge);
    assert_false(test_vpro_shown(entity, "host-device-id"));

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

    // ...while the one with it is, with the firmware of Small Business
    // Technology and of the level III manageability
    memcpy(data + 0x38, "vPro", 4);
    data[0x18] = 0x60;

    entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_ptr_equal(entity->spec, &dmi_intel_vpro_spec);

    const dmi_intel_vpro_t *info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
    assert_non_null(info);
    assert_true(info->me_capabilities.is_sbt_supported);
    assert_true(info->me_capabilities.is_l3_supported);
    assert_false(info->me_capabilities.is_amt_supported);

    // Version of the BIOS extension of all zeroes is not reported, which
    // does not make the layout the older one
    assert_false(info->has_mch_capabilities);
    assert_true(test_vpro_shown(entity, "mebx-version"));
    dmi_entity_destroy(entity);

    // Host bridge is not given when its device ID has either byte of all
    // bits set, or all of them
    static const uint16_t absent[] = { 0x00FF, 0xFF00, 0xFFFF };

    for (size_t i = 0; i < countof(absent); i++) {
        data[0x30] = absent[i] & 0xFF;
        data[0x31] = absent[i] >> 8;

        entity = dmi_test_entity_create(buffer, data, sizeof(data));
        assert_non_null(entity);
        assert_true(dmi_entity_decode(entity));

        info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
        assert_non_null(info);
        assert_false(info->has_host_bridge);
        dmi_entity_destroy(entity);
    }

    // ...while it is with any other, e.g. the one of Intel Haswell-ULT
    data[0x30] = 0x04;
    data[0x31] = 0x0A;

    entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
    assert_non_null(info);
    assert_true(info->has_host_bridge);
    assert_int_equal(info->host_device_id, 0x0A04);
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

//
// Older layout holds the memory controller hub in place of the version of
// the BIOS extension
//
static void test_vpro_legacy(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_entity_t *entity = nullptr;

    const dmi_intel_vpro_t *info = test_vpro_info(context, test_6930p_path, &entity);

    // Intel GM45 at 0:0.0, capable of VT-d and TXT
    assert_true(info->has_mch_capabilities);
    assert_int_equal(info->mch_devfn, 0x00);
    assert_int_equal(info->mch_bus, 0x00);
    assert_int_equal(info->mch_device_id, 0x2A40);
    assert_int_equal(info->mch_capabilities.__value, 0x05);
    assert_true(info->mch_capabilities.is_vtd_capable);
    assert_false(info->mch_capabilities.is_vtd_enabled);
    assert_true(info->mch_capabilities.is_txt_capable);

    assert_false(test_vpro_shown(entity, "mebx-version"));
    assert_true(test_vpro_shown(entity, "mch-device-id"));
    assert_true(test_vpro_shown(entity, "mch-capabilities"));

    // Host bridge is given in place of the wireless network controller too
    assert_true(info->has_host_bridge);
    assert_int_equal(info->host_devfn, 0x00);
    assert_int_equal(info->host_bus, 0x00);
    assert_int_equal(info->host_device_id, 0x2A40);
    assert_true(test_vpro_shown(entity, "host-device-id"));

    // Management Engine enabled, with AMT
    assert_true(info->me_capabilities.is_me_enabled);
    assert_true(info->me_capabilities.is_amt_supported);

    // TPM on board, whose version of the specification is not given
    assert_true(info->tpm_capabilities.is_tpm_present);
    assert_false(info->tpm_capabilities.is_tpm_enabled);
    assert_int_equal(info->tcg_major, 0);
}

static void test_vpro_tpm(void **pstate)
{
    dmi_context_t *context = *pstate;
    dmi_entity_t *entity = nullptr;

    const dmi_intel_vpro_t *info = test_vpro_info(context, test_w510_path, &entity);

    // TPM of version 1.2 of the TCG specification
    assert_int_equal(info->tpm_capabilities.__value, 0x02010001);
    assert_true(info->tpm_capabilities.is_tpm_present);
    assert_int_equal(info->tcg_major, 1);
    assert_int_equal(info->tcg_minor, 2);

    // Bits 7 to 9 give the highest version of the Virtual Appliance
    assert_int_equal(info->bios_capabilities.__value, 0x3E);
    assert_int_equal(info->va_version, 0);
}

static const dmi_intel_vpro_t *test_vpro_info(dmi_context_t *context, const char *path, dmi_entity_t **pentity)
{
    assert_true(dmi_load(context, path));

    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(intel_vpro), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_intel_vpro_spec);

    const dmi_intel_vpro_t *info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
    assert_non_null(info);

    *pentity = entity;

    return info;
}

static bool test_vpro_shown(const dmi_entity_t *entity, const char *code)
{
    for (const dmi_attribute_t *attr = entity->spec->attributes; attr->params.name != nullptr; attr++) {
        if (strcmp(attr->params.code, code) == 0)
            return dmi_attribute_resolve(attr, dmi_entity_info(entity, DMI_TYPE(intel_vpro))) != nullptr;
    }

    fail_msg("No attribute %s", code);
}
