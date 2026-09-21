//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/ipmi-device-internal.h>

const dmi_entity_spec_t dmi_ipmi_device_spec =
{
    .code            = "ipmi-device",
    .name            = "IPMI device information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of an "
        "Intelligent Platform Management Interface (IPMI) Baseboard "
        "Management Controller (BMC). See the Intelligent Platform Management "
        "Interface (IPMI) Interface Specification for full documentation of "
        "IPMI and additional information on the use of this structure.",
        //
        nullptr
    },
    .type            = DMI_TYPE(IPMI_DEVICE),
    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x12,
        .decoded_length  = sizeof(dmi_ipmi_device_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_ipmi_device_t, interface_type, BYTE),

        // Revision is one nibble of major and one of minor
        DMI_FIELD(dmi_ipmi_device_t, spec_version, BYTE,
                  .decode = dmi_ipmi_device_decode_version,
                  .encode = dmi_ipmi_device_encode_version),

        DMI_FIELD(dmi_ipmi_device_t, i2c_target_addr, BYTE),
        DMI_FIELD(dmi_ipmi_device_t, nv_storage_addr, BYTE),

        // Base address and the byte after it, which holds the modifier of the
        // address along with the interrupt information: the address means
        // what the interface type and the modifier say it does, so the two
        // are read as one
        DMI_FIELD_SPLIT(dmi_ipmi_device_t, BINARY,
                        .length = sizeof(dmi_qword_t) + sizeof(dmi_byte_t),
                        .decode = dmi_ipmi_device_decode_address,
                        .encode = dmi_ipmi_device_encode_address),

        DMI_FIELD(dmi_ipmi_device_t, intr_number, BYTE),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_ipmi_device_t, interface_type, ENUM, {
            .code   = "interface-type",
            .name   = "Interface type",
            .values = &dmi_ipmi_interface_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, spec_version, VERSION, {
            .code   = "specification-version",
            .name   = "Specification version",
            .scale  = 2
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, i2c_target_addr, INTEGER, {
            .code   = "i2c-target-address",
            .name   = "I2C target address",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, nv_storage_addr, INTEGER, {
            .code   = "nv-storage-address",
            .name   = "NV storage address",
            .unspec = dmi_value_ptr((uint8_t)UINT8_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        // SMBus target address of SSIF interface is shown as a short number
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "base-address",
            .name     = "Base address",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_IPMI_ADDR_TYPE_SMBUS, dmi_ipmi_device_t, base_addr, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                DMI_VARIANT_DEFAULT(dmi_ipmi_device_t, base_addr, ADDRESS, {}),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "base-address-lsb",
            .name     = "Base address LSB",
            .variants = dmi_ipmi_register_variants(base_addr_lsb, {})
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, base_addr_type, ENUM, {
            .code   = "base-address-type",
            .name   = "Base address type",
            .values = &dmi_ipmi_addr_type_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_trigger, ENUM, {
            .code   = "interrupt-trigger-mode",
            .name   = "Interrupt trigger mode",
            .unspec = dmi_value_ptr(DMI_IPMI_INTR_TRIGGER_UNSPEC),
            .values = &dmi_ipmi_intr_trigger_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_polarity, ENUM, {
            .code   = "interrupt-polarity",
            .name   = "Interrupt polarity",
            .unspec = dmi_value_ptr(DMI_IPMI_INTR_POLARITY_UNSPEC),
            .values = &dmi_ipmi_intr_polarity_names
        }),
        DMI_ATTRIBUTE(dmi_ipmi_device_t, intr_number, INTEGER, {
            .code   = "interrupt-number",
            .name   = "Interrupt number",
            .unspec = dmi_value_ptr((unsigned short)0)
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_ipmi_device_t, base_addr_type, {
            .code     = "register-spacing",
            .name     = "Register spacing",
            .variants = dmi_ipmi_register_variants(register_spacing, {
                .unit   = DMI_UNIT_BYTE,
                .unspec = dmi_value_ptr((unsigned short)0)
            })
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("ipmi-device.revision", dmi_ipmi_device_lint_revision, {
            .name              = "Revision of the IPMI specification is a binary-coded decimal",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    })
};
