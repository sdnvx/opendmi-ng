//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>

#include <opendmi/internal.h>
#include <opendmi/utils/string.h>
#include <opendmi/utils/version.h>

char *dmi_version_format(dmi_version_t version)
{
    if (dmi_version_revision(version) == 0)
        return dmi_version_format_ex(version, DMI_VERSION_LEVEL_MINOR);

    return dmi_version_format_ex(version, DMI_VERSION_LEVEL_REVISION);
}

char *dmi_version_format_ex(dmi_version_t version, dmi_version_level_t level)
{
    int   rv  = -1;
    char *str = nullptr;

    unsigned major    = dmi_version_major(version);
    unsigned minor    = dmi_version_minor(version);
    unsigned revision = dmi_version_revision(version);

    switch (level) {
    case DMI_VERSION_LEVEL_MAJOR:
        rv = dmi_asprintf(&str, "%u", major);
        break;

    case DMI_VERSION_LEVEL_MINOR:
        rv = dmi_asprintf(&str, "%u.%u", major, minor);
        break;

    case DMI_VERSION_LEVEL_REVISION:
        rv = dmi_asprintf(&str, "%u.%u.%u", major, minor, revision);
        break;

    default:
        return dmi_trace_argument_invalid(nullptr, level, nullptr);
    }

    if (rv < 0)
        return nullptr;

    return str;
}
