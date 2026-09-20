//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include "../src/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cmocka.h>

#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/format.h>
#include <opendmi/entity/string-property.h>
#include <opendmi/internal.h>
#include <opendmi/command/common.h>
#include <opendmi/utils/utf8.h>

#ifdef ENABLE_XML
#   include <libxml/parser.h>
#endif
#include <opendmi/test/helpers.h>

static int test_format_setup(void **pstate);
static int test_format_teardown(void **pstate);

static void test_format_many_strings(void **pstate);
static void test_format_set_high_bits(void **pstate);
static void test_format_invalid_utf8(void **pstate);
static void test_format_yaml_quoting(void **pstate);
static void test_format_xml_flag_names(void **pstate);
static void test_format_state(void **pstate);
static void test_format_text_quiet(void **pstate);
static void test_format_properties(void **pstate);
static void test_format_overlays(void **pstate);

static char *test_format_print(const dmi_format_t *format, const dmi_entity_t *entity, bool dump, dmi_format_mode_t mode);
static bool test_format_has_controls(const char *output);

// Firmware information (SMBIOS 2.0) with PCI support and vendor-reserved
// (high) feature bits
static const dmi_data_t test_firmware_data[] = {
    0, 0x12, 0x00, 0x00,                            // Header
    0x01, 0x02, 0x00, 0xE0, 0x00, 0x00,             // Vendor, version, segment, date, ROM size
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, // Features
    'V', 'e', 'n', 'd', 'o', 'r', 0,
    '1', '.', '0', 0,
    0
};

// Output of set flags with bit 31 clear and bit 7 set, for every format
static const struct {
    const char *code;
    const char *clear;
    const char *set;
} test_set_flags[] = {
    { "text", "NEC PC-98: no",                 "PCI support: yes"          },
    { "json", "\"nec-pc-98\": false",          "\"pci-support\": true"     },
    { "yaml", "nec-pc-98: false",              "pci-support: true"         },
    { "xml",  "<flag name=\"nec-pc-98\">false</flag>", "<flag name=\"pci-support\">true</flag>" }
};

// Number of strings exceeding the range of string references
static const size_t test_string_count = 300;

typedef struct test_format_state
{
    dmi_context_t *context;
    dmi_entity_t  *entity;
    dmi_data_t    *data;
    size_t         data_size;
} test_format_state_t;

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_format_many_strings),
        cmocka_unit_test(test_format_set_high_bits),
        cmocka_unit_test(test_format_invalid_utf8),
        cmocka_unit_test(test_format_yaml_quoting),
        cmocka_unit_test(test_format_xml_flag_names),
        cmocka_unit_test(test_format_state),
        cmocka_unit_test(test_format_text_quiet),
        cmocka_unit_test(test_format_properties),
        cmocka_unit_test(test_format_overlays)
    };

    return cmocka_run_group_tests(tests, test_format_setup, test_format_teardown);
}

static int test_format_setup(void **pstate)
{
    test_format_state_t *state = calloc(1, sizeof(*state));
    if (state == nullptr)
        return -1;

    *pstate = state;

    state->context = dmi_create(0);
    if (state->context == nullptr)
        return -1;

    // Inactive structure with 1-byte body and strings "S1" to "S300"
    state->data_size = 5 + test_string_count * 5 + 1;
    state->data      = calloc(1, state->data_size);
    if (state->data == nullptr)
        return -1;

    const dmi_data_t header[] = { 126, 5, 0x01, 0x00, 0x00 };
    memcpy(state->data, header, sizeof(header));

    size_t length = sizeof(header);
    for (size_t i = 1; i <= test_string_count; i++)
        length += (size_t)snprintf((char *)state->data + length, state->data_size - length, "S%zu", i) + 1;

    // String set is terminated by an additional NUL
    length++;

    state->entity = dmi_entity_create(state->context, state->data, length);
    if (state->entity == nullptr)
        return -1;

    return 0;
}

static int test_format_teardown(void **pstate)
{
    test_format_state_t *state = *pstate;

    if (state != nullptr) {
        dmi_entity_destroy(state->entity);
        dmi_destroy(state->context);
        free(state->data);
        free(state);
    }

    *pstate = nullptr;

    return 0;
}

static void test_format_many_strings(void **pstate)
{
    const test_format_state_t *state = *pstate;

    assert_int_equal(state->entity->string_count, test_string_count);
    assert_string_equal(dmi_entity_string_ex(state->entity, test_string_count, true), "S300");
    assert_null(dmi_entity_string_ex(state->entity, test_string_count + 1, true));

    // Strings beyond the range of string references are printed, and the
    // first ones are not printed again
    for (const dmi_format_t **pformat = dmi_formats; *pformat != nullptr; pformat++) {
        char *output = test_format_print(*pformat, state->entity, true, DMI_FORMAT_MODE_NORMAL);
        if (output == nullptr)
            fail_msg("Format %s: printing failed", (*pformat)->code);

        const char *last = strstr(output, "S300");
        const char *next = (last != nullptr) ? strstr(last, "S1") : nullptr;

        free(output);

        if (last == nullptr)
            fail_msg("Format %s: last string is missing", (*pformat)->code);
        if (next != nullptr)
            fail_msg("Format %s: strings are repeated", (*pformat)->code);
    }
}

static void test_format_set_high_bits(void **pstate)
{
    const test_format_state_t *state = *pstate;

    dmi_entity_t *entity = dmi_entity_create(state->context, test_firmware_data, sizeof(test_firmware_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    for (size_t i = 0; i < countof(test_set_flags); i++) {
        const dmi_format_t *format = dmi_format_get(test_set_flags[i].code);

        // Format may be disabled at build time
        if (format == nullptr)
            continue;

        char *output = test_format_print(format, entity, false, DMI_FORMAT_MODE_NORMAL);
        if (output == nullptr) {
            dmi_entity_destroy(entity);
            fail_msg("Format %s: printing failed", format->code);
        }

        bool clear = (strstr(output, test_set_flags[i].clear) != nullptr);
        bool set   = (strstr(output, test_set_flags[i].set) != nullptr);

        free(output);

        if (not clear or not set) {
            dmi_entity_destroy(entity);
            fail_msg("Format %s: invalid flags output", format->code);
        }
    }

    dmi_entity_destroy(entity);
}

static void test_format_invalid_utf8(void **pstate)
{
    const test_format_state_t *state = *pstate;

    // Inactive structure with a string containing invalid UTF-8 and control
    // character
    static const dmi_data_t data[] = {
        126, 5, 0x02, 0x00, 0x00,
        'A', 0xFF, 'B', 0x01, 'C', 0,
        0
    };

    // Formats, which must produce valid UTF-8 output without raw control
    // characters
    static const char *codes[] = { "json", "yaml", "xml" };

    dmi_entity_t *entity = dmi_entity_create(state->context, data, sizeof(data));
    assert_non_null(entity);

    for (size_t i = 0; i < countof(codes); i++) {
        const dmi_format_t *format = dmi_format_get(codes[i]);

        // Format may be disabled at build time
        if (format == nullptr)
            continue;

        char *output = test_format_print(format, entity, true, DMI_FORMAT_MODE_NORMAL);
        bool valid =
            (output != nullptr) and
            dmi_utf8_is_valid(output) and
            not test_format_has_controls(output);

        free(output);

        if (not valid) {
            dmi_entity_destroy(entity);
            fail_msg("Format %s: invalid output", format->code);
        }
    }

    dmi_entity_destroy(entity);
}

static void test_format_yaml_quoting(void **pstate)
{
    const test_format_state_t *state = *pstate;

    const dmi_format_t *format = dmi_format_get("yaml");

    // Format may be disabled at build time
    if (format == nullptr)
        skip();

    // Firmware information with strings, which look like values of other types
    static const dmi_data_t data[] = {
        0, 0x12, 0x00, 0x00,
        0x01, 0x02, 0x00, 0xE0, 0x03, 0x00,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        'y', 'e', 's', 0,
        '0', '3', '0', '5', 0,
        '1', '1', '/', '1', '3', '/', '2', '0', '0', '7', 0,
        0
    };

    // Strings and dates are quoted without tags, numbers and booleans are not
    static const char *expected[] = {
        "vendor: \"yes\"",
        "version: \"0305\"",
        "release-date: \"2007-11-13\"",
        "rom-size: 65536",
        "pci-support: true",
        "level: '2.0'"
    };

    dmi_entity_t *entity = dmi_entity_create(state->context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    char *output = test_format_print(format, entity, false, DMI_FORMAT_MODE_NORMAL);
    dmi_entity_destroy(entity);
    assert_non_null(output);

    const char *missing = nullptr;
    for (size_t i = 0; i < countof(expected); i++) {
        if (strstr(output, expected[i]) == nullptr) {
            missing = expected[i];
            break;
        }
    }

    bool tagged = (strstr(output, "! ") != nullptr);

    free(output);

    if (missing != nullptr)
        fail_msg("Missing output: %s", missing);

    assert_false(tagged);
}

static void test_format_xml_flag_names(void **pstate)
{
#ifdef ENABLE_XML
    const test_format_state_t *state = *pstate;

    const dmi_format_t *format = dmi_format_get("xml");
    assert_non_null(format);

    // Memory controller information with 5V required voltage, which code is
    // not a valid XML name
    static const dmi_data_t data[] = {
        5, 0x0F, 0x03, 0x00,
        0x03, 0x04, 0x03, 0x03, 0x0A, 0x02, 0x00, 0x04, 0x00, 0x01, 0x00,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(state->context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    char *output = test_format_print(format, entity, false, DMI_FORMAT_MODE_NORMAL);
    dmi_entity_destroy(entity);
    assert_non_null(output);

    bool found = (strstr(output, "<flag name=\"5v\">true</flag>") != nullptr);

    // Output is well-formed
    xmlDocPtr doc = xmlReadMemory(output, (int)strlen(output), "format-test.xml", nullptr,
                                  XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    bool parsed = (doc != nullptr);

    xmlFreeDoc(doc);
    free(output);

    assert_true(found);
    assert_true(parsed);
#else
    dmi_unused(pstate);
    skip();
#endif
}

static void test_format_state(void **pstate)
{
    const test_format_state_t *state = *pstate;

    static const struct {
        const char        *code;
        dmi_format_mode_t  mode;
        const char        *expected;
    } cases[] = {
        { "text", DMI_FORMAT_MODE_VERBOSE, ", decoded, partial\nPlatform firmware information (2.0)\n" },
        { "json", DMI_FORMAT_MODE_NORMAL,  "\"state\": [\n                \"decoded\",\n                \"partial\"\n            ]" },
        { "yaml", DMI_FORMAT_MODE_NORMAL,  "state: [decoded, partial]" },
        { "xml",  DMI_FORMAT_MODE_NORMAL,  "state=\"decoded partial\"" }
    };

    // Complete SMBIOS 2.0 structure is decoded partially
    dmi_entity_t *entity = dmi_entity_create(state->context, test_firmware_data, sizeof(test_firmware_data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    for (size_t i = 0; i < countof(cases); i++) {
        const dmi_format_t *format = dmi_format_get(cases[i].code);

        // Format may be disabled at build time
        if (format == nullptr)
            continue;

        char *output = test_format_print(format, entity, false, cases[i].mode);
        bool found = (output != nullptr) and (strstr(output, cases[i].expected) != nullptr);

        free(output);

        if (not found) {
            dmi_entity_destroy(entity);
            fail_msg("Format %s: state is missing", cases[i].code);
        }
    }

    // States and versions are not shown in text output by default
    char *output = test_format_print(dmi_format_get("text"), entity, false, DMI_FORMAT_MODE_NORMAL);
    bool found = (output != nullptr) and
        ((strstr(output, ", decoded") != nullptr) or (strstr(output, "information (") != nullptr));

    free(output);
    dmi_entity_destroy(entity);

    assert_false(found);
}

static void test_format_text_quiet(void **pstate)
{
    const test_format_state_t *state = *pstate;

    // Physical memory array with memory error information handle
    static const dmi_data_t data[] = {
        16, 0x0F, 0x20, 0x00,
        0x03, 0x03, 0x03, 0x00, 0x00, 0x40, 0x00, 0x30, 0x00, 0x02, 0x00,
        0x00, 0x00
    };

    dmi_entity_t *entity = dmi_entity_create(state->context, data, sizeof(data));
    assert_non_null(entity);
    assert_true(dmi_entity_decode(entity));

    const dmi_format_t *format = dmi_format_get("text");

    char *normal = test_format_print(format, entity, false, DMI_FORMAT_MODE_NORMAL);
    char *quiet  = test_format_print(format, entity, false, DMI_FORMAT_MODE_QUIET);

    dmi_entity_destroy(entity);

    bool normal_header = (normal != nullptr) and (strstr(normal, "Handle 0x0020") != nullptr);
    bool normal_handle = (normal != nullptr) and (strstr(normal, "Memory error information handle") != nullptr);
    bool quiet_valid   = (quiet != nullptr) and (strstr(quiet, "Physical memory array") != nullptr);
    bool quiet_header  = (quiet != nullptr) and (strstr(quiet, "Handle 0x0020") != nullptr);
    bool quiet_handle  = (quiet != nullptr) and (strstr(quiet, "Memory error information handle") != nullptr);

    free(normal);
    free(quiet);

    assert_true(normal_header);
    assert_true(normal_handle);

    // Structure header and handle references are hidden in quiet mode
    assert_true(quiet_valid);
    assert_false(quiet_header);
    assert_false(quiet_handle);
}

static void test_format_properties(void **pstate)
{
    const test_format_state_t *state = *pstate;

    // OEM strings with a single string
    static const dmi_data_t parent_data[] = {
        11, 5, 0x00, 0x01, 0x01,
        'O', 'E', 'M', 0,
        0
    };

    // String properties with named identifier, identifier within the vendor
    // range and without value
    static const dmi_data_t property_data[][13] = {
        { 46, 9, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x01, 'P', 0, 0 },
        { 46, 9, 0x01, 0x02, 0x01, 0x80, 0x01, 0x00, 0x01, 'V', 0, 0 },
        { 46, 9, 0x02, 0x02, 0x01, 0xC0, 0x00, 0x00, 0x01, 0, 0 }
    };

    static const struct {
        const char *code;
        const char *expected;
    } cases[] = {
        {
            "text",
            "\tProperties:\n"
            "\t\tUEFI device path: P\n"
            "\t\tFirmware vendor specific (0x8001): V\n"
            "\t\tOEM specific (0xC001): <unspecified>\n"
        },
        {
            "json",
            "\"properties\": [\n"
            "                {\n"
            "                    \"id\": \"0x0001\",\n"
            "                    \"code\": \"uefi-device-path\",\n"
            "                    \"value\": \"P\"\n"
            "                },\n"
            "                {\n"
            "                    \"id\": \"0x8001\",\n"
            "                    \"code\": \"vendor-specific\",\n"
            "                    \"value\": \"V\"\n"
            "                },\n"
            "                {\n"
            "                    \"id\": \"0xc001\",\n"
            "                    \"code\": \"oem-specific\",\n"
            "                    \"value\": null\n"
            "                }\n"
            "            ]"
        },
        {
            "yaml",
            "  properties:\n"
            "  - id: 0x0001\n"
            "    code: \"uefi-device-path\"\n"
            "    value: \"P\"\n"
            "  - id: 0x8001\n"
            "    code: \"vendor-specific\"\n"
            "    value: \"V\"\n"
            "  - id: 0xc001\n"
            "    code: \"oem-specific\"\n"
            "    value: null\n"
        },
        {
            "xml",
            "<dmi:properties>\n"
            "      <dmi:property id=\"0x0001\" code=\"uefi-device-path\">P</dmi:property>\n"
            "      <dmi:property id=\"0x8001\" code=\"vendor-specific\">V</dmi:property>\n"
            "      <dmi:property id=\"0xc001\" code=\"oem-specific\"></dmi:property>\n"
            "    </dmi:properties>\n"
        }
    };

    dmi_entity_t *parent = dmi_entity_create(state->context, parent_data, sizeof(parent_data));
    assert_non_null(parent);
    assert_true(dmi_entity_decode(parent));

    // Properties are not shown if there are none
    char *output = test_format_print(dmi_format_get("text"), parent, false, DMI_FORMAT_MODE_NORMAL);
    bool found = (output != nullptr) and (strstr(output, "Properties:") != nullptr);

    free(output);
    assert_false(found);

    dmi_entity_t *properties[countof(property_data)];

    for (size_t i = 0; i < countof(property_data); i++) {
        properties[i] = dmi_entity_create(state->context, property_data[i], sizeof(property_data[i]));
        assert_non_null(properties[i]);
        assert_true(dmi_entity_decode(properties[i]));
        assert_true(dmi_entity_add_property(parent, properties[i]->info));
    }

    const char *failed = nullptr;

    for (size_t i = 0; (i < countof(cases)) and (failed == nullptr); i++) {
        const dmi_format_t *format = dmi_format_get(cases[i].code);

        // Format may be disabled at build time
        if (format == nullptr)
            continue;

        output = test_format_print(format, parent, false, DMI_FORMAT_MODE_NORMAL);
        if ((output == nullptr) or (strstr(output, cases[i].expected) == nullptr))
            failed = cases[i].code;

        free(output);
    }

    // Properties are shown even in quiet mode, but not in raw dumps
    const dmi_format_t *format = dmi_format_get("text");

    char *quiet = test_format_print(format, parent, false, DMI_FORMAT_MODE_QUIET);
    char *dump  = test_format_print(format, parent, true, DMI_FORMAT_MODE_NORMAL);

    bool quiet_found = (quiet != nullptr) and (strstr(quiet, "UEFI device path: P") != nullptr);
    bool dump_valid  = (dump != nullptr) and (strstr(dump, "Header and data:") != nullptr);
    bool dump_found  = (dump != nullptr) and (strstr(dump, "Properties:") != nullptr);

    free(quiet);
    free(dump);

    for (size_t i = 0; i < countof(properties); i++)
        dmi_entity_destroy(properties[i]);

    dmi_entity_destroy(parent);

    if (failed != nullptr)
        fail_msg("Format %s: invalid properties output", failed);

    assert_true(quiet_found);
    assert_true(dump_valid);
    assert_false(dump_found);
}

static void test_format_overlays(void **pstate)
{
    const test_format_state_t *state = *pstate;

    // Additional information changing firmware version string number from 2
    // ("1.0") to 1 ("Vendor"), and the same without string, which overrides
    // the first entry
    static const dmi_data_t overlay_data[] = {
        40, 0x11, 0x40, 0x00, 0x02,
        0x06, 0x00, 0x00, 0x05, 0x01, 0x01,
        0x06, 0x00, 0x00, 0x05, 0x00, 0x01,
        'N', 'o', 't', 'e', 0,
        0
    };

    static const struct {
        const char *code;
        const char *expected;
    } cases[] = {
        {
            "text",
            "\tAdditional information:\n"
            "\t\t0x0040[0]: 01 at offset 0x05 - \"Note\"\n"
            "\t\t0x0040[1]: 01 at offset 0x05\n"
        },
        {
            "json",
            "\"overlays\": [\n"
            "                {\n"
            "                    \"source\": \"0x0040\",\n"
            "                    \"index\": 0,\n"
            "                    \"offset\": \"0x05\",\n"
            "                    \"value\": \"01\",\n"
            "                    \"string\": \"Note\"\n"
            "                },\n"
            "                {\n"
            "                    \"source\": \"0x0040\",\n"
            "                    \"index\": 1,\n"
            "                    \"offset\": \"0x05\",\n"
            "                    \"value\": \"01\",\n"
            "                    \"string\": null\n"
            "                }\n"
            "            ]"
        },
        {
            "yaml",
            "  overlays:\n"
            "  - source: 0x0040\n"
            "    index: 0\n"
            "    offset: 0x05\n"
            "    value: \"01\"\n"
            "    string: \"Note\"\n"
            "  - source: 0x0040\n"
            "    index: 1\n"
            "    offset: 0x05\n"
            "    value: \"01\"\n"
            "    string: null\n"
        },
        {
            "xml",
            "<dmi:overlays>\n"
            "      <dmi:overlay source=\"0x0040\" index=\"0\" offset=\"0x05\">\n"
            "        <dmi:value>01</dmi:value>\n"
            "        <dmi:string>Note</dmi:string>\n"
            "      </dmi:overlay>\n"
            "      <dmi:overlay source=\"0x0040\" index=\"1\" offset=\"0x05\">\n"
            "        <dmi:value>01</dmi:value>\n"
            "      </dmi:overlay>\n"
            "    </dmi:overlays>\n"
        }
    };

    dmi_entity_t *overlay = dmi_entity_create(state->context, overlay_data, sizeof(overlay_data));
    assert_non_null(overlay);
    assert_true(dmi_entity_decode(overlay));

    dmi_entity_t *entity = dmi_entity_create(state->context, test_firmware_data, sizeof(test_firmware_data));
    assert_non_null(entity);

    bool attached =
        dmi_entity_add_overlay(entity, overlay, 0) and
        dmi_entity_add_overlay(entity, overlay, 1) and
        dmi_entity_decode(entity);

    const char *failed = nullptr;

    for (size_t i = 0; attached and (i < countof(cases)) and (failed == nullptr); i++) {
        const dmi_format_t *format = dmi_format_get(cases[i].code);

        // Format may be disabled at build time
        if (format == nullptr)
            continue;

        char *output = test_format_print(format, entity, false, DMI_FORMAT_MODE_NORMAL);
        if ((output == nullptr) or (strstr(output, cases[i].expected) == nullptr))
            failed = cases[i].code;

        free(output);
    }

    dmi_entity_destroy(entity);
    dmi_entity_destroy(overlay);

    assert_true(attached);

    if (failed != nullptr)
        fail_msg("Format %s: invalid overlays output", failed);
}

static char *test_format_print(const dmi_format_t *format, const dmi_entity_t *entity, bool dump, dmi_format_mode_t mode)
{
    FILE *stream = tmpfile();
    if (stream == nullptr)
        return nullptr;

    char *output = nullptr;
    const dmi_format_options_t options = {
        .mode = mode,
        .dump = dump
    };

    void *session = format->handlers.initialize(entity->context, stream, &options);

    do {
        if (session == nullptr)
            break;

        // Entry point is not printed, since the context is not opened
        const dmi_format_ops_t *ops = &format->handlers;
        bool success =
            ((ops->dump_start == nullptr) or ops->dump_start(session)) and
            ((ops->table_start == nullptr) or ops->table_start(session)) and
            dmi_print_entity(format, entity, session, &options) and
            ((ops->table_end == nullptr) or ops->table_end(session)) and
            ((ops->dump_end == nullptr) or ops->dump_end(session));

        format->handlers.finalize(session);

        if (not success)
            break;

        if (fflush(stream) != 0)
            break;

        long size = ftell(stream);
        if ((size < 0) or (fseek(stream, 0, SEEK_SET) != 0))
            break;

        output = calloc(1, (size_t)size + 1);
        if (output == nullptr)
            break;

        if (fread(output, 1, (size_t)size, stream) != (size_t)size) {
            free(output);
            output = nullptr;
        }
    } while (false);

    fclose(stream);

    return output;
}

static bool test_format_has_controls(const char *output)
{
    for (const unsigned char *pos = dmi_cast(pos, output); *pos != 0; pos++) {
        if ((*pos < 0x20u) and (*pos != '\t') and (*pos != '\n') and (*pos != '\r'))
            return true;
    }

    return false;
}
