//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/module.h>
#include <opendmi/module/intel.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-network-card.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>

static int test_context_setup(void **pstate);
static int test_context_teardown(void **pstate);

static void test_context_close_resets_state(void **pstate);
static void test_context_reopen(void **pstate);
static void test_context_reopen_after_failure(void **pstate);
static void test_context_dump_save_after_close(void **pstate);
static void test_context_dump_save_roundtrip(void **pstate);
static void test_context_dump_save_errors(void **pstate);
static void test_context_add_extension(void **pstate);
static void test_context_add_extension_duplicate(void **pstate);

static const char *test_dump_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t14-g3-21aj.bin";

// Relative to test working directory
static const char *test_save_path = "context-test-dump.bin";

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_context_close_resets_state, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_reopen, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_reopen_after_failure, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_dump_save_after_close, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_dump_save_roundtrip, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_dump_save_errors, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_add_extension, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_add_extension_duplicate, test_context_setup, test_context_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_context_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_context_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_context_close_resets_state(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_spec_t **type_map = context->type_map;
    unsigned int flags = context->flags;

    assert_true(dmi_dump_load(context, test_dump_path));
    assert_non_null(context->state.backend);
    assert_non_null(context->state.session);
    assert_non_null(context->state.registry);
    assert_non_null(context->state.entry_data);
    assert_non_null(context->state.table_data);
    assert_int_not_equal(context->state.smbios_version, 0);

    assert_true(dmi_close(context));
    assert_null(context->state.backend);
    assert_null(context->state.session);
    assert_null(context->state.registry);
    assert_null(context->state.entry_data);
    assert_null(context->state.entry_spec);
    assert_null(context->state.table_data);
    assert_null(context->state.vendor_name);
    assert_int_equal(context->state.entry_size, 0);
    assert_int_equal(context->state.table_area_size, 0);
    assert_int_equal(context->state.smbios_version, 0);
    assert_int_equal(context->state.vendor, DMI_VENDOR_OTHER);

    // Context settings are preserved
    assert_ptr_equal(context->type_map, type_map);
    assert_ptr_equal(context->logger, &test_logger);
    assert_int_equal(context->flags, flags);

    // Closing already closed context is harmless
    assert_true(dmi_close(context));
}

static void test_context_reopen(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_dump_load(context, test_dump_path));
    assert_true(dmi_close(context));
    assert_true(dmi_dump_load(context, test_dump_path));
    assert_non_null(context->state.registry);
}

static void test_context_reopen_after_failure(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_false(dmi_dump_load(context, OPENDMI_TEST_DATA "/nonexistent.bin"));
    assert_null(context->state.backend);

    dmi_error_clear(context);
    assert_true(dmi_dump_load(context, test_dump_path));
}

static void test_context_dump_save_after_close(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Never opened
    dmi_error_clear(context);
    assert_false(dmi_dump_save(context, test_save_path, true));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_STATE);

    // Opened and then closed
    assert_true(dmi_dump_load(context, test_dump_path));
    assert_true(dmi_close(context));

    dmi_error_clear(context);
    assert_false(dmi_dump_save(context, test_save_path, true));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_INVALID_STATE);

    remove(test_save_path);
}

static void test_context_dump_save_roundtrip(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_dump_load(context, test_dump_path));
    assert_true(dmi_dump_save(context, test_save_path, true));
    assert_true(dmi_close(context));

    assert_true(dmi_dump_load(context, test_save_path));
    assert_non_null(context->state.registry);

    remove(test_save_path);
}

static void test_context_dump_save_errors(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_dump_load(context, test_dump_path));

    // Existing file is not overwritten
    assert_true(dmi_dump_save(context, test_save_path, false));

    dmi_error_clear(context);
    assert_false(dmi_dump_save(context, test_save_path, false));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_FILE_OPEN);

    remove(test_save_path);

    // Write errors are reported, and special files are not removed
    FILE *device = fopen("/dev/full", "r");
    if (device == nullptr)
        return;

    fclose(device);

    dmi_error_clear(context);
    assert_false(dmi_dump_save(context, "/dev/full", true));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_FILE_WRITE);

    device = fopen("/dev/full", "r");
    if (device != nullptr)
        fclose(device);
    else
        fail_msg("Device /dev/full has been removed");
}

static void test_context_add_extension(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_module_t *module = dmi_module_find("intel");
    assert_non_null(module);

    assert_false(dmi_has_extension(context, module));
    assert_true(dmi_add_extension(context, module));
    assert_true(dmi_has_extension(context, module));
    assert_false(dmi_has_extension(context, dmi_module_find("dell")));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(INTEL_RSD_NETWORK_CARD)),
                     &dmi_intel_rsd_network_card_spec);
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID)),
                     &dmi_intel_rsd_processor_cpuid_spec);

    // The same module cannot be added twice
    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, module));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);
    assert_true(dmi_has_extension(context, module));

    // Modules without entities can be enabled, but only once
    const dmi_module_t empty_module = {
        .code = "empty",
        .name = "Empty module"
    };

    assert_true(dmi_add_extension(context, &empty_module));
    assert_true(dmi_has_extension(context, &empty_module));

    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, &empty_module));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);

    // Enabled modules are preserved when context is closed
    assert_true(dmi_close(context));
    assert_true(dmi_has_extension(context, module));

    assert_false(dmi_has_extension(nullptr, module));
    assert_false(dmi_has_extension(context, nullptr));
}

static void test_context_add_extension_duplicate(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const dmi_entity_spec_t spec_1 = {
        .type = (dmi_type_t)250,
        .code = "test-1",
        .name = "Test 1"
    };
    static const dmi_entity_spec_t spec_2 = {
        .type = (dmi_type_t)250,
        .code = "test-2",
        .name = "Test 2"
    };

    const dmi_module_t module = {
        .code     = "test",
        .name     = "Test module",
        .entities = (const dmi_entity_spec_t *[]){ &spec_1, &spec_2, nullptr }
    };

    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, &module));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);

    // Type map is not modified on conflicts
    assert_null(dmi_type_spec(context, (dmi_type_t)250));
    assert_false(dmi_has_extension(context, &module));
}
