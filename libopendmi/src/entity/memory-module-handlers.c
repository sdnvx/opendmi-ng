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
        dmi_entity_t *entity,
        void         *value,
        const char   *name)
{
    dmi_byte_t raw = 0;

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_byte_t, &raw))
        return false;

    dmi_memory_module_size_t *size = value;

    dmi_memory_module_decode_size(size, raw);

    if (size->status == DMI_MEMORY_MODULE_SIZE_STATUS_INVALID) {
        dmi_log_warning(dmi_entity_context(entity),
                        "%s memory size is out of range: 0x%04hX: 0x%02hX",
                        name, dmi_entity_handle(entity), raw);
    }

    return true;
}

bool dmi_memory_module_decode_installed_size(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    return dmi_memory_module_decode_size_field(entity, value, "Installed");
}

bool dmi_memory_module_decode_enabled_size(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    return dmi_memory_module_decode_size_field(entity, value, "Enabled");
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
