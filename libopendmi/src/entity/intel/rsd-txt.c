//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-txt-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_txt_spec =
{
    .code = "intel-rsd-txt",
    .name = "Intel RSD TXT information",
    .type = DMI_TYPE(INTEL_RSD_TXT),

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x05,
        .decoded_length  = sizeof(dmi_intel_rsd_txt_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_txt_t, status, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_txt_t, status, ENUM, {
            .code   = "status",
            .name   = "Status",
            .values = &dmi_intel_rsd_txt_status_names
        }),
        {}
    })
};
