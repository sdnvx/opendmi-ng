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
        bool unknown : 1;
        bool unsupported : 1;
        bool isa_support : 1;
        bool mca_support : 1;
        bool eisa_support : 1;
        bool pci_support : 1;
        bool pcmcia_support : 1;
        bool pnp_support : 1;
        bool apm_support : 1;
        bool upgradeable : 1;
        bool shadowing : 1;
        bool vesa_support : 1;
        bool escd_support : 1;
        bool boot_cd : 1;
        bool boot_selectable : 1;
        bool socketed : 1;
        bool boot_pcmcia : 1;
        bool edd_support : 1;
        bool floppy_svc_nec : 1;
        bool floppy_svc_toshiba : 1;
        bool floppy_svc_525_360k : 1;
        bool floppy_svc_525_1m2 : 1;
        bool floppy_svc_35_720k : 1;
        bool floppy_svc_35_2m88 : 1;
        bool print_screen_svc : 1;
        bool keyboard_svc : 1;
        bool serial_svc : 1;
        bool printer_svc : 1;
        bool video_svc_cga_mono : 1;
        bool nec_pc_98 : 1;
    };
};

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

typedef union dmi_firmware_features_ex dmi_firmware_features_ex_t;

/**
 * @brief Platform firmware information structure (type 0).
 */
dmi_packed_struct(dmi_firmware_data)
{
    /**
     * @brief SMBIOS structure header.
     */
    dmi_header_t header;

    /**
     * @brief String number of the firmware vendor’s name.
     *
     * @since SMBIOS 2.0
     */
    dmi_string_t vendor;

    /**
     * @brief String number of the Firmware Version. This value is a free-form
     * string that may contain Core and OEM version information.
     *
     * @since SMBIOS 2.0
     */
    dmi_string_t version;

    /**
     * @brief Segment location of BIOS starting address (for example, 0xE800).
     * When not applicable, such as on UEFI-based systems, this value is set
     * to 0000h.
     *
     * @note
     * The size of the runtime BIOS image can be computed by subtracting the
     * starting address segment from 0x10000 and multiplying the result by 16.
     *
     * @since SMBIOS 2.0
     */
    dmi_word_t bios_segment;

    /**
     * @brief String number of the firmware release date. The date string, if
     * supplied, is in either mm/dd/yy or mm/dd/yyyy format. If the year
     * portion of the string is two digits, the year is assumed to be 19yy.
     *
     * @note
     * The mm/dd/yyyy format is required for SMBIOS version 2.3 and later.
     *
     * @since SMBIOS 2.0
     */
    dmi_string_t release_date;

    /**
     * @brief Size (n) where 64 KiB * (n+1) is the size of the physical device
     * containing the platform firmware, in bytes. 0xFF - size is 16MiB or
     * greater, see Extended Firmware ROM Size for actual size.
     *
     * @since SMBIOS 2.0
     */
    dmi_byte_t rom_size;

    /**
     * @brief Defines which functions the firmware supports: PCI, PCMCIA,
     * flash, and so on.
     *
     * @since SMBIOS 2.0
     */
    dmi_qword_t features;

    /**
     * @brief Optional space reserved for future supported functions.
     *
     * The number of extension bytes that is present is indicated by the length
     * in offset 1 minus 0x12. Extension byte 1 is defined for version 2.1 and
     * later implementations, extension byte 2 is defined for version 2.3 and
     * later implementations.
     *
     * @note Structure table of the specification states version 2.4 for this
     * field, but extension bytes descriptions and version history define it
     * since version 2.1.
     *
     * @since SMBIOS 2.1
     */
    dmi_byte_t features_ex[2];

    /**
     * @brief Identifies the major release of the platform firmware. For
     * example, the value is 0x0Ah for revision 10.22 and 0x02h for revision
     * 2.1.
     *
     * This field or the platform firmware minor release field or both are
     * updated each time a platform firmware update for a given system is
     * released.
     *
     * If the system does not support the use of this field, the value is 0x0FF
     * for both this field and the platform firmware minor release field.
     *
     * @since SMBIOS 2.4
     */
    dmi_byte_t platform_release_major;

    /**
     * @brief Identifies the minor release of the platform firmware. For
     * example, the value is 0x16 for revision 10.22 and 0x01 for revision 2.1.
     *
     * @since SMBIOS 2.4
     */
    dmi_byte_t platform_release_minor;

    /**
     * Identifies the major release of the embedded controller firmware. For
     * example, the value would be 0x0A for revision 10.22 and 0x02 for
     * revision 2.1.
     *
     * This field or the embedded controller firmware minor release field or
     * both are updated each time an embedded controller firmware update for a
     * given system is released.
     *
     * If the system does not have field upgradeable embedded controller
     * firmware, the value is 0FFh.
     *
     * @since SMBIOS 2.4
     */
    dmi_byte_t controller_release_major;

    /**
     * @brief Identifies the minor release of the embedded controller firmware.
     * For example, the value is 0x16 for revision 10.22 and 0x01 for revision
     * 2.1.
     *
     * If the system does not have field upgradeable embedded controller
     * firmware, the value is 0xFF.
     *
     * @since SMBIOS 2.4
     */
    dmi_byte_t controller_release_minor;

    /**
     * @brief Extended size of the physical device(s) containing the firmware,
     * rounded up if needed.
     *
     * @since SMBIOS 3.1
     */
    dmi_word_t rom_size_ex;
};

typedef struct dmi_firmware_data dmi_firmware_data_t;

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
