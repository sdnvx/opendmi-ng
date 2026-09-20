//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/processor-ex.h>

static bool dmi_processor_ex_decode(dmi_entity_t *entity);
static bool dmi_processor_ex_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_processor_arch_names =
{
    .code  = "processor-architectures",
    .names = (dmi_name_t[]){
        {
            .id   = DMI_PROCESSOR_ARCH_RESERVED,
            .code = "reserved",
            .name = "Reserved"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_IA32,
            .code = "ia32",
            .name = "IA32 (x86)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_AMD64,
            .code = "x64",
            .name = "x64 (x86-64, Intel 64, AMD64, EM64T)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_ITANIUM,
            .code = "itanium",
            .name = "Intel Itanium"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_AARCH32,
            .code = "aarch32",
            .name = "32-bit Arm (AArch32)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_AARCH64,
            .code = "aarch64",
            .name = "64-bit Arm (AArch64)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_RV32,
            .code = "rv32",
            .name = "32-bit RISC-V (RV32)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_RV64,
            .code = "rv64",
            .name = "64-bit RISC-V (RV64)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_RV128,
            .code = "rv128",
            .name = "128-bit RISC-V (RV128)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_LOONG_ARCH32,
            .code = "loongarch32",
            .name = "32-bit LoongArch (LoongArch32)"
        },
        {
            .id   = DMI_PROCESSOR_ARCH_LOONG_ARCH64,
            .code = "loongarch64",
            .name = "64-bit LoongArch (LoongArch64)"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_processor_ex_spec =
{
    .code            = "processor-ex",
    .name            = "Processor additional information",
    .description     = (const char *[]){
        "The information in this structure defines the processor additional "
        "information in case SMBIOS type 4 is not sufficient to describe "
        "processor characteristics.",
        //
        "The SMBIOS type 44 structure has a reference handle field to link "
        "back to the related SMBIOS type 4 structure. There may be multiple "
        "SMBIOS type 44 structures linked to the same SMBIOS type 4 "
        "structure. For example, when cores are not identical in a processor, "
        "SMBIOS type 44 structures describe different core-specific "
        "information.",
        //
        "SMBIOS type 44 defines the standard header for the "
        "processor-specific block (see 7.45.1), while the contents of "
        "processor-specific data are maintained by processor architecture "
        "workgroups or vendors in separate documents.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PROCESSOR_EX),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x06,
    .decoded_length  = sizeof(dmi_processor_ex_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_processor_ex_t, processor_handle, HANDLE, {
            .code   = "processor-handle",
            .name   = "Processor handle"
        }),
        DMI_ATTRIBUTE(dmi_processor_ex_t, arch, ENUM, {
            .code   = "architecture",
            .name   = "Architecture",
            .values = &dmi_processor_arch_names
        }),
        DMI_ATTRIBUTE(dmi_processor_ex_t, data, BINARY, {
            .code   = "data",
            .name   = "Processor-specific data"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers        = {
        .decode = dmi_processor_ex_decode,
        .link   = dmi_processor_ex_link
    }
};

static bool dmi_processor_ex_decode(dmi_entity_t *entity)
{
    dmi_processor_ex_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR_EX));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    // Processor-specific block may be absent, since the minimum length does
    // not include it
    if (not dmi_stream_decode(stream, dmi_word_t, &info->processor_handle))
        return false;
    if (dmi_stream_is_done(stream))
        return true;

    dmi_byte_t length = 0;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &length) and
        dmi_stream_decode(stream, dmi_byte_t, &info->arch);
    if (not status)
        return dmi_entity_incomplete(entity);

    // Data, which does not fit into the structure, is truncated
    if (length > dmi_stream_remaining(stream)) {
        dmi_stream_decode_bin(stream, dmi_stream_remaining(stream), &info->data);
        return dmi_entity_incomplete(entity);
    }

    return dmi_stream_decode_bin(stream, length, &info->data);
}

static bool dmi_processor_ex_link(dmi_entity_t *entity)
{
    dmi_processor_ex_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR_EX));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = dmi_get_registry(entity->context);

    return dmi_registry_resolve(registry, info->processor_handle, DMI_TYPE(PROCESSOR), &info->processor);
}
