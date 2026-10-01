//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include "memory-controller-internal.h"

bool dmi_memory_controller_decode_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    // Powers too large for the member are not sizes of any module
    if (data->number + 20 >= 63)
        return dmi_field_set(field, value, UINTMAX_MAX);

    return dmi_field_set(field, value, (uintmax_t)1 << (data->number + 20));
}

bool dmi_memory_controller_derive(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(memory_controller));
    if (info == nullptr)
        return false;

    if ((info->maximum_module_size == DMI_SIZE_MAX) or
        ((info->slot_count != 0) and (info->maximum_module_size > DMI_SIZE_MAX / info->slot_count)))
        info->maximum_memory_size = DMI_SIZE_MAX;
    else
        info->maximum_memory_size = info->maximum_module_size * info->slot_count;

    return true;
}

bool dmi_memory_controller_link(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(memory_controller));
    if (info == nullptr)
        return false;

    if (info->modules == nullptr)
        return true;

    for (size_t i = 0; i < info->slot_count; i++) {
        if (info->modules[i] == nullptr)
            continue;

        // Memory module may be left undecoded
        dmi_memory_module_t *module = dmi_entity_info(info->modules[i], DMI_TYPE(memory_module));
        if (module == nullptr)
            continue;

        // Bind memory controller to module
        module->controller = entity;
    }

    return true;
}

bool dmi_memory_controller_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    uintmax_t size  = dmi_field_get(field, value);
    unsigned  power = 0;

    while ((power + 20 < 63) and ((((uintmax_t)1) << (power + 20)) < size))
        power++;

    data->number = power;

    return true;
}
