//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entry.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/module.h>
#include <opendmi/module/intel.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
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
static void test_context_dump_save_relocated(void **pstate);
static void test_context_dump_save_generated(void **pstate);
static void test_context_add_extension(void **pstate);
static void test_context_add_extension_duplicate(void **pstate);

static dmi_data_t *test_dump_relocate(dmi_context_t *context, const char *path, bool legacy, size_t *psize);
static void test_dump_verify(dmi_context_t *context, const dmi_data_t *table, size_t table_size);
static void test_dump_write(const char *path, const dmi_data_t *data, size_t size);
static void test_checksum_fix(dmi_data_t *data, size_t checksum_offset, size_t start, size_t length);
static uint64_t test_address_get(const dmi_data_t *data, size_t size);
static void test_address_set(dmi_data_t *data, size_t size, uint64_t address);

static const char *test_dump_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t14-g3-21aj.bin";

// Relative to test working directory
static const char *test_save_path = "context-test-dump.bin";
static const char *test_source_path = "context-test-source.bin";

// SMBIOS 2.1+ (32-bit) entry point
static const char *test_dump_v21_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-x220-4290le6.bin";

// Table address as found in firmware
static const uint64_t test_table_address = 0x7AEB2000;

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
        cmocka_unit_test_setup_teardown(test_context_dump_save_relocated, test_context_setup, test_context_teardown),
        cmocka_unit_test_setup_teardown(test_context_dump_save_generated, test_context_setup, test_context_teardown),
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
    assert_int_equal(context->state.entry_data_size, 0);
    assert_int_equal(context->state.entry_length, 0);
    assert_int_equal(context->state.table_area_size, 0);
    assert_int_equal(context->state.table_size, 0);
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

static void test_context_dump_save_relocated(void **pstate)
{
    dmi_context_t *context = *pstate;

    const struct {
        const char *path;
        bool        legacy;
    } sources[] = {
        { test_dump_path,     false },
        { test_dump_v21_path, false },
        { test_dump_v21_path, true  }
    };

    for (size_t i = 0; i < countof(sources); i++) {
        size_t size = 0;
        dmi_data_t *source = test_dump_relocate(context, sources[i].path, sources[i].legacy, &size);
        test_dump_write(test_source_path, source, size);

        // Table address from the entry point is replaced on save
        assert_true(dmi_dump_load(context, test_source_path));
        assert_int_equal(context->state.table_area_addr, test_table_address);

        dmi_version_t version = context->state.smbios_version;
        size_t count = context->state.registry->count;

        assert_true(dmi_dump_save(context, test_save_path, true));
        assert_true(dmi_close(context));

        test_dump_verify(context, source + DMI_ENTRY_MAX_SIZE, size - DMI_ENTRY_MAX_SIZE);

        // Saved dump is loaded the same way as the source one
        assert_true(dmi_dump_load(context, test_save_path));
        assert_int_equal(context->state.table_area_addr, DMI_ENTRY_MAX_SIZE);
        assert_int_equal(context->state.smbios_version, version);
        assert_int_equal(context->state.registry->count, count);
        assert_true(dmi_close(context));

        dmi_free(source);
    }

    remove(test_source_path);
    remove(test_save_path);
}

static void test_context_dump_save_generated(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_dump_load(context, test_dump_v21_path));

    dmi_version_t version = context->state.smbios_version;
    size_t count = context->state.registry->count;

    // Simulate backend without entry point data, like the Windows one
    context->state.entry_data = nullptr;
    context->state.entry_data_size = 0;
    context->state.entry_spec = nullptr;

    assert_true(dmi_dump_save(context, test_save_path, true));

    size_t table_size = context->state.table_size;
    dmi_data_t *table = dmi_alloc(context, table_size);
    assert_non_null(table);
    memcpy(table, context->state.table_data, table_size);

    assert_true(dmi_close(context));

    test_dump_verify(context, table, table_size);
    dmi_free(table);

    // 64-bit entry point is generated
    assert_true(dmi_dump_load(context, test_save_path));
    assert_string_equal(context->state.entry_spec->anchor, DMI_ANCHOR_V30);
    assert_int_equal(context->state.smbios_version, version);
    assert_int_equal(context->state.table_area_max_size, table_size);
    assert_int_equal(context->state.registry->count, count);

    remove(test_save_path);
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

//
// Load dump file and replace the table address in its entry point, as if it
// was read from firmware. Optionally, SMBIOS 2.1+ entry point is converted to
// the legacy one.
//
static dmi_data_t *test_dump_relocate(dmi_context_t *context, const char *path, bool legacy, size_t *psize)
{
    dmi_data_t *data = dmi_file_get(context, path, -1, psize);
    assert_non_null(data);
    assert_true(*psize > DMI_ENTRY_MAX_SIZE);

    if (memcmp(data, DMI_ANCHOR_V30, strlen(DMI_ANCHOR_V30)) == 0) {
        test_address_set(data + 0x10, 8, test_table_address);
        test_checksum_fix(data, 0x05, 0x00, data[0x06]);
    } else if (legacy) {
        assert_memory_equal(data, DMI_ANCHOR_V21, strlen(DMI_ANCHOR_V21));

        memmove(data, data + 0x10, 0x0F);
        memset(data + 0x0F, 0, DMI_ENTRY_MAX_SIZE - 0x0F);

        test_address_set(data + 0x08, 4, test_table_address);
        test_checksum_fix(data, 0x05, 0x00, 0x0F);
    } else {
        assert_memory_equal(data, DMI_ANCHOR_V21, strlen(DMI_ANCHOR_V21));

        test_address_set(data + 0x18, 4, test_table_address);
        test_checksum_fix(data, 0x15, 0x10, 0x0F);
        test_checksum_fix(data, 0x04, 0x00, data[0x05]);
    }

    return data;
}

//
// Verify saved dump file the same way as `dmidecode --from-dump` reads it:
// the table is read at the address specified in the entry point.
//
static void test_dump_verify(dmi_context_t *context, const dmi_data_t *table, size_t table_size)
{
    size_t size = 0;
    dmi_data_t *data = dmi_file_get(context, test_save_path, -1, &size);
    assert_non_null(data);
    assert_int_equal(size, DMI_ENTRY_MAX_SIZE + table_size);

    size_t length = 0;
    uint64_t address = 0;

    if (memcmp(data, "_SM3_", 5) == 0) {
        length = data[0x06];
        assert_uint_in_range(length, 0x18, DMI_ENTRY_MAX_SIZE);
        assert_int_equal(test_address_get(data + 0x0C, 4), table_size);
        address = test_address_get(data + 0x10, 8);
    } else if (memcmp(data, "_SM_", 4) == 0) {
        length = data[0x05];
        assert_uint_in_range(length, 0x1E, DMI_ENTRY_MAX_SIZE);
        assert_memory_equal(data + 0x10, "_DMI_", 5);
        assert_true(dmi_checksum_test(data + 0x10, 0x0F));
        address = test_address_get(data + 0x18, 4);
    } else if (memcmp(data, "_DMI_", 5) == 0) {
        length = 0x0F;
        address = test_address_get(data + 0x08, 4);
    } else {
        fail_msg("No entry point found in %s", test_save_path);
    }

    assert_true(dmi_checksum_test(data, length));
    assert_int_equal(address, DMI_ENTRY_MAX_SIZE);

    // Entry point is padded with zeroes
    for (size_t i = length; i < DMI_ENTRY_MAX_SIZE; i++)
        assert_int_equal(data[i], 0);

    assert_memory_equal(data + address, table, table_size);

    dmi_free(data);
}

static void test_dump_write(const char *path, const dmi_data_t *data, size_t size)
{
    FILE *stream = fopen(path, "wb");

    if (stream != nullptr) {
        size_t written = fwrite(data, 1, size, stream);
        int rv = fclose(stream);

        assert_int_equal(written, size);
        assert_int_equal(rv, 0);
    } else {
        fail_msg("Unable to create file %s", path);
    }
}

static void test_checksum_fix(dmi_data_t *data, size_t checksum_offset, size_t start, size_t length)
{
    data[checksum_offset] = 0;
    data[checksum_offset] = dmi_checksum_calc(data + start, length);
}

static uint64_t test_address_get(const dmi_data_t *data, size_t size)
{
    if (size == sizeof(dmi_qword_t)) {
        dmi_qword_t address;
        memcpy(&address, data, sizeof(address));
        return dmi_decode_qword(address);
    } else {
        dmi_dword_t address;
        memcpy(&address, data, sizeof(address));
        return dmi_decode_dword(address);
    }
}

static void test_address_set(dmi_data_t *data, size_t size, uint64_t address)
{
    if (size == sizeof(dmi_qword_t)) {
        dmi_qword_t value = dmi_encode_qword(address);
        memcpy(data, &value, sizeof(value));
    } else {
        dmi_dword_t value = dmi_encode_dword((uint32_t)address);
        memcpy(data, &value, sizeof(value));
    }
}
