//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <ctype.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/stream.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache.h>
#include <opendmi/entity/processor-internal.h>

static const dmi_attribute_t dmi_processor_x86_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, type, INTEGER, {
        .code = "type",
        .name = "Type"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, family, INTEGER, {
        .code = "family",
        .name = "Family"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, model, INTEGER, {
        .code = "model",
        .name = "Model"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, stepping, INTEGER, {
        .code = "stepping",
        .name = "Stepping"
    }),
    {}
};

static const dmi_attribute_t dmi_processor_arm_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, implementer, INTEGER, {
        .code  = "implementer",
        .name  = "Implementer",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, variant, INTEGER, {
        .code  = "variant",
        .name  = "Variant",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, architecture, INTEGER, {
        .code  = "architecture",
        .name  = "Architecture",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, part_number, INTEGER, {
        .code  = "part-number",
        .name  = "Part number",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, revision, INTEGER, {
        .code  = "revision",
        .name  = "Revision",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

static const dmi_attribute_t dmi_processor_soc_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, jep106_bank, INTEGER, {
        .code  = "jep106-bank",
        .name  = "JEP106 bank",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, jep106_id, INTEGER, {
        .code  = "jep106-id",
        .name  = "JEP106 identification code",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, soc_id, INTEGER, {
        .code  = "soc-id",
        .name  = "SoC ID",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, soc_revision, INTEGER, {
        .code  = "soc-revision",
        .name  = "SoC revision",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    {}
};

const dmi_entity_spec_t dmi_processor_spec =
{
    .code            = "processor",
    .name            = "Processor information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a single "
        "processor; a separate structure instance is provided for each system "
        "processor socket/slot. For example, a system with an Intel DX2 "
        "processor would have a single structure instance while a system with "
        "an Intel SX2 processor would have a structure to describe the main "
        "CPU and a second structure to describe the 80487 co-processor.",
        //
        "Note: One structure is provided for each processor instance in a "
        "system. For example, a system that supports up to two processors "
        "includes two Processor Information structures - even if only one "
        "processor is currently installed. Software that interprets the SMBIOS "
        "information can count the Processor Information structures to "
        "determine the maximum possible configuration of the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PROCESSOR),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .required_till   = DMI_VERSION_NONE,
        .minimum_length  = 0x1A,
        .decoded_length  = sizeof(dmi_processor_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_processor_t, socket_designation, STRING),
        DMI_FIELD(dmi_processor_t, type,               BYTE),
        DMI_FIELD(dmi_processor_t, family,             BYTE),
        DMI_FIELD(dmi_processor_t, vendor,             STRING),

        // Processor identifier is taken as stored, and is read according to
        // the family and the vendor once they are known
        DMI_FIELD_BINARY(dmi_processor_t, id, sizeof(dmi_qword_t)),

        DMI_FIELD(dmi_processor_t, version, STRING),

        // One byte holding either the current voltage or the ones supported
        DMI_FIELD_SPLIT(dmi_processor_t, .decode = dmi_processor_decode_voltage),

        DMI_FIELD(dmi_processor_t, external_clock, WORD),
        DMI_FIELD(dmi_processor_t, maximum_speed,  WORD),
        DMI_FIELD(dmi_processor_t, current_speed,  WORD),

        // One byte holding the status of the processor and whether the socket
        // it belongs to is populated at all
        DMI_FIELD_BITS(dmi_processor_t, status, 3),
        DMI_FIELD_BITS_SKIP(3),
        DMI_FIELD_BITS(dmi_processor_t, is_populated, 1),
        DMI_FIELD_PAD(BYTE),

        DMI_FIELD(dmi_processor_t, upgrade, BYTE),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 1, 0)),
        DMI_FIELD_PRESET(dmi_processor_t, l1_cache_handle, DMI_HANDLE_INVALID),
        DMI_FIELD_PRESET(dmi_processor_t, l2_cache_handle, DMI_HANDLE_INVALID),
        DMI_FIELD_PRESET(dmi_processor_t, l3_cache_handle, DMI_HANDLE_INVALID),
        DMI_FIELD(dmi_processor_t, l1_cache_handle, WORD),
        DMI_FIELD(dmi_processor_t, l2_cache_handle, WORD),
        DMI_FIELD(dmi_processor_t, l3_cache_handle, WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 3, 0)),
        DMI_FIELD(dmi_processor_t, serial_number, STRING),
        DMI_FIELD(dmi_processor_t, asset_tag,     STRING),
        DMI_FIELD(dmi_processor_t, part_number,   STRING),

        // Counts of one byte are too narrow for the processors of today, so
        // the wider ones were added to SMBIOS 3.0
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 5, 0)),
        DMI_FIELD(dmi_processor_t, core_count,   BYTE),
        DMI_FIELD(dmi_processor_t, core_enabled, BYTE),
        DMI_FIELD(dmi_processor_t, thread_count, BYTE),
        DMI_FIELD(dmi_processor_t, features,     WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 6, 0)),
        DMI_FIELD_EXTENDED(dmi_processor_t, family, WORD,
                           .when_raw = DMI_PROCESSOR_FAMILY_EXTENDED),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 0, 0)),
        DMI_FIELD_EXTENDED(dmi_processor_t, core_count,   WORD, .when_raw = 0xFFu),
        DMI_FIELD_EXTENDED(dmi_processor_t, core_enabled, WORD, .when_raw = 0xFFu),
        DMI_FIELD_EXTENDED(dmi_processor_t, thread_count, WORD, .when_raw = 0xFFu),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 6, 0)),
        DMI_FIELD(dmi_processor_t, thread_enabled, WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 8, 0)),
        DMI_FIELD(dmi_processor_t, socket_type, STRING),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_processor_t, socket_designation, STRING, {
            .code    = "socket-designation",
            .name    = "Socket designation"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_TYPE_UNKNOWN),
            .values  = &dmi_processor_type_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, family, ENUM, {
            .code    = "family",
            .name    = "Family",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_FAMILY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_FAMILY_UNKNOWN),
            .values  = &dmi_processor_family_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, id, BINARY, {
            .code    = "id",
            .name    = "ID"
        }),
        // Fields of processor ID depend on the processor architecture
        DMI_ATTRIBUTE_VARIANT(dmi_processor_t, id_format, {
            .code     = "signature",
            .name     = "Signature",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_X86, dmi_processor_t, x86_id, STRUCT, {
                    .attrs = dmi_processor_x86_id_attrs
                }),
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_MIDR, dmi_processor_t, arm_id, STRUCT, {
                    .attrs = dmi_processor_arm_id_attrs
                }),
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_SOC_ID, dmi_processor_t, soc_id, STRUCT, {
                    .attrs = dmi_processor_soc_id_attrs
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_processor_t, id_format, {
            .code     = "flags",
            .name     = "Flags",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_X86, dmi_processor_t, x86_id.features, SET, {
                    .values = &dmi_processor_x86_feature_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_processor_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, voltage, DECIMAL, {
            .code    = "voltage",
            .name    = "Voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 1,
            .unspec  = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, supported_voltages, SET, {
            .code    = "supported-voltages",
            .name    = "Supported voltages",
            .unspec  = dmi_value_ptr((dmi_byte_t)0),
            .values  = &dmi_processor_voltage_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, external_clock, INTEGER, {
            .code    = "external-clock",
            .name    = "External clock",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, maximum_speed, INTEGER, {
            .code    = "maximum-speed",
            .name    = "Maximum speed",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, current_speed, INTEGER, {
            .code    = "current-speed",
            .name    = "Current speed",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, is_populated, BOOL, {
            .code    = "is-populated",
            .name    = "Socket populated"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unknown = dmi_value_ptr(DMI_PROCESSOR_STATUS_UNKNOWN),
            .values  = &dmi_processor_status_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, upgrade, ENUM, {
            .code    = "upgrade",
            .name    = "Upgrade",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_UPGRADE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_UPGRADE_UNKNOWN),
            .values  = &dmi_processor_upgrade_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l1_cache_handle, HANDLE, {
            .code    = "l1-cache-handle",
            .name    = "L1 Cache handle",
            .targets = dmi_types(DMI_TYPE_CACHE),
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l2_cache_handle, HANDLE, {
            .code    = "l2-cache-handle",
            .name    = "L2 Cache handle",
            .targets = dmi_types(DMI_TYPE_CACHE),
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l3_cache_handle, HANDLE, {
            .code    = "l3-cache-handle",
            .name    = "L3 Cache handle",
            .targets = dmi_types(DMI_TYPE_CACHE),
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, part_number, STRING, {
            .code    = "part-number",
            .name    = "Part number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, features, SET, {
            .code    = "features",
            .name    = "Features",
            .values  = &dmi_processor_features_names,
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, core_count, INTEGER, {
            .code    = "core-count",
            .name    = "Core count",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, core_enabled, INTEGER, {
            .code    = "core-enabled",
            .name    = "Enabled cores",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, thread_count, INTEGER, {
            .code    = "thread-count",
            .name    = "Thread count",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, thread_enabled, INTEGER, {
            .code    = "thread-enabled",
            .name    = "Enabled threads",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(3, 6, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, socket_type, STRING, {
            .code    = "socket-type",
            .name    = "Socket type"
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("processor.family", dmi_processor_lint_family, {
            .name              = "Extended family is present when the plain one needs it",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("processor.id", dmi_processor_lint_id, {
            .name              = "Words of the processor identifier are in the order of the specification",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("processor.cores", dmi_processor_lint_cores, {
            .name              = "Enabled cores and threads fit the ones the processor has",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("processor.speed", dmi_processor_lint_speed, {
            .name              = "Processor runs no faster than it is capable of",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_WARNING
        }),
        DMI_LINT_RULE("processor.cache", dmi_processor_lint_cache, {
            .name              = "Cache handles of the processor refer to the caches of their levels",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_processor_derive,
        .link   = dmi_processor_link
    }
};
