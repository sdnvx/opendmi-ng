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
#include <ctype.h>
#include <cmocka.h>

#include <opendmi/anonymize.h>
#include <opendmi/buffer.h>
#include <opendmi/context.h>
#include <opendmi/error.h>
#include <opendmi/registry.h>
#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/chassis.h>
#include <opendmi/entity/system.h>
#include <opendmi/entity/hpe/nic.h>
#include <opendmi/entity/hpe/physical-attrs.h>
#include <opendmi/module.h>
#include <opendmi/module/dell.h>
#include <opendmi/module/hpe.h>

static void test_anonymize_serials(void **pstate);
static void test_anonymize_uuid(void **pstate);
static void test_anonymize_placeholders(void **pstate);
static void test_anonymize_proliant(void **pstate);
static void test_anonymize_random_key(void **pstate);
static void test_anonymize_overlay(void **pstate);
static void test_anonymize_context(void **pstate);

static dmi_context_t *test_open(const char *path, dmi_context_flags_t flags);
static dmi_context_t *test_anonymized(dmi_context_t *context);
static const void *test_info(dmi_context_t *context, const dmi_type_t *type);
static bool test_contains(const dmi_buffer_t *buffer, const void *data, size_t length);
static void test_same_format(const char *original, const char *replaced);

static const char *test_dell_path     = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_asrock_path   = OPENDMI_TEST_DATA "/asrock/b460-pro4.bin";
static const char *test_proliant_path = OPENDMI_TEST_DATA "/hp/proliant-dl360-g6-484184-b21.bin";

// Relative to test working directory
static const char *test_save_path = "anonymize-test-dump.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_anonymize_serials),
        cmocka_unit_test(test_anonymize_uuid),
        cmocka_unit_test(test_anonymize_placeholders),
        cmocka_unit_test(test_anonymize_proliant),
        cmocka_unit_test(test_anonymize_random_key),
        cmocka_unit_test(test_anonymize_overlay),
        cmocka_unit_test(test_anonymize_context)
    };

    int rv = cmocka_run_group_tests(tests, nullptr, nullptr);
    remove(test_save_path);

    return rv;
}

static void test_anonymize_serials(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context = test_open(test_dell_path, DMI_CONTEXT_FLAG_AUTO_MODULES);

    dmi_buffer_t *table = dmi_buffer_create(context);
    assert_true(dmi_anonymize(context, table));

    // Copy has the layout of the table, and holds the service tag nowhere
    assert_int_equal(table->length, context->state.table->length);
    assert_false(test_contains(table, "FY59ZK3", 7));

    // Context itself is left as it is
    const dmi_system_t *system = test_info(context, DMI_TYPE(system));
    assert_string_equal(system->serial_number, "FY59ZK3");

    dmi_buffer_destroy(table);

    // Service tag of the system and of the chassis is replaced the same way
    dmi_context_t *anonymized = test_anonymized(context);

    const dmi_system_t *replaced = test_info(anonymized, DMI_TYPE(system));
    test_same_format("FY59ZK3", replaced->serial_number);

    const dmi_chassis_t *chassis = test_info(anonymized, DMI_TYPE(chassis));
    assert_string_equal(chassis->serial_number, replaced->serial_number);

    // Other values are kept
    assert_string_equal(replaced->product, system->product);

    dmi_destroy(anonymized);
    dmi_destroy(context);
}

static void test_anonymize_uuid(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context    = test_open(test_dell_path, DMI_CONTEXT_FLAG_AUTO_MODULES);
    dmi_context_t *anonymized = test_anonymized(context);

    const dmi_system_t *original = test_info(context, DMI_TYPE(system));
    const dmi_system_t *replaced = test_info(anonymized, DMI_TYPE(system));

    // Version and variant tell how the UUID has been made, and are kept
    assert_memory_not_equal(&original->uuid, &replaced->uuid, sizeof(dmi_uuid_t));
    assert_int_equal(original->uuid.time_hi_and_version & 0xF000,
                     replaced->uuid.time_hi_and_version & 0xF000);
    assert_int_equal(original->uuid.clock_seq_hi_and_reserved & 0xC0,
                     replaced->uuid.clock_seq_hi_and_reserved & 0xC0);

    dmi_destroy(anonymized);
    dmi_destroy(context);
}

static void test_anonymize_placeholders(void **pstate)
{
    dmi_unused(pstate);

    // Placeholders of the firmware vendor identify nothing
    dmi_context_t *context    = test_open(test_asrock_path, DMI_CONTEXT_FLAG_AUTO_MODULES);
    dmi_context_t *anonymized = test_anonymized(context);

    const dmi_system_t *system = test_info(anonymized, DMI_TYPE(system));
    assert_string_equal(system->serial_number, "To Be Filled By O.E.M.");

    const dmi_chassis_t *chassis = test_info(anonymized, DMI_TYPE(chassis));
    assert_string_equal(chassis->asset_tag, "To Be Filled By O.E.M.");

    dmi_destroy(anonymized);
    dmi_destroy(context);
}

static void test_anonymize_proliant(void **pstate)
{
    dmi_unused(pstate);

    dmi_context_t *context    = test_open(test_proliant_path, DMI_CONTEXT_FLAG_AUTO_MODULES);
    dmi_context_t *anonymized = test_anonymized(context);

    // Serial number kept in place by the physical attributes of G7 and older
    // servers is found and replaced the way the one of the system is
    const dmi_system_t *system = test_info(anonymized, DMI_TYPE(system));
    test_same_format("GB894484YN", system->serial_number);

    const dmi_hpe_physical_attrs_t *attrs = test_info(anonymized, DMI_TYPE(hpe_physical_attrs));
    assert_non_null(attrs->identifier);
    assert_string_equal(attrs->identifier + 6, system->serial_number);
    assert_string_equal(attrs->serial_number, system->serial_number);

    // MAC addresses keep the part telling the manufacturer
    const dmi_hpe_nic_info_t *original = test_info(context, DMI_TYPE(hpe_pxe_nic));
    const dmi_hpe_nic_info_t *replaced = test_info(anonymized, DMI_TYPE(hpe_pxe_nic));

    assert_int_equal(replaced->port_count, original->port_count);
    assert_memory_equal(replaced->ports[0].mac_address.data, original->ports[0].mac_address.data, 3);
    assert_memory_not_equal(replaced->ports[0].mac_address.data, original->ports[0].mac_address.data, 6);

    dmi_destroy(anonymized);
    dmi_destroy(context);
}

static void test_anonymize_random_key(void **pstate)
{
    dmi_unused(pstate);

    // Replacements differ each time, since the key is never saved
    dmi_context_t *context = test_open(test_dell_path, DMI_CONTEXT_FLAG_AUTO_MODULES);

    dmi_buffer_t *first  = dmi_buffer_create(context);
    dmi_buffer_t *second = dmi_buffer_create(context);

    assert_true(dmi_anonymize(context, first));
    assert_true(dmi_anonymize(context, second));
    assert_int_equal(first->length, second->length);
    assert_memory_not_equal(first->data, second->data, first->length);

    dmi_buffer_destroy(second);
    dmi_buffer_destroy(first);
    dmi_destroy(context);
}

static void test_anonymize_overlay(void **pstate)
{
    dmi_unused(pstate);

    // Structures carrying additional information are decoded from copies of
    // their own, which the table does not hold
    dmi_context_t *context = test_open(test_dell_path, DMI_CONTEXT_FLAG_OVERLAY);

    dmi_buffer_t *table = dmi_buffer_create(context);
    assert_false(dmi_anonymize(context, table));

    const dmi_error_t *error = dmi_error_peek_last(context);
    assert_non_null(error);
    assert_int_equal(error->reason, DMI_ERROR_INVALID_STATE);

    assert_false(dmi_save(context, test_save_path, DMI_SAVE_FLAG_OVERWRITE | DMI_SAVE_FLAG_ANONYMIZE));

    dmi_buffer_destroy(table);
    dmi_destroy(context);
}

static void test_anonymize_context(void **pstate)
{
    dmi_unused(pstate);

    // Structures are read anew from the anonymized table, in place
    dmi_context_t *context = test_open(test_dell_path, DMI_CONTEXT_FLAG_AUTO_MODULES | DMI_CONTEXT_FLAG_LINK);

    const dmi_system_t *system = test_info(context, DMI_TYPE(system));
    char *product = strdup(system->product);
    assert_non_null(product);

    assert_true(dmi_anonymize_context(context));

    system = test_info(context, DMI_TYPE(system));
    test_same_format("FY59ZK3", system->serial_number);
    assert_string_equal(system->product, product);

    const dmi_chassis_t *chassis = test_info(context, DMI_TYPE(chassis));
    assert_string_equal(chassis->serial_number, system->serial_number);

    // Modules told from the table stay enabled
    assert_true(dmi_has_extension(context, dmi_module_find("dell")));
    assert_non_null(dmi_registry_lookup_first(dmi_get_registry(context), DMI_TYPE(dell_revisions), false));

    free(product);
    dmi_destroy(context);

    // Context whose structures carry additional information is left as it is
    context = test_open(test_dell_path, DMI_CONTEXT_FLAG_OVERLAY);

    assert_false(dmi_anonymize_context(context));

    system = test_info(context, DMI_TYPE(system));
    assert_string_equal(system->serial_number, "FY59ZK3");

    dmi_destroy(context);
}

static dmi_context_t *test_open(const char *path, dmi_context_flags_t flags)
{
    dmi_context_t *context = dmi_create(flags);
    assert_non_null(context);

    dmi_set_logger(context, &test_logger);
    assert_true(dmi_load(context, path));

    return context;
}

static dmi_context_t *test_anonymized(dmi_context_t *context)
{
    assert_true(dmi_save(context, test_save_path, DMI_SAVE_FLAG_OVERWRITE | DMI_SAVE_FLAG_ANONYMIZE));

    return test_open(test_save_path, DMI_CONTEXT_FLAG_AUTO_MODULES);
}

static const void *test_info(dmi_context_t *context, const dmi_type_t *type)
{
    dmi_entity_t *entity = dmi_registry_lookup_first(dmi_get_registry(context), type, false);
    assert_non_null(entity);

    const void *info = dmi_entity_info(entity, type);
    assert_non_null(info);

    return info;
}

static bool test_contains(const dmi_buffer_t *buffer, const void *data, size_t length)
{
    for (size_t i = 0; i + length <= buffer->length; i++) {
        if (memcmp(buffer->data + i, data, length) == 0)
            return true;
    }

    return false;
}

static void test_same_format(const char *original, const char *replaced)
{
    assert_non_null(replaced);
    assert_int_equal(strlen(replaced), strlen(original));
    assert_string_not_equal(replaced, original);

    // Letters and digits are replaced with ones of their own kind
    for (size_t i = 0; original[i] != 0; i++) {
        unsigned char o = (unsigned char)original[i];
        unsigned char r = (unsigned char)replaced[i];

        assert_int_equal(isdigit(o) != 0, isdigit(r) != 0);
        assert_int_equal(isupper(o) != 0, isupper(r) != 0);
        assert_int_equal(islower(o) != 0, islower(r) != 0);
    }
}
