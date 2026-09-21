//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/baseboard-internal.h>

const dmi_name_set_t dmi_baseboard_type_names =
{
    .code  = "baseboard-type",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_BASEBOARD_TYPE_UNSPEC),
        DMI_NAME_UNKNOWN(DMI_BASEBOARD_TYPE_UNKNOWN),
        DMI_NAME_OTHER(DMI_BASEBOARD_TYPE_OTHER),
        {
            .id   = DMI_BASEBOARD_TYPE_SERVER_BLADE,
            .code = "server-blade",
            .name = "Server blade"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_CONNECTIVITY_SWITCH,
            .code = "connectivity-switch",
            .name = "Connectivity switch"
        },
        {
            .id   =  DMI_BASEBOARD_TYPE_SYSTEM_MANAGEMENT_MODULE,
            .code = "system-management-module",
            .name = "System management module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_MODULE,
            .code = "processor-module",
            .name = "Processor module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_IO_MODULE,
            .code = "io-module",
            .name = "IO module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_MEMORY_MODULE,
            .code = "memory-module",
            .name = "Memory module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_DAUGHTERBOARD,
            .code = "daughterboard",
            .name = "Daughterboard"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_MOTHERBOARD,
            .code = "motherboard",
            .name = "Motherboard"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_MEMORY_MODULE,
            .code = "processor-memory-module",
            .name = "Processor/memory module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_PROCESSOR_IO_MODULE,
            .code = "processor-io-module",
            .name = "Processor/IO module"
        },
        {
            .id   = DMI_BASEBOARD_TYPE_INTERCONNECT_BOARD,
            .code = "interconnect-board",
            .name = "Interconnect board"
        },
        {}
    })
};

const dmi_name_set_t dmi_baseboard_feature_names =
{
    .code  = "baseboard-feature",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "is-hosting-board",
            .name = "Hosting board"
        },
        {
            .id   = 1,
            .code = "require-daughter-board",
            .name = "Require daughter board"
        },
        {
            .id   = 2,
            .code = "is-removable",
            .name = "Removable"
        },
        {
            .id   = 3,
            .code = "is-replaceable",
            .name = "Replaceable"
        },
        {
            .id   = 4,
            .code = "is-hot-swappable",
            .name = "Hot-swappable"
        },
        {}
    })
};

const char *dmi_baseboard_type_name(dmi_baseboard_type_t value)
{
    return dmi_name_lookup(&dmi_baseboard_type_names, (int)value);
}
