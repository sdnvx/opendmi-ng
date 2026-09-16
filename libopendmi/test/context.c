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
#include <opendmi/test/logger.h>

static int test_context_setup(void **pstate);
static int test_context_teardown(void **pstate);

static void test_context_close_resets_state(void **pstate);
static void test_context_reopen(void **pstate);
static void test_context_reopen_after_failure(void **pstate);
static void test_context_dump_save_after_close(void **pstate);
static void test_context_dump_save_roundtrip(void **pstate);

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
        cmocka_unit_test_setup_teardown(test_context_dump_save_roundtrip, test_context_setup, test_context_teardown)
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
