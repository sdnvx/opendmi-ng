//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-module-internal.h>

static void dmi_memory_module_decode_size(dmi_memory_module_size_t *psize, dmi_byte_t value);

//
// Sizes are carried as the power of two they are a number of megabytes of,
// together with the flags saying how the module is filled in.
//
static bool dmi_memory_module_decode_size_field(
        const dmi_field_data_t *data,
        void                   *value,
        const char             *name)
{
    dmi_memory_module_size_t *size = value;
    dmi_byte_t                raw  = (dmi_byte_t)data->number;

    dmi_memory_module_decode_size(size, raw);

    if ((size->status == DMI_MEMORY_MODULE_SIZE_STATUS_INVALID) and (data->entity != nullptr)) {
        dmi_log_warning(dmi_entity_context(data->entity),
                        "%s memory size is out of range: 0x%04hX: 0x%02hX",
                        name, dmi_entity_handle(data->entity), raw);
    }

    return true;
}

bool dmi_memory_module_decode_installed_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    return dmi_memory_module_decode_size_field(data, value, "Installed");
}

bool dmi_memory_module_decode_enabled_size(
        const dmi_field_t      *field,
        const dmi_field_data_t *data,
        void                   *value)
{
    dmi_unused(field);

    return dmi_memory_module_decode_size_field(data, value, "Enabled");
}

static void dmi_memory_module_decode_size(dmi_memory_module_size_t *psize, dmi_byte_t value)
{
    assert(psize != nullptr);

    psize->value      = 0;
    psize->bank_count = value & 0x80u ? 2 : 1;

    dmi_byte_t power = value & 0x7Fu;

    switch (power) {
    case 0x7Fu:
        psize->status = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_INSTALLED;
        break;

    case 0x7Eu:
        psize->status = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_ENABLED;
        break;

    case 0x7Du:
        psize->status = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_DETERMINABLE;
        break;

    default:
        if ((uint64_t)power < (sizeof(uint64_t) * CHAR_BIT - 20)) {
            psize->value  = ((dmi_size_t)1 << power) << 20;
            psize->status = DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT;
        } else {
            psize->status = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID;
        }
    }
}

//
// Size is written as the power of two it is a number of megabytes of, or as
// the value saying why there is no size, along with the number of the banks.
//
bool dmi_memory_module_encode_size(
        const dmi_field_t *field,
        const void        *value,
        dmi_field_data_t  *data)
{
    dmi_unused(field);

    const dmi_memory_module_size_t *size = value;

    dmi_byte_t power;

    switch (size->status) {
    case DMI_MEMORY_MODULE_SIZE_STATUS_NOT_INSTALLED:
        power = 0x7Fu;
        break;

    case DMI_MEMORY_MODULE_SIZE_STATUS_NOT_ENABLED:
        power = 0x7Eu;
        break;

    case DMI_MEMORY_MODULE_SIZE_STATUS_NOT_DETERMINABLE:
        power = 0x7Du;
        break;

    case DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT:
        power = 0;
        while ((power < 43) and ((((dmi_size_t)1 << power) << 20) < size->value))
            power++;
        break;

    default:
        // Powers too large for a size stand for the ones out of range
        power = 0x7Cu;
        break;
    }

    data->number = power | ((size->bank_count > 1) ? 0x80u : 0x00u);

    return true;
}
