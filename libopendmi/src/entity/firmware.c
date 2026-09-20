//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/firmware.h>

static bool dmi_firmware_decode(dmi_entity_t *entity);

static const dmi_name_set_t dmi_firmware_feature_names =
{
    .code  = "firmware-feature",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_firmware_feature_ex_names =
{
    .code  = "firmware-features-ex",
    .names = (dmi_name_t[]){
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
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_firmware_spec =
{
    .code            = "firmware",
    .name            = "Platform firmware information",
    .type            = DMI_TYPE(FIRMWARE),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .required_from   = DMI_VERSION(2, 3, 0),
    .required_till   = DMI_VERSION_NONE,
    .unique          = true,
    .minimum_length  = 0x12,
    .decoded_length  = sizeof(dmi_firmware_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_firmware_t, vendor, STRING, {
            .code   = "vendor",
            .name   = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, version, STRING, {
            .code   = "version",
            .name   = "Version"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, bios_segment, ADDRESS, {
            .code   = "bios-segment",
            .name   = "BIOS segment",
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, release_date, DATE, {
            .code   = "release-date",
            .name   = "Release date",
            .unspec = dmi_value_ptr(DMI_DATE_NONE)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, rom_size, SIZE, {
            .code   = "rom-size",
            .name   = "ROM size"
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, features, SET, {
            .code   = "features",
            .name   = "Features",
            .values = &dmi_firmware_feature_names
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, features_ex, SET, {
            .code   = "features-ex",
            .name   = "Extra features",
            .values = &dmi_firmware_feature_ex_names,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, platform_version, VERSION, {
            .code   = "platform-version",
            .name   = "Platform firmware version",
            .scale  = 2,
            .unspec = dmi_value_ptr(DMI_VERSION_NONE),
            .level  = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE(dmi_firmware_t, controller_version, VERSION, {
            .code   = "controller-version",
            .name   = "Embedded controller firmware version",
            .scale  = 2,
            .unspec = dmi_value_ptr(DMI_VERSION_NONE),
            .level  = DMI_VERSION(2, 4, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode = dmi_firmware_decode
    }
};

dmi_size_t dmi_firmware_rom_size(dmi_byte_t value)
{
    return ((dmi_size_t)value + 1) << 16;
}

dmi_size_t dmi_firmware_rom_size_ex(dmi_word_t value)
{
    dmi_size_t size = (dmi_size_t)(value & 0x3FFF);
    dmi_word_t scale = (value & 0xC000) >> 14;

    if (scale == 0)
        size <<= 20;
    else if (scale == 1)
        size <<= 30;
    else
        return 0;

    return size;
}

static bool dmi_firmware_decode(dmi_entity_t *entity)
{
    dmi_firmware_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(FIRMWARE));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    // SMBIOS 2.0 fields
    const char *release_date = nullptr;
    dmi_byte_t rom_size = 0;
    dmi_qword_t features = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode(stream, dmi_word_t, &info->bios_segment) and
        dmi_stream_decode_str(stream, &release_date) and
        dmi_stream_decode(stream, dmi_byte_t, &rom_size) and
        dmi_stream_decode(stream, dmi_qword_t, &features);
    if (not status)
        return false;

    info->release_date     = DMI_DATE_NONE;
    info->rom_size         = dmi_firmware_rom_size(rom_size);
    info->features.__value = features;

    if (release_date != nullptr) {
        info->release_date = dmi_date_parse(release_date);
        if (info->release_date == DMI_DATE_NONE)
            dmi_log_warning(context,
                            "Invalid firmware release date format: '%s'", release_date);
    }

    // SMBIOS 2.1 fields: extension byte 1
    dmi_byte_t features_ex[2] = {};

    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    if (not dmi_stream_decode(stream, dmi_byte_t, &features_ex[0]))
        return dmi_entity_incomplete(entity);

    info->features_ex = (dmi_firmware_features_ex_t){
        .__value = { features_ex[0], features_ex[1] }
    };

    // SMBIOS 2.3 fields: extension byte 2
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 3, 0);

    if (not dmi_stream_decode(stream, dmi_byte_t, &features_ex[1]))
        return dmi_entity_incomplete(entity);

    info->features_ex = (dmi_firmware_features_ex_t){
        .__value = { features_ex[0], features_ex[1] }
    };

    // SMBIOS 2.4 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 4, 0);

    // Major release 0xFF means that the version is not supported
    dmi_byte_t major = 0;
    dmi_byte_t minor = 0;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &major) and
        dmi_stream_decode(stream, dmi_byte_t, &minor);
    if (status and (major != 0xFFu))
        info->platform_version = dmi_version(major, minor, 0);

    status = status and
        dmi_stream_decode(stream, dmi_byte_t, &major) and
        dmi_stream_decode(stream, dmi_byte_t, &minor);
    if (status and (major != 0xFFu))
        info->controller_version = dmi_version(major, minor, 0);

    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 1, 0);

    dmi_word_t rom_size_ex = 0;
    if (not dmi_stream_decode(stream, dmi_word_t, &rom_size_ex))
        return dmi_entity_incomplete(entity);

    // Actual size is stored in extended field
    if (rom_size == 0xFFu)
        info->rom_size = dmi_firmware_rom_size_ex(rom_size_ex);

    return true;
}
