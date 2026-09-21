//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <string.h>
#include <opendmi/stream.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware-internal.h>

const dmi_name_set_t dmi_firmware_feature_names =
{
    .code  = "firmware-feature",
    .names = DMI_NAMES({
        DMI_NAME_UNKNOWN(2),
        DMI_NAME_UNSUPPORTED(3),
        {
            .id   = 4,
            .code = "isa-support",
            .name = "ISA support"
        },
        {
            .id   = 5,
            .code = "mca-support",
            .name = "MCA support"
        },
        {
            .id   = 6,
            .code = "eisa-support",
            .name = "EISA support"
        },
        {
            .id   = 7,
            .code = "pci-support",
            .name = "PCI support"
        },
        {
            .id   = 8,
            .code = "pcmcia-support",
            .name = "PC card (PCMCIA) support"
        },
        {
            .id   = 9,
            .code = "pnp-support",
            .name = "Plug and Play support"
        },
        {
            .id   = 10,
            .code = "apm-support",
            .name = "APM support"
        },
        {
            .id   = 11,
            .code = "upgradeable-flash",
            .name = "Firmware is upgradeable (flash)"
        },
        {
            .id   = 12,
            .code = "shadowing-allowed",
            .name = "Firmware shadowing is allowed"
        },
        {
            .id   = 13,
            .code = "vesa-support",
            .name = "VL-VESA support"
        },
        {
            .id   = 14,
            .code = "escd-support",
            .name = "ESCD support"
        },
        {
            .id   = 15,
            .code = "boot-cd",
            .name = "Boot from CD"
        },
        {
            .id   = 16,
            .code = "boot-selectable",
            .name = "Selectable boot"
        },
        {
            .id   = 17,
            .code = "socketed-rom",
            .name = "Socketed ROM"
        },
        {
            .id   = 18,
            .code = "boot-pcmcia-support",
            .name = "Boot from PC card (PCMCIA)"
        },
        {
            .id   = 19,
            .code = "edd-support",
            .name = "EDD specification support"
        },
        {
            .id   = 20,
            .code = "floppy-nec-support",
            .name = "Int 13h japanese floppy for NEC 9800 1.2 MB support"
        },
        {
            .id   = 21,
            .code = "floppy-toshiba-support",
            .name = "Int 13h japanese floppy for Toshiba 1.2 MB support"
        },
        {
            .id   = 22,
            .code = "floppy-525-360k-support",
            .name = "Int 13h 5.25\"” / 360 KB floppy services support"
        },
        {
            .id   = 23,
            .code = "floppy-525-1m2-support",
            .name = "Int 13h 5.25\" / 1.2 MB floppy services support"
        },
        {
            .id   = 24,
            .code = "floppy-35-720k-support",
            .name = "Int 13h 3.5\" / 720 KB floppy services support"
        },
        {
            .id   = 25,
            .code = "floppy-35-2m88-support",
            .name = "Int 13h 3.5\" / 2.88 MB floppy services support"
        },
        {
            .id   = 26,
            .code = "print-screen-support",
            .name = "Int 5h print screen service support"
        },
        {
            .id   = 27,
            .code = "keyboard-support",
            .name = "Int 9h 8042 keyboard services support"
        },
        {
            .id   = 28,
            .code = "serial-support",
            .name = "Int 14h serial services support"
        },
        {
            .id   = 29,
            .code = "printer-support",
            .name = "Int 17h printer services support"
        },
        {
            .id   = 30,
            .code = "video-cga-mono-support",
            .name = "Int 10h CGA/Mono video services support"
        },
        {
            .id   = 31,
            .code = "nec-pc-98",
            .name = "NEC PC-98"
        },
        {}
    })
};

const dmi_name_set_t dmi_firmware_feature_ex_names =
{
    .code  = "firmware-features-ex",
    .names = DMI_NAMES({
        {
            .id   = 0,
            .code = "acpi-support",
            .name = "ACPI support"
        },
        {
            .id   = 1,
            .code = "usb-legacy-support",
            .name = "USB legacy support"
        },
        {
            .id   = 2,
            .code = "agp-support",
            .name = "AGP support"
        },
        {
            .id   = 3,
            .code = "boot-i2o",
            .name = "I2O boot support"
        },
        {
            .id   = 4,
            .code = "boot-ls120",
            .name = "LS-120 SuperDisk boot support"
        },
        {
            .id   = 5,
            .code = "boot-atapi-zip",
            .name = "ATAPI ZIP drive boot support"
        },
        {
            .id   = 6,
            .code = "boot-ieee1394",
            .name = "IEEE1394 boot support"
        },
        {
            .id   = 7,
            .code = "smart-battery",
            .name = "Smart battery support"
        },
        {
            .id   = 8,
            .code = "bios-boot-spec",
            .name = "BIOS boot specification support"
        },
        {
            .id   = 9,
            .code = "boot-network-fn-key",
            .name = "Function key-initiated network boot support"
        },
        {
            .id   = 10,
            .code = "content-distribution",
            .name = "Targeted content distribution enabled"
        },
        {
            .id   = 11,
            .code = "uefi-spec",
            .name = "UEFI specification support"
        },
        {
            .id   = 12,
            .code = "virtual-machine",
            .name = "SMBIOS describes a virtual machine"
        },
        {
            .id   = 13,
            .code = "mfg-mode-support",
            .name = "Manufacturing mode support"
        },
        {
            .id = 14,
            .code = "mfg-mode-enabled",
            .name = "Manufacturing mode enabled"
        },
        {}
    })
};
