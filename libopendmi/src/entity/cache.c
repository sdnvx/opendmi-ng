//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <inttypes.h>
#include <opendmi/context.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache-internal.h>

const dmi_entity_spec_t dmi_cache_spec =
{
    .code            = "cache",
    .name            = "Cache information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of CPU cache "
        "device in the system. One structure is specified for each such "
        "device, whether the device is internal to or external to the CPU "
        "module. Cache modules can be associated with a processor structure in "
        "one or two ways depending on the SMBIOS version.",
        //
        nullptr
    },
    .type            = DMI_TYPE(CACHE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x0F,
        .decoded_length  = sizeof(dmi_cache_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_cache_t, socket_designator, STRING),

        // One word holding the configuration of the cache
        DMI_FIELD_BITS(dmi_cache_t, level, 3, .convert = dmi_cache_convert_level),
        DMI_FIELD_BITS(dmi_cache_t, socketed, 1),
        DMI_FIELD_BITS_SKIP(1),
        DMI_FIELD_BITS(dmi_cache_t, location, 2),
        DMI_FIELD_BITS(dmi_cache_t, enabled,  1),
        DMI_FIELD_BITS(dmi_cache_t, mode,     2),
        DMI_FIELD_PAD(WORD),

        // Sizes are carried in granules of one or of sixty-four kibibytes,
        // and the caches too large for a word carry them in the extended
        // fields instead
        DMI_FIELD(dmi_cache_t, maximum_size,   WORD, .convert = dmi_cache_convert_size),
        DMI_FIELD(dmi_cache_t, installed_size, WORD, .convert = dmi_cache_convert_size),

        DMI_FIELD(dmi_cache_t, supported_sram, WORD),
        DMI_FIELD(dmi_cache_t, current_sram,   WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD(dmi_cache_t, speed,            BYTE),
        DMI_FIELD(dmi_cache_t, error_correction, BYTE),
        DMI_FIELD(dmi_cache_t, type,             BYTE),
        DMI_FIELD(dmi_cache_t, associativity,    BYTE),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 1, 0)),
        DMI_FIELD_EXTENDED(dmi_cache_t, maximum_size, DWORD,
                           .when_raw = 0xFFFFu,
                           .convert  = dmi_cache_convert_size_ex),
        DMI_FIELD_EXTENDED(dmi_cache_t, installed_size, DWORD,
                           .when_raw = 0xFFFFu,
                           .convert  = dmi_cache_convert_size_ex),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_cache_t, socket_designator, STRING, {
            .code    = "socket-designator",
            .name    = "Socket designator"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, level, INTEGER, {
            .code    = "level",
            .name    = "Cache level",
            .minimum = dmi_value_ptr((unsigned short)1),
            .maximum = dmi_value_ptr((unsigned short)8),
        }),
        DMI_ATTRIBUTE(dmi_cache_t, socketed, BOOL, {
            .code    = "socketed",
            .name    = "Socketed",
        }),
        DMI_ATTRIBUTE(dmi_cache_t, location, ENUM, {
            .code    = "location",
            .name    = "Location",
            .unknown = dmi_value_ptr(DMI_CACHE_LOCATION_UNKNOWN),
            .values  = &dmi_cache_location_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, enabled, BOOL, {
            .code    = "enabled",
            .name    = "Enabled"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, mode, ENUM, {
            .code    = "mode",
            .name    = "Operational mode",
            .unknown = dmi_value_ptr(DMI_CACHE_MODE_UNKNOWN),
            .values  = &dmi_cache_mode_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, maximum_size, SIZE, {
            .code    = "maximum-size",
            .name    = "Maximum cache size"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, installed_size, SIZE, {
            .code    = "installed-size",
            .name    = "Installed cache size"
        }),
        DMI_ATTRIBUTE(dmi_cache_t, supported_sram, SET, {
            .code    = "supported-sram",
            .name    = "Supported SRAM type",
            .values  = &dmi_cache_sram_type_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, current_sram, SET, {
            .code    = "current-sram",
            .name    = "Current SRAM type",
            .values  = &dmi_cache_sram_type_names
        }),
        DMI_ATTRIBUTE(dmi_cache_t, speed, INTEGER, {
            .code    = "speed",
            .name    = "Cache speed",
            .unit    = DMI_UNIT_NANOSECOND,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, error_correction, ENUM, {
            .code    = "error-correction",
            .name    = "Error correction type",
            .unspec  = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_ERROR_CORRECT_TYPE_UNKNOWN),
            .values  = &dmi_error_correct_type_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, type, ENUM, {
            .code    = "type",
            .name    = "System cache type",
            .unspec  = dmi_value_ptr(DMI_CACHE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CACHE_TYPE_UNKNOWN),
            .values  = &dmi_cache_type_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_cache_t, associativity, ENUM, {
            .code    = "associativity",
            .name    = "Associativity",
            .unspec  = dmi_value_ptr(DMI_CACHE_ASSOC_UNSPEC),
            .unknown = dmi_value_ptr(DMI_CACHE_ASSOC_UNKNOWN),
            .values  = &dmi_cache_assoc_names,
            .level   = DMI_VERSION(2, 1, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("cache.size", dmi_cache_lint_size, {
            .name              = "Installed size of the cache fits its maximum size",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("cache.sram", dmi_cache_lint_sram, {
            .name              = "Current SRAM type of the cache is one of the supported ones",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
