//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/pointing-device.h>

typedef struct test_state test_state_t;
typedef struct test_pointing_device test_pointing_device_t;

struct test_state
{
    dmi_context_t *context;
    dmi_entity_t  *entity;
};

dmi_packed_struct(test_pointing_device)
{
    /**
     * @brief Structure header.
     */
    dmi_header_t header;

    /**
     * @brief Type of pointing device.
     */
    dmi_byte_t type;

    /**
     * @brief Interface type for the pointing device.
     */
    dmi_byte_t interface;

    /**
     * @brief Number of buttons on the pointing device. If the device has
     * three buttons, the field value is 0x03.
     */
    dmi_byte_t button_count;
};

static void test_pointing_device_type_name(void **pstate);
static void test_pointing_device_iface_name(void **pstate);

static int test_pointing_device_initialize(void **pstate);
static int test_pointing_device_cleanup(void **pstate);
static int test_pointing_device_finalize(void **pstate);

static void test_pointing_device_decode_min_length(void **pstate);
static void test_pointing_device_decode_empty(void **pstate);

static void test_pointing_device_decode_type(void **pstate);
static void test_pointing_device_decode_interface(void **pstate);
static void test_pointing_device_decode_button_count(void **pstate);

static dmi_header_t test_pointing_device_header(ssize_t extra_length);

static dmi_log_t test_logger = { &dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pointing_device_type_name),
        cmocka_unit_test(test_pointing_device_iface_name),

        cmocka_unit_test_teardown(
                test_pointing_device_decode_min_length,
                test_pointing_device_cleanup),
        cmocka_unit_test_teardown(
                test_pointing_device_decode_empty,
                test_pointing_device_cleanup),
        cmocka_unit_test_teardown(
                test_pointing_device_decode_type,
                test_pointing_device_cleanup),
        cmocka_unit_test_teardown(
                test_pointing_device_decode_interface,
                test_pointing_device_cleanup),
        cmocka_unit_test_teardown(
                test_pointing_device_decode_button_count,
                test_pointing_device_cleanup)
    };

    return cmocka_run_group_tests(
            tests,
            test_pointing_device_initialize,
            test_pointing_device_finalize);
}

static void test_pointing_device_type_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_pointing_device_type_name(__DMI_POINTING_DEVICE_TYPE_COUNT));

    for (int i = 0; i < __DMI_POINTING_DEVICE_TYPE_COUNT; i++) {
        assert_non_null(dmi_pointing_device_type_name(i));
    }
}

static void test_pointing_device_iface_name(void **pstate)
{
    dmi_unused(pstate);

    assert_null(dmi_pointing_device_iface_name(__DMI_POINTING_DEVICE_IFACE_COUNT));

    for (int i = 0; i < __DMI_POINTING_DEVICE_IFACE_COUNT; i++) {
        if ((i >= __DMI_POINTING_DEVICE_IFACE_UNASSIGNED_START) and
            (i <= __DMI_POINTING_DEVICE_IFACE_UNASSIGNED_END))
            assert_null(dmi_pointing_device_iface_name(i));
        else
            assert_non_null(dmi_pointing_device_iface_name(i));
    }
}

static int test_pointing_device_initialize(void **pstate)
{
    test_state_t *state = nullptr;

    state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    state->context = dmi_create(DMI_CONTEXT_FLAG_RELAXED);
    if (state->context == nullptr) {
        free(state);
        return -1;
    }

    dmi_set_logger(state->context, &test_logger);

    *pstate = state;
    return 0;
}

static int test_pointing_device_cleanup(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_entity_destroy(state->entity);
    state->entity = nullptr;

    return 0;
}

static int test_pointing_device_finalize(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_destroy(state->context);
    free(state);

    *pstate = nullptr;
    return 0;
}

static void test_pointing_device_decode_min_length(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_packed_struct() {
        test_pointing_device_t data;
        dmi_word_t terminator;
    } envelope = {
        .data = {
            .header = test_pointing_device_header(-1)
        },
        .terminator = 0
    };

    state->entity = dmi_entity_create(state->context, &envelope, sizeof(envelope));

    assert_non_null(state->entity);
    assert_false(dmi_entity_decode(state->entity));

    const dmi_error_t *error = dmi_error_get_last(state->context);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_ENTITY_LENGTH);
}

static void test_pointing_device_decode_empty(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    dmi_packed_struct() {
        test_pointing_device_t data;
        dmi_word_t terminator;
    } envelope = {
        .data = {
            .header = test_pointing_device_header(0)
        },
        .terminator = 0
    };

    state->entity = dmi_entity_create(state->context, &envelope, sizeof(envelope));

    assert_non_null(state->entity);
    assert_true(dmi_entity_decode(state->entity));

    assert_int_equal(state->entity->type, DMI_TYPE(POINTING_DEVICE));
    assert_non_null(state->entity->spec);
    assert_int_equal(state->entity->spec->type, DMI_TYPE(POINTING_DEVICE));
    assert_uint_equal(state->entity->handle, DMI_HANDLE_TEST);
    assert_uint_equal(state->entity->total_length, sizeof(test_pointing_device_t) + sizeof(dmi_word_t));
    assert_uint_equal(state->entity->body_length, sizeof(test_pointing_device_t));
    assert_uint_equal(state->entity->extra_length, sizeof(dmi_word_t));
    assert_uint_equal(state->entity->string_count, 0);
    assert_uint_equal(state->entity->level, DMI_VERSION(2, 1, 0));

    const dmi_pointing_device_t *info = dmi_entity_info(state->entity, DMI_TYPE(POINTING_DEVICE));

    assert_non_null(info);
    assert_int_equal(info->type, DMI_POINTING_DEVICE_TYPE_UNSPEC);
    assert_int_equal(info->interface, DMI_POINTING_DEVICE_IFACE_UNSPEC);
    assert_uint_equal(info->button_count, 0);
}

static void test_pointing_device_decode_type(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    for (int type = 0; type < __DMI_POINTING_DEVICE_TYPE_COUNT; type++) {
        dmi_packed_struct() {
            test_pointing_device_t data;
            dmi_word_t terminator;
        } envelope = {
            .data = {
                .header = test_pointing_device_header(0),
                .type   = dmi_encode((dmi_byte_t)type),
            },
            .terminator = 0
        };

        state->entity = dmi_entity_create(state->context, &envelope, sizeof(envelope));

        assert_non_null(state->entity);
        assert_true(dmi_entity_decode(state->entity));

        const dmi_pointing_device_t *info = dmi_entity_info(state->entity, DMI_TYPE(POINTING_DEVICE));

        assert_non_null(info);
        assert_int_equal(info->type, type);

        dmi_entity_destroy(state->entity);
        state->entity = nullptr;
    }
}

static void test_pointing_device_decode_interface(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    for (int iface = 0; iface < __DMI_POINTING_DEVICE_IFACE_COUNT; iface++) {
        if ((iface >= __DMI_POINTING_DEVICE_IFACE_UNASSIGNED_START) and
            (iface <= __DMI_POINTING_DEVICE_IFACE_UNASSIGNED_END))
            continue;

        dmi_packed_struct() {
            test_pointing_device_t data;
            dmi_word_t terminator;
        } envelope = {
            .data = {
                .header    = test_pointing_device_header(0),
                .interface = dmi_encode((dmi_byte_t)iface),
            },
            .terminator = 0
        };

        state->entity = dmi_entity_create(state->context, &envelope, sizeof(envelope));

        assert_non_null(state->entity);
        assert_true(dmi_entity_decode(state->entity));

        const dmi_pointing_device_t *info = dmi_entity_info(state->entity, DMI_TYPE(POINTING_DEVICE));

        assert_non_null(info);
        assert_int_equal(info->interface, iface);

        dmi_entity_destroy(state->entity);
        state->entity = nullptr;
    }
}

static void test_pointing_device_decode_button_count(void **pstate)
{
    test_state_t *state = dmi_cast(state, *pstate);

    for (int buttons = 0; buttons < 16; buttons++) {
        dmi_packed_struct() {
            test_pointing_device_t data;
            dmi_word_t terminator;
        } envelope = {
            .data = {
                .header       = test_pointing_device_header(0),
                .button_count = dmi_encode((dmi_byte_t)buttons)
            },
            .terminator = 0
        };

        state->entity = dmi_entity_create(state->context, &envelope, sizeof(envelope));

        assert_non_null(state->entity);
        assert_true(dmi_entity_decode(state->entity));

        const dmi_pointing_device_t *info = dmi_entity_info(state->entity, DMI_TYPE(POINTING_DEVICE));

        assert_non_null(info);
        assert_uint_equal(info->button_count, buttons);

        dmi_entity_destroy(state->entity);
        state->entity = nullptr;
    }
}

static dmi_header_t test_pointing_device_header(ssize_t extra_length)
{
    assert_int_in_range(
            extra_length,
            -sizeof(dmi_header_t),
            UINT8_MAX - sizeof(test_pointing_device_t));

    dmi_header_t header = {
        .type   = dmi_encode((dmi_byte_t)DMI_TYPE(POINTING_DEVICE)),
        .length = sizeof(test_pointing_device_t) + extra_length,
        .handle = dmi_encode(DMI_HANDLE_TEST)
    };

    return header;
}
