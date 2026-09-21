//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>

#include <opendmi/entity/system-boot-internal.h>

const dmi_entity_spec_t dmi_system_boot_spec =
{
    .code           = "system-boot",
    .name           = "System boot information",
    .description    = (const char *[]){
        "The client system firmware (for example, BIOS) communicates the "
        "System Boot Status to the client’s Pre-boot Execution Environment "
        "(PXE) boot image or OS-present management application through this "
        "structure.",
        //
        "When used in the PXE environment, for example, this code identifies "
        "the reason the PXE was initiated and can be used by boot-image "
        "software to further automate an enterprise’s PXE sessions. For "
        "example, an enterprise could choose to automatically download a "
        "hardware-diagnostic image to a client whose reason code indicated "
        "either a firmware- or an operating system-detected hardware failure.",
        //
        nullptr
    },
    .type           = DMI_TYPE(SYSTEM_BOOT),
    .params = {
        .required_from  = DMI_VERSION(2, 3, 0),
        .required_till  = DMI_VERSION_NONE,
        .minimum_length = 0x0B,
        .decoded_length = sizeof(dmi_system_boot_t)
    },

    .fields = DMI_FIELDS({
        // Reserved bytes
        DMI_FIELD_SKIP(6 * sizeof(dmi_byte_t)),

        // Boot status has variable length, and starts with the status code
        DMI_FIELD(dmi_system_boot_t, status, BYTE),
        DMI_FIELD_BINARY(dmi_system_boot_t, status_data, DMI_FIELD_LENGTH_REST),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_system_boot_t, status, ENUM, {
            .code   = "status",
            .name   = "Boot status",
            .values = &dmi_system_boot_status_names
        }),
        // Additional data is defined only for vendor and product codes
        DMI_ATTRIBUTE_VARIANT(dmi_system_boot_t, has_status_data, {
            .code     = "status-data",
            .name     = "Boot status data",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_system_boot_t, status_data, BINARY, {}),
                {}
            })
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_system_boot_derive
    }
};
