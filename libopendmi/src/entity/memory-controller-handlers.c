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

#include <opendmi/entity/memory-controller-internal.h>

//
// Module size is carried as the power of two it is a number of megabytes of.
//
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

//
// Memory the controller supports is what its slots hold when every one of
// them carries a module of the largest size.
//
bool dmi_memory_controller_derive(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    info->maximum_memory_size = info->maximum_module_size * info->slot_count;

    return true;
}

//
// Modules of a controller are linked by the attributes, and learn the
// controller they belong to here. The sizes they declare are checked against
// the largest one the controller supports by the lint rules, since linking
// leaves the members the data decodes into as they are.
//
bool dmi_memory_controller_link(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    if (info->modules == nullptr)
        return true;

    for (size_t i = 0; i < info->slot_count; i++) {
        if (info->modules[i] == nullptr)
            continue;

        // Memory module may be left undecoded
        dmi_memory_module_t *module = dmi_entity_info(info->modules[i], DMI_TYPE(MEMORY_MODULE));
        if (module == nullptr)
            continue;

        // Bind memory controller to module
        module->controller = entity;
    }

    return true;
}

void dmi_memory_controller_cleanup(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return;

    dmi_free(info->module_handles);
    dmi_free(info->modules);
}

//
// Module size is written as the power of two it is a number of megabytes of,
// which the widest size the member holds bounds.
//
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
