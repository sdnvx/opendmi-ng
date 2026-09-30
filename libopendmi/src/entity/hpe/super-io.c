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

#include <opendmi/entity/hpe/super-io.h>

const dmi_entity_spec_t dmi_hpe_super_io_spec =
{
    .type        = DMI_TYPE(hpe_super_io),
    .code        = "hpe-super-io",
    .name        = "HP/HPE Super I/O enable/disable indicator",
    .description = (const char *[]){
        "Tells which ports of the Super I/O controller are enabled.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x05,
        .decoded_length = sizeof(dmi_hpe_super_io_t)
    },

    .fields = DMI_FIELDS({
        DMI_FIELD_BITS(dmi_hpe_super_io_t, is_serial_a_enabled,       1),
        DMI_FIELD_BITS(dmi_hpe_super_io_t, is_serial_b_enabled,       1),
        DMI_FIELD_BITS(dmi_hpe_super_io_t, is_parallel_enabled,       1),
        DMI_FIELD_BITS(dmi_hpe_super_io_t, is_floppy_enabled,         1),
        DMI_FIELD_BITS(dmi_hpe_super_io_t, is_virtual_serial_enabled, 1),
        DMI_FIELD_PAD(dmi_byte_t),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_hpe_super_io_t, is_serial_a_enabled, BOOL, {
            .code = "is-serial-a-enabled",
            .name = "Serial port A enabled"
        }),
        DMI_ATTRIBUTE(dmi_hpe_super_io_t, is_serial_b_enabled, BOOL, {
            .code = "is-serial-b-enabled",
            .name = "Serial port B enabled"
        }),
        DMI_ATTRIBUTE(dmi_hpe_super_io_t, is_parallel_enabled, BOOL, {
            .code = "is-parallel-enabled",
            .name = "Parallel port enabled"
        }),
        DMI_ATTRIBUTE(dmi_hpe_super_io_t, is_floppy_enabled, BOOL, {
            .code = "is-floppy-enabled",
            .name = "Floppy disk port enabled"
        }),
        DMI_ATTRIBUTE(dmi_hpe_super_io_t, is_virtual_serial_enabled, BOOL, {
            .code = "is-virtual-serial-enabled",
            .name = "Virtual serial port enabled"
        }),
        {}
    })
};
