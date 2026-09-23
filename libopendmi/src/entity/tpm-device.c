//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/reader.h>
#include <opendmi/internal.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/tpm-device-internal.h>

const dmi_entity_spec_t dmi_tpm_device_spec =
{
    .code = "tpm-device",
    .name = "TPM device",
    .type = DMI_TYPE(TPM_DEVICE),

    .params = {
        .minimum_version = DMI_VERSION(2, 0, 0),
        .minimum_length  = 0x1F,
        .decoded_length  = sizeof(dmi_tpm_device_t)
    },

    .fields = DMI_FIELDS({
        // Vendor identifier is four bytes of text, which the specification
        // does not terminate
        DMI_FIELD(dmi_tpm_device_t, vendor_id, dmi_dword_t,
                  .decode = dmi_tpm_device_decode_vendor_id,
                  .encode = dmi_tpm_device_encode_vendor_id),

        // Specification version is one byte of major and one of minor
        DMI_FIELD(dmi_tpm_device_t, spec_version, dmi_word_t,
                  .decode = dmi_tpm_device_decode_version,
                  .encode = dmi_tpm_device_encode_version),

        // Firmware version is kept as stored, and is read according to the
        // version of the specification once that is known
        DMI_FIELD(dmi_tpm_device_t, firmware_version, dmi_qword_t,
                  .decode = dmi_tpm_device_decode_firmware_version,
                  .encode = dmi_tpm_device_encode_firmware_version),

        DMI_FIELD_STRING(dmi_tpm_device_t, description),
        DMI_FIELD(dmi_tpm_device_t, features,    dmi_qword_t),
        DMI_FIELD(dmi_tpm_device_t, oem_defined, dmi_dword_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_tpm_device_t, vendor, STRING, {
            .code   = "vendor-id",
            .name   = "Vendor ID"
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, spec_version, VERSION, {
            .code   = "specification-version",
            .name   = "Specification version",
            .scale  = 2
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_tpm_device_t, firmware_version_format, {
            .code     = "firmware-version",
            .name     = "Firmware version",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_1, dmi_tpm_device_t, firmware_revision, VERSION, {
                    .scale = 2
                }),
                DMI_VARIANT(DMI_TPM_FIRMWARE_VERSION_FORMAT_TPM_2, dmi_tpm_device_t, firmware_version_2, STRUCT, {
                    .attrs = DMI_ATTRIBUTES({
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, major, INTEGER, {
                            .code = "major",
                            .name = "Major"
                        }),
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, minor, INTEGER, {
                            .code = "minor",
                            .name = "Minor"
                        }),
                        DMI_ATTRIBUTE(dmi_tpm_firmware_version_t, vendor_specific, INTEGER, {
                            .code  = "vendor-specific",
                            .name  = "Vendor-specific",
                            .flags = DMI_ATTRIBUTE_FLAG_HEX
                        }),
                        {}
                    })
                }),
                DMI_VARIANT_DEFAULT(dmi_tpm_device_t, firmware_version, INTEGER, {
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, description, STRING, {
            .code   = "description",
            .name   = "Description"
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, features, SET, {
            .code   = "characteristics",
            .name   = "Characteristics",
            .values = &dmi_tpm_device_feature_names
        }),
        DMI_ATTRIBUTE(dmi_tpm_device_t, oem_defined, INTEGER, {
            .code   = "oem-defined",
            .name   = "OEM-defined",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        {}
    }),

    .lint_rules = DMI_LINT_RULES({
        DMI_LINT_RULE("tpm-device.vendor", dmi_tpm_device_lint_vendor, {
            .name              = "Vendor identifier is stored in the order of the specification",
            .severity          = DMI_LINT_SEVERITY_NOTE,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        DMI_LINT_RULE("tpm-device.version", dmi_tpm_device_lint_version, {
            .name              = "Specification version of the device is one the TCG has published",
            .severity          = DMI_LINT_SEVERITY_WARNING,
            .producer_severity = DMI_LINT_SEVERITY_ERROR
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_tpm_device_derive
    }
};
