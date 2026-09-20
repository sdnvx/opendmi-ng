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
#include <opendmi/entity.h>
#include <opendmi/entry.h>
#include <opendmi/error.h>
#include <opendmi/log.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

static int test_dump_setup(void **pstate);
static int test_dump_teardown(void **pstate);

static void test_dump_load_valid(void **pstate);
static void test_dump_load_not_dump(void **pstate);
static void test_dump_load_too_small(void **pstate);
static void test_dump_load_bad_checksum(void **pstate);
static void test_dump_load_entry_only(void **pstate);
static void test_dump_load_truncated(void **pstate);
static void test_dump_load_bad_length(void **pstate);
static void test_dump_load_bad_length_strict(void **pstate);
static void test_dump_load_mutated(void **pstate);

static void test_dump_write(const dmi_data_t *data, size_t size);
static bool test_dump_has_error(dmi_context_t *context, dmi_error_code_t reason);
static size_t test_dump_decode_all(dmi_context_t *context);

static dmi_log_t test_logger = { DMI_LOG_ERROR, dmi_test_log_handler };

static const char *test_source_path = OPENDMI_TEST_DATA "/lenovo/thinkpad-t14-g3-21aj.bin";

// Relative to test working directory
static const char *test_dump_path = "dump-test.bin";

// Number of structures in the source dump
static const size_t test_source_count = 80;

// Number of mutated dumps to load
static const size_t test_mutation_count = 500;

typedef struct test_dump_state
{
    dmi_context_t *context;
    dmi_data_t    *source;
    size_t         source_size;
} test_dump_state_t;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_dump_load_valid),
        cmocka_unit_test(test_dump_load_not_dump),
        cmocka_unit_test(test_dump_load_too_small),
        cmocka_unit_test(test_dump_load_bad_checksum),
        cmocka_unit_test(test_dump_load_entry_only),
        cmocka_unit_test(test_dump_load_truncated),
        cmocka_unit_test(test_dump_load_bad_length),
        cmocka_unit_test(test_dump_load_bad_length_strict),
        cmocka_unit_test(test_dump_load_mutated)
    };

    return cmocka_run_group_tests(tests, test_dump_setup, test_dump_teardown);
}

static int test_dump_setup(void **pstate)
{
    test_dump_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    *pstate = state;

    state->context = dmi_create(0);
    if (state->context == nullptr)
        return -1;

    dmi_set_logger(state->context, &test_logger);

    state->source = dmi_file_get(state->context, test_source_path, -1, &state->source_size);
    if (state->source == nullptr)
        return -1;

    return 0;
}

static int test_dump_teardown(void **pstate)
{
    test_dump_state_t *state = *pstate;

    if (state != nullptr) {
        dmi_destroy(state->context);
        dmi_free(state->source);
        free(state);
    }

    remove(test_dump_path);

    *pstate = nullptr;

    return 0;
}

static void test_dump_load_valid(void **pstate)
{
    test_dump_state_t *state = *pstate;

    test_dump_write(state->source, state->source_size);

    assert_true(dmi_dump_load(state->context, test_dump_path));

    const dmi_registry_t *registry = dmi_get_registry(state->context);

    assert_int_equal(state->context->state.table_size, state->source_size - DMI_ENTRY_MAX_SIZE);
    assert_int_equal(registry->count, test_source_count);
    assert_false(registry->status & DMI_REGISTRY_STATUS_TRUNCATED);
    assert_true(dmi_close(state->context));
}

static void test_dump_load_not_dump(void **pstate)
{
    test_dump_state_t *state = *pstate;

    static const char text[] = "This is a text file, not an SMBIOS dump.\n";
    test_dump_write((const dmi_data_t *)text, sizeof(text) - 1);

    dmi_error_clear(state->context);
    assert_false(dmi_dump_load(state->context, test_dump_path));
    assert_true(test_dump_has_error(state->context, DMI_ERROR_INVALID_DUMP));
}

static void test_dump_load_too_small(void **pstate)
{
    test_dump_state_t *state = *pstate;

    // Empty file, and entry point without a room for structure header
    const size_t sizes[] = { 0, 5, DMI_ENTRY_MAX_SIZE, DMI_ENTRY_MAX_SIZE + 3 };

    for (size_t i = 0; i < countof(sizes); i++) {
        test_dump_write(state->source, sizes[i]);

        dmi_error_clear(state->context);
        assert_false(dmi_dump_load(state->context, test_dump_path));
        assert_true(test_dump_has_error(state->context, DMI_ERROR_INVALID_DUMP));
    }
}

static void test_dump_load_bad_checksum(void **pstate)
{
    test_dump_state_t *state = *pstate;

    dmi_data_t *data = dmi_alloc(state->context, state->source_size);
    assert_non_null(data);

    memcpy(data, state->source, state->source_size);
    data[0x10] ^= 0xFF;

    test_dump_write(data, state->source_size);
    dmi_free(data);

    dmi_error_clear(state->context);
    assert_false(dmi_dump_load(state->context, test_dump_path));
    assert_true(test_dump_has_error(state->context, DMI_ERROR_INVALID_EPS_CHECKSUM));
}

static void test_dump_load_entry_only(void **pstate)
{
    test_dump_state_t *state = *pstate;

    // Table area contains zero-filled structure header only
    dmi_data_t data[DMI_ENTRY_MAX_SIZE + sizeof(dmi_header_t)] = {};
    memcpy(data, state->source, DMI_ENTRY_MAX_SIZE);

    test_dump_write(data, sizeof(data));

    assert_true(dmi_dump_load(state->context, test_dump_path));
    assert_true(dmi_get_registry(state->context)->status & DMI_REGISTRY_STATUS_TRUNCATED);
    assert_true(dmi_close(state->context));
}

static void test_dump_load_truncated(void **pstate)
{
    test_dump_state_t *state = *pstate;

    // Table is truncated in the middle of a structure at every position
    // within the first structures
    for (size_t size = DMI_ENTRY_MAX_SIZE + sizeof(dmi_header_t); size < DMI_ENTRY_MAX_SIZE + 512; size++) {
        test_dump_write(state->source, size);

        assert_true(dmi_dump_load(state->context, test_dump_path));

        // Table area size from the entry point is not affected by actual
        // table data size
        assert_int_equal(state->context->state.table_area_max_size, state->source_size - DMI_ENTRY_MAX_SIZE);
        assert_int_equal(state->context->state.table_size, size - DMI_ENTRY_MAX_SIZE);

        // Registry is created anew on every load
        const dmi_registry_t *registry = dmi_get_registry(state->context);

        assert_true(registry->status & DMI_REGISTRY_STATUS_TRUNCATED);
        assert_true(registry->count < test_source_count);

        test_dump_decode_all(state->context);
        assert_true(dmi_close(state->context));
    }
}

static void test_dump_load_bad_length(void **pstate)
{
    test_dump_state_t *state = *pstate;

    dmi_data_t *data = dmi_alloc(state->context, state->source_size);
    assert_non_null(data);

    // Structure length exceeding the table area makes the structure truncated
    memcpy(data, state->source, state->source_size);
    data[DMI_ENTRY_MAX_SIZE + 1] = 0xFF;
    test_dump_write(data, DMI_ENTRY_MAX_SIZE + 0x40);

    assert_true(dmi_dump_load(state->context, test_dump_path));
    assert_true(dmi_get_registry(state->context)->status & DMI_REGISTRY_STATUS_TRUNCATED);
    assert_true(dmi_close(state->context));

    // Structure length shorter than its header truncates the table
    memcpy(data, state->source, state->source_size);
    data[DMI_ENTRY_MAX_SIZE + 1] = 0x02;
    test_dump_write(data, state->source_size);

    assert_true(dmi_dump_load(state->context, test_dump_path));

    const dmi_registry_t *registry = dmi_get_registry(state->context);

    assert_true(registry->status & DMI_REGISTRY_STATUS_TRUNCATED);
    assert_int_equal(registry->count, 0);
    assert_true(dmi_close(state->context));

    // The same for structures in the middle of the table
    dmi_entity_t *first = dmi_entity_create(state->context, state->source + DMI_ENTRY_MAX_SIZE,
                                            state->source_size - DMI_ENTRY_MAX_SIZE);
    assert_non_null(first);

    size_t offset = DMI_ENTRY_MAX_SIZE + first->total_length;
    dmi_entity_destroy(first);

    memcpy(data, state->source, state->source_size);
    data[offset + 1] = 0x03;
    test_dump_write(data, state->source_size);

    assert_true(dmi_dump_load(state->context, test_dump_path));

    registry = dmi_get_registry(state->context);

    assert_true(registry->status & DMI_REGISTRY_STATUS_TRUNCATED);
    assert_int_equal(registry->count, 1);
    assert_true(dmi_close(state->context));

    dmi_free(data);
}

static void test_dump_load_bad_length_strict(void **pstate)
{
    test_dump_state_t *state = *pstate;

    dmi_data_t *data = dmi_alloc(state->context, state->source_size);
    assert_non_null(data);

    memcpy(data, state->source, state->source_size);
    data[DMI_ENTRY_MAX_SIZE + 1] = 0x02;
    test_dump_write(data, state->source_size);
    dmi_free(data);

    // Structure length shorter than its header is an error in strict mode
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_STRICT);
    assert_non_null(context);
    dmi_set_logger(context, &test_logger);

    bool loaded = dmi_dump_load(context, test_dump_path);
    bool found  = test_dump_has_error(context, DMI_ERROR_INVALID_ENTITY_LENGTH);

    dmi_destroy(context);

    assert_false(loaded);
    assert_true(found);
}

static void test_dump_load_mutated(void **pstate)
{
    test_dump_state_t *state = *pstate;

    dmi_data_t *data = dmi_alloc(state->context, state->source_size + 16);
    assert_non_null(data);

    // Deterministic pseudo-random generator, so failures are reproducible
    uint32_t seed = 1;
    #define TEST_RANDOM() (seed = seed * 1103515245u + 12345u, (seed >> 16) & 0x7FFFu)

    size_t loaded = 0;

    for (size_t i = 0; i < test_mutation_count; i++) {
        size_t size = state->source_size;
        memcpy(data, state->source, size);

        // Entry point is left intact, so the table is always parsed
        size_t count = 1 + TEST_RANDOM() % 16;
        for (size_t k = 0; k < count; k++) {
            size_t pos = DMI_ENTRY_MAX_SIZE + TEST_RANDOM() % (size - DMI_ENTRY_MAX_SIZE);
            data[pos] = (dmi_data_t)TEST_RANDOM();
        }

        // Truncate some of the dumps
        if (TEST_RANDOM() % 4 == 0)
            size = DMI_ENTRY_MAX_SIZE + sizeof(dmi_header_t) + TEST_RANDOM() % (size - DMI_ENTRY_MAX_SIZE);

        test_dump_write(data, size);

        // Loading may fail, but must not crash
        dmi_error_clear(state->context);
        if (dmi_dump_load(state->context, test_dump_path)) {
            test_dump_decode_all(state->context);
            assert_true(dmi_close(state->context));
            loaded++;
        }
    }

    #undef TEST_RANDOM

    dmi_free(data);

    // Most of the mutated dumps are loaded in relaxed mode
    assert_true(loaded > test_mutation_count / 2);
}

static void test_dump_write(const dmi_data_t *data, size_t size)
{
    FILE *stream = fopen(test_dump_path, "wb");

    if (stream != nullptr) {
        size_t written = fwrite(data, 1, size, stream);
        int rv = fclose(stream);

        assert_int_equal(written, size);
        assert_int_equal(rv, 0);
    } else {
        fail_msg("Unable to create file %s", test_dump_path);
    }
}

static bool test_dump_has_error(dmi_context_t *context, dmi_error_code_t reason)
{
    const dmi_error_t *error;

    while ((error = dmi_error_get_first(context)) != nullptr) {
        if (error->reason == reason)
            return true;
    }

    return false;
}

//
// Access all entities and their strings, and return the number of decoded
// entities.
//
static size_t test_dump_decode_all(dmi_context_t *context)
{
    dmi_registry_iter_t iter;
    const dmi_entity_t *entity;
    size_t decoded = 0;

    dmi_registry_t *registry = dmi_get_registry(context);
    assert_true(dmi_registry_iter_init(&iter, registry, nullptr));

    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        for (size_t i = 1; i <= entity->string_count; i++)
            assert_non_null(dmi_entity_string_ex(entity, i, true));

        if (entity->state & DMI_ENTITY_STATE_DECODED)
            decoded++;
    }

    return decoded;
}
