//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/revisions-internal.h>

//
// Implementation version is carried as the major and the minor number, one
// byte each, which the version number puts in the order it counts them.
//
bool dmi_dell_revisions_decode_version(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, dmi_version((unsigned int)(data->number & 0xFFu), (unsigned int)((data->number >> 8) & 0xFFu), 0));
}

bool dmi_dell_revisions_encode_version(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_version_t version = (dmi_version_t)dmi_field_get(field, value);

    data->number = dmi_version_major(version) | (dmi_version_minor(version) << 8);

    return true;
}
