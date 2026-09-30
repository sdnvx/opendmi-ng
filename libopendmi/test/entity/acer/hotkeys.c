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
#include <opendmi/registry.h>
#include <opendmi/encoder.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>
#include <opendmi/module/acer.h>

#include <opendmi/entity/acer/devices.h>
#include <opendmi/entity/acer/hotkeys.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, dmi_handle_t handle, const dmi_entity_spec_t *spec);

static void test_acer_hotkeys(void **pstate);
static void test_acer_devices(void **pstate);
static void test_acer_hotkeys_basic(void **pstate);

static const char *test_nitro_path = OPENDMI_TEST_DATA "/acer/nitro-an515-31.bin";

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_acer_hotkeys, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_acer_devices, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_acer_hotkeys_basic, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_acer_hotkeys(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Acer laptops carry the firmware of Insyde, and are told by the system
    assert_true(dmi_load(context, test_nitro_path));
    assert_true(dmi_has_extension(context, &dmi_acer_module));

    const dmi_acer_hotkeys_t *info = test_info(context, 0x0014, &dmi_acer_hotkeys_spec);
    assert_int_equal(info->comm_functions, 0x0001);
    assert_int_equal(info->app_functions, 0x0000);
    assert_int_equal(info->media_functions, 0x007F);
    assert_int_equal(info->display_functions, 0x000F);
    assert_int_equal(info->other_functions, 0x0006);

    // Key of the communication function is the one of the first hotkey
    assert_int_equal(info->comm_key, 0x03);
    assert_int_equal(info->hotkey_count, 16);
    assert_int_equal(info->hotkeys[0].function, 0x0001);

    // Functions of the keys of a button group add up to its bitmap
    uint16_t media = 0;
    uint16_t display = 0;

    for (size_t i = 0; i < info->hotkey_count; i++) {
        assert_int_equal(info->hotkeys[i].kind, 2);

        if ((info->hotkeys[i].key & 0xE0) == 0x40)
            media |= info->hotkeys[i].function;
        if ((info->hotkeys[i].key & 0xE0) == 0x60)
            display |= info->hotkeys[i].function;
    }

    assert_int_equal(media, info->media_functions);
    assert_int_equal(display, info->display_functions);
}

static dmi_log_t test_logger = { dmi_test_log_handler };

static void test_acer_devices(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_nitro_path));

    // Webcam of Realtek, graphics of Intel and NVIDIA, and the wireless
    // network adapter of Intel, the absent devices having no IDs
    const dmi_acer_devices_t *info = test_info(context, 0x0015, &dmi_acer_devices_spec);
    assert_int_equal(info->device_count, 10);
    assert_int_equal(info->devices[0].kind, DMI_ACER_DEVICE_KIND_CAMERA);
    assert_string_equal(dmi_acer_device_kind_name(info->devices[0].kind), "Webcam");
    assert_int_equal(info->devices[0].vendor_id, 0x0BDA);
    assert_int_equal(info->devices[0].device_id, 0x5621);
    assert_int_equal(info->devices[1].vendor_id, 0x0000);
    assert_int_equal(info->devices[2].vendor_id, 0x8086);
    assert_int_equal(info->devices[3].vendor_id, 0x10DE);
    assert_int_equal(info->devices[3].device_id, 0x1D10);
    assert_int_equal(info->devices[6].kind, DMI_ACER_DEVICE_KIND_WLAN);
    assert_int_equal(info->devices[6].device_id, 0x2725);

    // Kinds whose meaning is not established are named by their values
    assert_int_equal(info->devices[2].kind, DMI_ACER_DEVICE_KIND_UNKNOWN_1);
    assert_string_equal(dmi_acer_device_kind_name(info->devices[2].kind), "Unknown 1");
    assert_string_equal(dmi_acer_device_kind_name(DMI_ACER_DEVICE_KIND_UNKNOWN_25), "Unknown 25");
    assert_null(dmi_acer_device_kind_name((dmi_acer_device_kind_t)0x06));
}

//
// Structures of 0x0F bytes end with the number of the key of the
// communication function, the way the Acer WMI driver reads them, and hold no
// hotkeys
//
static void test_acer_hotkeys_basic(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_add_extension(context, &dmi_acer_module));

    static const uint8_t data[] = {
        170, 0x0F, 0x14, 0x00,
        0x01, 0x08, 0x00, 0x00, 0x7F, 0x00, 0x0F, 0x00, 0x06, 0x00,
        0xFF,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_ptr_equal(entity->spec, &dmi_acer_hotkeys_basic_spec);
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_acer_hotkeys_t *info = dmi_entity_info(entity, DMI_TYPE(acer_hotkeys));
    assert_non_null(info);
    assert_int_equal(info->comm_functions, 0x0801);
    assert_int_equal(info->other_functions, 0x0006);
    assert_int_equal(info->comm_key, 0xFF);
    assert_int_equal(info->hotkey_count, 0);

    // Bits of the communication functions are named the way the Acer WMI
    // driver of Linux names them
    const dmi_attribute_t *attr = entity->spec->attributes;
    while ((attr->params.code != nullptr) and (strcmp(attr->params.code, "comm-functions") != 0))
        attr++;
    assert_non_null(attr->params.values);
    assert_string_equal(dmi_code_lookup(attr->params.values, DMI_ACER_COMM_FUNCTION_BLUETOOTH), "bluetooth");
    assert_string_equal(dmi_code_lookup(attr->params.values, DMI_ACER_COMM_FUNCTION_RF_BUTTON), "rf-button");

    // Structure is written back as it is
    dmi_buffer_t *output = dmi_buffer_create(context);
    dmi_encoder_t encoder;

    assert_true(dmi_encoder_initialize(&encoder, output, entity, DMI_ENCODE_MODE_CANONICAL, DMI_VERSION_NONE));
    assert_true(dmi_entity_encode(&encoder));
    assert_int_equal(output->length, entity->body_length);
    assert_memory_equal(output->data, data, output->length);
    dmi_encoder_finalize(&encoder);

    dmi_buffer_destroy(output);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
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
