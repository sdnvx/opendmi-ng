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

#include "dimm-config-internal.h"

const dmi_entity_spec_t dmi_hpe_dimm_config_spec =
{
    .type        = DMI_TYPE(hpe_dimm_config),
    .code        = "hpe-dimm-config",
    .name        = "HP/HPE DIMM current configuration record",
    .description = (const char *[]){
        "Describes a memory region configured on a module of a memory device "
        "(type 17), and the interleave set it belongs to.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x14,
        .decoded_length = sizeof(dmi_hpe_dimm_config_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_hpe_dimm_config_t, device_handle, dmi_word_t),
        DMI_FIELD(dmi_hpe_dimm_config_t, region_id,     dmi_byte_t),

        DMI_FIELD_BITS(dmi_hpe_dimm_config_t, is_volatile,        1),
        DMI_FIELD_BITS(dmi_hpe_dimm_config_t, is_byte_accessible, 1),
        DMI_FIELD_BITS(dmi_hpe_dimm_config_t, is_block_io,        1),
        DMI_FIELD_PAD(dmi_word_t),

        DMI_FIELD(dmi_hpe_dimm_config_t, raw_size, dmi_qword_t),

        // Passphrase is enabled for any state other than zero
        DMI_FIELD(dmi_hpe_dimm_config_t, passphrase_state, dmi_byte_t),

        DMI_FIELD(dmi_hpe_dimm_config_t, interleave_set, dmi_word_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_config_t, interleave_dimm_count, dmi_byte_t),

        DMI_FIELD_GROUP(),
        DMI_FIELD(dmi_hpe_dimm_config_t, interleave_health, dmi_byte_t,
                  .absent = dmi_value_ptr(DMI_HPE_INTERLEAVE_HEALTH_ABSENT)),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, device_handle, HANDLE, {
            .code    = "device-handle",
            .name    = "Memory device handle",
            .targets = dmi_types(DMI_TYPE(memory_device))
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, region_id, INTEGER, {
            .code = "region-id",
            .name = "Region ID"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, is_volatile, BOOL, {
            .code = "is-volatile",
            .name = "Volatile"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, is_byte_accessible, BOOL, {
            .code = "is-byte-accessible",
            .name = "Byte accessible persistent"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, is_block_io, BOOL, {
            .code = "is-block-io",
            .name = "Block I/O persistent"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, size, SIZE, {
            .code = "size",
            .name = "Size"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, is_passphrase_enabled, BOOL, {
            .code = "is-passphrase-enabled",
            .name = "Passphrase enabled"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, interleave_set, INTEGER, {
            .code   = "interleave-set",
            .name   = "Interleave set index",
            .unspec = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, interleave_dimm_count, INTEGER, {
            .code = "interleave-dimm-count",
            .name = "Interleave DIMM count"
        }),
        DMI_ATTRIBUTE(dmi_hpe_dimm_config_t, interleave_health, ENUM, {
            .code   = "interleave-health",
            .name   = "Interleave set health",
            .unspec = dmi_value_ptr(DMI_HPE_INTERLEAVE_HEALTH_ABSENT),
            .values = &dmi_hpe_interleave_health_names
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_hpe_dimm_config_derive
    }
};

const dmi_name_set_t dmi_hpe_interleave_health_names =
{
    .code  = "hpe-interleave-health",
    .names = DMI_NAMES({
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_HEALTHY,
            .code = "healthy",
            .name = "Healthy"
        },
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_DIMM_MISSING,
            .code = "dimm-missing",
            .name = "DIMM missing"
        },
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_INACTIVE,
            .code = "inactive",
            .name = "Configuration inactive"
        },
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_SPA_MISSING,
            .code = "spa-missing",
            .name = "SPA missing"
        },
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_NEW_GOAL,
            .code = "new-goal",
            .name = "New goal"
        },
        {
            .id   = DMI_HPE_INTERLEAVE_HEALTH_LOCKED,
            .code = "locked",
            .name = "Locked"
        },
        {}
    }),
    .ranges = DMI_NAME_RANGES({
        {
            .start_id = DMI_HPE_INTERLEAVE_HEALTH_LOCKED + 1,
            .end_id   = UINT8_MAX,
            .code     = "reserved",
            .name     = "Reserved"
        },
        {}
    })
};

DMI_NAME_FUNCTION(dmi_hpe_interleave_health)
