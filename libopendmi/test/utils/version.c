//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>
#include <opendmi/utils/version.h>

static void test_version_build(void **pstate);
static void test_version_format(void **pstate);
static void test_version_format_ex(void **pstate);
static int  free_version_format(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_version_build),
        cmocka_unit_test_teardown(test_version_format, free_version_format),
        cmocka_unit_test_teardown(test_version_format_ex, free_version_format)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

static void test_version_build(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        unsigned int major;
        unsigned int minor;
        unsigned int revision;
        dmi_version_t version;
    } test_data[] = {
        { 0x00, 0x00, 0x00, 0x00000000u },
        { 0x00, 0x00, 0x12, 0x00000012u },
        { 0x00, 0x12, 0x00, 0x00001200u },
        { 0x00, 0x12, 0x34, 0x00001234u },
        { 0x12, 0x00, 0x00, 0x00120000u },
        { 0x12, 0x00, 0x34, 0x00120034u },
        { 0x12, 0x34, 0x00, 0x00123400u },
        { 0x12, 0x34, 0x56, 0x00123456u }
    };

    for (size_t i = 0; i < countof(test_data); i++) {
        unsigned int  major    = test_data[i].major;
        unsigned int  minor    = test_data[i].minor;
        unsigned int  revision = test_data[i].revision;
        dmi_version_t version  = test_data[i].version;

        assert_uint_equal(dmi_version(major, minor, revision), version);
    }
}

static void test_version_format(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        dmi_version_t value;
        const char *expected;
    } test_data[] = {
        { DMI_VERSION(0, 0, 0), "0.0"   },
        { DMI_VERSION(0, 1, 0), "0.1"   },
        { DMI_VERSION(1, 0, 0), "1.0"   },
        { DMI_VERSION(1, 2, 0), "1.2"   },
        { DMI_VERSION(1, 2, 3), "1.2.3" }
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_version_t  value    = test_data[i].value;
        const char    *expected = test_data[i].expected;
        char          *result   = nullptr;

        result = dmi_version_format(value);
        *pstate = result;

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static void test_version_format_ex(void **pstate)
{
    dmi_unused(pstate);

    static const struct {
        dmi_version_t value;
        int level;
        const char *expected;
    } test_data[] = {
        { DMI_VERSION(0, 0, 0), DMI_VERSION_LEVEL_MAJOR, "0" },
        { DMI_VERSION(0, 1, 0), DMI_VERSION_LEVEL_MAJOR, "0" },
        { DMI_VERSION(1, 0, 0), DMI_VERSION_LEVEL_MAJOR, "1" },
        { DMI_VERSION(1, 2, 0), DMI_VERSION_LEVEL_MAJOR, "1" },
        { DMI_VERSION(1, 2, 3), DMI_VERSION_LEVEL_MAJOR, "1" },

        { DMI_VERSION(0, 0, 0), DMI_VERSION_LEVEL_MINOR, "0.0" },
        { DMI_VERSION(0, 1, 0), DMI_VERSION_LEVEL_MINOR, "0.1" },
        { DMI_VERSION(1, 0, 0), DMI_VERSION_LEVEL_MINOR, "1.0" },
        { DMI_VERSION(1, 2, 0), DMI_VERSION_LEVEL_MINOR, "1.2" },
        { DMI_VERSION(1, 2, 3), DMI_VERSION_LEVEL_MINOR, "1.2" },

        { DMI_VERSION(0, 0, 0), DMI_VERSION_LEVEL_REVISION, "0.0.0" },
        { DMI_VERSION(0, 1, 0), DMI_VERSION_LEVEL_REVISION, "0.1.0" },
        { DMI_VERSION(1, 0, 0), DMI_VERSION_LEVEL_REVISION, "1.0.0" },
        { DMI_VERSION(1, 2, 0), DMI_VERSION_LEVEL_REVISION, "1.2.0" },
        { DMI_VERSION(1, 2, 3), DMI_VERSION_LEVEL_REVISION, "1.2.3" },

        { DMI_VERSION(0, 0, 0), -1, nullptr },
        { DMI_VERSION(0, 1, 0), -1, nullptr },
        { DMI_VERSION(1, 0, 0), -1, nullptr },
        { DMI_VERSION(1, 2, 0), -1, nullptr },
        { DMI_VERSION(1, 2, 3), -1, nullptr },

        { DMI_VERSION(0, 0, 0), INT_MAX, nullptr },
        { DMI_VERSION(0, 1, 0), INT_MAX, nullptr },
        { DMI_VERSION(1, 0, 0), INT_MAX, nullptr },
        { DMI_VERSION(1, 2, 0), INT_MAX, nullptr },
        { DMI_VERSION(1, 2, 3), INT_MAX, nullptr },
    };

    *pstate = nullptr;

    for (size_t i = 0; i < countof(test_data); i++) {
        dmi_version_t  value    = test_data[i].value;
        int            level    = test_data[i].level;
        const char    *expected = test_data[i].expected;
        char          *result   = nullptr;

        result = dmi_version_format_ex(value, level);
        *pstate = result;

        if (expected == nullptr) {
            assert_null(result);
            continue;
        }

        assert_non_null(result);
        assert_string_equal(result, expected);

        free(result);
        *pstate = nullptr;
    }
}

static int free_version_format(void **pstate)
{
    if (*pstate == nullptr)
        return 0;

    free(*pstate);
    *pstate = nullptr;

    return 0;
}
