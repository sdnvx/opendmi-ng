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

#include <opendmi/entity/processor-ex-internal.h>

const dmi_name_set_t dmi_processor_arch_names =
{
    .code  = "processor-architecture",
    .names = DMI_NAMES({
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
        {}
    })
};
