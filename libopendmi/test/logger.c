//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/test/logger.h>

void dmi_test_log_handler(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args)
{
    dmi_unused(target);
    assert(level >= 0);
    assert(format != nullptr);

    fprintf(stderr, "%s: ", dmi_log_level_name(level));
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}
