//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <opendmi/test/helpers.h>

static bool dmi_test_option_has_short(const dmi_option_t *option, char name);
static bool dmi_test_option_has_long(const dmi_option_t *option, const char *name);

size_t dmi_test_option_duplicates(const dmi_option_set_t **options)
{
    size_t count = 0;

    assert(options != nullptr);

    for (const dmi_option_set_t **pset = options; *pset != nullptr; pset++) {
        for (const dmi_option_t *option = (*pset)->options; option->short_names or option->long_names; option++) {
            // Compare with all options preceding the current one
            for (const dmi_option_set_t **pprev_set = options; pprev_set <= pset; pprev_set++) {
                for (const dmi_option_t *prev = (*pprev_set)->options; prev->short_names or prev->long_names; prev++) {
                    if (prev == option)
                        break;

                    for (const char *name = option->short_names; (name != nullptr) and (*name != 0); name++) {
                        if (dmi_test_option_has_short(prev, *name)) {
                            fprintf(stderr, "Duplicate option: -%c (%s)\n", *name, (*pset)->name);
                            count++;
                        }
                    }

                    for (const char **name = option->long_names; (name != nullptr) and (*name != nullptr); name++) {
                        if (dmi_test_option_has_long(prev, *name)) {
                            fprintf(stderr, "Duplicate option: --%s (%s)\n", *name, (*pset)->name);
                            count++;
                        }
                    }
                }
            }
        }
    }

    return count;
}

static bool dmi_test_option_has_short(const dmi_option_t *option, char name)
{
    return (option->short_names != nullptr) and (strchr(option->short_names, name) != nullptr);
}

static bool dmi_test_option_has_long(const dmi_option_t *option, const char *name)
{
    if (option->long_names == nullptr)
        return false;

    for (const char **item = option->long_names; *item != nullptr; item++) {
        if (strcmp(*item, name) == 0)
            return true;
    }

    return false;
}
