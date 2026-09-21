//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/dell.h>

#include <opendmi/entity/dell/infrared-port-internal.h>

const dmi_name_set_t dmi_dell_infrared_proto_names =
{
    .code = "dell-infrared-protocol",
    .names = (const dmi_name_t[]) {
        DMI_NAME_UNSPEC(DMI_DELL_INFRARED_PROTO_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_INFRARED_PROTO_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_INFRARED_PROTO_UNKNOWN),
        {
            .id   = DMI_DELL_INFRARED_PROTO_SIR,
            .code = "sir",
            .name = "SIR (Standard IR)"
        },
        {
            .id   = DMI_DELL_INFRARED_PROTO_FIR,
            .code = "fir",
            .name = "FIR (Fast IR)"
        },
        {
            .id   = DMI_DELL_INFRARED_PROTO_MIR,
            .code = "mir",
            .name = "MIR (Medium Speed IR)"
        },
        {}
    }
};

const char *dmi_dell_infrared_proto_name(dmi_dell_infrared_proto_t value)
{
    return dmi_name_lookup(&dmi_dell_infrared_proto_names, (int)value);
}
