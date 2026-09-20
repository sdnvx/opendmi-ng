//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-tpm.h>

static bool dmi_intel_rsd_tpm_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_tpm_status_names =
{
    .code  = "intel-rsd-tpm-status",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_TPM_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_INTEL_RSD_TPM_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {}
    }
};

const dmi_entity_spec_t dmi_intel_rsd_tpm_spec =
{
    .code            = "intel-rsd-tpm",
    .name            = "Intel RSD Trusted Platform Module (TPM) information",
    .type            = DMI_TYPE(INTEL_RSD_TPM),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x07,
    .decoded_length  = sizeof(dmi_intel_rsd_tpm_t),
    .attributes      = (const dmi_attribute_t[]){
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
    },
    .handlers = {
        .decode = dmi_intel_rsd_tpm_decode
    }
};

const char *dmi_intel_rsd_tpm_status_name(dmi_intel_rsd_tpm_status_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_tpm_status_names, (int)value);
}

static bool dmi_intel_rsd_tpm_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_tpm_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_TPM));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return
        dmi_stream_decode(stream, dmi_byte_t, &info->config_index) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode(stream, dmi_byte_t, &info->status);
}
