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
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    dmi_chassis_element_t *element = value;
    dmi_byte_t             raw     = (dmi_byte_t)data->number;

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
bool dmi_chassis_decode_maximum_count(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    return dmi_field_set(field, value, (data->number != 0) ? data->number : UINTMAX_MAX);
}

void dmi_chassis_cleanup(dmi_entity_t *entity)
{
    dmi_chassis_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(CHASSIS));
    if (info == nullptr)
        return;

    dmi_free(info->elements);
}

bool dmi_chassis_encode_element_type(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    const dmi_chassis_element_t *element = value;

    if (element->type != DMI_TYPE_INVALID)
        data->number = 0x80u | ((unsigned)element->type & 0x7Fu);
    else
        data->number = (unsigned)element->board_type & 0x7Fu;

    return true;
}

bool dmi_chassis_encode_maximum_count(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t count = dmi_field_get(field, value);

    data->number = (count != UINTMAX_MAX) ? count : 0;

    return true;
}
