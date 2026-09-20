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

#include <opendmi/entity/memory-module.h>

static bool dmi_memory_module_decode(dmi_entity_t *entity);
static void dmi_memory_module_decode_size(dmi_memory_module_size_t *psize, dmi_byte_t value);

const dmi_name_set_t dmi_memory_module_type_names =
{
    .code  = "memory-module-types",
    .names = (const dmi_name_t[]){
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "standard",
            .name = "Standard"
        },
        {
            .id   = 3,
            .code = "fpm",
            .name = "Fast page mode"
        },
        {
            .id   = 4,
            .code = "edo",
            .name = "EDO"
        },
        {
            .id   = 5,
            .code = "parity",
            .name = "Parity"
        },
        {
            .id   = 6,
            .code = "ecc",
            .name = "ECC"
        },
        {
            .id   = 7,
            .code = "simm",
            .name = "SIMM"
        },
        {
            .id   = 8,
            .code = "dimm",
            .name = "DIMM"
        },
        {
            .id   = 9,
            .code = "burst-edo",
            .name = "Burst EDO"
        },
        {
            .id   = 10,
            .code = "sdram",
            .name = "SDRAM"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_module_size_status_names =
{
    .code = "memory-module-size-statuses",
    .names = (const dmi_name_t[]){
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID,
            .code = "invalid",
            .name = "Invalid"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT,
            .code = "present",
            .name = "Present"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_DETERMINABLE,
            .code = "not-determinable",
            .name = "Not determinable"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_ENABLED,
            .code = "not-enabled",
            .name = "Not enabled"
        },
        {
            .id   = DMI_MEMORY_MODULE_SIZE_STATUS_NOT_INSTALLED,
            .code = "not-installed",
            .name = "Not installed"
        },
        {}
    }
};

static const dmi_name_set_t dmi_memory_module_error_names =
{
    .code  = "memory-module-errors",
    .names = (const dmi_name_t[]){
        {
            .id   = 0,
            .code = "uncorrectable",
            .name = "Uncorrectable"
        },
        {
            .id   = 1,
            .code = "correctable",
            .name = "Correctable"
        },
        {
            .id   = 2,
            .code = "event-log",
            .name = "Event log"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_memory_module_spec =
{
    .code            = "memory-module",
    .name            = "Memory module information",
    .description     = (const char *[]){
        "One Memory Module Information structure is included for each "
        "memory-module socket in the system. The structure describes the "
        "speed, type, size, and error status of each system memory module. "
        "The supported attributes of each module are described by the "
        "\"owning\" Memory Controller Information structure.",
        //
        "Note: This structure and its companion Memory Controller Information "
        "(Type 5) are obsolete starting with version 2.1 of this "
        "specification; the Physical Memory Array (Type 16) and Memory Device "
        "(Type 17) structures should be used instead. BIOS providers might "
        "choose to implement both memory description types to allow existing "
        "DMI browsers to properly display the system’s memory attributes.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_MODULE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0C,
    .decoded_length  = sizeof(dmi_memory_module_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_memory_module_t, socket, STRING, {
            .code   = "socket",
            .name   = "Socket designator"
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, bank_connections[0], INTEGER, {
            .code   = "bank-connection-1",
            .name   = "Bank connection 1",
            .unspec = dmi_value_ptr((unsigned short)0xF)
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, bank_connections[1], INTEGER, {
            .code   = "bank-connection-2",
            .name   = "Bank connection 2",
            .unspec = dmi_value_ptr((unsigned short)0xF)
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, current_speed, INTEGER, {
            .code   = "current-speed",
            .name   = "Current speed",
            .unit   = DMI_UNIT_NANOSECOND,
            .unspec = dmi_value_ptr((unsigned short)0)
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, current_type, SET, {
            .code   = "current-type",
            .name   = "Current type",
            .values = &dmi_memory_module_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, installed_size, STRUCT, {
            .code   = "installed-size",
            .name   = "Installed size",
            .attrs  = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_memory_module_size_t, value, SIZE, {
                    .code   = "size",
                    .name   = "Size",
                    .unspec = dmi_value_ptr((dmi_size_t)0)
                }),
                DMI_ATTRIBUTE(dmi_memory_module_size_t, bank_count, INTEGER, {
                    .code   = "bank-count",
                    .name   = "Bank count"
                }),
                DMI_ATTRIBUTE(dmi_memory_module_size_t, status, ENUM, {
                    .code   = "status",
                    .name   = "Status",
                    .values = &dmi_memory_module_size_status_names
                }),
                {}
            }
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, enabled_size, STRUCT, {
            .code   = "enabled-size",
            .name   = "Enabled size",
            .attrs  = (const dmi_attribute_t[]){
                DMI_ATTRIBUTE(dmi_memory_module_size_t, value, SIZE, {
                    .code   = "size",
                    .name   = "Size",
                    .unspec = dmi_value_ptr((dmi_size_t)0)
                }),
                DMI_ATTRIBUTE(dmi_memory_module_size_t, bank_count, INTEGER, {
                    .code   = "bank-count",
                    .name   = "Bank count"
                }),
                DMI_ATTRIBUTE(dmi_memory_module_size_t, status, ENUM, {
                    .code   = "status",
                    .name   = "Status",
                    .values = &dmi_memory_module_size_status_names
                }),
                {}
            }
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, error_status, SET, {
            .code   = "error-status",
            .name   = "Error status",
            .values = &dmi_memory_module_error_names
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_memory_module_decode
    }
};

const char *dmi_memory_module_size_status_name(dmi_memory_module_size_status_t value)
{
    return dmi_name_lookup(&dmi_memory_module_size_status_names, (int)value);
}

static bool dmi_memory_module_decode(dmi_entity_t *entity)
{
    dmi_memory_module_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_MODULE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    dmi_byte_t bank_connections;
    dmi_byte_t installed_size;
    dmi_byte_t enabled_size;

    bool status =
        dmi_stream_decode_str(stream, &info->socket) and
        dmi_stream_decode(stream, dmi_byte_t, &bank_connections) and
        dmi_stream_decode(stream, dmi_byte_t, &info->current_speed) and
        dmi_stream_decode(stream, dmi_word_t, &info->current_type.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &installed_size) and
        dmi_stream_decode(stream, dmi_byte_t, &enabled_size) and
        dmi_stream_decode(stream, dmi_byte_t, &info->error_status.__value);
    if (not status)
        return false;

    // Decode bank connections
    info->bank_connections[0] = (bank_connections & 0x0Fu);
    info->bank_connections[1] = (bank_connections & 0xF0u) >> 4;

    // Decode installed size
    dmi_memory_module_decode_size(&info->installed_size, installed_size);
    if (info->installed_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_INVALID) {
        dmi_log_warning(context,
                        "Installed memory size is out of range: 0x%04hX: 0x%02hX",
                        dmi_entity_handle(entity), installed_size);
    }

    // Decode enabled size
    dmi_memory_module_decode_size(&info->enabled_size, enabled_size);
    if (info->enabled_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_INVALID) {
        dmi_log_warning(context,
                        "Enabled memory size is out of range: 0x%04hX: 0x%02hX",
                        dmi_entity_handle(entity), enabled_size);
    }

    return true;
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
