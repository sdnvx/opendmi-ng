//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/value.h>

#include <opendmi/entity/memory-device-internal.h>

const dmi_entity_spec_t dmi_memory_device_spec =
{
    .code            = "memory-device",
    .name            = "Memory device",
    .description     = (const char *[]){
        "This structure describes a single memory device that is part of a "
        "larger Physical Memory Array (Type 16) structure.",
        //
        "Note: If a system includes memory-device sockets, the SMBIOS "
        "implementation includes a Memory Device structure instance for each "
        "slot, whether the socket is currently populated.",
        //
        nullptr
    },
    .type            = DMI_TYPE(MEMORY_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 1, 0),
        .required_from   = DMI_VERSION(2, 3, 0),
        .minimum_length  = 0x15,
        .decoded_length  = sizeof(dmi_memory_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_memory_device_t, array_handle,       WORD),
        DMI_FIELD(dmi_memory_device_t, error_info_handle,  WORD),
        DMI_FIELD(dmi_memory_device_t, total_width,        WORD, .unknown_raw = 0xFFFF),
        DMI_FIELD(dmi_memory_device_t, data_width,         WORD, .unknown_raw = 0xFFFF),
        DMI_FIELD(dmi_memory_device_t, size,               WORD,
                  .unknown_raw = 0xFFFF,
                  .decode      = dmi_memory_device_decode_size,
                  .encode      = dmi_memory_device_encode_size),
        DMI_FIELD(dmi_memory_device_t, form_factor,        BYTE),
        DMI_FIELD(dmi_memory_device_t, device_set,         BYTE, .unknown_raw = 0xFF),
        DMI_FIELD(dmi_memory_device_t, device_locator,     STRING),
        DMI_FIELD(dmi_memory_device_t, bank_locator,       STRING),
        DMI_FIELD(dmi_memory_device_t, memory_type,        BYTE),
        DMI_FIELD(dmi_memory_device_t, memory_type_detail, WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 3, 0)),
        DMI_FIELD(dmi_memory_device_t, maximum_speed, WORD),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_memory_device_t, vendor,        STRING),
        DMI_FIELD(dmi_memory_device_t, serial_number, STRING),
        DMI_FIELD(dmi_memory_device_t, asset_tag,     STRING),
        DMI_FIELD(dmi_memory_device_t, part_number,   STRING),

        // Rank is held by the low bits, and the rest of the byte is reserved
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 6, 0)),
        DMI_FIELD_BITS(dmi_memory_device_t, rank, 4),
        DMI_FIELD_PAD(BYTE),

        // Sizes and speeds which do not fit their plain fields are carried by
        // the extended ones, which the plain fields point at
        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 7, 0)),
        DMI_FIELD_EXTENDED(dmi_memory_device_t, size, DWORD,
                           .when_raw = 0x7FFF,
                           .decode   = dmi_memory_device_decode_size_ex,
                           .encode   = dmi_memory_device_encode_size_ex),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_memory_device_t, configured_speed, WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(2, 8, 0)),
        DMI_FIELD(dmi_memory_device_t, minimum_voltage,    WORD),
        DMI_FIELD(dmi_memory_device_t, maximum_voltage,    WORD),
        DMI_FIELD(dmi_memory_device_t, configured_voltage, WORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 2, 0)),
        DMI_FIELD(dmi_memory_device_t, memory_tech,           BYTE),
        DMI_FIELD(dmi_memory_device_t, memory_mode_caps,      WORD),
        DMI_FIELD(dmi_memory_device_t, firmware_version,      STRING),
        DMI_FIELD(dmi_memory_device_t, module_vendor_id,      WORD),
        DMI_FIELD(dmi_memory_device_t, module_product_id,     WORD),
        DMI_FIELD(dmi_memory_device_t, controller_vendor_id,  WORD),
        DMI_FIELD(dmi_memory_device_t, controller_product_id, WORD),
        DMI_FIELD(dmi_memory_device_t, non_volatile_size,     QWORD),
        DMI_FIELD(dmi_memory_device_t, volatile_size,         QWORD),
        DMI_FIELD(dmi_memory_device_t, cache_size,            QWORD),
        DMI_FIELD(dmi_memory_device_t, logical_size,          QWORD),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 3, 0)),
        DMI_FIELD_EXTENDED(dmi_memory_device_t, maximum_speed,    DWORD, .when_raw = 0xFFFF),
        DMI_FIELD_EXTENDED(dmi_memory_device_t, configured_speed, DWORD, .when_raw = 0xFFFF),

        DMI_FIELD_GROUP(.since = DMI_VERSION(3, 7, 0)),
        DMI_FIELD(dmi_memory_device_t, pmic0_vendor_id, WORD),
        DMI_FIELD(dmi_memory_device_t, pmic0_revision,  WORD),
        DMI_FIELD(dmi_memory_device_t, rcd_vendor_id,   WORD),
        DMI_FIELD(dmi_memory_device_t, rcd_revision,    WORD),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_memory_device_t, array_handle, HANDLE, {
            .code    = "array-handle",
            .name    = "Memory array handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_ARRAY),
            .link    = dmi_member(dmi_memory_device_t, array)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, error_info_handle, HANDLE, {
            .code    = "error-info-handle",
            .name    = "Memory error information handle",
            .targets = dmi_types(DMI_TYPE_MEMORY_ERROR_32, DMI_TYPE_MEMORY_ERROR_64),
            .link    = dmi_member(dmi_memory_device_t, error_info)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, total_width, INTEGER, {
            .code    = "total-width",
            .name    = "Total width",
            .unit    = DMI_UNIT_BIT,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, data_width, INTEGER, {
            .code    = "data-width",
            .name    = "Data width",
            .unit    = DMI_UNIT_BIT,
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, size, SIZE, {
            .code    = "size",
            .name    = "Size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, form_factor, ENUM, {
            .code    = "form-factor",
            .name    = "Form-factor",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_FORM_FACTOR_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_FORM_FACTOR_UNKNOWN),
            .values  = &dmi_memory_device_form_factor_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, device_set, INTEGER, {
            .code    = "device-set",
            .name    = "Device set",
            .unknown = dmi_value_ptr((unsigned short)USHRT_MAX)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, device_locator, STRING, {
            .code    = "device-locator",
            .name    = "Device locator"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, bank_locator, STRING, {
            .code    = "bank-locator",
            .name    = "Bank locator"
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_type, ENUM, {
            .code    = "memory-type",
            .name    = "Memory type",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_TYPE_UNKNOWN),
            .values  = &dmi_memory_device_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_type_detail, SET, {
            .code    = "memory-type-details",
            .name    = "Memory type details",
            .values  = &dmi_memory_device_type_detail_names
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, maximum_speed, INTEGER, {
            .code    = "maximum-speed",
            .name    = "Maximum speed",
            .unit    = DMI_UNIT_MEGAXA_SECOND,
            .unknown = dmi_value_ptr((unsigned long)0),
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Manufacturer",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset tag",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, part_number, STRING, {
            .code    = "part-number",
            .name    = "Part number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rank, INTEGER, {
            .code    = "rank",
            .name    = "Rank",
            .minimum = dmi_value_ptr((unsigned short)0),
            .maximum = dmi_value_ptr((unsigned short)15),
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 6, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, configured_speed, INTEGER, {
            .code    = "configured-speed",
            .name    = "Configured speed",
            .unit    = DMI_UNIT_MEGAXA_SECOND,
            .unknown = dmi_value_ptr((unsigned long)0),
            .level   = DMI_VERSION(2, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, minimum_voltage, DECIMAL, {
            .code    = "minimum-voltage",
            .name    = "Minimum voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, maximum_voltage, DECIMAL, {
            .code    = "maximum-voltage",
            .name    = "Maximum voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, configured_voltage, DECIMAL, {
            .code    = "configured-voltage",
            .name    = "Configured voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 3,
            .unknown = dmi_value_ptr((unsigned short)0),
            .level   = DMI_VERSION(2, 8, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_tech, ENUM, {
            .code    = "memory-technology",
            .name    = "Memory technology",
            .unspec  = dmi_value_ptr(DMI_MEMORY_DEVICE_TECH_UNSPEC),
            .unknown = dmi_value_ptr(DMI_MEMORY_DEVICE_TECH_UNKNOWN),
            .values  = &dmi_memory_device_tech_names,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, memory_mode_caps, INTEGER, {
            .code    = "memory-mode-caps",
            .name    = "Memory operating mode capabilities",
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, firmware_version, STRING, {
            .code    = "firmware-version",
            .name    = "Firmware version",
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, module_vendor_id, INTEGER, {
            .code    = "module-vendor-id",
            .name    = "Module manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_JEP106,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, module_product_id, INTEGER, {
            .code    = "module-product-id",
            .name    = "Module product ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, controller_vendor_id, INTEGER, {
            .code    = "controller-vendor-id",
            .name    = "Memory subsystem controller manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_JEP106,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, controller_product_id, INTEGER, {
            .code    = "controller-product-id",
            .name    = "Memory subsystem controller product ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, non_volatile_size, SIZE, {
            .code    = "non-volatile-size",
            .name    = "Non-volatile size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, volatile_size, SIZE, {
            .code    = "volatile-size",
            .name    = "Volatile size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, cache_size, SIZE, {
            .code    = "cache-size",
            .name    = "Cache size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, logical_size, SIZE, {
            .code    = "logical-size",
            .name    = "Logical size",
            .unknown = dmi_value_ptr(DMI_SIZE_MAX),
            .level   = DMI_VERSION(3, 2, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, pmic0_vendor_id, INTEGER, {
            .code    = "pmi0-vendor-id",
            .name    = "PMIC0 manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_JEP106,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, pmic0_revision, INTEGER, {
            .code    = "pmic0-revision",
            .name    = "PMIC0 revision number",
            .unknown = dmi_value_ptr((uint16_t)0xFF00u),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rcd_vendor_id, INTEGER, {
            .code    = "rcd-vendor-id",
            .name    = "RCD manufacturer ID",
            .unknown = dmi_value_ptr((uint16_t)0),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX | DMI_ATTRIBUTE_FLAG_JEP106,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        DMI_ATTRIBUTE(dmi_memory_device_t, rcd_revision, INTEGER, {
            .code    = "rcd-revision",
            .name    = "RCD revision number",
            .unknown = dmi_value_ptr((uint16_t)0xFF00u),
            .flags   = DMI_ATTRIBUTE_FLAG_HEX,
            .level   = DMI_VERSION(3, 7, 0)
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("memory-device.extended-size", dmi_memory_device_lint_extended_size, {
            .name              = "Extended size is present when the plain one needs it",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-device.width", dmi_memory_device_lint_width, {
            .name              = "Total width of the device covers its data width",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-device.speed", dmi_memory_device_lint_speed, {
            .name              = "Device is configured no faster than it is capable of",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_WARNING
        }),
        DMI_LINT_RULE("memory-device.voltage", dmi_memory_device_lint_voltage, {
            .name              = "Configured voltage of the device is within its limits",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("memory-device.sizes", dmi_memory_device_lint_sizes, {
            .name              = "Volatile and non-volatile sizes fit the size of the device",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
