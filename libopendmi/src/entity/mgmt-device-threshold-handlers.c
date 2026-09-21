//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/value.h>

#include <opendmi/entity/mgmt-device-threshold-internal.h>

void dmi_mgmt_device_threshold_set_component(dmi_entity_t *entity, dmi_type_t type)
{
    dmi_mgmt_device_threshold_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_DEVICE_THRESHOLD));
    if ((info == nullptr) or info->is_ambiguous)
        return;

    if (info->component_type == DMI_TYPE_INVALID) {
        info->component_type = type;
    } else if (info->component_type != type) {
        info->component_type = DMI_TYPE_INVALID;
        info->is_ambiguous   = true;
    }
}

//
// Thresholds of a structure which firmware has left as a template, with the
// ordinals of the fields in place of the values.
//
bool dmi_mgmt_device_threshold_is_template(const dmi_mgmt_device_threshold_t *info)
{
    const short values[] =
    {
        info->lower_non_critical,    info->upper_non_critical,
        info->lower_critical,        info->upper_critical,
        info->lower_non_recoverable, info->upper_non_recoverable
    };

    // Fields of the structure follow each other, so the ordinals differ by
    // one, and start where the first field is
    for (size_t i = 1; i < countof(values); i++) {
        if (values[i] != values[i - 1] + 1)
            return false;
    }

    return true;
}
