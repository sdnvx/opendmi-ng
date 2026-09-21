//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-tpm-internal.h>

const dmi_entity_spec_t dmi_intel_rsd_tpm_spec =
{
    .code = "intel-rsd-tpm",
    .name = "Intel RSD Trusted Platform Module (TPM) information",
    .type = DMI_TYPE(INTEL_RSD_TPM),

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x07,
        .decoded_length  = sizeof(dmi_intel_rsd_tpm_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_rsd_tpm_t, config_index, BYTE),
        DMI_FIELD(dmi_intel_rsd_tpm_t, version,      STRING),
        DMI_FIELD(dmi_intel_rsd_tpm_t, status,       BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_rsd_tpm_t, config_index, INTEGER, {
            .code   = "config-index",
            .name   = "Configuration index"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_tpm_t, version, STRING, {
            .code   = "version",
            .name   = "Version"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_tpm_t, status, ENUM, {
            .code   = "status",
            .name   = "Status",
            .values = &dmi_intel_rsd_tpm_status_names
        }),
        {}
    })
};
