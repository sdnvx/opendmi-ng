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
#include <opendmi/registry.h>
#include <opendmi/module/apple.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/memory-device.h>
#include <opendmi/entity/apple/firmware-volume.h>
#include <opendmi/entity/apple/memory-spd-data.h>
#include <opendmi/entity/apple/platform-feature.h>
#include <opendmi/entity/apple/processor-bus-speed.h>
#include <opendmi/entity/apple/processor-type.h>
#include <opendmi/entity/apple/smc-version.h>

static int test_setup(void **pstate);
static int test_teardown(void **pstate);
static const void *test_info(dmi_context_t *context, const dmi_type_t *type);
static dmi_entity_t *test_decode(dmi_context_t *context, const dmi_byte_t *data, size_t size,
                                 dmi_buffer_t **pbuffer);

static void test_apple_imac(void **pstate);
static void test_apple_extended_features(void **pstate);
static void test_apple_platform_feature(void **pstate);
static void test_apple_smc_version(void **pstate);

static const char *test_imac_path = OPENDMI_TEST_DATA "/apple/imac11-3.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_apple_imac, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_apple_extended_features, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_apple_platform_feature, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_apple_smc_version, test_setup, test_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_apple_imac(void **pstate)
{
    dmi_context_t *context = *pstate;

    assert_true(dmi_load(context, test_imac_path));
    assert_true(dmi_has_extension(context, &dmi_apple_module));

    // Firmware of 2010, which tells no extended features
    const dmi_apple_firmware_volume_t *volume = test_info(context, DMI_TYPE(apple_firmware_volume));
    assert_int_equal(volume->region_count, 3);
    assert_int_equal(volume->features, 0xC00C9423);
    assert_int_equal(volume->features_mask, 0xFF1FFF3F);
    assert_int_equal(volume->regions[0].type, DMI_APPLE_REGION_TYPE_RECOVERY);
    assert_int_equal(volume->regions[1].type, DMI_APPLE_REGION_TYPE_MAIN);
    assert_int_equal(volume->regions[1].start_address, 0xFFC21000);
    assert_int_equal(volume->regions[1].end_address, 0xFFEB1FFF);
    assert_int_equal(volume->regions[2].type, DMI_APPLE_REGION_TYPE_NVRAM);
    assert_int_equal(volume->regions[3].type, DMI_APPLE_REGION_TYPE_UNUSED);
    assert_int_equal(volume->extended_features, 0);

    // SPD data of the memory modules, which the processor type and the bus
    // speed of the processor follow; the Intel structures of the same types
    // are told apart by their signatures
    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(apple_memory_spd_data), false);
    assert_non_null(entity);
    assert_ptr_equal(entity->spec, &dmi_apple_memory_spd_data_spec);

    const dmi_apple_memory_spd_data_t *spd = dmi_entity_info(entity, DMI_TYPE(apple_memory_spd_data));
    assert_non_null(spd);
    assert_int_equal(spd->offset, 0);
    assert_int_equal(spd->size, 176);
    assert_int_equal(spd->data.length, 176);
    assert_int_equal(spd->data.data[2], 0x0B);

    entity = dmi_registry_lookup(dmi_get_registry(context), spd->handle, DMI_TYPE(memory_device), false);
    assert_non_null(entity);

    const dmi_apple_processor_type_t *type = test_info(context, DMI_TYPE(apple_processor_type));
    assert_int_equal(type->processor_class, DMI_APPLE_PROCESSOR_CLASS_CORE_I7);
    assert_int_equal(type->generation, 1);

    const dmi_apple_processor_bus_speed_t *bus = test_info(context, DMI_TYPE(apple_processor_bus_speed));
    assert_int_equal(bus->speed, 4800);
}

static void test_apple_extended_features(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Newer firmware follows the regions with the extended features
    dmi_byte_t data[0x60 + 2] = {
        DMI_TYPE_ID(APPLE_FIRMWARE_VOLUME), 0x60, 0x00, 0x10,
        0x01                                                    // Number of regions
    };
    data[0x08] = 0x00;                                          // Features: extended ones
    data[0x0B] = 0x02;
    data[0x10] = DMI_APPLE_REGION_TYPE_MAIN;
    data[0x58] = 0x08;                                          // Extended features
    data[0x5C] = 0x08;                                          // Extended features mask

    dmi_buffer_t *buffer = nullptr;
    dmi_entity_t *entity = test_decode(context, data, sizeof(data), &buffer);

    const dmi_apple_firmware_volume_t *info = dmi_entity_info(entity, DMI_TYPE(apple_firmware_volume));
    assert_non_null(info);
    assert_int_equal(info->features, 1u << DMI_APPLE_FIRMWARE_FEATURE_EXTENDED_FEATURES);
    assert_int_equal(info->regions[0].type, DMI_APPLE_REGION_TYPE_MAIN);
    assert_int_equal(info->extended_features, 1u << DMI_APPLE_EXTENDED_FEATURE_LARGE_BASESYSTEM);
    assert_int_equal(info->extended_features_mask, 0x08);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_apple_platform_feature(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_byte_t data[] = {
        DMI_TYPE_ID(APPLE_PLATFORM_FEATURE), 0x0C, 0x00, 0x10,
        0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        // Platform features
        0x00, 0x00
    };

    dmi_buffer_t *buffer = nullptr;
    dmi_entity_t *entity = test_decode(context, data, sizeof(data), &buffer);

    const dmi_apple_platform_feature_t *info = dmi_entity_info(entity, DMI_TYPE(apple_platform_feature));
    assert_non_null(info);
    assert_int_equal(info->features, (1u << DMI_APPLE_PLATFORM_FEATURE_SOLDERED_MEMORY) |
                                     (1u << DMI_APPLE_PLATFORM_FEATURE_HOST_PM) |
                                     (1u << DMI_APPLE_PLATFORM_FEATURE_POWER_CHIME));

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_apple_smc_version(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Text is followed by bytes of zero
    const dmi_byte_t data[] = {
        DMI_TYPE_ID(APPLE_SMC_VERSION), 0x14, 0x00, 0x10,
        '2', '.', '1', '5', 'f', '7', 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = nullptr;
    dmi_entity_t *entity = test_decode(context, data, sizeof(data), &buffer);

    const dmi_apple_smc_version_t *info = dmi_entity_info(entity, DMI_TYPE(apple_smc_version));
    assert_non_null(info);
    assert_string_equal(info->version, "2.15f7");
    assert_int_equal(info->version_raw.length, DMI_APPLE_SMC_VERSION_SIZE);

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

static const void *test_info(dmi_context_t *context, const dmi_type_t *type)
{
    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), type, false);
    assert_non_null(entity);

    const void *info = dmi_entity_info(entity, type);
    assert_non_null(info);

    return info;
}

static dmi_entity_t *test_decode(dmi_context_t *context, const dmi_byte_t *data, size_t size,
                                 dmi_buffer_t **pbuffer)
{
    // Structures are decoded by the specifications of the module, which is
    // enabled explicitly, since no data tells the platform
    if (not dmi_has_extension(context, &dmi_apple_module))
        assert_true(dmi_add_extension(context, &dmi_apple_module));

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    assert_non_null(buffer);

    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    *pbuffer = buffer;
    return entity;
}
