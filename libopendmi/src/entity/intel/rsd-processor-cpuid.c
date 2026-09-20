//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>

/**
 * @brief Size of the leaf in the structure: EAX, EBX, ECX and EDX.
 */
#define DMI_INTEL_RSD_CPUID_LEAF_SIZE 16

/**
 * @brief Leaf stored in the structure.
 */
typedef struct dmi_intel_rsd_cpuid_input
{
    uint32_t leaf;
    bool     has_subleaf;
    uint32_t subleaf;
} dmi_intel_rsd_cpuid_input_t;

static bool dmi_intel_rsd_processor_cpuid_decode(dmi_entity_t *entity);
static void dmi_intel_rsd_processor_cpuid_cleanup(dmi_entity_t *entity);

#define LEAF(__leaf)              { .leaf = (__leaf) }
#define SUBLEAF(__leaf, __subleaf) { .leaf = (__leaf), .has_subleaf = true, .subleaf = (__subleaf) }

// Leaves of subtype 1, in the order they are stored
static const dmi_intel_rsd_cpuid_input_t dmi_intel_rsd_cpuid_basic[] = {
    LEAF(0x00), LEAF(0x01), LEAF(0x02), LEAF(0x03), LEAF(0x04), LEAF(0x05),
    LEAF(0x06), LEAF(0x07), LEAF(0x09), LEAF(0x0A), LEAF(0x0B),
    SUBLEAF(0x0D, 0x00), SUBLEAF(0x0F, 0x00), SUBLEAF(0x10, 0x00)
};

// Leaves of subtype 2, in the order they are stored
static const dmi_intel_rsd_cpuid_input_t dmi_intel_rsd_cpuid_extended[] = {
    SUBLEAF(0x14, 0x00), LEAF(0x15), LEAF(0x16),
    SUBLEAF(0x17, 0x00), SUBLEAF(0x17, 0x01), SUBLEAF(0x17, 0x02), SUBLEAF(0x17, 0x03),
    LEAF(0x80000000), LEAF(0x80000001), LEAF(0x80000002), LEAF(0x80000003),
    LEAF(0x80000004), LEAF(0x80000006), LEAF(0x80000007), LEAF(0x80000008)
};

#undef LEAF
#undef SUBLEAF

const dmi_name_set_t dmi_intel_rsd_cpuid_subtype_names =
{
    .code  = "intel-rsd-cpuid-subtype",
    .names = (const dmi_name_t[]){
        {
            .id   = DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC,
            .code = "basic",
            .name = "Leaves 00h to 10h"
        },
        {
            .id   = DMI_INTEL_RSD_CPUID_SUBTYPE_EXTENDED,
            .code = "extended",
            .name = "Leaves 14h to 80000008h"
        },
        DMI_NAME_NULL
    }
};

static const dmi_attribute_t dmi_intel_rsd_cpuid_leaf_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, leaf, INTEGER, {
        .code     = "leaf",
        .name     = "Leaf",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_cpuid_leaf_t, has_subleaf, {
        .code     = "subleaf",
        .name     = "Subleaf",
        .variants = (const dmi_attribute_variant_t[]){
            DMI_VARIANT(true, dmi_intel_rsd_cpuid_leaf_t, subleaf, INTEGER, {
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            DMI_VARIANT_NULL
        }
    }),
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, eax, INTEGER, {
        .code     = "eax",
        .name     = "EAX",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, ebx, INTEGER, {
        .code     = "ebx",
        .name     = "EBX",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, ecx, INTEGER, {
        .code     = "ecx",
        .name     = "ECX",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, edx, INTEGER, {
        .code     = "edx",
        .name     = "EDX",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_NULL
};

const dmi_entity_spec_t dmi_intel_rsd_processor_cpuid_spec =
{
    .type            = DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID),
    .code            = "intel-rsd-processor-cpuid",
    .name            = "Intel RSD processor CPUID information",
    .description     = (const char *[]){
        "Declares CPUID leaves of the processors in the system, as returned by "
        "the CPUID instruction. Leaves 00h to 10h and 14h to 80000008h are "
        "stored in structures of different subtypes.",
        //
        nullptr
    },
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_intel_rsd_processor_cpuid_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_intel_rsd_processor_cpuid_t, socket_designation, STRING, {
            .code     = "socket-designation",
            .name     = "Socket designation"
        }),
        DMI_ATTRIBUTE(dmi_intel_rsd_processor_cpuid_t, subtype, ENUM, {
            .code     = "subtype",
            .name     = "Subtype",
            .values   = &dmi_intel_rsd_cpuid_subtype_names
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_intel_rsd_processor_cpuid_t, leaves, leaf_count, STRUCT, {
            .code     = "leaves",
            .name     = "Leaves",
            .attrs    = dmi_intel_rsd_cpuid_leaf_attrs
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_processor_cpuid_t, is_raw, {
            .code     = "data",
            .name     = "CPUID data",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(true, dmi_intel_rsd_processor_cpuid_t, data, BINARY, {}),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_intel_rsd_processor_cpuid_decode,
        .cleanup = dmi_intel_rsd_processor_cpuid_cleanup
    }
};

static bool dmi_intel_rsd_processor_cpuid_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_processor_cpuid_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_stream_t  *stream  = dmi_entity_stream(entity);

    dmi_byte_t subtype = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->socket_designation) and
        dmi_stream_decode(stream, dmi_byte_t, &subtype);
    if (not status)
        return false;

    info->subtype = dmi_cast(info->subtype, subtype);

    const dmi_intel_rsd_cpuid_input_t *inputs = nullptr;
    size_t count = 0;

    switch (info->subtype) {
    case DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC:
        inputs = dmi_intel_rsd_cpuid_basic;
        count  = countof(dmi_intel_rsd_cpuid_basic);
        break;
    case DMI_INTEL_RSD_CPUID_SUBTYPE_EXTENDED:
        inputs = dmi_intel_rsd_cpuid_extended;
        count  = countof(dmi_intel_rsd_cpuid_extended);
        break;
    default:
        break;
    }

    // Data of unknown subtypes is shown as stored
    if (inputs == nullptr) {
        info->is_raw = true;
        return dmi_stream_decode_bin(stream, dmi_stream_remaining(stream), &info->data);
    }

    info->leaves = dmi_alloc_array(context, sizeof(*info->leaves), count);
    if (info->leaves == nullptr)
        return false;

    for (size_t i = 0; i < count; i++) {
        dmi_intel_rsd_cpuid_leaf_t *leaf = &info->leaves[i];

        status =
            dmi_stream_decode(stream, dmi_dword_t, &leaf->eax) and
            dmi_stream_decode(stream, dmi_dword_t, &leaf->ebx) and
            dmi_stream_decode(stream, dmi_dword_t, &leaf->ecx) and
            dmi_stream_decode(stream, dmi_dword_t, &leaf->edx);
        if (not status)
            return dmi_entity_incomplete(entity);

        leaf->leaf        = inputs[i].leaf;
        leaf->has_subleaf = inputs[i].has_subleaf;
        leaf->subleaf     = inputs[i].subleaf;

        info->leaf_count++;
    }

    return true;
}

static void dmi_intel_rsd_processor_cpuid_cleanup(dmi_entity_t *entity)
{
    dmi_intel_rsd_processor_cpuid_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    if (info == nullptr)
        return;

    dmi_free(info->leaves);
}
