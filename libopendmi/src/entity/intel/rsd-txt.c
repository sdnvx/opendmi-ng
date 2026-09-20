//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-txt.h>

static bool dmi_intel_rsd_txt_decode(dmi_entity_t *entity);

const dmi_name_set_t dmi_intel_rsd_txt_status_names =
{
    .code  = "intel-rsd-txt-statuses",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_TXT_STATUS_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {
            .id   = DMI_INTEL_RSD_TXT_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {}
    }
};

const dmi_entity_spec_t dmi_intel_rsd_txt_spec =
{
    .code            = "intel-rsd-txt",
    .name            = "Intel RSD TXT information",
    .type            = DMI_TYPE(INTEL_RSD_TXT),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x05,
    .decoded_length  = sizeof(dmi_intel_rsd_txt_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_txt_t, status, ENUM, {
            .code   = "status",
            .name   = "Status",
            .values = &dmi_intel_rsd_txt_status_names
        }),
        {}
    },
    .handlers = {
        .decode = dmi_intel_rsd_txt_decode
    }
};

const char *dmi_intel_rsd_txt_status_name(dmi_intel_rsd_txt_status_t value)
{
    return dmi_name_lookup(&dmi_intel_rsd_txt_status_names, (int)value);
}

static bool dmi_intel_rsd_txt_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_txt_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_TXT));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = dmi_entity_stream(entity);

    return dmi_stream_decode(stream, dmi_byte_t, &info->status);
}
