//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/field.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/vpro-internal.h>

// Attributes of the versions of the firmware components
static const dmi_attribute_t dmi_intel_vpro_version_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, major, INTEGER, {
        .code = "major",
        .name = "Major version"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, minor, INTEGER, {
        .code = "minor",
        .name = "Minor version"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, hotfix, INTEGER, {
        .code = "hotfix",
        .name = "Hotfix"
    }),
    DMI_ATTRIBUTE(dmi_intel_vpro_version_t, build, INTEGER, {
        .code = "build",
        .name = "Build number"
    }),
    {}
};

// Bits of the capabilities, named as the Intel AMT implementation guide
// names them
static const dmi_name_set_t dmi_intel_vpro_cpu_caps_names =
{
    .code  = "intel-vpro-cpu-caps",
    .names = DMI_NAMES({
        { .id = 0, .code = "vmx-enabled",  .name = "VMX enabled"  },
        { .id = 1, .code = "smx-enabled",  .name = "SMX enabled"  },
        { .id = 2, .code = "txt-capable",  .name = "TXT capable"  },
        { .id = 3, .code = "txt-enabled",  .name = "TXT enabled"  },
        { .id = 4, .code = "vtx-capable",  .name = "VT-x capable" },
        { .id = 5, .code = "vtx-enabled",  .name = "VT-x enabled" },
        {}
    })
};

static const dmi_name_set_t dmi_intel_vpro_mch_caps_names =
{
    .code  = "intel-vpro-mch-caps",
    .names = DMI_NAMES({
        { .id = 0, .code = "vtd-capable", .name = "VT-d capable" },
        { .id = 1, .code = "vtd-enabled", .name = "VT-d enabled" },
        { .id = 2, .code = "txt-capable", .name = "TXT capable"  },
        { .id = 3, .code = "txt-enabled", .name = "TXT enabled"  },
        {}
    })
};

static const dmi_name_set_t dmi_intel_vpro_me_caps_names =
{
    .code  = "intel-vpro-me-caps",
    .names = DMI_NAMES({
        { .id = 0, .code = "me-enabled", .name = "ME enabled" },
        { .id = 1, .code = "qst",        .name = "Quiet System Technology" },
        { .id = 2, .code = "asf",        .name = "Alert Standard Format" },
        { .id = 3, .code = "amt",        .name = "Active Management Technology" },
        { .id = 5, .code = "sbt",        .name = "Small Business Technology" },
        { .id = 6, .code = "l3",         .name = "Level III manageability" },
        {}
    })
};

static const dmi_name_set_t dmi_intel_vpro_tpm_caps_names =
{
    .code  = "intel-vpro-tpm-caps",
    .names = DMI_NAMES({
        { .id = 0, .code = "present", .name = "TPM on board" },
        { .id = 1, .code = "enabled", .name = "TPM enabled"  },
        {}
    })
};

static const dmi_name_set_t dmi_intel_vpro_bios_caps_names =
{
    .code  = "intel-vpro-bios-caps",
    .names = DMI_NAMES({
        { .id = 0, .code = "vtx-configurable", .name = "VT-x configurable in setup" },
        { .id = 1, .code = "vtd-configurable", .name = "VT-d configurable in setup" },
        { .id = 2, .code = "txt-configurable", .name = "TXT configurable in setup" },
        { .id = 3, .code = "tpm-configurable", .name = "TPM configurable in setup" },
        { .id = 4, .code = "me-configurable",  .name = "ME configurable in setup" },
        { .id = 5, .code = "va-extensions",    .name = "Virtual Appliance extensions" },
        { .id = 6, .code = "spi-data-reserved", .name = "SPI flash platform data reserved" },
        {}
    })
};

const dmi_entity_spec_t dmi_intel_vpro_spec =
{
    .type        = DMI_TYPE(intel_vpro),
    .code        = "intel-vpro",
    .name        = "Intel vPro information",
    .description = (const char *[]){
        "Describes the parts of the platform Intel vPro technology relies "
        "on: the capabilities of the processor, of the Management Engine "
        "firmware, of the TPM and of the BIOS, the versions of the firmware "
        "and of its BIOS extension, and the PCI functions of the chipset and "
        "of the network controller the Management Engine uses.",
        //
        nullptr
    },
    .params = {
        .minimum_length = 0x40,
        .decoded_length = sizeof(dmi_intel_vpro_t),
        .signature      = DMI_SIGNATURE({
            .offset = 0x38,
            .bytes  = "vPro",
            .size   = 4
        })
    },

    .fields = DMI_FIELDS({
        DMI_FIELD(dmi_intel_vpro_t, cpu_capabilities, dmi_dword_t),

        // Version of the BIOS extension is laid out from the major part down
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.major,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.minor,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.hotfix, dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, mebx_version.build,  dmi_word_t),

        DMI_FIELD(dmi_intel_vpro_t, lpc_devfn,     dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, lpc_bus,       dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, lpc_device_id, dmi_word_t),
        DMI_FIELD_SKIP(4),
        DMI_FIELD(dmi_intel_vpro_t, me_capabilities, dmi_dword_t),

        // Version of the firmware is laid out as the firmware reports it
        DMI_FIELD(dmi_intel_vpro_t, me_version.minor,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.major,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.build,  dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, me_version.hotfix, dmi_word_t),
        DMI_FIELD(dmi_intel_vpro_t, tpm_capabilities, dmi_dword_t),

        DMI_FIELD(dmi_intel_vpro_t, gbe_devfn,     dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, gbe_bus,       dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, gbe_device_id, dmi_word_t),
        DMI_FIELD_SKIP(2),

        // Firmware of HP laptops gives the host bridge in place of the
        // wireless network controller
        DMI_FIELD(dmi_intel_vpro_t, wlan_devfn,     dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, wlan_bus,       dmi_byte_t),
        DMI_FIELD(dmi_intel_vpro_t, wlan_device_id, dmi_word_t),
        DMI_FIELD_SKIP(2),
        DMI_FIELD(dmi_intel_vpro_t, bios_capabilities, dmi_dword_t),

        // Signature is kept, so that the structure is written back with it
        DMI_FIELD_BINARY(dmi_intel_vpro_t, signature, 4),
        DMI_FIELD_SKIP(4),
        {}
    }),

    .attributes = DMI_ATTRIBUTES({
        DMI_ATTRIBUTE(dmi_intel_vpro_t, cpu_capabilities, SET, {
            .code   = "cpu-capabilities",
            .name   = "Processor capabilities",
            .values = &dmi_intel_vpro_cpu_caps_names
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_mch_capabilities, {
            .code     = "mebx-version",
            .name     = "MEBx version",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(false, dmi_intel_vpro_t, mebx_version, STRUCT, {
                    .attrs = dmi_intel_vpro_version_attrs
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_mch_capabilities, {
            .code     = "mch-devfn",
            .name     = "MCH device and function",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, mch_devfn, INTEGER, {
                    .code  = "mch-devfn",
                    .name  = "MCH device and function",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_mch_capabilities, {
            .code     = "mch-bus",
            .name     = "MCH bus",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, mch_bus, INTEGER, {
                    .code  = "mch-bus",
                    .name  = "MCH bus",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_mch_capabilities, {
            .code     = "mch-device-id",
            .name     = "MCH device ID",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, mch_device_id, INTEGER, {
                    .code  = "mch-device-id",
                    .name  = "MCH device ID",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_mch_capabilities, {
            .code     = "mch-capabilities",
            .name     = "MCH capabilities",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, mch_capabilities, SET, {
                    .code   = "mch-capabilities",
                    .name   = "MCH capabilities",
                    .values = &dmi_intel_vpro_mch_caps_names
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, lpc_devfn, INTEGER, {
            .code  = "lpc-devfn",
            .name  = "LPC bridge device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, lpc_bus, INTEGER, {
            .code  = "lpc-bus",
            .name  = "LPC bridge bus",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, lpc_device_id, INTEGER, {
            .code  = "lpc-device-id",
            .name  = "LPC bridge device ID",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, me_capabilities, SET, {
            .code   = "me-capabilities",
            .name   = "ME capabilities",
            .values = &dmi_intel_vpro_me_caps_names
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, me_version, STRUCT, {
            .code  = "me-version",
            .name  = "ME firmware version",
            .attrs = dmi_intel_vpro_version_attrs
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, tpm_capabilities, SET, {
            .code   = "tpm-capabilities",
            .name   = "TPM capabilities",
            .values = &dmi_intel_vpro_tpm_caps_names
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, tcg_major, INTEGER, {
            .code = "tcg-major",
            .name = "TCG specification major version"
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, tcg_minor, INTEGER, {
            .code = "tcg-minor",
            .name = "TCG specification minor version"
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, gbe_devfn, INTEGER, {
            .code  = "gbe-devfn",
            .name  = "Wired network controller device and function",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, gbe_bus, INTEGER, {
            .code  = "gbe-bus",
            .name  = "Wired network controller bus",
            .flags = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, gbe_device_id, INTEGER, {
            .code   = "gbe-device-id",
            .name   = "Wired network controller device ID",
            .unspec = dmi_value_ptr((uint16_t)UINT16_MAX),
            .flags  = DMI_ATTRIBUTE_FLAG_HEX
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_wlan, {
            .code     = "wlan-devfn",
            .name     = "Wireless network controller device and function",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, wlan_devfn, INTEGER, {
                    .code  = "wlan-devfn",
                    .name  = "Wireless network controller device and function",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_wlan, {
            .code     = "wlan-bus",
            .name     = "Wireless network controller bus",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, wlan_bus, INTEGER, {
                    .code  = "wlan-bus",
                    .name  = "Wireless network controller bus",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_vpro_t, has_wlan, {
            .code     = "wlan-device-id",
            .name     = "Wireless network controller device ID",
            .variants = DMI_VARIANTS({
                DMI_VARIANT(true, dmi_intel_vpro_t, wlan_device_id, INTEGER, {
                    .code  = "wlan-device-id",
                    .name  = "Wireless network controller device ID",
                    .flags = DMI_ATTRIBUTE_FLAG_HEX
                }),
                {}
            })
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, bios_capabilities, SET, {
            .code   = "bios-capabilities",
            .name   = "BIOS capabilities",
            .values = &dmi_intel_vpro_bios_caps_names
        }),
        DMI_ATTRIBUTE(dmi_intel_vpro_t, va_version, INTEGER, {
            .code = "va-version",
            .name = "Highest Virtual Appliance version code"
        }),
        {}
    }),

    .handlers = {
        .derive = dmi_intel_vpro_derive
    }
};

bool dmi_intel_vpro_derive(dmi_entity_t *entity)
{
    dmi_intel_vpro_t *info = dmi_entity_info(entity, DMI_TYPE(intel_vpro));
    if (info == nullptr)
        return false;

    info->tcg_major  = info->tpm_capabilities.tcg_major;
    info->tcg_minor  = info->tpm_capabilities.tcg_minor;
    info->va_version = info->bios_capabilities.va_version;

    // Firmware which gives no wireless network controller fills its device
    // ID with either byte of all bits set, or leaves it zero
    uint16_t wlan = info->wlan_device_id;
    info->has_wlan = (wlan != 0) and (wlan != 0x00FF) and (wlan != 0xFF00) and (wlan != UINT16_MAX);

    // Older layout holds the memory controller hub, which is found at 0:0.0,
    // in place of the version of the BIOS extension, whose major part is
    // never zero when the rest of it is given
    info->has_mch_capabilities = (info->mebx_version.major == 0) and (info->mebx_version.minor != 0);

    // Words of the version are the ones of the hub: the device and function
    // and the bus, the device ID, and the two words of the capabilities
    if (info->has_mch_capabilities) {
        const dmi_intel_vpro_version_t *mch = &info->mebx_version;

        info->mch_devfn                = mch->major & 0xFF;
        info->mch_bus                  = mch->major >> 8;
        info->mch_device_id            = mch->minor;
        info->mch_capabilities.__value = (uint32_t)mch->hotfix | ((uint32_t)mch->build << 16);
    }

    return true;
}
