//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <ctype.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/stream.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache.h>
#include <opendmi/entity/processor-internal.h>

static void dmi_processor_decode_id(const dmi_entity_t *entity, dmi_processor_t *info);
static void dmi_processor_decode_id_x86(const dmi_entity_t *entity, dmi_processor_t *info,
                                        uint32_t low, uint32_t high);
static void dmi_processor_decode_id_midr(dmi_processor_t *info, uint32_t low);
static void dmi_processor_decode_id_soc(dmi_processor_t *info, uint32_t low, uint32_t high);
static dmi_processor_id_format_t dmi_processor_id_format(const dmi_processor_t *info);
static bool dmi_processor_is_x86_vendor(const char *str);
static bool dmi_processor_has_word(const char *str, const char *word);

//
// One byte carries either the current voltage or the ones the processor
// supports, which the most significant bit of it tells apart.
//
bool dmi_processor_decode_voltage(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_processor_voltage_data_t data = {};

    if (not dmi_stream_decode(dmi_entity_stream(entity), dmi_byte_t, &data.__value))
        return false;

    dmi_processor_t *info = value;

    if (data.is_current) {
        info->voltage = data.value;
    } else {
        info->supported_voltages.__value    = data.value;
        info->supported_voltages.__reserved = 0;
    }

    return true;
}

//
// Processor identifier is taken as stored, and means what the family and the
// vendor say it does, so it is read once the fields are there.
//
bool dmi_processor_derive(dmi_entity_t *entity)
{
    dmi_processor_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return false;

    dmi_processor_decode_id(entity, info);

    return true;
}

bool dmi_processor_link(dmi_entity_t *entity)
{
    dmi_processor_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return false;

    dmi_context_t  *context  = dmi_entity_context(entity);
    dmi_registry_t *registry = dmi_get_registry(context);

    bool success = true;
    if (not dmi_registry_resolve(registry, info->l1_cache_handle, DMI_TYPE(CACHE), &info->l1_cache))
        success = false;
    if (not dmi_registry_resolve(registry, info->l2_cache_handle, DMI_TYPE(CACHE), &info->l2_cache))
        success = false;
    if (not dmi_registry_resolve(registry, info->l3_cache_handle, DMI_TYPE(CACHE), &info->l3_cache))
        success = false;

    return success;
}

static void dmi_processor_decode_id(const dmi_entity_t *entity, dmi_processor_t *info)
{
    info->id_format = DMI_PROCESSOR_ID_FORMAT_RAW;

    if ((info->id.data == nullptr) or (info->id.length < sizeof(dmi_qword_t)))
        return;

    // Structure data is not aligned
    dmi_dword_t words[2];
    memcpy(words, info->id.data, sizeof(words));

    uint32_t low  = dmi_decode(words[0]);
    uint32_t high = dmi_decode(words[1]);

    info->id_format = dmi_processor_id_format(info);

    switch (info->id_format) {
    case DMI_PROCESSOR_ID_FORMAT_X86:
        dmi_processor_decode_id_x86(entity, info, low, high);
        break;

    case DMI_PROCESSOR_ID_FORMAT_MIDR:
        dmi_processor_decode_id_midr(info, low);
        break;

    case DMI_PROCESSOR_ID_FORMAT_SOC_ID:
        dmi_processor_decode_id_soc(info, low, high);
        break;

    default:
        break;
    }
}

/**
 * @internal
 * @brief Decode signature (EAX) and feature flags (EDX) of CPUID leaf 1.
 */
static void dmi_processor_decode_id_x86(const dmi_entity_t *entity, dmi_processor_t *info,
                                        uint32_t low, uint32_t high)
{
    // Some firmware stores feature flags before the signature, which is told
    // by reserved bits of the signature
    if ((low & DMI_PROCESSOR_ID_RESERVED) and not (high & DMI_PROCESSOR_ID_RESERVED)) {
        dmi_log_notice(dmi_entity_context(entity),
                       "Handle 0x%04hx (%s): Processor ID words are swapped",
                       dmi_entity_handle(entity), dmi_type_name(dmi_entity_context(entity), entity->type));

        uint32_t swap = low;
        low  = high;
        high = swap;
    }

    uint8_t  family = (low >> 8) & 0x0Fu;
    uint16_t model  = (low >> 4) & 0x0Fu;

    info->x86_id.type     = (low >> 12) & 0x03u;
    info->x86_id.stepping = low & 0x0Fu;
    info->x86_id.features = high;

    // Extended fields apply to some base families only
    info->x86_id.family = family;
    if (family == 0x0Fu)
        info->x86_id.family += (low >> 20) & 0xFFu;
    if ((family == 0x06u) or (family == 0x0Fu))
        model |= ((low >> 16) & 0x0Fu) << 4;

    info->x86_id.model = model;
}

/**
 * @internal
 * @brief Decode Main ID Register (MIDR or MIDR_EL1) of Arm processors.
 */
static void dmi_processor_decode_id_midr(dmi_processor_t *info, uint32_t low)
{
    info->arm_id.implementer  = (low >> 24) & 0xFFu;
    info->arm_id.variant      = (low >> 20) & 0x0Fu;
    info->arm_id.architecture = (low >> 16) & 0x0Fu;
    info->arm_id.part_number  = (low >> 4) & 0x0FFFu;
    info->arm_id.revision     = low & 0x0Fu;
}

/**
 * @internal
 * @brief Decode SoC ID version and revision, as returned by SMCCC_ARCH_SOC_ID.
 */
static void dmi_processor_decode_id_soc(dmi_processor_t *info, uint32_t low, uint32_t high)
{
    info->soc_id.jep106_bank  = (low >> 24) & 0x7Fu;
    info->soc_id.jep106_id    = (low >> 16) & 0x7Fu;
    info->soc_id.soc_id       = low & 0xFFFFu;
    info->soc_id.soc_revision = high & 0x7FFFFFFFu;
}

static dmi_processor_id_format_t dmi_processor_id_format(const dmi_processor_t *info)
{
    unsigned int family = info->family;

    // Apple M1 and M2 families were Cyrix M1 and M2 x86 families in older
    // specification versions, and are still reported so by older firmware
    if ((family == DMI_PROCESSOR_FAMILY_APPLE_M1) or (family == DMI_PROCESSOR_FAMILY_APPLE_M2)) {
        if (not dmi_processor_has_word(info->vendor, "apple") and
            not dmi_processor_has_word(info->version, "apple"))
            return DMI_PROCESSOR_ID_FORMAT_X86;
    }

    // Arm processors report SoC ID if they support it, and MIDR otherwise
    if (((family >= DMI_PROCESSOR_FAMILY_ARM_V7) and (family <= DMI_PROCESSOR_FAMILY_ARM_V9)) or
        (family == DMI_PROCESSOR_FAMILY_APPLE_M1) or (family == DMI_PROCESSOR_FAMILY_APPLE_M2) or
        (family == DMI_PROCESSOR_FAMILY_ARM))
    {
        if ((family != DMI_PROCESSOR_FAMILY_ARM_V7) and (family != DMI_PROCESSOR_FAMILY_ARM) and
            info->features.arm64_soc_id)
            return DMI_PROCESSOR_ID_FORMAT_SOC_ID;

        return DMI_PROCESSOR_ID_FORMAT_MIDR;
    }

    // Processor families of Intel, AMD, VIA, Cyrix, IDT and Transmeta x86
    // processors, supporting CPUID instruction
    static const struct {
        unsigned int first;
        unsigned int last;
    } x86_families[] = {
        { DMI_PROCESSOR_FAMILY_INTEL_80386,         DMI_PROCESSOR_FAMILY_INTEL_80386          },
        { DMI_PROCESSOR_FAMILY_INTEL_80486,         DMI_PROCESSOR_FAMILY_INTEL_80486          },
        { DMI_PROCESSOR_FAMILY_INTEL_PENTIUM,       DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_3      },
        { DMI_PROCESSOR_FAMILY_INTEL_CELERON_M,     DMI_PROCESSOR_FAMILY_INTEL                },
        { DMI_PROCESSOR_FAMILY_AMD_DURON,           DMI_PROCESSOR_FAMILY_AMD_ATHLON           },
        { DMI_PROCESSOR_FAMILY_AMD_K6_2_PLUS,       DMI_PROCESSOR_FAMILY_AMD_K6_2_PLUS        },
        { DMI_PROCESSOR_FAMILY_INTEL_CORE_DUO,      DMI_PROCESSOR_FAMILY_INTEL_CORE_M7        },
        { DMI_PROCESSOR_FAMILY_AMD_TURION_2_ULTRA_2C_M, DMI_PROCESSOR_FAMILY_AMD_FX           },
        { DMI_PROCESSOR_FAMILY_AMD_C,               DMI_PROCESSOR_FAMILY_AMD_FIREPRO          },
        { DMI_PROCESSOR_FAMILY_AMD_ATHLON_X4_4C,    DMI_PROCESSOR_FAMILY_AMD_ZEN              },
        { DMI_PROCESSOR_FAMILY_CRUSOE_TM5000,       DMI_PROCESSOR_FAMILY_EFFICEON_TM8000      },
        { DMI_PROCESSOR_FAMILY_AMD_ATHLON_64,       DMI_PROCESSOR_FAMILY_AMD_ATHLON_X2_2C     },
        { DMI_PROCESSOR_FAMILY_INTEL_XEON_3200_4C,  DMI_PROCESSOR_FAMILY_INTEL_XEON           },
        { DMI_PROCESSOR_FAMILY_INTEL_XEON_MP,       DMI_PROCESSOR_FAMILY_AMD_ATHLON_MP        },
        { DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_M,     DMI_PROCESSOR_FAMILY_INTEL_CELERON_2C     },
        { DMI_PROCESSOR_FAMILY_INTEL_CORE_I5,       DMI_PROCESSOR_FAMILY_INTEL_XEON_D         },
        { DMI_PROCESSOR_FAMILY_VIA_C7_M,            DMI_PROCESSOR_FAMILY_INTEL_XEON_5XXX_4C   },
        { DMI_PROCESSOR_FAMILY_INTEL_XEON_7XXX_2C,  DMI_PROCESSOR_FAMILY_INTEL_XEON_3400_MC   },
        { DMI_PROCESSOR_FAMILY_AMD_OPTERON_3000,    DMI_PROCESSOR_FAMILY_AMD_SEMPRON_M        },
        { DMI_PROCESSOR_FAMILY_CYRIX_6X86,          DMI_PROCESSOR_FAMILY_CYRIX_M2             },
        { DMI_PROCESSOR_FAMILY_WINCHIP,             DMI_PROCESSOR_FAMILY_WINCHIP              },
        { DMI_PROCESSOR_FAMILY_INTEL_CORE_3,        DMI_PROCESSOR_FAMILY_INTEL_CORE_ULTRA_9   }
    };

    for (size_t i = 0; i < countof(x86_families); i++) {
        if ((family >= x86_families[i].first) and (family <= x86_families[i].last))
            return DMI_PROCESSOR_ID_FORMAT_X86;
    }

    // Some firmware reports x86 processors as other or unknown ones, so they
    // are recognized by vendor
    if ((family == DMI_PROCESSOR_FAMILY_OTHER) or (family == DMI_PROCESSOR_FAMILY_UNKNOWN)) {
        if (dmi_processor_is_x86_vendor(info->vendor) or dmi_processor_is_x86_vendor(info->version))
            return DMI_PROCESSOR_ID_FORMAT_X86;
    }

    return DMI_PROCESSOR_ID_FORMAT_RAW;
}

static bool dmi_processor_is_x86_vendor(const char *str)
{
    // Vendor names, including CPUID vendor identification strings
    static const char *vendors[] = {
        "intel", "amd", "advanced micro devices", "hygon", "zhaoxin", "centaur", "via",
        "genuineintel", "authenticamd", "hygongenuine", "centaurhauls"
    };

    for (size_t i = 0; i < countof(vendors); i++) {
        if (dmi_processor_has_word(str, vendors[i]))
            return true;
    }

    return false;
}

static bool dmi_processor_has_word(const char *str, const char *word)
{
    if (str == nullptr)
        return false;

    size_t length = strlen(word);

    // Word is matched case-insensitively, and must not be a part of a longer
    // word
    for (const char *pos = str; *pos != 0; pos++) {
        if ((pos != str) and isalnum((unsigned char)pos[-1]))
            continue;

        size_t i = 0;
        while ((i < length) and (tolower((unsigned char)pos[i]) == word[i]))
            i++;

        if ((i == length) and not isalnum((unsigned char)pos[i]))
            return true;
    }

    return false;
}
