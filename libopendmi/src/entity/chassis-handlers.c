//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/chassis-internal.h>

//
// An element is named by either an SMBIOS structure type or a baseboard type,
// which the most significant bit of the field tells apart.
//
bool dmi_chassis_decode_element_type(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_byte_t raw = 0;

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_byte_t, &raw))
        return false;

    dmi_chassis_element_t *element = value;

    if (raw & 0x80u) {
        element->type       = raw & 0x7Fu;
        element->board_type = DMI_BASEBOARD_TYPE_UNSPEC;
    } else {
        element->type       = DMI_TYPE_INVALID;
        element->board_type = raw;
    }

    return true;
}

//
// Elements which may be there in any number carry a maximum of zero, which
// the specification reserves for saying that there is no maximum.
//
uintmax_t dmi_chassis_convert_maximum_count(uintmax_t raw)
{
    return (raw != 0) ? raw : UINTMAX_MAX;
}

void dmi_chassis_cleanup(dmi_entity_t *entity)
{
    dmi_chassis_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    if (info == nullptr)
        return;

    dmi_free(info->elements);
}
