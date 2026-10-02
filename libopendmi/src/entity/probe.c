//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <limits.h>

#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/entity/probe.h>

#include "probe-internal.h"

const dmi_field_t dmi_probe_fields[] =
{
    DMI_FIELD_STRING(dmi_probe_t, description),

    DMI_FIELD_BITS(dmi_probe_t, location, 5),
    DMI_FIELD_BITS(dmi_probe_t, status,   3),
    DMI_FIELD_PAD(dmi_byte_t),

    DMI_FIELD(dmi_probe_t, maximum_value, dmi_word_t),
    DMI_FIELD(dmi_probe_t, minimum_value, dmi_word_t),
    DMI_FIELD(dmi_probe_t, resolution,    dmi_word_t),
    DMI_FIELD(dmi_probe_t, tolerance,     dmi_word_t),
    DMI_FIELD(dmi_probe_t, accuracy,      dmi_word_t),
    DMI_FIELD(dmi_probe_t, oem_defined,   dmi_dword_t),

    // Probes which read nothing of their own carry no value
    DMI_FIELD_PRESET(dmi_probe_t, nominal_value, (short)SHRT_MIN),
    DMI_FIELD_GROUP(),
    DMI_FIELD(dmi_probe_t, nominal_value, dmi_word_t),
    {}
};
