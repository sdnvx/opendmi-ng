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
#include <opendmi/field.h>
#include <opendmi/filter.h>
#include <opendmi/platform.h>
#include <opendmi/internal.h>
#include <opendmi/module.h>
#include <opendmi/module/dell.h>
#include <opendmi/module/hpe.h>
#include <opendmi/module/intel.h>
#include <opendmi/test/entity.h>
#include <opendmi/test/logger.h>

#include <opendmi/entity/dell/revisions.h>

// Structure types of the test modules: one of two layouts, one a module of
// any platform decodes at the same type number, and one of the structures
// told by their signatures
static const dmi_type_t test_type        = { .id = (dmi_type_id_t)250 };
static const dmi_type_t test_shared_type = { .id = (dmi_type_id_t)250 };
static const dmi_type_t test_signed_type = { .id = (dmi_type_id_t)245 };

static int test_platform_setup(void **pstate);
static int test_platform_teardown(void **pstate);

static void test_platform_object(void **pstate);
static void test_platform_match(void **pstate);
static void test_platform_generations(void **pstate);
static void test_platform_hpe_detect(void **pstate);
static void test_platform_detect(void **pstate);
static void test_platform_auto_modules(void **pstate);
static void test_platform_auto_modules_precedence(void **pstate);
static void test_platform_any_vendor(void **pstate);
static void test_platform_spec_generations(void **pstate);
static void test_platform_relocation(void **pstate);
static void test_platform_suppression(void **pstate);
static void test_platform_yield(void **pstate);
static void test_platform_set(void **pstate);
static void test_platform_filter_module(void **pstate);
static void test_platform_signature(void **pstate);
static void test_platform_signature_slots(void **pstate);
static void test_platform_vendor_detect(void **pstate);

static dmi_platform_t *test_platform_create(
        dmi_vendor_t  vendor,
        const char   *product,
        const char   *family,
        unsigned      generation);

static const char *test_proliant_path = OPENDMI_TEST_DATA "/hp/proliant-dl385-g1-1.bin";
static const char *test_elitebook_path = OPENDMI_TEST_DATA "/hp/elitebook-820-g4-x3t22av.bin";
static const char *test_dell_path = OPENDMI_TEST_DATA "/dell/g15-5510.bin";
static const char *test_asrock_path = OPENDMI_TEST_DATA "/asrock/b460-pro4.bin";
static const char *test_huawei_path = OPENDMI_TEST_DATA "/huawei/matebook-d14-2024-mdg-x.bin";
static const char *test_amd_path = OPENDMI_TEST_DATA "/asus/tuf-gaming-b650-plus.bin";

static dmi_log_t test_logger = { dmi_test_log_handler };

//
// Test structures of type 250, whose layout depends on the generation of
// the platform: a byte up to generation 89, and a word from generation 90
// onwards. Another module places the latter at type 240.
//
typedef struct test_info
{
    uint16_t value;
} test_info_t;

static const dmi_entity_spec_t test_old_spec =
{
    .type   = &test_type,
    .code   = "test-old",
    .name   = "Test structure, old layout",
    .params = {
        .generations    = { .maximum = 89 },
        .minimum_length = 0x05,
        .decoded_length = sizeof(test_info_t)
    },
    .fields = DMI_FIELDS({
        DMI_FIELD(test_info_t, value, dmi_byte_t),
        {}
    })
};

static const dmi_entity_spec_t test_new_spec =
{
    .type   = &test_type,
    .code   = "test-new",
    .name   = "Test structure, new layout",
    .params = {
        .generations    = { .minimum = 90 },
        .minimum_length = 0x06,
        .decoded_length = sizeof(test_info_t)
    },
    .fields = DMI_FIELDS({
        DMI_FIELD(test_info_t, value, dmi_word_t),
        {}
    })
};

static const dmi_module_t test_module =
{
    .code     = "test",
    .name     = "Test module",
    .entities = (const dmi_entity_spec_t *[]){
        &test_old_spec,
        &test_new_spec,
        nullptr
    }
};

static const dmi_module_t test_relocating_module =
{
    .code        = "test-relocating",
    .name        = "Test relocating module",
    .relocations = DMI_RELOCATIONS({
        { &test_new_spec, (dmi_type_id_t)240 },
        {}
    })
};

//
// Module of structures any platform may carry, which gives way to the modules
// of the vendors
//
static const dmi_entity_spec_t test_shared_spec =
{
    .type   = &test_shared_type,
    .code   = "test-shared",
    .name   = "Test shared structure"
};

static const dmi_module_t test_yielding_module =
{
    .code     = "test-yielding",
    .name     = "Test yielding module",
    .entities = (const dmi_entity_spec_t *[]){
        &test_shared_spec,
        nullptr
    },
    .flags    = DMI_MODULE_FLAG_YIELD
};

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_platform_object),
        cmocka_unit_test(test_platform_match),
        cmocka_unit_test(test_platform_generations),
        cmocka_unit_test(test_platform_hpe_detect),
        cmocka_unit_test_setup_teardown(test_platform_detect, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_auto_modules, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_auto_modules_precedence, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_any_vendor, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_spec_generations, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_relocation, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_suppression, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_yield, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_set, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_filter_module, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_signature, test_platform_setup, test_platform_teardown),
        cmocka_unit_test_setup_teardown(test_platform_signature_slots, test_platform_setup, test_platform_teardown),
        cmocka_unit_test(test_platform_vendor_detect)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static int test_platform_setup(void **pstate)
{
    dmi_context_t *context = dmi_create(DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == nullptr)
        return -1;

    dmi_set_logger(context, &test_logger);

    *pstate = context;

    return 0;
}

static int test_platform_teardown(void **pstate)
{
    dmi_destroy(*pstate);
    *pstate = nullptr;

    return 0;
}

static void test_platform_object(void **pstate)
{
    dmi_unused(pstate);

    dmi_platform_t *platform = dmi_platform_create(nullptr);
    assert_non_null(platform);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(platform->baseboard_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(platform->processor_vendor, DMI_VENDOR_OTHER);
    assert_null(platform->product);
    assert_null(platform->family);
    assert_int_equal(platform->generation, 0);

    // Strings are copied
    char product[] = "ProLiant DL360 Gen10";
    assert_true(dmi_platform_set_product(platform, product));
    assert_true(dmi_platform_set_family(platform, DMI_HPE_FAMILY_SERVER));
    assert_ptr_not_equal(platform->product, product);
    product[0] = 'X';
    assert_string_equal(platform->product, "ProLiant DL360 Gen10");

    platform->firmware_vendor  = DMI_VENDOR_HPE;
    platform->system_vendor    = DMI_VENDOR_HPE;
    platform->processor_vendor = DMI_VENDOR_INTEL;
    platform->generation       = DMI_HPE_GEN10;

    // Copy holds strings of its own
    dmi_platform_t *copy = dmi_platform_clone(platform);
    assert_non_null(copy);
    assert_int_equal(copy->firmware_vendor, DMI_VENDOR_HPE);
    assert_int_equal(copy->system_vendor, DMI_VENDOR_HPE);
    assert_int_equal(copy->baseboard_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(copy->processor_vendor, DMI_VENDOR_INTEL);
    assert_int_equal(copy->generation, DMI_HPE_GEN10);
    assert_string_equal(copy->product, platform->product);
    assert_string_equal(copy->family, platform->family);
    assert_ptr_not_equal(copy->product, platform->product);
    assert_ptr_not_equal(copy->family, platform->family);

    // Strings are cleared by nullptr
    assert_true(dmi_platform_set_product(platform, nullptr));
    assert_null(platform->product);
    assert_string_equal(copy->product, "ProLiant DL360 Gen10");

    assert_false(dmi_platform_set_product(nullptr, "Product"));
    assert_false(dmi_platform_set_family(nullptr, "family"));
    assert_null(dmi_platform_clone(nullptr));

    dmi_platform_destroy(copy);
    dmi_platform_destroy(platform);
    dmi_platform_destroy(nullptr);
}

static void test_platform_match(void **pstate)
{
    dmi_unused(pstate);

    dmi_platform_t *server = test_platform_create(DMI_VENDOR_HP, nullptr, DMI_HPE_FAMILY_SERVER, 0);
    dmi_platform_t *laptop = test_platform_create(DMI_VENDOR_HP, nullptr, nullptr, 0);
    dmi_platform_t *other  = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 0);

    const dmi_platform_match_t any        = { .firmware_vendor = DMI_VENDOR_ANY };
    const dmi_platform_match_t any_hp     = { .firmware_vendor = DMI_VENDOR_HP };
    const dmi_platform_match_t hp_servers = { .firmware_vendor = DMI_VENDOR_HP, .family = DMI_HPE_FAMILY_SERVER };
    const dmi_platform_match_t dell       = { .firmware_vendor = DMI_VENDOR_DELL };
    const dmi_platform_match_t intel      = { .processor_vendor = DMI_VENDOR_INTEL };
    const dmi_platform_match_t hp_intel   = { .firmware_vendor = DMI_VENDOR_HP, .processor_vendor = DMI_VENDOR_INTEL };

    assert_true(dmi_platform_match(server, &any_hp));
    assert_true(dmi_platform_match(server, &hp_servers));
    assert_false(dmi_platform_match(server, &dell));

    // Condition on the family is not satisfied by a platform of no family
    assert_true(dmi_platform_match(laptop, &any_hp));
    assert_false(dmi_platform_match(laptop, &hp_servers));

    // Any vendor includes an unknown one
    assert_true(dmi_platform_match(server, &any));
    assert_true(dmi_platform_match(other, &any));
    assert_false(dmi_platform_match(other, &any_hp));

    // Conditions on other vendors are satisfied by their own vendors only
    assert_false(dmi_platform_match(server, &intel));
    assert_false(dmi_platform_match(server, &hp_intel));
    server->processor_vendor = DMI_VENDOR_INTEL;
    assert_true(dmi_platform_match(server, &intel));
    assert_true(dmi_platform_match(server, &hp_intel));
    assert_false(dmi_platform_match(other, &hp_intel));

    assert_false(dmi_platform_match(nullptr, &any_hp));
    assert_false(dmi_platform_match(server, nullptr));

    dmi_platform_destroy(other);
    dmi_platform_destroy(laptop);
    dmi_platform_destroy(server);
}

static void test_platform_generations(void **pstate)
{
    dmi_unused(pstate);

    dmi_platform_t *unknown = test_platform_create(DMI_VENDOR_HPE, nullptr, nullptr, 0);
    dmi_platform_t *gen9    = test_platform_create(DMI_VENDOR_HPE, nullptr, nullptr, DMI_HPE_GEN9);

    const dmi_generations_t all       = { 0 };
    const dmi_generations_t from_gen9 = { .minimum = DMI_HPE_GEN9 };
    const dmi_generations_t till_gen8 = { .maximum = DMI_HPE_GEN8 };
    const dmi_generations_t only_gen9 = { .minimum = DMI_HPE_GEN9, .maximum = DMI_HPE_GEN9 };

    assert_true(dmi_platform_in_generations(gen9, &all));
    assert_true(dmi_platform_in_generations(gen9, nullptr));
    assert_true(dmi_platform_in_generations(gen9, &from_gen9));
    assert_false(dmi_platform_in_generations(gen9, &till_gen8));
    assert_true(dmi_platform_in_generations(gen9, &only_gen9));

    // Bounded ranges never apply to a platform of unknown generation
    assert_true(dmi_platform_in_generations(unknown, &all));
    assert_false(dmi_platform_in_generations(unknown, &from_gen9));
    assert_false(dmi_platform_in_generations(unknown, &till_gen8));
    assert_false(dmi_platform_in_generations(nullptr, &from_gen9));

    dmi_platform_destroy(gen9);
    dmi_platform_destroy(unknown);
}

static void test_platform_hpe_detect(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        dmi_vendor_t  vendor;
        const char   *product;
        const char   *family;
        unsigned      generation;
    } cases[] = {
        { DMI_VENDOR_HP,  "ProLiant DL385 G1",          DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN1       },
        { DMI_VENDOR_HP,  "ProLiant DL360 G6",          DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN6       },
        { DMI_VENDOR_HP,  "ProLiant BL460c G7",         DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN7       },
        { DMI_VENDOR_HP,  "ProLiant DL380p Gen8",       DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN8       },
        { DMI_VENDOR_HP,  "ProLiant DL380 Gen9",        DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN9       },
        { DMI_VENDOR_HPE, "Synergy 480 Gen10",          DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN10      },
        { DMI_VENDOR_HPE, "Apollo 4200 Gen10",          DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN10      },
        { DMI_VENDOR_HPE, "ProLiant DL360 Gen10 Plus",  DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN10_PLUS },
        { DMI_VENDOR_HPE, "ProLiant DL380 Gen11",       DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN11      },
        { DMI_VENDOR_HPE, "ProLiant DL380 Gen12",       DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN12      },

        // Servers naming no generation are taken for Gen10 Plus when of
        // HPE, and for G6 when of HP, the way dmidecode takes them
        { DMI_VENDOR_HPE, "Edgeline e920t",             DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN10_PLUS },
        { DMI_VENDOR_HP,  "ProLiant ML110",             DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN6       },

        // Generation is a word of its own
        { DMI_VENDOR_HP,  "ProLiant DL360G5",           DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN6       },
        { DMI_VENDOR_HP,  "ProLiant DL360 G5x",         DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN6       },
        { DMI_VENDOR_HPE, "ProLiant DL360 Gen10 Plusx", DMI_HPE_FAMILY_SERVER, DMI_HPE_GEN10      },

        // Other products have no family, whatever their names look like
        { DMI_VENDOR_HP,  "HP EliteBook 820 G4",        nullptr,               0                  },
        { DMI_VENDOR_HP,  "HP Z600 Workstation",        nullptr,               0                  }
    };

    for (size_t i = 0; i < countof(cases); i++) {
        dmi_platform_t *platform = test_platform_create(cases[i].vendor, cases[i].product, nullptr, 0);

        assert_true(dmi_hpe_platform_detect(platform));

        if (cases[i].family == nullptr)
            assert_null(platform->family);
        else
            assert_string_equal(platform->family, cases[i].family);

        if (platform->generation != cases[i].generation)
            fail_msg("%s: generation %u, expected %u", cases[i].product,
                     platform->generation, cases[i].generation);

        dmi_platform_destroy(platform);
    }

    // Platforms without product name are left as they are
    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_HPE, nullptr, nullptr, 0);
    assert_true(dmi_hpe_platform_detect(platform));
    assert_null(platform->family);
    assert_int_equal(platform->generation, 0);
    dmi_platform_destroy(platform);

    assert_true(dmi_hpe_platform_detect(nullptr));
}

static void test_platform_detect(void **pstate)
{
    dmi_context_t *context = *pstate;
    const dmi_platform_t *platform;

    // Closed context has no platform
    assert_null(dmi_get_platform(context));

    // HP servers have family and generation
    assert_true(dmi_load(context, test_proliant_path));
    platform = dmi_get_platform(context);
    assert_non_null(platform);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_HP);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_HP);
    assert_int_equal(platform->processor_vendor, DMI_VENDOR_AMD);
    assert_string_equal(platform->product, "ProLiant DL385 G1");
    assert_string_equal(platform->family, DMI_HPE_FAMILY_SERVER);
    assert_int_equal(platform->generation, DMI_HPE_GEN1);
    assert_true(dmi_close(context));

    // Platform is gone as the context is closed
    assert_null(dmi_get_platform(context));

    // Other HP products have none
    assert_true(dmi_load(context, test_elitebook_path));
    platform = dmi_get_platform(context);
    assert_non_null(platform);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_HP);
    assert_int_equal(platform->processor_vendor, DMI_VENDOR_INTEL);
    assert_string_equal(platform->product, "HP EliteBook 820 G4");
    assert_null(platform->family);
    assert_int_equal(platform->generation, 0);
    assert_true(dmi_close(context));

    // Vendors of the firmware, the system and the baseboard differ
    assert_true(dmi_load(context, test_asrock_path));
    platform = dmi_get_platform(context);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_AMI);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(platform->baseboard_vendor, DMI_VENDOR_OTHER);
    assert_int_equal(platform->processor_vendor, DMI_VENDOR_INTEL);
    assert_true(dmi_close(context));

    assert_true(dmi_load(context, test_huawei_path));
    platform = dmi_get_platform(context);
    assert_int_equal(platform->firmware_vendor, DMI_VENDOR_HUAWEI);
    assert_int_equal(platform->system_vendor, DMI_VENDOR_HUAWEI);
    assert_int_equal(platform->baseboard_vendor, DMI_VENDOR_HUAWEI);
    assert_true(dmi_close(context));

    assert_null(dmi_get_platform(nullptr));
}

static void test_platform_auto_modules(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_module_t *dell = dmi_module_find("dell");
    const dmi_module_t *hpe  = dmi_module_find("hpe");

    // Module of the vendor is enabled for the platform
    assert_true(dmi_load(context, test_dell_path));
    assert_true(dmi_has_extension(context, dell));
    assert_false(dmi_has_extension(context, hpe));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE_ID(DELL_REVISIONS)), &dmi_dell_revisions_spec);

    // ...until the context is closed
    assert_true(dmi_close(context));
    assert_false(dmi_has_extension(context, dell));
    assert_null(dmi_type_spec(context, DMI_TYPE_ID(DELL_REVISIONS)));

    // Module of HP servers is not enabled for other HP products
    assert_true(dmi_load(context, test_proliant_path));
    assert_true(dmi_has_extension(context, hpe));
    assert_true(dmi_close(context));

    assert_true(dmi_load(context, test_elitebook_path));
    assert_false(dmi_has_extension(context, hpe));
    assert_true(dmi_close(context));

    // Nothing is enabled without the flag
    dmi_set_flags(context, 0);
    assert_true(dmi_load(context, test_dell_path));
    assert_false(dmi_has_extension(context, dell));
    assert_null(dmi_type_spec(context, DMI_TYPE_ID(DELL_REVISIONS)));
}

static void test_platform_auto_modules_precedence(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_spec_t spec = {
        .type = DMI_TYPE(dell_revisions),
        .code = "test-revisions",
        .name = "Test revisions"
    };
    const dmi_module_t module = {
        .code     = "test-conflicting",
        .name     = "Test conflicting module",
        .entities = (const dmi_entity_spec_t *[]){ &spec, nullptr }
    };

    // Module enabled explicitly takes the type of the module of the platform,
    // which is skipped then
    assert_true(dmi_add_extension(context, &module));
    assert_true(dmi_load(context, test_dell_path));
    assert_false(dmi_has_extension(context, dmi_module_find("dell")));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE_ID(DELL_REVISIONS)), &spec);
    assert_true(dmi_close(context));

    // Module enabled explicitly stays enabled for the next data
    assert_true(dmi_has_extension(context, &module));
    assert_ptr_equal(dmi_type_spec(context, DMI_TYPE_ID(DELL_REVISIONS)), &spec);
}

static void test_platform_any_vendor(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Module of the structures of any Intel platform is enabled along with
    // the module of the vendor
    assert_true(dmi_load(context, test_dell_path));
    assert_true(dmi_has_extension(context, &dmi_intel_module));
    assert_true(dmi_has_extension(context, dmi_module_find("dell")));
    assert_true(dmi_close(context));

    assert_true(dmi_load(context, test_elitebook_path));
    assert_true(dmi_has_extension(context, &dmi_intel_module));
    assert_true(dmi_close(context));

    // ...but not for the platforms of other processors
    assert_true(dmi_load(context, test_amd_path));
    assert_int_equal(dmi_get_platform(context)->processor_vendor, DMI_VENDOR_AMD);
    assert_false(dmi_has_extension(context, &dmi_intel_module));
    assert_true(dmi_close(context));
}

static void test_platform_spec_generations(void **pstate)
{
    dmi_context_t *context = *pstate;

    // Specifications bound to generations are not mapped while the generation
    // is unknown
    assert_true(dmi_add_extension(context, &test_module));
    assert_null(dmi_type_spec(context, (dmi_type_id_t)250));

    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 80);
    assert_true(dmi_set_platform(context, platform));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_old_spec);

    platform->generation = 100;
    assert_true(dmi_set_platform(context, platform));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_new_spec);

    // Context holds a copy of the platform
    dmi_platform_destroy(platform);
    assert_int_equal(dmi_get_platform(context)->generation, 100);

    // Structure is decoded by the layout of the generation
    static const uint8_t data[] = {
        250, 0x06, 0x00, 0x10,
        0x34, 0x12,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const test_info_t *info = dmi_entity_info(entity, &test_type);
    assert_non_null(info);
    assert_int_equal(info->value, 0x1234);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);

    // Closed context has no platform once the one set is unset
    assert_true(dmi_set_platform(context, nullptr));
    assert_null(dmi_get_platform(context));
    assert_null(dmi_type_spec(context, (dmi_type_id_t)250));
}

static void test_platform_relocation(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 100);
    assert_true(dmi_set_platform(context, platform));
    dmi_platform_destroy(platform);

    // Relocation applies while the relocating module is enabled
    assert_true(dmi_add_extension(context, &test_module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_new_spec);
    assert_null(dmi_type_spec(context, (dmi_type_id_t)240));

    assert_true(dmi_add_extension(context, &test_relocating_module));
    assert_null(dmi_type_spec(context, (dmi_type_id_t)250));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)240), &test_new_spec);

    // Relocated structure is decoded by its original specification, and is
    // checked against its original type
    static const uint8_t data[] = {
        240, 0x06, 0x00, 0x10,
        0x34, 0x12,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    assert_int_equal(dmi_entity_type_id(entity), 240);
    assert_ptr_equal(dmi_entity_type(entity), &test_type);
    assert_ptr_equal(entity->spec, &test_new_spec);
    assert_non_null(dmi_entity_data(entity, &test_type));

    const test_info_t *info = dmi_entity_info(entity, &test_type);
    assert_non_null(info);
    assert_int_equal(info->value, 0x1234);

    // Structures of other types at the same type number are told apart
    dmi_error_clear(context);
    assert_null(dmi_entity_info(entity, &test_shared_type));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_ENTITY_TYPE_INVALID);

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

static void test_platform_suppression(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_module_t suppressing_module = {
        .code        = "test-suppressing",
        .name        = "Test suppressing module",
        .relocations = DMI_RELOCATIONS({
            { &test_shared_spec, DMI_TYPE_ID_INVALID },
            {}
        })
    };

    assert_true(dmi_add_extension(context, &test_yielding_module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_shared_spec);

    // Platforms of the vendor never carry the structure, whose type is left
    // unknown
    assert_true(dmi_add_extension(context, &suppressing_module));
    assert_null(dmi_type_spec(context, (dmi_type_id_t)250));
}

static void test_platform_yield(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 100);
    assert_true(dmi_set_platform(context, platform));
    dmi_platform_destroy(platform);

    // Yielding module takes the types left free
    assert_true(dmi_add_extension(context, &test_yielding_module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_shared_spec);

    // ...and gives them way to the modules of the vendors, whenever these
    // are enabled
    assert_true(dmi_add_extension(context, &test_module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_new_spec);

    // Relocated structure of a vendor frees the type again
    assert_true(dmi_add_extension(context, &test_relocating_module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_shared_spec);
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)240), &test_new_spec);
}

static void test_platform_set(void **pstate)
{
    dmi_context_t *context = *pstate;

    static const dmi_entity_spec_t spec = {
        .type   = &test_type,
        .code   = "test-conflicting",
        .name   = "Test conflicting",
        .params = {
            .generations = { .minimum = 100 }
        }
    };
    const dmi_module_t module = {
        .code     = "test-conflicting",
        .name     = "Test conflicting module",
        .entities = (const dmi_entity_spec_t *[]){ &spec, nullptr }
    };

    // Modules conflict for the generations their ranges share only
    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 80);
    assert_true(dmi_set_platform(context, platform));
    assert_true(dmi_add_extension(context, &test_module));
    assert_true(dmi_add_extension(context, &module));
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_old_spec);

    // Platform is left as it was on conflicts
    platform->generation = 100;
    dmi_error_clear(context);
    assert_false(dmi_set_platform(context, platform));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);
    assert_int_equal(dmi_get_platform(context)->generation, 80);
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)250), &test_old_spec);

    // Platform set explicitly is kept as the context is opened and closed,
    // while the one told from the data is used again once it is unset
    platform->generation = 80;
    platform->firmware_vendor = DMI_VENDOR_HPE;
    assert_true(dmi_set_platform(context, platform));
    assert_true(dmi_load(context, test_dell_path));
    assert_int_equal(dmi_get_platform(context)->firmware_vendor, DMI_VENDOR_HPE);
    assert_int_equal(dmi_get_platform(context)->generation, 80);
    assert_false(dmi_has_extension(context, dmi_module_find("dell")));

    assert_true(dmi_set_platform(context, nullptr));
    assert_int_equal(dmi_get_platform(context)->firmware_vendor, DMI_VENDOR_DELL);
    assert_true(dmi_set_platform(context, platform));

    assert_true(dmi_close(context));
    assert_non_null(dmi_get_platform(context));
    assert_int_equal(dmi_get_platform(context)->generation, 80);

    assert_false(dmi_set_platform(nullptr, platform));

    dmi_platform_destroy(platform);
}

static void test_platform_filter_module(void **pstate)
{
    dmi_context_t *context = *pstate;

    dmi_platform_t *platform = test_platform_create(DMI_VENDOR_OTHER, nullptr, nullptr, 100);
    assert_true(dmi_set_platform(context, platform));
    dmi_platform_destroy(platform);

    assert_true(dmi_add_extension(context, &test_module));
    assert_true(dmi_add_extension(context, &test_relocating_module));

    static const uint8_t data[] = {
        240, 0x06, 0x00, 0x10,
        0x34, 0x12,
        0x00, 0x00
    };

    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    dmi_filter_t *filter = dmi_filter_create(context);
    assert_non_null(filter);
    assert_true(dmi_filter_is_empty(filter));

    // Module which brings no specification matches nothing
    assert_true(dmi_filter_add_module(filter, &test_relocating_module));
    assert_false(dmi_filter_is_empty(filter));
    assert_false(dmi_filter_match(filter, entity));

    // Relocated structure is matched by the module of its specification
    assert_true(dmi_filter_add_module(filter, &test_module));
    assert_true(dmi_filter_match(filter, entity));

    assert_false(dmi_filter_add_module(filter, nullptr));
    assert_false(dmi_filter_add_module(nullptr, &test_module));

    dmi_filter_destroy(filter);
    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);
}

//
// Structures of type 245 are told apart by their content: by bytes, by
// length and string, or by nothing for the rest
//
static const dmi_entity_spec_t test_bytes_spec =
{
    .type   = &test_signed_type,
    .code   = "test-bytes",
    .name   = "Test bytes",
    .params = {
        .signature = DMI_SIGNATURE({ .offset = 0x04, .bytes = "AB", .size = 2 })
    }
};

static const dmi_entity_spec_t test_string_spec =
{
    .type   = &test_signed_type,
    .code   = "test-string",
    .name   = "Test string",
    .params = {
        .signature = DMI_SIGNATURE({ .length = 0x06, .string = 1, .text = "Test" })
    }
};

static const dmi_entity_spec_t test_rest_spec =
{
    .type = &test_signed_type,
    .code = "test-rest",
    .name = "Test rest"
};

static const dmi_entity_spec_t *test_platform_select(dmi_context_t *context, const uint8_t *data, size_t size);

// Specifications with signatures, one more than the candidates of a type have
// room for besides the one without a signature
static const dmi_signature_t test_slot_signature = { .length = 0x05 };

#define TEST_SLOT_SPEC                        \
    {                                         \
        .type   = &test_signed_type,          \
        .code   = "test-slot",                \
        .name   = "Test slot",                \
        .params = {                           \
            .signature = &test_slot_signature \
        }                                     \
    }

static_assert(DMI_TYPE_CANDIDATES == 4, "test slot specifications are listed one by one");

static const dmi_entity_spec_t test_slot_specs[DMI_TYPE_CANDIDATES] = {
    TEST_SLOT_SPEC, TEST_SLOT_SPEC, TEST_SLOT_SPEC, TEST_SLOT_SPEC
};

static void test_platform_signature(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_module_t module = {
        .code     = "test-signatures",
        .name     = "Test signatures module",
        .entities = (const dmi_entity_spec_t *[]){
            &test_bytes_spec, &test_string_spec, &test_rest_spec, nullptr
        }
    };

    assert_true(dmi_add_extension(context, &module));

    // Specification without a signature represents the type
    assert_ptr_equal(dmi_type_spec(context, (dmi_type_id_t)245), &test_rest_spec);

    static const uint8_t bytes[]  = { 245, 0x06, 0x00, 0x10, 'A', 'B', 0x00, 0x00 };
    static const uint8_t string[] = { 245, 0x06, 0x00, 0x10, 0x01, 0x00, 'T', 'e', 's', 't', 0x00, 0x00 };
    static const uint8_t longer[] = { 245, 0x07, 0x00, 0x10, 0x01, 0x00, 0x00, 'T', 'e', 's', 't', 0x00, 0x00 };
    static const uint8_t other[]  = { 245, 0x06, 0x00, 0x10, 'A', 'C', 0x00, 0x00 };

    assert_ptr_equal(test_platform_select(context, bytes, sizeof(bytes)), &test_bytes_spec);
    assert_ptr_equal(test_platform_select(context, string, sizeof(string)), &test_string_spec);

    // Every condition of a signature is to be met
    assert_ptr_equal(test_platform_select(context, longer, sizeof(longer)), &test_rest_spec);
    assert_ptr_equal(test_platform_select(context, other, sizeof(other)), &test_rest_spec);

    // Type told by signatures only is represented by the first of them, and
    // the structures no signature matches are left undecoded
    const dmi_module_t signed_only = {
        .code     = "test-signed-only",
        .name     = "Test signed only module",
        .entities = (const dmi_entity_spec_t *[]){ &test_bytes_spec, nullptr }
    };

    dmi_context_t *second = dmi_create(0);
    assert_non_null(second);
    assert_true(dmi_add_extension(second, &signed_only));
    assert_ptr_equal(dmi_type_spec(second, (dmi_type_id_t)245), &test_bytes_spec);
    assert_null(test_platform_select(second, other, sizeof(other)));
    dmi_destroy(second);
}

static void test_platform_signature_slots(void **pstate)
{
    dmi_context_t *context = *pstate;

    const dmi_entity_spec_t *specs = test_slot_specs;

    // All candidates but the one without a signature are taken
    const dmi_entity_spec_t *entities[DMI_TYPE_CANDIDATES + 1] = {};
    for (size_t i = 0; i < DMI_TYPE_CANDIDATES - 1; i++)
        entities[i] = &specs[i];

    const dmi_module_t module = {
        .code     = "test-slots",
        .name     = "Test slots module",
        .entities = entities
    };

    assert_true(dmi_add_extension(context, &module));

    // ...so no room is left for one more
    const dmi_module_t more = {
        .code     = "test-slots-more",
        .name     = "Test more slots module",
        .entities = (const dmi_entity_spec_t *[]){ &specs[DMI_TYPE_CANDIDATES - 1], nullptr }
    };

    dmi_error_clear(context);
    assert_false(dmi_add_extension(context, &more));
    assert_int_equal(dmi_error_peek_last(context)->reason, DMI_ERROR_MODULE_CONFLICT);

    // Specification without a signature still fits
    assert_true(dmi_add_extension(context, &(const dmi_module_t){
        .code     = "test-slots-rest",
        .name     = "Test rest slot module",
        .entities = (const dmi_entity_spec_t *[]){ &test_rest_spec, nullptr }
    }));
}

//
// Vendor is told by any of its names, case and surrounding whitespace
// ignored, the way strings of the structures are trimmed.
//
static void test_platform_vendor_detect(void **pstate)
{
    dmi_unused(pstate);

    const char *names[] = { "LENOVO", "lenovo", " LENOVO ", "LENOVO\t" };

    for (size_t i = 0; i < countof(names); i++) {
        const dmi_vendor_spec_t *vendor = dmi_vendor_detect(names[i]);

        assert_non_null(vendor);
        assert_int_equal(vendor->id, DMI_VENDOR_LENOVO);
    }

    // Other spellings match no vendor
    assert_null(dmi_vendor_detect("LENOVO Group"));
    assert_null(dmi_vendor_detect("LENOV"));
    assert_null(dmi_vendor_detect(""));
    assert_null(dmi_vendor_detect("   "));
    assert_null(dmi_vendor_detect(nullptr));
}

static const dmi_entity_spec_t *test_platform_select(dmi_context_t *context, const uint8_t *data, size_t size)
{
    dmi_buffer_t *buffer = dmi_buffer_create(context);
    dmi_entity_t *entity = dmi_test_entity_create(buffer, data, size);
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_entity_spec_t *spec = entity->spec;

    dmi_entity_destroy(entity);
    dmi_buffer_destroy(buffer);

    return spec;
}

static dmi_platform_t *test_platform_create(
        dmi_vendor_t  vendor,
        const char   *product,
        const char   *family,
        unsigned      generation)
{
    dmi_platform_t *platform = dmi_platform_create(nullptr);
    assert_non_null(platform);

    platform->firmware_vendor = vendor;
    platform->generation      = generation;

    assert_true(dmi_platform_set_product(platform, product));
    assert_true(dmi_platform_set_family(platform, family));

    return platform;
}
