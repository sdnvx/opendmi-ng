//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>
#include <opendmi/entity/memory-device.h>

#include <opendmi/entity/memory-channel-internal.h>

const dmi_name_set_t dmi_memory_channel_type_names =
{
    .code  = "memory-channel-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_MEMORY_CHANNEL_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_CHANNEL_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_CHANNEL_TYPE_UNKNOWN),
        {
            .id   = DMI_MEMORY_CHANNEL_TYPE_RAMBUS,
            .code = "rambus",
            .name = "RamBus"
        },
        {
            .id   = DMI_MEMORY_CHANNEL_TYPE_SYNCLINK,
            .code = "synclink",
            .name = "SyncLink"
        },
        {}
    })
};

const char *dmi_memory_channel_type_name(dmi_memory_channel_type_t value)
{
    return dmi_name_lookup(&dmi_memory_channel_type_names, (int)value);
}
