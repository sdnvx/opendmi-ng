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
#include <opendmi/module/intel.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/intel/rsd-processor-cpuid.h>

static int test_rsd_processor_cpuid_setup(void **pstate);
static int test_rsd_processor_cpuid_teardown(void **pstate);

static void test_rsd_processor_cpuid_decode(void **pstate);
static void test_rsd_processor_cpuid_decode_extended(void **pstate);
static void test_rsd_processor_cpuid_decode_truncated(void **pstate);

static dmi_log_t test_logger = { DMI_LOG_DEBUG, dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_rsd_processor_cpuid_decode, test_rsd_processor_cpuid_setup, test_rsd_processor_cpuid_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_processor_cpuid_decode_extended, test_rsd_processor_cpuid_setup, test_rsd_processor_cpuid_teardown),
        cmocka_unit_test_setup_teardown(test_rsd_processor_cpuid_decode_truncated, test_rsd_processor_cpuid_setup, test_rsd_processor_cpuid_teardown)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_rsd_processor_cpuid_setup(void **pstate)
{
    dmi_context_t *context;

    context = dmi_create(0);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    if (not dmi_add_extension(context, dmi_module_find("intel"))) {
        dmi_destroy(context);
        return -1;
    }

    *pstate = context;

    return 0;
}

static int test_rsd_processor_cpuid_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

// Build processor CPUID structure with leaf registers filled with the leaf
// index and register number, and a socket designation string
static size_t create_cpuid(uint8_t *data, size_t size, uint8_t subtype, size_t leaf_count)
{
    static const char socket[] = "Socket 0";

    size_t length = 6 + leaf_count * 16;
    assert_true(length + sizeof(socket) + 1 <= size);

    memset(data, 0, size);
    data[0] = 193;
    data[1] = (uint8_t)length;
    data[2] = 0x00;
    data[3] = 0xC1;
    data[4] = 1;
    data[5] = subtype;

    for (size_t i = 0; i < leaf_count; i++) {
        for (size_t reg = 0; reg < 4; reg++)
            data[6 + i * 16 + reg * 4] = (uint8_t)((i << 4) | reg);
    }

    memcpy(data + length, socket, sizeof(socket));

    return length + sizeof(socket) + 1;
}

static void test_rsd_processor_cpuid_decode(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[256];
    size_t size = create_cpuid(data, sizeof(data), 1, 14);

    dmi_entity_t *entity = dmi_entity_create(context, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_false(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_intel_rsd_processor_cpuid_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    assert_non_null(info);

    assert_string_equal(info->socket_designation, "Socket 0");
    assert_int_equal(info->subtype, DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC);
    assert_false(info->is_raw);
    assert_int_equal(info->leaf_count, 14);

    // Registers are stored in EAX, EBX, ECX, EDX order
    assert_int_equal(info->leaves[1].leaf, 0x01);
    assert_false(info->leaves[1].has_subleaf);
    assert_int_equal(info->leaves[1].eax, 0x10);
    assert_int_equal(info->leaves[1].ebx, 0x11);
    assert_int_equal(info->leaves[1].ecx, 0x12);
    assert_int_equal(info->leaves[1].edx, 0x13);

    // Leaf 08h is not stored
    assert_int_equal(info->leaves[8].leaf, 0x09);
    assert_int_equal(info->leaves[11].leaf, 0x0D);
    assert_true(info->leaves[11].has_subleaf);
    assert_int_equal(info->leaves[11].subleaf, 0x00);
    assert_int_equal(info->leaves[13].leaf, 0x10);

    dmi_entity_destroy(entity);
}

static void test_rsd_processor_cpuid_decode_extended(void **pstate)
{
    dmi_context_t *context = *pstate;

    uint8_t data[512];
    size_t size = create_cpuid(data, sizeof(data), 2, 15);

    dmi_entity_t *entity = dmi_entity_create(context, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_intel_rsd_processor_cpuid_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    assert_non_null(info);

    assert_int_equal(info->subtype, DMI_INTEL_RSD_CPUID_SUBTYPE_EXTENDED);
    assert_int_equal(info->leaf_count, 15);
    assert_int_equal(info->leaves[4].leaf, 0x17);
    assert_int_equal(info->leaves[4].subleaf, 0x01);
    assert_int_equal(info->leaves[14].leaf, 0x80000008);
    assert_int_equal(info->leaves[14].edx, 0xE3);

    dmi_entity_destroy(entity);
}

static void test_rsd_processor_cpuid_decode_truncated(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Only three leaves are present
    uint8_t data[256];
    size_t size = create_cpuid(data, sizeof(data), 1, 3);

    dmi_entity_t *entity = dmi_entity_create(context, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));
    assert_true(entity->state & DMI_ENTITY_STATE_INCOMPLETE);

    const dmi_intel_rsd_processor_cpuid_t *info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    assert_non_null(info);
    assert_int_equal(info->leaf_count, 3);

    dmi_entity_destroy(entity);

    // Data of unknown subtype is kept as stored
    size = create_cpuid(data, sizeof(data), 3, 2);

    entity = dmi_entity_create(context, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    assert_non_null(info);
    assert_true(info->is_raw);
    assert_int_equal(info->leaf_count, 0);
    assert_int_equal(info->data.length, 32);

    dmi_entity_destroy(entity);
}
