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
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x0C,
        .decoded_length  = sizeof(dmi_memory_module_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_STRING(dmi_memory_module_t, socket),

        // One byte holding the two banks the module connects to
        DMI_FIELD_BITS(dmi_memory_module_t, bank_connections[0], 4),
        DMI_FIELD_BITS(dmi_memory_module_t, bank_connections[1], 4),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD(dmi_memory_module_t, current_speed, dmi_byte_t),
        DMI_FIELD(dmi_memory_module_t, current_type,  dmi_word_t),

        // Sizes are carried as the power of two they are a number of
        // megabytes of, together with the flags of the module
        DMI_FIELD(dmi_memory_module_t, installed_size, dmi_byte_t,
                  .decode = dmi_memory_module_decode_installed_size,
                  .encode = dmi_memory_module_encode_size),
        DMI_FIELD(dmi_memory_module_t, enabled_size, dmi_byte_t,
                  .decode = dmi_memory_module_decode_enabled_size,
                  .encode = dmi_memory_module_encode_size),

        DMI_FIELD(dmi_memory_module_t, error_status, dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
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
            .attrs  = DMI_ATTRIBUTES({
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
            })
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, enabled_size, STRUCT, {
            .code   = "enabled-size",
            .name   = "Enabled size",
            .attrs  = DMI_ATTRIBUTES({
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
            })
        }),
        DMI_ATTRIBUTE(dmi_memory_module_t, error_status, SET, {
            .code   = "error-status",
            .name   = "Error status",
            .values = &dmi_memory_module_error_names
        }),
        {}
    })
};
