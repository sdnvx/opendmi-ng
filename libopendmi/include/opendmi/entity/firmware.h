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
 *
 * Bits 32 to 47 of the value are reserved for the firmware vendor, and bits
 * 48 to 63 for the system vendor.
 */
dmi_packed_union(dmi_firmware_features)
{
    dmi_qword_t __value;

    dmi_packed_struct() {
        dmi_qword_t __reserved : 2;
        dmi_qword_t unknown : 1;             ///< Features are unknown
        dmi_qword_t unsupported : 1;         ///< Features are not reported
        dmi_qword_t isa_support : 1;         ///< ISA
        dmi_qword_t mca_support : 1;         ///< MCA
        dmi_qword_t eisa_support : 1;        ///< EISA
        dmi_qword_t pci_support : 1;         ///< PCI
        dmi_qword_t pcmcia_support : 1;      ///< PC card (PCMCIA)
        dmi_qword_t pnp_support : 1;         ///< Plug and Play
        dmi_qword_t apm_support : 1;         ///< APM
        dmi_qword_t upgradeable : 1;         ///< Firmware is upgradeable (flash)
        dmi_qword_t shadowing : 1;           ///< Firmware shadowing is allowed
        dmi_qword_t vesa_support : 1;        ///< VL-VESA
        dmi_qword_t escd_support : 1;        ///< ESCD
        dmi_qword_t boot_cd : 1;             ///< Boot from CD
        dmi_qword_t boot_selectable : 1;     ///< Selectable boot
        dmi_qword_t socketed : 1;            ///< Firmware ROM is socketed
        dmi_qword_t boot_pcmcia : 1;         ///< Boot from PC card (PCMCIA)
        dmi_qword_t edd_support : 1;         ///< EDD specification
        dmi_qword_t floppy_svc_nec : 1;      ///< Int 13h Japanese floppy for NEC 9800 1.2 MB
        dmi_qword_t floppy_svc_toshiba : 1;  ///< Int 13h Japanese floppy for Toshiba 1.2 MB
        dmi_qword_t floppy_svc_525_360k : 1; ///< Int 13h 5.25" 360 KB floppy services
        dmi_qword_t floppy_svc_525_1m2 : 1;  ///< Int 13h 5.25" 1.2 MB floppy services
        dmi_qword_t floppy_svc_35_720k : 1;  ///< Int 13h 3.5" 720 KB floppy services
        dmi_qword_t floppy_svc_35_2m88 : 1;  ///< Int 13h 3.5" 2.88 MB floppy services
        dmi_qword_t print_screen_svc : 1;    ///< Int 5h print screen service
        dmi_qword_t keyboard_svc : 1;        ///< Int 9h 8042 keyboard services
        dmi_qword_t serial_svc : 1;          ///< Int 14h serial services
        dmi_qword_t printer_svc : 1;         ///< Int 17h printer services
        dmi_qword_t video_svc_cga_mono : 1;  ///< Int 10h CGA/mono video services
        dmi_qword_t nec_pc_98 : 1;           ///< NEC PC-98
    };
};

dmi_static_assert_value_union(dmi_firmware_features);

#ifndef DMI_FIRMWARE_FEATURES_T
#   define DMI_FIRMWARE_FEATURES_T
    typedef union dmi_firmware_features dmi_firmware_features_t;
#endif // !DMI_FIRMWARE_FEATURES_T

/**
 * @brief Platform firmware extended features.
 *
 * The first byte is present since SMBIOS 2.1, the second one since
 * SMBIOS 2.3.
 */
dmi_packed_union(dmi_firmware_features_ex)
{
    /**
     * @brief Raw value.
     */
    dmi_byte_t __value[2];

    dmi_packed_struct()
    {
        bool acpi_support         : 1; ///< ACPI
        bool usb_legacy_support   : 1; ///< USB legacy
        bool agp_support          : 1; ///< AGP
        bool boot_i2o             : 1; ///< Boot from I2O
        bool boot_ls120           : 1; ///< Boot from LS-120 SuperDisk
        bool boot_atapi_zip       : 1; ///< Boot from ATAPI ZIP drive
        bool boot_ieee1394        : 1; ///< Boot from IEEE 1394
        bool smart_battery        : 1; ///< Smart battery

        bool bios_boot_spec       : 1; ///< BIOS Boot Specification
        bool boot_network_fn_key  : 1; ///< Network boot started by a function key
        bool content_distribution : 1; ///< Data fit for targeted content distribution
        bool uefi_spec            : 1; ///< UEFI specification
        bool virtual_machine      : 1; ///< System is a virtual machine
        bool mfg_mode_support     : 1; ///< Manufacturing mode supported
        bool mfg_mode_enabled     : 1; ///< Manufacturing mode enabled
    };
};

dmi_static_assert_value_union(dmi_firmware_features_ex);

#ifndef DMI_FIRMWARE_FEATURES_EX_T
#   define DMI_FIRMWARE_FEATURES_EX_T
    typedef union dmi_firmware_features_ex dmi_firmware_features_ex_t;
#endif // !DMI_FIRMWARE_FEATURES_EX_T

/**
 * @brief Platform firmware information structure (type 0).
 *
 * Describes the platform firmware (BIOS or UEFI) of the system: its vendor,
 * version, release date, ROM size and the features it supports. A table has
 * exactly one such structure.
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
     * @brief Firmware release date, `DMI_DATE_NONE` if the date is missing or
     * malformed.
     */
    dmi_date_t release_date;

    /**
     * @brief Size of the physical device(s) containing the firmware, in
     * bytes. Sizes of 16 MiB and above come from the extended ROM size field
     * added in SMBIOS 3.1, which yields 0 if its unit is reserved.
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
     * @brief Major and minor release of the platform firmware,
     * `DMI_VERSION_NONE` if the system does not report it.
     *
     * @since SMBIOS 2.4
     */
    dmi_version_t platform_version;

    /**
     * @brief Major and minor release of the embedded controller firmware,
     * `DMI_VERSION_NONE` if the system has no field upgradeable embedded
     * controller firmware.
     *
     * @since SMBIOS 2.4
     */
    dmi_version_t controller_version;
};

#ifndef DMI_FIRMWARE_T
#   define DMI_FIRMWARE_T
    typedef struct dmi_firmware dmi_firmware_t;
#endif // !DMI_FIRMWARE_T

/**
 * @brief Platform firmware information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_firmware_spec;

#endif // !OPENDMI_ENTITY_FIRMWARE_H
