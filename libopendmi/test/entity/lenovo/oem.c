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
#include <opendmi/platform.h>
#include <opendmi/registry.h>
#include <opendmi/test/logger.h>
#include <opendmi/module/lenovo.h>

#include <opendmi/entity/lenovo/oem.h>
#include <opendmi/entity/lenovo/records.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static void test_lenovo_mobile_oem(void **pstate);
static void test_lenovo_oem(void **pstate);
static void test_lenovo_date(void **pstate);
static void test_lenovo_mtm(void **pstate);
static void test_lenovo_platforms(void **pstate);

static const char *test_b590_path = OPENDMI_TEST_DATA "/lenovo/b590.bin";
static const char *test_t61p_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t61p-6460-6xg.bin";
static const char *test_x280_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x280-20kf.bin";
static const char *test_x220_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x220-4290le6.bin";
static const char *test_thinkbook_path = OPENDMI_TEST_DATA "/lenovo/thinkbook-15-g2-itl-20ve.bin";
static const char *test_l420_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-l420-7854.bin";

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_lenovo_mobile_oem, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_lenovo_oem, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_lenovo_date, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_lenovo_mtm, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_lenovo_platforms, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_lenovo_mobile_oem(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_b590_path));

    // OEM structure 3 tells the devices present
    const dmi_lenovo_device_presence_t *presence = test_info(context, 0x0034, &dmi_lenovo_device_presence_spec);
    assert_int_equal(presence->number, 3);
    assert_int_equal(presence->devices, 0x04);
    assert_false(presence->is_fingerprint_reader);

    // OEM structure 2 carries a signature of its own in place of the revision
    const dmi_lenovo_bay_io_t *bay = test_info(context, 0x0032, &dmi_lenovo_bay_io_spec);
    assert_int_equal(bay->version, 3);
    assert_memory_equal(bay->bay_signature.data, "BAY I/O ", 8);

    // Other OEM structures are told by the signature alone
    const dmi_lenovo_oem_t *oem = test_info(context, 0x0038, &dmi_lenovo_mobile_oem_spec);
    assert_int_equal(oem->number, 1);
    assert_int_equal(oem->revision, 1);
    assert_int_equal(oem->data.length, 9);

    dmi_close(context);

    assert_true(dmi_load(context, test_t61p_path));

    oem = test_info(context, 0x0041, &dmi_lenovo_mobile_oem_spec);
    assert_int_equal(oem->number, 4);
}

static void test_lenovo_oem(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_x280_path));

    // OEM structure 7 tells the version of the program of the embedded
    // controller
    const dmi_lenovo_ecp_t *ecp = test_info(context, 0x003C, &dmi_lenovo_ecp_spec);
    assert_string_equal(ecp->version, "N20HT28W");
    assert_string_equal(ecp->release_date, "10/08/2020");

    const dmi_lenovo_oem_t *oem = test_info(context, 0x0036, &dmi_lenovo_oem_spec);
    assert_int_equal(oem->number, 4);
    assert_int_equal(oem->revision, 1);
    assert_memory_equal(oem->data.data, "\xB2\x00MS \x00", 6);
}

static dmi_log_t test_logger = { dmi_test_log_handler };

static void test_lenovo_date(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Date in binary-coded decimal, the year low byte first
    assert_true(dmi_load(context, test_x280_path));

    const dmi_lenovo_date_t *info = test_info(context, 0x0002, &dmi_lenovo_date_spec);
    assert_int_equal(info->date, dmi_date(2019, 6, 27));

    dmi_close(context);

    assert_true(dmi_load(context, test_x220_path));

    info = test_info(context, 0x000C, &dmi_lenovo_date_spec);
    assert_int_equal(info->date, dmi_date(2012, 4, 11));

    // Structures holding TPM information are told by their first string
    const dmi_lenovo_tpm_info_t *tpm = test_info(context, 0x0000, &dmi_lenovo_tpm_info_spec);
    assert_string_equal(tpm->vendor, "STM ");
    assert_string_equal(tpm->description, "System Reserved");

    dmi_close(context);

    assert_true(dmi_load(context, test_t61p_path));

    tpm = test_info(context, 0x003D, &dmi_lenovo_tpm_info_spec);
    assert_string_equal(tpm->vendor, "ATML");
    assert_int_equal(tpm->unknown_4, 3);
}

static void test_lenovo_mtm(void **pstate)
{
    dmi_context_t *context = *pstate;

    // System information holds only the machine type
    assert_true(dmi_load(context, test_thinkbook_path));

    const dmi_lenovo_mtm_t *info = test_info(context, 0x003B, &dmi_lenovo_mtm_spec);
    assert_string_equal(info->brand, "IdeaPad");
    assert_string_equal(info->mtm, "20VE00U9RU");
    assert_int_equal(info->data.length, 10);

    // ThinkPads carry structures of another layout at the same type
    dmi_close(context);
    assert_true(dmi_load(context, test_l420_path));

    dmi_entity_t *entity = dmi_registry_lookup_first_id(dmi_get_registry(context), DMI_TYPE_ID(LENOVO_MTM), false);
    assert_non_null(entity);
    assert_null(entity->spec);

    // Structure is not named after the specification it does not match
    assert_string_equal(dmi_entity_name(entity), "OEM-specific");
}

static int test_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);
    *pstate = context;

    return 0;
}

static int test_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec)
{
    dmi_entity_t *entity = dmi_registry_lookup(dmi_get_registry(context), handle, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, spec->type);
    assert_non_null(info);

    return info;
}

//
// Systems of Lenovo are told by the vendor of the system too, since they may
// carry the firmware of other vendors
//
static void test_lenovo_platforms(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const struct {
        dmi_vendor_t firmware_vendor;
        dmi_vendor_t system_vendor;
        bool         matches;
    } test_cases[] = {
        { DMI_VENDOR_LENOVO, DMI_VENDOR_LENOVO, true  },
        { DMI_VENDOR_AMI,    DMI_VENDOR_LENOVO, true  },
        { DMI_VENDOR_AMI,    DMI_VENDOR_IBM,    true  },
        { DMI_VENDOR_AMI,    DMI_VENDOR_ACER,   false }
    };

    for (size_t i = 0; i < countof(test_cases); i++) {
        dmi_platform_t *platform = dmi_platform_create(context);
        assert_non_null(platform);

        platform->firmware_vendor = test_cases[i].firmware_vendor;
        platform->system_vendor   = test_cases[i].system_vendor;

        bool matches = false;
        for (const dmi_platform_match_t *match = dmi_lenovo_module.platforms;
             match->firmware_vendor != DMI_VENDOR_INVALID; match++) {
            if (dmi_platform_match(platform, match))
                matches = true;
        }

        assert_int_equal(matches, test_cases[i].matches);

        dmi_platform_destroy(platform);
    }
}
