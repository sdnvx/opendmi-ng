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

#include "ipmi-device-internal.h"

const dmi_name_set_t dmi_ipmi_iface_type_names =
{
    .code  = "ipmi-interface-type",
    .names = DMI_NAMES({
        DMI_NAME_UNKNOWN(DMI_IPMI_IFACE_TYPE_UNKNOWN),
        {
            .id   = DMI_IPMI_IFACE_TYPE_KCS,
            .code = "kcs",
            .name = "KCS: Keyboard Controller Style"
        },
        {
            .id   = DMI_IPMI_IFACE_TYPE_SMIC,
            .code = "smic",
            .name = "SMIC: Server Management Interface Chip"
        },
        {
            .id   = DMI_IPMI_IFACE_TYPE_BT,
            .code = "bt",
            .name = "BT: Block Transfer"
        },
        {
            .id   = DMI_IPMI_IFACE_TYPE_SSIF,
            .code = "ssif",
            .name = "SSIF: SMBus System Interface"
        },
        {}
    })
};

const dmi_name_set_t dmi_ipmi_addr_type_names =
{
    .code  = "ipmi-addr-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_IPMI_ADDR_TYPE_MEMORY,
            .code = "memory",
            .name = "Memory"

        },
        {
            .id   = DMI_IPMI_ADDR_TYPE_IO,
            .code = "io",
            .name = "I/O"
        },
        {
            .id   = DMI_IPMI_ADDR_TYPE_SMBUS,
            .code = "smbus",
            .name = "SMBus"
        },
        {}
    })
};

const dmi_name_set_t dmi_ipmi_intr_trigger_mode_names =
{
    .code  = "ipmi-interrupt-trigger-mode",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_IPMI_INTR_TRIGGER_MODE_UNSPEC),
        {
            .id   = DMI_IPMI_INTR_TRIGGER_MODE_EDGE,
            .code = "edge",
            .name = "Edge"
        },
        {
            .id   = DMI_IPMI_INTR_TRIGGER_MODE_LEVEL,
            .code = "level",
            .name = "Level"
        },
        {}
    })
};

const dmi_name_set_t dmi_ipmi_intr_polarity_names =
{
    .code  = "ipmi-intr-polarity",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_IPMI_INTR_POLARITY_UNSPEC),
        {
            .id   = DMI_IPMI_INTR_POLARITY_LOW,
            .code = "active-low",
            .name = "Active low"
        },
        {
            .id   = DMI_IPMI_INTR_POLARITY_HIGH,
            .code = "active-high",
            .name = "Active high"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_ipmi_iface_type)
DMI_NAME_FUNCTION(dmi_ipmi_addr_type)
DMI_NAME_FUNCTION(dmi_ipmi_intr_trigger_mode)
DMI_NAME_FUNCTION(dmi_ipmi_intr_polarity)
