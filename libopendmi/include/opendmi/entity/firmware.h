//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_FIRMWARE_H
#define OPENDMI_ENTITY_FIRMWARE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/utils/datetime.h>

/**
 * @brief Platform firmware features.
 */
dmi_packed_union(dmi_firmware_features)
{
    dmi_qword_t __value;

    dmi_packed_struct() {
        dmi_qword_t __reserved : 2;
        dmi_qword_t unknown : 1;
        dmi_qword_t unsupported : 1;
        dmi_qword_t isa_support : 1;
        dmi_qword_t mca_support : 1;
        dmi_qword_t eisa_support : 1;
        dmi_qword_t pci_support : 1;
        dmi_qword_t pcmcia_support : 1;
        dmi_qword_t pnp_support : 1;
        dmi_qword_t apm_support : 1;
        dmi_qword_t upgradeable : 1;
        dmi_qword_t shadowing : 1;
        dmi_qword_t vesa_support : 1;
        dmi_qword_t escd_support : 1;
        dmi_qword_t boot_cd : 1;
        dmi_qword_t boot_selectable : 1;
        dmi_qword_t socketed : 1;
        dmi_qword_t boot_pcmcia : 1;
        dmi_qword_t edd_support : 1;
        dmi_qword_t floppy_svc_nec : 1;
        dmi_qword_t floppy_svc_toshiba : 1;
        dmi_qword_t floppy_svc_525_360k : 1;
        dmi_qword_t floppy_svc_525_1m2 : 1;
        dmi_qword_t floppy_svc_35_720k : 1;
        dmi_qword_t floppy_svc_35_2m88 : 1;
        dmi_qword_t print_screen_svc : 1;
        dmi_qword_t keyboard_svc : 1;
        dmi_qword_t serial_svc : 1;
        dmi_qword_t printer_svc : 1;
        dmi_qword_t video_svc_cga_mono : 1;
        dmi_qword_t nec_pc_98 : 1;
    };
};

dmi_static_assert_value_union(dmi_firmware_features);

typedef union dmi_firmware_features dmi_firmware_features_t;

/**
 * @brief Platform firmware extended features.
 */
dmi_packed_union(dmi_firmware_features_ex)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value[2];

    dmi_packed_struct()
    {
        bool acpi_support         : 1;
        bool usb_legacy_support   : 1;
        bool agp_support          : 1;
        bool boot_i2o             : 1;
        bool boot_ls120           : 1;
        bool boot_atapi_zip       : 1;
        bool boot_ieee1394        : 1;
        bool smart_battery        : 1;

        bool bios_boot_spec       : 1;
        bool boot_network_fn_key  : 1;
        bool content_distribution : 1;
        bool uefi_spec            : 1;
        bool virtual_machine      : 1;
        bool mfg_mode_support     : 1;
        bool mfg_mode_enabled     : 1;
    };
};

dmi_static_assert_value_union(dmi_firmware_features_ex);

typedef union dmi_firmware_features_ex dmi_firmware_features_ex_t;

/**
 * @brief Platform firmware information structure (type 0).
 */
struct dmi_firmware
{
    /**
     * @brief Firmware vendor's name.
     */
    const char *vendor;

    /**
     * @brief Firmware version. This value is a free-form string that may
     * contain Core and OEM version information.
     */
    const char *version;

    /**
     * @brief Segment location of BIOS starting address (for example, 0xE800).
     * When not applicable, such as on UEFI-based systems, this value is set
     * to 0000h.
     */
    uint16_t bios_segment;

    /**
     * @brief Firmware release date.
     */
    dmi_date_t release_date;

    /**
     * @brief Extended size of the physical device(s) containing the firmware,
     * rounded up if needed.
     */
    dmi_size_t rom_size;

    /**
     * @brief Defines which functions the firmware supports: PCI, PCMCIA,
     * flash, and so on.
     */
    dmi_firmware_features_t features;

    /**
     * @brief Extended firmware features.
     *
     * @since SMBIOS 2.1
     */
    dmi_firmware_features_ex_t features_ex;

    /**
     * @brief Identifies the minor release of the platform firmware.
     */
    dmi_version_t platform_version;

    /**
     * @brief Identifies the minor release of the embedded controller firmware.
     */
    dmi_version_t controller_version;
};

typedef struct dmi_firmware dmi_firmware_t;

/**
 * @brief Platform firmware information entity specification.
 */
extern const dmi_entity_spec_t dmi_firmware_spec;

__BEGIN_DECLS

/**
 * @internal
 */
dmi_size_t dmi_firmware_rom_size(dmi_byte_t value);

/**
 * @internal
 */
dmi_size_t dmi_firmware_rom_size_ex(dmi_word_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_FIRMWARE_H
