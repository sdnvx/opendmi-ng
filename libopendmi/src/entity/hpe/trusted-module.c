//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/hpe.h>

#include "trusted-module-internal.h"

const dmi_entity_spec_t dmi_hpe_trusted_module_spec =
{
    .type        = DMI_TYPE(hpe_trusted_module),
    .code        = "hpe-trusted-module",
    .name        = "HP/HPE trusted module status",
    .description = (const char *[]){
        "Tells whether a trusted module (TPM or TCM) is present and enabled, "
        "and what kind of module it is.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_hpe_trusted_module_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, presence,          2),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, is_orom_measuring, 1),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, is_hidden,         1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_trusted_module_t, has_extended_status)),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, disable_reason,  2),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, error_condition, 4),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, type,                  4),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, is_standard_algorithm, 1),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, is_chinese_algorithm,  1),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, mounting, 2),
        DMI_FIELD_BITS(dmi_hpe_trusted_module_t, fips,     2),
        DMI_FIELD_PAD(dmi_byte_t),

        DMI_FIELD(dmi_hpe_trusted_module_t, version_handle, dmi_word_t,
                  .absent = dmi_value_ptr(DMI_HANDLE_INVALID)),

        // Chip is told by the low byte of the identifier word
        DMI_FIELD_GROUP(.present = dmi_member(dmi_hpe_trusted_module_t, has_chip)),
        DMI_FIELD(dmi_hpe_trusted_module_t, chip, dmi_byte_t),
        DMI_FIELD_SKIP(sizeof(dmi_byte_t)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_trusted_module_t, presence, ENUM, {
            .code   = "presence",
            .name   = "Status",
            .values = &dmi_hpe_tm_presence_names
        }),
        DMI_ATTRIBUTE(dmi_hpe_trusted_module_t, is_orom_measuring, BOOL, {
            .code = "is-orom-measuring",
            .name = "Option ROM measuring"
        }),
        DMI_ATTRIBUTE(dmi_hpe_trusted_module_t, is_hidden, BOOL, {
            .code = "is-hidden",
            .name = "Hidden"
        }),
        // Reason is given for a disabled module only, and the error
        // condition for a module disabled for an error only
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, presence, {
            .code     = "disable-reason",
            .name     = "Disable reason",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_HPE_TM_PRESENCE_DISABLED, dmi_hpe_trusted_module_t, disable_reason, ENUM, {
                    .code   = "disable-reason",
                    .name   = "Disable reason",
                    .unspec = dmi_value_ptr(DMI_HPE_TM_DISABLE_REASON_UNSPEC),
                    .values = &dmi_hpe_tm_disable_reason_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, disable_reason, {
            .code     = "error-condition",
            .name     = "Error condition",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(DMI_HPE_TM_DISABLE_REASON_ERROR, dmi_hpe_trusted_module_t, error_condition, ENUM, {
                    .code   = "error-condition",
                    .name   = "Error condition",
                    .unspec = dmi_value_ptr(DMI_HPE_TM_ERROR_UNSPEC),
                    .values = &dmi_hpe_tm_error_names
                }),
                {}
            })
        }),

        // Rest of the fields is left out by the structures of 5 bytes
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "type",
            .name     = "Type",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, type, ENUM, {
                    .code   = "type",
                    .name   = "Type",
                    .unspec = dmi_value_ptr(DMI_HPE_TM_TYPE_UNSPEC),
                    .values = &dmi_hpe_tm_type_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "is-standard-algorithm",
            .name     = "Standard algorithm supported",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, is_standard_algorithm, BOOL, {
                    .code = "is-standard-algorithm",
                    .name = "Standard algorithm supported"
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "is-chinese-algorithm",
            .name     = "Chinese algorithm supported",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, is_chinese_algorithm, BOOL, {
                    .code = "is-chinese-algorithm",
                    .name = "Chinese algorithm supported"
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "mounting",
            .name     = "Trusted module attributes",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, mounting, ENUM, {
                    .code   = "mounting",
                    .name   = "Trusted module attributes",
                    .unspec = dmi_value_ptr(DMI_HPE_TM_MOUNTING_UNSPEC),
                    .values = &dmi_hpe_tm_mounting_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "fips",
            .name     = "FIPS certification",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, fips, ENUM, {
                    .code   = "fips",
                    .name   = "FIPS certification",
                    .unspec = dmi_value_ptr(DMI_HPE_TM_FIPS_UNSPEC),
                    .values = &dmi_hpe_tm_fips_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_extended_status, {
            .code     = "version-handle",
            .name     = "Version indicator handle",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, version_handle, HANDLE, {
                    .code    = "version-handle",
                    .name    = "Version indicator handle",
                    .unspec  = dmi_value_ptr(DMI_HANDLE_INVALID),
                    .targets = dmi_types(DMI_TYPE(hpe_version))
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_hpe_trusted_module_t, has_chip, {
            .code     = "chip",
            .name     = "Chip identifier",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_hpe_trusted_module_t, chip, ENUM, {
                    .code   = "chip",
                    .name   = "Chip identifier",
                    .values = &dmi_hpe_tm_chip_names
                }),
                {}
            })
        }),
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_presence_names =
{
    .code  = "hpe-tm-presence",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_PRESENCE_ABSENT,
            .code = "absent",
            .name = "Not present"
        },
        {
            .id   = DMI_HPE_TM_PRESENCE_ENABLED,
            .code = "enabled",
            .name = "Present and enabled"
        },
        {
            .id   = DMI_HPE_TM_PRESENCE_DISABLED,
            .code = "disabled",
            .name = "Present and disabled"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_disable_reason_names =
{
    .code  = "hpe-tm-disable-reason",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_DISABLE_REASON_USER,
            .code = "user",
            .name = "Disabled by the user"
        },
        {
            .id   = DMI_HPE_TM_DISABLE_REASON_ERROR,
            .code = "error",
            .name = "Error condition"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_type_names =
{
    .code  = "hpe-tm-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_TYPE_TPM_1_2,
            .code = "tpm-1.2",
            .name = "TPM 1.2"
        },
        {
            .id   = DMI_HPE_TM_TYPE_TPM_2_0,
            .code = "tpm-2.0",
            .name = "TPM 2.0"
        },
        {
            .id   = DMI_HPE_TM_TYPE_PTT,
            .code = "ptt",
            .name = "Intel PTT firmware TPM"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_mounting_names =
{
    .code  = "hpe-tm-mounting",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_MOUNTING_OPTIONAL,
            .code = "optional",
            .name = "Pluggable and optional"
        },
        {
            .id   = DMI_HPE_TM_MOUNTING_STANDARD,
            .code = "standard",
            .name = "Pluggable but standard"
        },
        {
            .id   = DMI_HPE_TM_MOUNTING_SOLDERED,
            .code = "soldered",
            .name = "Soldered down on the system board"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_fips_names =
{
    .code  = "hpe-tm-fips",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_FIPS_NOT_CERTIFIED,
            .code = "not-certified",
            .name = "Not FIPS certified"
        },
        {
            .id   = DMI_HPE_TM_FIPS_CERTIFIED,
            .code = "certified",
            .name = "FIPS certified"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_chip_names =
{
    .code  = "hpe-tm-chip",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_CHIP_NONE,
            .code = "none",
            .name = "None"
        },
        {
            .id   = DMI_HPE_TM_CHIP_STM_GEN10,
            .code = "stm-gen10",
            .name = "STMicro TPM of Gen10"
        },
        {
            .id   = DMI_HPE_TM_CHIP_PTT,
            .code = "ptt",
            .name = "Intel firmware TPM (PTT)"
        },
        {
            .id   = DMI_HPE_TM_CHIP_NATIONZ,
            .code = "nationz",
            .name = "Nationz TPM"
        },
        {
            .id   = DMI_HPE_TM_CHIP_STM_GEN10_PLUS,
            .code = "stm-gen10-plus",
            .name = "STMicro TPM of Gen10 Plus"
        },
        {
            .id   = DMI_HPE_TM_CHIP_STM_GEN11,
            .code = "stm-gen11",
            .name = "STMicro TPM of Gen11"
        },
        {
            .id   = DMI_HPE_TM_CHIP_STM_GEN12,
            .code = "stm-gen12",
            .name = "STMicro TPM of Gen12"
        },
        {}
    })
};

const dmi_name_set_t dmi_hpe_tm_error_names =
{
    .code  = "hpe-tm-error",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_TM_ERROR_SELF_TEST,
            .code = "self-test",
            .name = "Self-test failure"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hpe_tm_presence)
DMI_NAME_FUNCTION(dmi_hpe_tm_disable_reason)
DMI_NAME_FUNCTION(dmi_hpe_tm_type)
DMI_NAME_FUNCTION(dmi_hpe_tm_mounting)
DMI_NAME_FUNCTION(dmi_hpe_tm_fips)
DMI_NAME_FUNCTION(dmi_hpe_tm_chip)
DMI_NAME_FUNCTION(dmi_hpe_tm_error)
