//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_ENTITY_PROCESSOR_EX_H
#define OPENDMI_ENTITY_PROCESSOR_EX_H

#pragma once

#include <opendmi/entity.h>

#ifndef DMI_PROCESSOR_EX_T
#   define DMI_PROCESSOR_EX_T
    typedef struct dmi_processor_ex dmi_processor_ex_t;
#endif // !DMI_PROCESSOR_EX_T

#ifndef DMI_PROCESSOR_SPECIFIC_BLOCK_T
#   define DMI_PROCESSOR_SPECIFIC_BLOCK_T
    typedef struct dmi_processor_specific_block dmi_processor_specific_block_t;
#endif // !DMI_PROCESSOR_SPECIFIC_BLOCK_T

#ifndef DMI_PROCESSOR_AARCH64_DATA_T
#   define DMI_PROCESSOR_AARCH64_DATA_T
    typedef struct dmi_processor_aarch64_data dmi_processor_aarch64_data_t;
#endif // !DMI_PROCESSOR_AARCH64_DATA_T

#ifndef DMI_PROCESSOR_AMD64_ATTRIBUTE_T
#   define DMI_PROCESSOR_AMD64_ATTRIBUTE_T
    typedef struct dmi_processor_amd64_attribute dmi_processor_amd64_attribute_t;
#endif // !DMI_PROCESSOR_AMD64_ATTRIBUTE_T

#ifndef DMI_PROCESSOR_REVISION_T
#   define DMI_PROCESSOR_REVISION_T
    typedef union dmi_processor_revision dmi_processor_revision_t;
#endif // !DMI_PROCESSOR_REVISION_T

/**
 * @brief Processor architecture types.
 */
typedef enum dmi_processor_arch
{
    DMI_PROCESSOR_ARCH_RESERVED     = 0x00, ///< Reserved
    DMI_PROCESSOR_ARCH_IA32         = 0x01, ///< IA32 (x86)
    DMI_PROCESSOR_ARCH_AMD64        = 0x02, ///< x64 (x86-64, Intel64, AMD64, EM64T)
    DMI_PROCESSOR_ARCH_ITANIUM      = 0x03, ///< Intel Itanium architecture
    DMI_PROCESSOR_ARCH_AARCH32      = 0x04, ///< 32-bit ARM (Aarch32)
    DMI_PROCESSOR_ARCH_AARCH64      = 0x05, ///< 64-bit ARM (Aarch64)
    DMI_PROCESSOR_ARCH_RV32         = 0x06, ///< 32-bit RISC-V (RV32)
    DMI_PROCESSOR_ARCH_RV64         = 0x07, ///< 64-bit RISC-V (RV64)
    DMI_PROCESSOR_ARCH_RV128        = 0x08, ///< 128-bit RISC-V (RV128)
    DMI_PROCESSOR_ARCH_LOONG_ARCH32 = 0x09, ///< 32-bit LoongArch (LoongArch32)
    DMI_PROCESSOR_ARCH_LOONG_ARCH64 = 0x0A, ///< 64-bit LoongArch (LoongArch64)
} dmi_processor_arch_t;

/**
 * @brief Processor-specific block of the processor additional information
 * structure (type 44).
 *
 * The block is a header naming the length and the architecture of the data,
 * followed by the data, whose format depends on the architecture.
 */
dmi_packed_struct(dmi_processor_specific_block)
{
    /**
     * @brief Length of processor-specific data.
     */
    dmi_byte_t length;

    /**
     * @brief Processor architecture the data is about, one of the
     * `dmi_processor_arch_t` values.
     */
    dmi_byte_t arch;

    /**
     * @brief Raw processor-specific data, `length` bytes.
     */
    dmi_byte_t __data[];
};

/**
 * @brief Sub-types of the 64-bit Arm (AArch64) processor-specific data.
 */
typedef enum dmi_processor_aarch64_data_subtype
{
    DMI_PROCESSOR_AARCH64_DATA_SUBTYPE_AARCH64          = 0x00, ///< AArch64 Architecture data
    DMI_PROCESSOR_AARCH64_DATA_SUBTYPE_VENDOR_SPECIFIC  = 0x01, ///< Vendor specific data
} dmi_processor_aarch64_data_subtype_t;

/**
 * @brief Revision of the 64-bit Arm (Aarch64) Processor Specific Data.
 */
dmi_packed_union(dmi_processor_revision)
{
    /**
     * @brief Raw value.
     */
    dmi_word_t __value;

    /**
     * @brief Revision split into its minor and major numbers.
     */
    dmi_packed_struct()
    {
        /**
         * @brief Bits 7:0 Minor revision.
         */
        dmi_byte_t minor;

        /**
         * @brief Bits 15:8 Major revision.
         */
        dmi_byte_t major;
    };
};

dmi_static_assert_value_union(dmi_processor_revision);

/**
 * @brief The 64-bit ARM (Aarch64) processor-specific data.
 */
dmi_packed_struct(dmi_processor_aarch64_data)
{
    /**
     * @brief Revision of the 64-bit Arm (Aarch64) Processor Specific Data.
     */
    dmi_processor_revision_t revision;

    /**
     * @brief Length of the processor-specific data, in bytes: 8 bytes of
     * this header and the length of the sub-type specific data.
     */
    dmi_byte_t length;

    /**
     * @brief Reserved for future use. Must be zero.
     */
    dmi_byte_t __reserved;

    /**
     * @brief JEP-106 code of the processor vendor or silicon provider: the
     * bank index in bits 14:8, and the identification code with its parity
     * bit in bits 7:0. Bit 15 is zero.
     *
     * @todo It's a way more complex field.
     */
    dmi_word_t vendor_id;

    /**
     * @brief Sub-type of the data, one of the
     * `dmi_processor_aarch64_data_subtype_t` values.
     */
    dmi_byte_t subtype;

    /**
     * @brief Reserved for future use. Must be zero.
     */
    dmi_byte_t __reserved2;

    /**
     * @brief Data of the sub-type: AArch64 architecture data
     * (`struct dmi_processor_aarch64_arch_data`), or data defined by the
     * vendor.
     */
    dmi_byte_t subtype_specific_data[];
};

/**
 * @brief AArch64 architecture data, the sub-type 0 of the 64-bit Arm
 * (AArch64) processor-specific data.
 *
 * The data holds the values of the AArch64 identification registers. The
 * bit fields of the registers are defined by the Arm A-profile architecture.
 */
dmi_packed_struct(dmi_processor_aarch64_arch_data)
{
    /**
     * @brief Version of the AArch64 architecture data.
     */
    dmi_word_t version;

    /**
     * @brief Length, in bytes, of the AArch64 architecture data.
     */
    dmi_byte_t length;

    /**
     * @brief Reserved for future use. Must be zero.
     */
    dmi_byte_t __reserved;

    /**
     * @brief Reserved for future use. Must be zero.
     */
    dmi_dword_t __reserved2;

    /**
     * @brief Auxiliary feature register 0.
     */
    dmi_qword_t id_aa64afr0_el1;

    /**
     * @brief Auxiliary feature register 1.
     */
    dmi_qword_t id_aa64afr1_el1;

    /**
     * @brief Debug feature register 0.
     */
    dmi_qword_t id_aa64dfr0_el1;

    /**
     * @brief Debug feature register 1.
     */
    dmi_qword_t id_aa64dfr1_el1;

    /**
     * @brief Debug feature register 2.
     */
    dmi_qword_t id_aa64dfr2_el1;

    /**
     * @brief Floating-point feature register 0.
     */
    dmi_qword_t id_aa64fpfr0_el1;

    /**
     * @brief Instruction set attribute register 0.
     */
    dmi_qword_t id_aa64isar0_el1;

    /**
     * @brief Instruction set attribute register 1.
     */
    dmi_qword_t id_aa64isar1_el1;

    /**
     * @brief Instruction set attribute register 2.
     */
    dmi_qword_t id_aa64isar2_el1;

    /**
     * @brief Instruction set attribute register 3.
     */
    dmi_qword_t id_aa64isar3_el1;

    /**
     * @brief Memory model feature register 0.
     */
    dmi_qword_t id_aa64mmfr0_el1;

    /**
     * @brief Memory model feature register 1.
     */
    dmi_qword_t id_aa64mmfr1_el1;

    /**
     * @brief Memory model feature register 2.
     */
    dmi_qword_t id_aa64mmfr2_el1;

    /**
     * @brief Memory model feature register 3.
     */
    dmi_qword_t id_aa64mmfr3_el1;

    /**
     * @brief Memory model feature register 4.
     */
    dmi_qword_t id_aa64mmfr4_el1;

    /**
     * @brief Processor feature register 0.
     */
    dmi_qword_t id_aa64pfr0_el1;

    /**
     * @brief Processor feature register 1.
     */
    dmi_qword_t id_aa64pfr1_el1;

    /**
     * @brief Processor feature register 2.
     */
    dmi_qword_t id_aa64pfr2_el1;

    /**
     * @brief SME feature ID register 0.
     */
    dmi_qword_t id_aa64smfr0_el1;

    /**
     * @brief SVE feature ID register 0.
     */
    dmi_qword_t id_aa64zfr0_el1;
};

/**
 * @brief Use conditions of x64 processors.
 */
typedef enum dmi_processor_amd64_use_condition {
    DMI_PROCESSOR_AMD64_USE_CONDITION_CLIENT     = 0x00, ///< Client
    DMI_PROCESSOR_AMD64_USE_CONDITION_EMBEDDED   = 0x01, ///< Embedded
    DMI_PROCESSOR_AMD64_USE_CONDITION_INDUSTRIAL = 0x02  ///< Industrial
} dmi_processor_amd64_use_condition_t;

/**
 * @brief Temperature ranges of x64 processors.
 */
typedef enum dmi_processor_amd64_temperature_range {
    DMI_PROCESSOR_AMD64_TEMPERATURE_RANGE_COMMERCIAL = 0x00, ///< Commercial
    DMI_PROCESSOR_AMD64_TEMPERATURE_RANGE_EXTENDED   = 0x01  ///< Extended
} dmi_processor_amd64_temperature_range_t;

/**
 * @brief Attributes of the x64 processor use condition data.
 */
dmi_packed_struct(dmi_processor_amd64_attribute)
{
    /**
     * @brief Bits 7:0 Processor use condition, one of the
     * `dmi_processor_amd64_use_condition_t` values.
     */
    dmi_byte_t use_condition;

    /**
     * @brief Bits 15:8 Temperature range, one of the
     * `dmi_processor_amd64_temperature_range_t` values.
     */
    dmi_byte_t temperature_range;

    /**
     * @brief Bits 31:16 Reserved.
     */
    dmi_word_t __reserved;
};

/**
 * @brief Processor use condition data of the x64 processor-specific data.
 *
 * @todo Make sure this structure is correct.
 */
dmi_packed_struct(dmi_processor_amd64_data)
{
    /**
     * @brief Processor use condition data (0x01).
     */
    dmi_byte_t id;

    /**
     * @brief Length of processor use condition data structure.
     */
    dmi_byte_t length;

    /**
     * @brief Revision of the processor use condition data.
     */
    dmi_processor_revision_t revision;

    /**
     * @brief Use condition and temperature range of the processor.
     */
    dmi_processor_amd64_attribute_t attr;
};

/**
 * @brief RISC-V processor-specific data.
 */
dmi_packed_struct(dmi_processor_rv64_data)
{
    /**
     * @brief Revision of the RISC-V processor-specific data.
     */
    dmi_processor_revision_t revision;

    /**
     * @brief Identifier of the hardware thread (hart) described.
     */
    dmi_qword_t hart_id;

    /**
     * @brief Vendor of the core, as the `mvendorid` register holds it.
     */
    dmi_qword_t vendor_id;

    /**
     * @brief Base microarchitecture of the hart, as the `marchid` register
     * holds it.
     */
    dmi_qword_t arch_id;

    /**
     * @brief Version of the processor implementation, as the `mimpid`
     * register holds it.
     */
    dmi_qword_t machine_impl_id;
};

/**
 * @brief Processor additional information structure (type 44).
 */
struct dmi_processor_ex
{
    /**
     * @brief Handle of the processor structure (type 4), which the
     * information describes.
     */
    dmi_handle_t processor_handle;

    /**
     * @brief Processor structure (type 4). Set when linking, `nullptr` if
     * the handle refers to no structure.
     */
    dmi_entity_t *processor;

    /**
     * @brief Processor architecture of the processor-specific block.
     */
    dmi_processor_arch_t arch;

    /**
     * @brief Processor-specific data, as stored. Its format is defined by
     * processor architecture workgroups or vendors.
     */
    dmi_binary_t data;
};

/**
 * @brief Processor additional information entity specification.
 */
extern __dmi_api const dmi_entity_spec_t dmi_processor_ex_spec;

__BEGIN_DECLS

/**
 * @brief Get processor architecture name.
 *
 * Returns the name of the processor architecture, as the command line tool
 * shows it, translated into the locale when the library is built with the
 * translations.
 *
 * @param[in] value Processor architecture value.
 *
 * @return The name of the value, or @c nullptr if @p value has no name.
 */
__dmi_api const char *dmi_processor_arch_name(dmi_processor_arch_t value);

__END_DECLS

#endif // !OPENDMI_ENTITY_PROCESSOR_EX_H
