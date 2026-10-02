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
#include <opendmi/error.h>
#include <opendmi/entity.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/bios-flags.h>
#include <opendmi/entity/dell/hotkeys.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static void test_dell_bios_flags(void **pstate);
static void test_dell_hotkeys(void **pstate);

static const char *test_g15_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_dell_bios_flags, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_dell_hotkeys, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_dell_bios_flags(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    const dmi_dell_bios_flags_t *info = test_info(context, 0xB100, &dmi_dell_bios_flags_spec);
    assert_int_equal(info->flags, 0x001E);
    assert_true(info->is_acpi_wmi);

    // Flags take 8 bytes, and the shorter structures are not decoded, the way
    // the Dell SMBIOS WMI driver of Linux skips them
    static const uint8_t data[] = {
        177, 0x06, 0x00, 0xB1,
        0x02, 0x00,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_false(dmi_entity_decode(entity));

    const dmi_error_t *error = dmi_error_get_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_ENTITY_LENGTH_INVALID);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_dell_hotkeys(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_g15_path));

    // Mappings run to the end of the structure
    const dmi_dell_hotkeys_t *info = test_info(context, 0xB200, &dmi_dell_hotkeys_spec);
    assert_int_equal(info->hotkey_count, 22);
    assert_int_equal(info->hotkeys[0].scancode, 0x010A);
    assert_int_equal(info->hotkeys[0].keycode, 0x0012);
    assert_int_equal(info->hotkeys[9].scancode, 0x0048);
    assert_int_equal(info->hotkeys[9].keycode, 0xFFFF);
    assert_int_equal(info->hotkeys[21].scancode, 0x003F);
    assert_int_equal(info->hotkeys[21].keycode, 0x0016);
}

static dmi_log_t test_logger = { dmi_test_log_handler };

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
    dmi_registry_t *registry = dmi_get_registry(context);
    const dmi_entity_t *entity = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, spec);

    const void *info = dmi_entity_info(entity, spec->type);
    assert_non_null(info);

    return info;
}
