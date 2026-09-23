//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/writer.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/module/intel.h>

#include <opendmi/entity/intel/rsd-processor-cpuid-internal.h>

#define LEAF(__leaf)              { .leaf = (__leaf) }
#define SUBLEAF(__leaf, __subleaf) { .leaf = (__leaf), .has_subleaf = true, .subleaf = (__subleaf) }

/**
 * @brief Leaf stored in the structure.
 */
typedef struct dmi_intel_rsd_cpuid_input
{
    uint32_t leaf;
    bool     has_subleaf;
    uint32_t subleaf;
} dmi_intel_rsd_cpuid_input_t;

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

const dmi_attribute_t dmi_intel_rsd_cpuid_leaf_attrs[] =
{
    DMI_ATTRIBUTE(dmi_intel_rsd_cpuid_leaf_t, leaf, INTEGER, {
        .code     = "leaf",
        .name     = "Leaf",
        .flags    = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_VARIANT(dmi_intel_rsd_cpuid_leaf_t, has_subleaf, {
        .code     = "subleaf",
        .name     = "Subleaf",
        .variants = DMI_VARIANTS({
            DMI_VARIANT(true, dmi_intel_rsd_cpuid_leaf_t, subleaf, INTEGER, {
                .flags = DMI_ATTRIBUTE_FLAG_HEX
            }),
            {}
        })
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
    {}
};

bool dmi_intel_rsd_processor_cpuid_decode(dmi_entity_t *entity)
{
    dmi_intel_rsd_processor_cpuid_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_reader_t  *reader  = dmi_entity_reader(entity);

    dmi_byte_t subtype = 0;

    bool status =
        dmi_reader_get_string(reader, &info->socket_designation) and
        dmi_reader_get(reader, dmi_byte_t, &subtype);
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
        return dmi_reader_get_binary(reader, dmi_reader_remaining(reader), &info->data);
    }

    info->leaves = dmi_alloc_array(context, sizeof(*info->leaves), count);
    if (info->leaves == nullptr)
        return false;

    for (size_t i = 0; i < count; i++) {
        dmi_intel_rsd_cpuid_leaf_t *leaf = &info->leaves[i];

        status =
            dmi_reader_get(reader, dmi_dword_t, &leaf->eax) and
            dmi_reader_get(reader, dmi_dword_t, &leaf->ebx) and
            dmi_reader_get(reader, dmi_dword_t, &leaf->ecx) and
            dmi_reader_get(reader, dmi_dword_t, &leaf->edx);
        if (not status)
            return dmi_entity_incomplete(entity);

        leaf->leaf        = inputs[i].leaf;
        leaf->has_subleaf = inputs[i].has_subleaf;
        leaf->subleaf     = inputs[i].subleaf;

        info->leaf_count++;
    }

    return true;
}

void dmi_intel_rsd_processor_cpuid_cleanup(dmi_entity_t *entity)
{
    dmi_intel_rsd_processor_cpuid_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    if (info == nullptr)
        return;

    dmi_free(info->leaves);
}

#undef LEAF
#undef SUBLEAF

//
// Leaves of the known subtypes are written in the order the subtype lists
// them, and the data of the unknown ones as it is stored.
//
bool dmi_intel_rsd_processor_cpuid_encode(dmi_writer_t *writer)
{
    const dmi_intel_rsd_processor_cpuid_t *info =
            dmi_entity_info(writer->entity, DMI_TYPE(INTEL_RSD_PROCESSOR_CPUID));
    if (info == nullptr)
        return false;

    bool status =
        dmi_writer_put_string(writer, info->socket_designation) and
        dmi_writer_put(writer, dmi_byte_t, info->subtype);
    if (not status)
        return false;

    if (info->is_raw)
        return dmi_writer_put_bytes(writer, info->data.data, info->data.length);

    for (size_t i = 0; i < info->leaf_count; i++) {
        const dmi_intel_rsd_cpuid_leaf_t *leaf = &info->leaves[i];

        status =
            dmi_writer_put(writer, dmi_dword_t, leaf->eax) and
            dmi_writer_put(writer, dmi_dword_t, leaf->ebx) and
            dmi_writer_put(writer, dmi_dword_t, leaf->ecx) and
            dmi_writer_put(writer, dmi_dword_t, leaf->edx);
        if (not status)
            return false;
    }

    return true;
}
