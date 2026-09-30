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

#include <opendmi/entity/intel-rsd/tpm.h>

static int test_rsd_tpm_setup(void **pstate);
static int test_rsd_tpm_teardown(void **pstate);

static void test_rsd_tpm_decode(void **pstate);
static void test_rsd_tpm_decode_short(void **pstate);

static dmi_log_t test_logger = { dmi_test_log_handler };

// Intel RSD TPM information structure (0x07 bytes) followed by strings
static const uint8_t test_data[] = {
    195, 0x07, 0x00, 0x32,          // Header
    0x01,                           // Configuration index
    0x01,                           // Version
    0x01,                           // Status (enabled)
    'T', 'P', 'M', ' ', '2', '.', '0', 0,
    0
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_tpm_decode,
                                        test_rsd_tpm_setup, test_rsd_tpm_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_tpm_decode_short,
                                        test_rsd_tpm_setup, test_rsd_tpm_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_tpm_setup(void **pstate)
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

static int test_rsd_tpm_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_rsd_tpm_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, test_data, sizeof(test_data));
    assert_non_null(entity);
    assert_int_equal(entity->body_length, 0x07);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_tpm_t *info = dmi_entity_info(entity, DMI_TYPE(intel_rsd_tpm));
    assert_non_null(info);

    assert_int_equal(info->config_index, 1);
    assert_string_equal(info->version, "TPM 2.0");
    assert_int_equal(info->status, DMI_INTEL_RSD_TPM_STATUS_ENABLED);

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}

//
// Structure one byte shorter than its minimum length is rejected.
//
static void test_rsd_tpm_decode_short(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[0x06 + 2] = {};
    memcpy(data, test_data, 0x06);
    data[1] = 0x06;

    dmi_buffer_t *entity_buffer = dmi_buffer_create(context);

    dmi_entity_t *entity = dmi_test_entity_create(entity_buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    dmi_entity_destroy(entity);

    dmi_buffer_destroy(entity_buffer);
}
