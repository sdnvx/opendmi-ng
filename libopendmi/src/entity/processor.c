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
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/processor.h>

static bool dmi_processor_decode(dmi_entity_t *entity);
static bool dmi_processor_decode_fields(dmi_entity_t *entity);
static void dmi_processor_decode_id(dmi_entity_t *entity, dmi_processor_t *info);
static void dmi_processor_decode_id_x86(dmi_entity_t *entity, dmi_processor_t *info,
                                        uint32_t low, uint32_t high);
static void dmi_processor_decode_id_midr(dmi_processor_t *info, uint32_t low);
static void dmi_processor_decode_id_soc(dmi_processor_t *info, uint32_t low, uint32_t high);
static dmi_processor_id_format_t dmi_processor_id_format(const dmi_processor_t *info);
static bool dmi_processor_is_x86_vendor(const char *str);
static bool dmi_processor_has_word(const char *str, const char *word);
static bool dmi_processor_link(dmi_entity_t *entity);

static const dmi_name_set_t dmi_processor_type_names =
{
    .code  = "processor-types",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_PROCESSOR_TYPE_UNSPEC),
        DMI_NAME_OTHER(DMI_PROCESSOR_TYPE_OTHER),
        DMI_NAME_UNKNOWN(DMI_PROCESSOR_TYPE_UNKNOWN),
        {
            .id   = DMI_PROCESSOR_TYPE_CENTRAL,
            .code = "central",
            .name = "Central processor"
        },
        {
            .id   = DMI_PROCESSOR_TYPE_MATH,
            .code = "math",
            .name = "Math processor"
        },
        {
            .id   = DMI_PROCESSOR_TYPE_DSP,
            .code = "dsp",
            .name = "DSP processor"
        },
        {
            .id   = DMI_PROCESSOR_TYPE_VIDEO,
            .code = "video",
            .name = "Video processor"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_processor_family_names =
{
    .code  = "processor-families",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_PROCESSOR_FAMILY_UNSPEC),
        DMI_NAME_OTHER(DMI_PROCESSOR_FAMILY_OTHER),
        DMI_NAME_UNKNOWN(DMI_PROCESSOR_FAMILY_UNKNOWN),
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_8086,
            .code = "intel-8086",
            .name = "Intel 8086"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80286,
            .code = "intel-80286",
            .name = "Intel 80286"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80386,
            .code = "intel-80386",
            .name = "Intel 80386"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80486,
            .code = "intel-80486",
            .name = "Intel 80486"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_8087,
            .code = "intel-8087",
            .name = "Intel 8087"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80287,
            .code = "intel-80287",
            .name = "Intel 80287"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80387,
            .code = "intel-80387",
            .name = "Intel 80387"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_80487,
            .code = "intel-80487",
            .name = "Intel 80487"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM,
            .code = "intel-pentium",
            .name = "Intel Pentium"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_PRO,
            .code = "intel-pentium-pro",
            .name = "Intel Pentium Pro"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_2,
            .code = "intel-pentium-2",
            .name = "Intel Pentium II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_MMX,
            .code = "intel-pentium-mmx",
            .name = "Intel Pentium MMX"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CELERON,
            .code = "intel-celeron",
            .name = "Intel Celeron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_2_XEON,
            .code = "intel-pentium-2-xeon",
            .name = "Intel Pentium II Xeon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_3,
            .code = "intel-pentium-3",
            .name = "Intel Pentium III"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_APPLE_M1,
            .code = "apple-m1",
            .name = "Apple M1"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_APPLE_M2,
            .code = "apple-m2",
            .name = "Apple M2",
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CELERON_M,
            .code = "intel-celeron-m",
            .name = "Intel Celeron M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_4_HT,
            .code = "intel-pentium-4-ht",
            .name = "Intel Pentium 4 HT"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL,
            .code = "intel",
            .name = "Intel"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_DURON,
            .code = "amd-duron",
            .name = "AMD Duron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_K5,
            .code = "amd-k5",
            .name = "AMD K5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_K6,
            .code = "amd-k6",
            .name = "AMD K6"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_K6_2,
            .code = "amd-k6-2",
            .name = "AMD K6-2"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_K6_3,
            .code = "amd-k6-3",
            .name = "AMD K6-3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON,
            .code = "amd-athlon",
            .name = "AMD Athlon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_29000,
            .code = "amd-29000",
            .name = "AMD 29000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_K6_2_PLUS,
            .code = "amd-k6-2-plus",
            .name = "AMD K6-2+"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC,
            .code = "powerpc",
            .name = "PowerPC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_601,
            .code = "powerpc-601",
            .name = "PowerPC 601"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_603,
            .code = "powerpc-603",
            .name = "PowerPC 603"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_603_PLUS,
            .code = "powerpc-603-plus",
            .name = "PowerPC 603+"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_604,
            .code = "powerpc-604",
            .name = "PowerPC 604"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_620,
            .code = "powerpc-620",
            .name = "PowerPC 620"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_X704,
            .code = "powerpc-x704",
            .name = "PowerPC x704"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_750,
            .code = "powerpc-750",
            .name = "PowerPC 750"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_DUO,
            .code = "intel-core-duo",
            .name = "Intel Core Duo"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_DUO_M,
            .code = "intel-core-duo-m",
            .name = "Intel Core Duo M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_SOLO_M,
            .code = "intel-core-solo-m",
            .name = "Intel Core Solo M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_ATOM,
            .code = "intel-atom",
            .name = "Intel Atom"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_M,
            .code = "intel-core-m",
            .name = "Intel Core M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_M3,
            .code = "intel-core-m3",
            .name = "Intel Core m3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_M5,
            .code = "intel-core-m5",
            .name = "Intel Core m5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_M7,
            .code = "intel-core-m7",
            .name = "Intel Core m7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA,
            .code = "alpha",
            .name = "Alpha"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21064,
            .code = "alpha-21064",
            .name = "Alpha 21064"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21066,
            .code = "alpha-21066",
            .name = "Alpha 21066"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21164,
            .code = "alpha-21164",
            .name = "Alpha 21164"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21164PC,
            .code = "alpha-21164pc",
            .name = "Alpha 21164PC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21164A,
            .code = "alpha-21164a",
            .name = "Alpha 21164a"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21264,
            .code = "alpha-21264",
            .name = "Alpha 21264"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ALPHA_21364,
            .code = "alpha-21364",
            .name = "Alpha 21364"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_2_ULTRA_2C_M,
            .code = "amd-turion-2-ultra-2c-m",
            .name = "AMD Turion II Ultra Dual-Core M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_2_2C_M,
            .code = "amd-turion-2-2c-m",
            .name = "AMD Turion II Dual-Core M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_2_2C_M,
            .code = "amd-athlon-2-2c-m",
            .name = "AMD Athlon II Dual-Core M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_6100,
            .code = "amd-opteron-6100",
            .name = "AMD Opteron 6100 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_4100,
            .code = "amd-opteron-4100",
            .name = "AMD Opteron 4100 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_6200,
            .code = "amd-opteron-6200",
            .name = "AMD Opteron 6200 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_4200,
            .code = "amd-opteron-4200",
            .name = "AMD Opteron 4200 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_FX,
            .code = "amd-fx",
            .name = "AMD FX series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS,
            .code = "mips",
            .name = "MIPS"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS_R4000,
            .code = "mips-r4000",
            .name = "MIPS R4000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS_R4200,
            .code = "mips-r4200",
            .name = "MIPS R4200"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS_R4400,
            .code = "mips-r4400",
            .name = "MIPS R4400"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS_R4600,
            .code = "mips-r4600",
            .name = "MIPS R4600"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MIPS_R10000,
            .code = "mips-r10000",
            .name = "MIPS R10000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_C,
            .code = "amd-c",
            .name = "AMD C-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_E,
            .code = "amd-e",
            .name = "AMD E-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_A,
            .code = "amd-a",
            .name = "AMD A-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_G,
            .code = "amd-g",
            .name = "AMD G-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_Z,
            .code = "amd-z",
            .name = "AMD Z-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_R,
            .code = "amd-r",
            .name = "AMD R-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_4300,
            .code = "amd-opteron-4300",
            .name = "AMD Opteron 4300 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_6300,
            .code = "amd-opteron-6300",
            .name = "AMD Opteron 6300 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_3300,
            .code = "amd-opteron-3300",
            .name = "AMD Opteron 3300 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_FIREPRO,
            .code = "amd-firepro",
            .name = "AMD FirePro series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_SPARC,
            .code = "sparc",
            .name = "SPARC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_SUPERSPARC,
            .code = "supersparc",
            .name = "SuperSPARC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MICROSPARC_2,
            .code = "microsparc-2",
            .name = "microSPARC II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MICROSPARC_2EP,
            .code = "microsparc-2ep",
            .name = "microSPARC IIep"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ULTRASPARC,
            .code = "ultrasparc",
            .name = "UltraSPARC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ULTRASPARC_2,
            .code = "ultrasparc-2",
            .name = "UltraSPARC II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ULTRASPARC_2I,
            .code = "ultrasparc-2i",
            .name = "UltraSPARC IIi"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ULTRASPARC_3,
            .code = "ultrasparc-3",
            .name = "UltraSPARC III"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ULTRASPARC_3I,
            .code = "ultrasparc-3i",
            .name = "UltraSPARC IIIi"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68040,
            .code = "motorola-68040",
            .name = "Motorola 68040"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68XXX,
            .code = "motorola-68xxx",
            .name = "Motorola 68xxx"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68000,
            .code = "motorola-68000",
            .name = "Motorola 68000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68010,
            .code = "motorola-68010",
            .name = "Motorola 68010"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68020,
            .code = "motorola-68020",
            .name = "Motorola 68020"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_MOTOROLA_68030,
            .code = "motorola-68030",
            .name = "Motorola 68030"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_X4_4C,
            .code = "amd-athlon-x4-4c",
            .name = "AMD Athlon X4 Quad-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_X1000,
            .code = "amd-opteron-x1000",
            .name = "AMD Opteron X1000 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_X2000,
            .code = "amd-opteron-x2000",
            .name = "AMD Opteron X2000 series APU"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_A,
            .code = "amd-opteron-a",
            .name = "AMD Opteron A-series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_X3000,
            .code = "amd-opteron-x3000",
            .name = "AMD Opteron X3000 series APU"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ZEN,
            .code = "amd-zen",
            .name = "AMD Zen"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_HOBBIT,
            .code = "hobbit",
            .name = "Hobbit"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_CRUSOE_TM5000,
            .code = "crusoe-tm5000",
            .name = "Transmeta Crusoe TM5000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_CRUSOE_TM3000,
            .code = "crusoe-tm3000",
            .name = "Transmeta Crusoe TM3000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_EFFICEON_TM8000,
            .code = "efficeon-tm8000",
            .name = "Transmeta Efficeon TM8000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_WEITEK,
            .code = "weitek",
            .name = "Weitek"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_ITANIUM,
            .code = "intel-itanium",
            .name = "Intel Itanium"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_64,
            .code = "amd-athlon-64",
            .name = "AMD Athlon 64"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON,
            .code = "amd-opteron",
            .name = "AMD Opteron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_SEMPRON,
            .code = "amd-sempron",
            .name = "AMD Sempron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_64_M,
            .code = "amd-turion-64-m",
            .name = "AMD Turion 64 Mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_2C,
            .code = "amd-opteron-2c",
            .name = "Dual-Core AMD Opteron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_64_X2_2C,
            .code = "amd-athlon-64-x2-2c",
            .name = "AMD Athlon 64 X2 Dual-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_64_X2_M,
            .code = "amd-turion-64-x2-m",
            .name = "AMD Turion 64 X2 Mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_4C,
            .code = "amd-opteron-4c",
            .name = "Quad-Core AMD Opteron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_G3,
            .code = "amd-opteron-g3",
            .name = "Third-Generation AMD Opteron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_PHENOM_FX_4C,
            .code = "amd-phenom-fx-4c",
            .name = "AMD Phenom FX Quad-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_PHENOM_X4_4C,
            .code = "amd-phenom-x4-4c",
            .name = "AMD Phenom X4 Quad-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_PHENOM_X2_2C,
            .code = "amd-phenom-x2-2c",
            .name = "AMD Phenom X2 Dual-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_X2_2C,
            .code = "amd-athlon-x2-2c",
            .name = "AMD Athlon X2 Dual-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC,
            .code = "parisc",
            .name = "PA-RISC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_8500,
            .code = "parisc-8500",
            .name = "PA-RISC 8500"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_8000,
            .code = "parisc-8000",
            .name = "PA-RISC 8000"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_7300LC,
            .code = "parisc-7300lc",
            .name = "PA-RISC 7300LC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_7200,
            .code = "parisc-7200",
            .name = "PA-RISC 7200"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_7100LC,
            .code = "parisc-7100lc",
            .name = "PA-RISC 7100LC"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_PARISC_7100,
            .code = "parisc-7100",
            .name = "PA-RISC 7100"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_V30,
            .code = "v30",
            .name = "V30"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_3200_4C,
            .code = "intel-xeon-3200-4c",
            .name = "Quad-Core Intel Xeon 3200 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_3000_2C,
            .code = "intel-xeon-3000-2c",
            .name = "Dual-Core Intel Xeon 3000 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5300_4C,
            .code = "intel-xeon-5300-4c",
            .name = "Quad-Core Intel Xeon 5300 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5100_2C,
            .code = "intel-xeon-5100-2c",
            .name = "Dual-Core Intel Xeon 5100 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5000_2C,
            .code = "intel-xeon-5000-2c",
            .name = "Dual-Core Intel Xeon 5000 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_LV_2C,
            .code = "intel-xeon-lv-2c",
            .name = "Dual-Core Intel Xeon LV"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_ULV_2C,
            .code = "intel-xeon-ulv-2c",
            .name = "Dual-Core Intel Xeon ULV"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7100_2C,
            .code = "intel-xeon-7100-2c",
            .name = "Dual-Core Intel Xeon 7100 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5400_4C,
            .code = "intel-xeon-5400-4c",
            .name = "Quad-Core Intel Xeon 5400 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_4C,
            .code = "intel-xeon-4c",
            .name = "Quad-Core Intel Xeon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5200_2C,
            .code = "intel-xeon-5200-2c",
            .name = "Dual-Core Intel Xeon 5200 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7200_2C,
            .code = "intel-xeon-7200-2c",
            .name = "Dual-Core Intel Xeon 7200 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7300_4C,
            .code = "intel-xeon-7300-4c",
            .name = "Quad-Core Intel Xeon 7300 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7400_4C,
            .code = "intel-xeon-7400-4c",
            .name = "Quad-Core Intel Xeon 7400 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7400_MC,
            .code = "intel-xeon-7400-mc",
            .name = "Multi-Core Intel Xeon 7400 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_3_XEON,
            .code = "intel-pentium-3-xeon",
            .name = "Intel Pentium III Xeon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_3_SS,
            .code = "intel-pentium-3-ss",
            .name = "Intel Pentium III with SpeedStep technology"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_4,
            .code = "intel-pentium-4",
            .name = "Intel Pentium 4"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON,
            .code = "intel-xeon",
            .name = "Intel Xeon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_IBM_AS400,
            .code = "ibm-as400",
            .name = "IBM AS400"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_MP,
            .code = "intel-xeon-mp",
            .name = "Intel Xeon MP"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_XP,
            .code = "amd-athlon-xp",
            .name = "AMD Athlon XP"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_MP,
            .code = "amd-athlon-mp",
            .name = "AMD Athlon MP"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_ITANIUM_2,
            .code = "intel-itanium-2",
            .name = "Intel Itanium 2"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_M,
            .code = "intel-pentium-m",
            .name = "Intel Pentium M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CELERON_D,
            .code = "intel-celeron-d",
            .name = "Intel Celeron D"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_D,
            .code = "intel-pentium-d",
            .name = "Intel Pentium D"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_PENTIUM_EXTREME,
            .code = "intel-pentium-extreme",
            .name = "Intel Pentium Extreme Edition"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_SOLO,
            .code = "intel-core-solo",
            .name = "Intel Core Solo"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_RESERVED_1,
            .code = "reserved",
            .name = "Reserved (AMD K7/Intel Core 2)"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_DUO,
            .code = "intel-core-2-duo",
            .name = "Intel Core 2 Duo"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_SOLO,
            .code = "intel-core-2-solo",
            .name = "Intel Core 2 Solo"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_EXTREME,
            .code = "intel-core-2-extreme",
            .name = "Intel Core 2 Extreme"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_QUAD,
            .code = "intel-core-2-quad",
            .name = "Intel Core 2 Quad"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_EXTREME_M,
            .code = "intel-core-2-extreme-m",
            .name = "Intel Core 2 Extreme mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_DUO_M,
            .code = "intel-core-2-duo-m",
            .name = "Intel Core 2 Duo mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_2_SOLO_M,
            .code = "intel-core-2-solo-m",
            .name = "Intel Core 2 Solo mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_I7,
            .code = "intel-core-i7",
            .name = "Intel Core i7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CELERON_2C,
            .code = "intel-celeron-2c",
            .name = "Dual-Core Intel Celeron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_IBM_ESA390,
            .code = "ibm-esa390",
            .name = "IBM ESA/390"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_G4,
            .code = "powerpc-g4",
            .name = "PowerPC G4"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_POWERPC_G5,
            .code = "powerpc-g5",
            .name = "PowerPC G5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_IBM_ESA390_G6,
            .code = "ibm-esa390-g6",
            .name = "IBM ESA/390 G6"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_IBM_ZARCH,
            .code = "ibm-zarch",
            .name = "IBM z/Architecture"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_I5,
            .code = "intel-core-i5",
            .name = "Intel Core i5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_I3,
            .code = "intel-core-i3",
            .name = "Intel Core i3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_I9,
            .code = "intel-core-i9",
            .name = "Intel Core i9"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_D,
            .code = "intel-xeon-d",
            .name = "Intel Xeon D"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIA_C7_M,
            .code = "via-c7-m",
            .name = "VIA C7-M"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIA_C7_D,
            .code = "via-c7-d",
            .name = "VIA C7-D"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIA_C7,
            .code = "via-c7",
            .name = "VIA C7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIA_EDEN,
            .code = "via-eden",
            .name = "VIA Eden"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_MC,
            .code = "intel-xeon-mc",
            .name = "Multi-Core Intel Xeon"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_3XXX_2C,
            .code = "intel-xeon-3xxx-2c",
            .name = "Dual-Core Intel Xeon 3xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_3XXX_4C,
            .code = "intel-xeon-3xxx-4c",
            .name = "Quad-Core Intel Xeon 3xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIA_NANO,
            .code = "via-nano",
            .name = "VIA Nano"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5XXX_2C,
            .code = "intel-xeon-5xxx-2c",
            .name = "Dual-Core Intel Xeon 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_5XXX_4C,
            .code = "intel-xeon-5xxx-4c",
            .name = "Quad-Core Intel Xeon 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7XXX_2C,
            .code = "intel-xeon-7xxx-2c",
            .name = "Dual-Core Intel Xeon 7xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7XXX_4C,
            .code = "intel-xeon-7xxx-4c",
            .name = "Quad-Core Intel Xeon 7xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_7XXX_MC,
            .code = "intel-xeon-7xxx-mc",
            .name = "Multi-Core Intel Xeon 7xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_XEON_3400_MC,
            .code = "intel-xeon-3400-mc",
            .name = "Multi-Core Intel Xeon 3400 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_3000,
            .code = "amd-opteron-3000",
            .name = "AMD Opteron 3000 series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_SEMPRON_2,
            .code = "amd-sempron-2",
            .name = "AMD Sempron II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_EMBEDDED_4C,
            .code = "amd-opteron-embedded-4c",
            .name = "Embedded AMD Opteron Quad-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_PHENOM_3C,
            .code = "amd-phenon-3c",
            .name = "AMD Phenom Triple-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_ULTRA_M_2C,
            .code = "amd-turion-ultra-m-2c",
            .name = "AMD Turion Ultra Dual-Core mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_TURION_M_2C,
            .code = "amd-turion-m-2c",
            .name = "AMD Turion Dual-Core mobile"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_2C,
            .code = "amd-athlon-2c",
            .name = "AMD Athlon Dual-Core"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_SEMPRON_SI,
            .code = "amd-sempron-si",
            .name = "AMD Sempron SI"
        },
        {
            .id = DMI_PROCESSOR_FAMILY_AMD_PHENOM_2,
            .code = "amd-phenon-2",
            .name = "AMD Phenom II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_ATHLON_2,
            .code = "amd-athlon-2",
            .name = "AMD Athlon II"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_OPTERON_6C,
            .code = "amd-opteron-6c",
            .name = "Six-Core AMD Opteron"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_AMD_SEMPRON_M,
            .code = "amd-sempron-m",
            .name = "AMD Sempron M"
        },
        {
            .id = DMI_PROCESSOR_FAMILY_INTEL_I860,
            .code = "intel-i860",
            .name = "Intel i860"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_I960,
            .code = "intel-i960",
            .name = "Intel i960"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_EXTENDED,
            .code = "extended",
            .name = "Processor family from the extended field"
        },
        {
            .id = DMI_PROCESSOR_FAMILY_RESERVED_2,
            .code = "reserved",
            .name = "Reserved"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ARM_V7,
            .code = "arm-v7",
            .name = "ARM v7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ARM_V8,
            .code = "arm-v8",
            .name = "ARMv8"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ARM_V9,
            .code = "arm-v9",
            .name = "ARMv9"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ARM_RESERVED,
            .code = "arm-reserved",
            .name = "Reserved for future use by ARM"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_HITACHI_SH_3,
            .code = "hitachi-sh-3",
            .name = "Hitachi SH-3",
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_HITACHI_SH_4,
            .code = "hitachi-sh-4",
            .name = "Hitachi SH-4"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_ARM,
            .code = "arm",
            .name = "ARM"
        },
        {
            .id   = DMI_PROCESSOR_FAMILT_STRONGARM,
            .code = "strongarm",
            .name = "StrongARM"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_CYRIX_6X86,
            .code = "cyrix-6x86",
            .name = "Cyrix 6x86"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_CYRIX_MEDIAGX,
            .code = "cyrix-mediagx",
            .name = "Cyrix MediaGX"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_CYRIX_M2,
            .code = "cyrix-m2",
            .name = "Cyrix MII"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_WINCHIP,
            .code = "winchip",
            .name = "WinChip"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_DSP,
            .code = "dsp",
            .name = "DSP"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_VIDEO,
            .code = "video",
            .name = "Video"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_RISCV_RV32,
            .code = "riscv-rv32",
            .name = "RISC-V RV32"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_RISCV_RV64,
            .code = "riscv-rv64",
            .name = "RISC-V RV64"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_RISCV_RV128,
            .code = "riscv-rv128",
            .name = "RISC-V RV128"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGARCH,
            .code = "loongarch",
            .name = "LoongArch"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_1,
            .code = "loongson-1",
            .name = "Loongson 1"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_2,
            .code = "loongson-2",
            .name = "Loongson 2",
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3,
            .code = "loongson-3",
            .name = "Loongson 3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_2K,
            .code = "loongson-2k",
            .name = "Loongson 2K"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3A,
            .code = "loongson-3a",
            .name = "Loongson 3A"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3B,
            .code = "loongson-3b",
            .name = "Loongson 3B"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3C,
            .code = "loongson-3c",
            .name = "Loongson 3C"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3D,
            .code = "loongson-3d",
            .name = "Loongson 3D"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3E,
            .code = "loongson-3e",
            .name = "Loongson 3E"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_2K_2XXX_2C,
            .code = "loongson-2k-2xxx-2c",
            .name = "Dual-Core Loongson 2K 2xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3A_5XXX_4C,
            .code = "loongson-3a-5xxx-4c",
            .name = "Quad-Core Loongson 3A 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3A_5XXX_MC,
            .code = "loongson-3a-5xxx-mc",
            .name = "Multi-Core Loongson 3A 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3B_5XXX_4C,
            .code = "loongson-3b-5xxx-4c",
            .name = "Quad-Core Loongson 3B 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3B_5XXX_MC,
            .code = "loongson-3b-5xxx-mc",
            .name = "Multi-Core Loongson 3B 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3C_5XXX_MC,
            .code = "loongson-3c-5xxx-mc",
            .name = "Multi-Core Loongson 3C 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_LOONGSON_3D_5XXX_MC,
            .code = "loongson-3d-5xxx-mc",
            .name = "Multi-Core Loongson 3D 5xxx series"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_3,
            .code = "intel-core-3",
            .name = "Intel Core 3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_5,
            .code = "intel-core-5",
            .name = "Intel Core 5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_7,
            .code = "intel-core-7",
            .name = "Intel Core 7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_9,
            .code = "intel-core-9",
            .name = "Intel Core 9"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_ULTRA_3,
            .code = "intel-core-ultra-3",
            .name = "Intel Core Ultra 3"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_ULTRA_5,
            .code = "intel-core-ultra-5",
            .name = "Intel Core Ultra 5"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_ULTRA_7,
            .code = "intel-core-ultra-7",
            .name = "Intel Core Ultra 7"
        },
        {
            .id   = DMI_PROCESSOR_FAMILY_INTEL_CORE_ULTRA_9,
            .code = "intel-core-ultra-9",
            .name = "Intel Core Ultra 9"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_processor_status_names =
{
    .code  = "processor-statuses",
    .names = (dmi_name_t[]){
        DMI_NAME_UNKNOWN(DMI_PROCESSOR_STATUS_UNKNOWN),
        {
            .id   = DMI_PROCESSOR_STATUS_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_PROCESSOR_STATUS_DISABLED_BY_USER,
            .code = "disabled-by-user",
            .name = "Disabled by user"
        },
        {
            .id   = DMI_PROCESSOR_STATUS_DISABLED_BY_FW,
            .code = "disabled-by-firmware",
            .name = "Disabled by firmware"
        },
        {
            .id   = DMI_PROCESSOR_STATUS_IDLE,
            .code = "idle",
            .name = "Idle"
        },
        DMI_NAME_OTHER(DMI_PROCESSOR_STATUS_OTHER),
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_processor_voltage_names =
{
    .code  = "processor-voltages",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "5v",
            .name = "5V"
        },
        {
            .id   = 1,
            .code = "3v3",
            .name = "3.3V"
        },
        {
            .id   = 2,
            .code = "2v9",
            .name = "2.9V"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_processor_upgrade_names =
{
    .code  = "processor-upgrades",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_PROCESSOR_UPGRADE_UNSPEC),
        DMI_NAME_OTHER(DMI_PROCESSOR_UPGRADE_OTHER),
        DMI_NAME_UNKNOWN(DMI_PROCESSOR_UPGRADE_UNKNOWN),
        {
            .id   = DMI_PROCESSOR_UPGRADE_DAUGHTER_BOARD,
            .code = "daughter-board",
            .name = "Daughter board"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_ZIF,
            .code = "socket-zif",
            .name = "ZIF socket"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_PIGGY_BACK,
            .code = "piggy-back",
            .name = "Replaceable piggy-back"
        },
        DMI_NAME_NONE(DMI_PROCESSOR_UPGRADE_NONE),
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LIF,
            .code = "socket-lif",
            .name = "LIF socket"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SLOT_1,
            .code = "slot-1",
            .name = "Slot 1"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SLOT_2,
            .code = "slot-2",
            .name = "Slot 2"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_370PIN,
            .code = "socket-370pin",
            .name = "370-pin socket"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SLOT_A,
            .code = "slot-a",
            .name = "Slot A"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SLOT_M,
            .code = "slot-m",
            .name = "Slot M"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_423,
            .code = "socket-423",
            .name = "Socket 423"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_A,
            .code = "socket-a",
            .name = "Socket A (462)"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_478,
            .code = "socket-478",
            .name = "Socket 478"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_754,
            .code = "socket-754",
            .name = "Socket 754"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_940,
            .code = "socket-940",
            .name = "Socket 940"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_939,
            .code = "socket-939",
            .name = "Socket 939"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_MPGA604,
            .code = "socket-mpga604",
            .name = "Socket mPGA604"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA771,
            .code = "socket-lga771",
            .name = "Socket LGA771"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA775,
            .code = "socket-lga775",
            .name = "Socket LGA775"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_S1,
            .code = "socket-s1",
            .name = "Socket S1"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_AM2,
            .code = "socket-am2",
            .name = "Socket AM2"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_F,
            .code = "socket-f",
            .name = "Socket F (1207)"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1366,
            .code = "socket-lga1366",
            .name = "Socket LGA1366"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_G34,
            .code = "socket-g34",
            .name = "Socket G34"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_AM3,
            .code = "socket-am3",
            .name = "Socket AM3"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_C32,
            .code = "socket-c32",
            .name = "Socket C32"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1156,
            .code = "socket-lga1156",
            .name = "Socket LGA1156"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1567,
            .code = "socket-lga1567",
            .name = "Socket LGA1567"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_PGA988A,
            .code = "socket-pga988a",
            .name = "Socket PGA988A"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1288,
            .code = "socket-bga1288",
            .name = "Socket BGA1288"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_RPGA988B,
            .code = "socket-rpga988b",
            .name = "Socket rPGA988B"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1023,
            .code = "socket-bga1023",
            .name = "Socket BGA1023"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1224,
            .code = "socket-bga1224",
            .name = "Socket BGA1224"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1155,
            .code = "socket-lga1155",
            .name = "Socket LGA1155"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1356,
            .code = "socket-lga1356",
            .name = "Socket LGA1356"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA2011,
            .code = "socket-lga2011",
            .name = "Socket LGA2011"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_FS1,
            .code = "socket-fs1",
            .name = "Socket FS1"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_FS2,
            .code = "socket-fs2",
            .name = "Socket FS2"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_FM1,
            .code = "socket-fm1",
            .name = "Socket FM1"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_FM2,
            .code = "socket-fm2",
            .name = "Socket FM2"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA2011_3,
            .code = "socket-lga2011-3",
            .name = "Socket LGA2011-3"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1356_3,
            .code = "socket-lga1356-3",
            .name = "Socket LGA1356-3"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1150,
            .code = "socket-lga1150",
            .name = "Socket LGA1150"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1168,
            .code = "socket-bga1168",
            .name = "Socket BGA1168"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1234,
            .code = "socket-bga1234",
            .name = "Socket BGA1234"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1364,
            .code = "socket-bga1364",
            .name = "Socket BGA1364"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_AM4,
            .code = "socket-am4",
            .name = "Socket AM4"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1151,
            .code = "socket-lga1151",
            .name = "Socket LGA1151"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1356,
            .code = "socket-bga1356",
            .name = "Socket BGA1356"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1440,
            .code = "socket-bga1440",
            .name = "Socket BGA1440"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1515,
            .code = "socket-bga1515",
            .name = "Socket BGA1515"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA3647_1,
            .code = "socket-lga3647-1",
            .name = "Socket LGA3647-1"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_SP3,
            .code = "socket-sp3",
            .name = "Socket SP3"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_SP3R2,
            .code = "socket-sp3r2",
            .name = "Socket SP3r2"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA2066,
            .code = "socket-lga2066",
            .name = "Socket LGA2066"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1392,
            .code = "socket-bga1392",
            .name = "Socket BGA1392"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1510,
            .code = "socket-bga1510",
            .name = "Socket BGA1510"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1528,
            .code = "socket-bga1528",
            .name = "Socket BGA1528"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA4189,
            .code = "socket-lga4189",
            .name = "Socket LGA4189"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1200,
            .code = "socket-lga1200",
            .name = "Socket LGA1200"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA4677,
            .code = "socket-lga4677",
            .name = "Socket LGA4677"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1700,
            .code = "socket-lga1700",
            .name = "Socket LGA1700"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1744,
            .code = "socket-bga1744",
            .name = "Socket BGA1744"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1781,
            .code = "socket-bga1781",
            .name = "Socket BGA1781"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1211,
            .code = "socket-bga1211",
            .name = "Socket BGA1211"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA2422,
            .code = "socket-bga2422",
            .name = "Socket BGA2422"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1211,
            .code = "socket-lga1211",
            .name = "Socket LGA1211"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA2422,
            .code = "socket-lga2422",
            .name = "Socket LGA2422"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA5773,
            .code = "socket-lga5773",
            .name = "Socket LGA5773"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA5773,
            .code = "sockt-bga5773",
            .name = "Socket BGA5773"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_AM5,
            .code = "socket-am5",
            .name = "Socket AM5"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_SP5,
            .code = "socket-sp5",
            .name = "Socket SP5"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_SP6,
            .code = "socket-sp6",
            .name = "Socket SP6"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA883,
            .code = "socket-bga883",
            .name = "Socket BGA883"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1190,
            .code = "socket-bga1190",
            .name = "Socket BGA1190"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA4129,
            .code = "socket-bga4129",
            .name = "Socket BGA4129"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA4710,
            .code = "socket-lga4710",
            .name = "Socket LGA4710"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA7529,
            .code = "socket-lga7529",
            .name = "Socket LGA7529"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1964,
            .code = "socket-bga1964",
            .name = "Socket BGA1964"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA1792,
            .code = "socket-bga1792",
            .name = "Socket BGA1792"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA2049,
            .code = "socket-bga2049",
            .name = "Socket BGA2049"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA2551,
            .code = "socket-bga2551",
            .name = "Socket BGA2551"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_LGA1851,
            .code = "socket-lga1851",
            .name = "Socket LGA1851"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA2114,
            .code = "socket-bga2114",
            .name = "Socket BGA2114"
        },
        {
            .id   = DMI_PROCESSOR_UPGRADE_SOCKET_BGA2833,
            .code = "socket-bga2833",
            .name = "Socket BGA2833"
        },
        DMI_NAME_NULL
    }
};

const dmi_name_set_t dmi_processor_features_names =
{
    .code  = "features",
    .names = (dmi_name_t[]){
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "capable-64bit",
            .name = "64-bit Capable"
        },
        {
            .id   = 3,
            .code = "multicore",
            .name = "Multi-Core"
        },
        {
            .id   = 4,
            .code = "hardware_thread",
            .name = "Hardware Thread"
        },
        {
            .id   = 5,
            .code = "execute-protection",
            .name = "Execute Protection"
        },
        {
            .id   = 6,
            .code = "enhanced-virtualization",
            .name = "Enhanced Virtualization"
        },
        {
            .id   = 7,
            .code = "power-perf-control",
            .name = "Power/Performance Control"
        },
        {
            .id   = 8,
            .code = "capable-128bit",
            .name = "128-bit Capable"
        },
        {
            .id   = 9,
            .code = "arm64-soc-id",
            .name = "Arm64 SoC ID"
        },
        DMI_NAME_NULL
    }
};

//
// Feature flags of x86 processors, as returned by CPUID leaf 1 in EDX register
//
static const dmi_name_set_t dmi_processor_x86_feature_names =
{
    .code  = "processor-x86-features",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "fpu",
            .name = "FPU (floating-point unit on-chip)"
        },
        {
            .id   = 1,
            .code = "vme",
            .name = "VME (virtual mode extension)"
        },
        {
            .id   = 2,
            .code = "de",
            .name = "DE (debugging extension)"
        },
        {
            .id   = 3,
            .code = "pse",
            .name = "PSE (page size extension)"
        },
        {
            .id   = 4,
            .code = "tsc",
            .name = "TSC (time stamp counter)"
        },
        {
            .id   = 5,
            .code = "msr",
            .name = "MSR (model specific registers)"
        },
        {
            .id   = 6,
            .code = "pae",
            .name = "PAE (physical address extension)"
        },
        {
            .id   = 7,
            .code = "mce",
            .name = "MCE (machine check exception)"
        },
        {
            .id   = 8,
            .code = "cx8",
            .name = "CX8 (CMPXCHG8B instruction)"
        },
        {
            .id   = 9,
            .code = "apic",
            .name = "APIC (on-chip APIC)"
        },
        {
            .id   = 11,
            .code = "sep",
            .name = "SEP (fast system call)"
        },
        {
            .id   = 12,
            .code = "mtrr",
            .name = "MTRR (memory type range registers)"
        },
        {
            .id   = 13,
            .code = "pge",
            .name = "PGE (page global enable)"
        },
        {
            .id   = 14,
            .code = "mca",
            .name = "MCA (machine check architecture)"
        },
        {
            .id   = 15,
            .code = "cmov",
            .name = "CMOV (conditional move instructions)"
        },
        {
            .id   = 16,
            .code = "pat",
            .name = "PAT (page attribute table)"
        },
        {
            .id   = 17,
            .code = "pse-36",
            .name = "PSE-36 (36-bit page size extension)"
        },
        {
            .id   = 18,
            .code = "psn",
            .name = "PSN (processor serial number)"
        },
        {
            .id   = 19,
            .code = "clfsh",
            .name = "CLFSH (CLFLUSH instruction)"
        },
        {
            .id   = 21,
            .code = "ds",
            .name = "DS (debug store)"
        },
        {
            .id   = 22,
            .code = "acpi",
            .name = "ACPI (thermal monitor and software controlled clock)"
        },
        {
            .id   = 23,
            .code = "mmx",
            .name = "MMX (MMX technology)"
        },
        {
            .id   = 24,
            .code = "fxsr",
            .name = "FXSR (FXSAVE and FXRSTOR instructions)"
        },
        {
            .id   = 25,
            .code = "sse",
            .name = "SSE (streaming SIMD extensions)"
        },
        {
            .id   = 26,
            .code = "sse2",
            .name = "SSE2 (streaming SIMD extensions 2)"
        },
        {
            .id   = 27,
            .code = "ss",
            .name = "SS (self-snoop)"
        },
        {
            .id   = 28,
            .code = "htt",
            .name = "HTT (multi-threading)"
        },
        {
            .id   = 29,
            .code = "tm",
            .name = "TM (thermal monitor)"
        },
        {
            .id   = 31,
            .code = "pbe",
            .name = "PBE (pending break enable)"
        },
        DMI_NAME_NULL
    }
};

static const dmi_attribute_t dmi_processor_x86_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, type, INTEGER, {
        .code = "type",
        .name = "Type"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, family, INTEGER, {
        .code = "family",
        .name = "Family"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, model, INTEGER, {
        .code = "model",
        .name = "Model"
    }),
    DMI_ATTRIBUTE(dmi_processor_x86_id_t, stepping, INTEGER, {
        .code = "stepping",
        .name = "Stepping"
    }),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_processor_arm_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, implementer, INTEGER, {
        .code  = "implementer",
        .name  = "Implementer",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, variant, INTEGER, {
        .code  = "variant",
        .name  = "Variant",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, architecture, INTEGER, {
        .code  = "architecture",
        .name  = "Architecture",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, part_number, INTEGER, {
        .code  = "part-number",
        .name  = "Part number",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_arm_id_t, revision, INTEGER, {
        .code  = "revision",
        .name  = "Revision",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_NULL
};

static const dmi_attribute_t dmi_processor_soc_id_attrs[] =
{
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, jep106_bank, INTEGER, {
        .code  = "jep106-bank",
        .name  = "JEP106 bank",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, jep106_id, INTEGER, {
        .code  = "jep106-id",
        .name  = "JEP106 identification code",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, soc_id, INTEGER, {
        .code  = "soc-id",
        .name  = "SoC ID",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE(dmi_processor_soc_id_t, soc_revision, INTEGER, {
        .code  = "soc-revision",
        .name  = "SoC revision",
        .flags = DMI_ATTRIBUTE_FLAG_HEX
    }),
    DMI_ATTRIBUTE_NULL
};

const dmi_entity_spec_t dmi_processor_spec =
{
    .code            = "processor",
    .name            = "Processor information",
    .description     = (const char *[]){
        "The information in this structure defines the attributes of a single "
        "processor; a separate structure instance is provided for each system "
        "processor socket/slot. For example, a system with an Intel DX2 "
        "processor would have a single structure instance while a system with "
        "an Intel SX2 processor would have a structure to describe the main "
        "CPU and a second structure to describe the 80487 co-processor.",
        //
        "Note: One structure is provided for each processor instance in a "
        "system. For example, a system that supports up to two processors "
        "includes two Processor Information structures - even if only one "
        "processor is currently installed. Software that interprets the SMBIOS "
        "information can count the Processor Information structures to "
        "determine the maximum possible configuration of the system.",
        //
        nullptr
    },
    .type            = DMI_TYPE(PROCESSOR),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .required_from   = DMI_VERSION(2, 3, 0),
    .required_till   = DMI_VERSION_NONE,
    .minimum_length  = 0x1A,
    .decoded_length  = sizeof(dmi_processor_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_processor_t, socket_designation, STRING, {
            .code    = "socket-designation",
            .name    = "Socket Designation"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, type, ENUM, {
            .code    = "type",
            .name    = "Type",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_TYPE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_TYPE_UNKNOWN),
            .values  = &dmi_processor_type_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, family, ENUM, {
            .code    = "family",
            .name    = "Family",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_FAMILY_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_FAMILY_UNKNOWN),
            .values  = &dmi_processor_family_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, vendor, STRING, {
            .code    = "vendor",
            .name    = "Vendor"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, id, BINARY, {
            .code    = "id",
            .name    = "ID"
        }),
        // Fields of processor ID depend on the processor architecture
        DMI_ATTRIBUTE_VARIANT(dmi_processor_t, id_format, {
            .code     = "signature",
            .name     = "Signature",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_X86, dmi_processor_t, x86_id, STRUCT, {
                    .attrs = dmi_processor_x86_id_attrs
                }),
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_MIDR, dmi_processor_t, arm_id, STRUCT, {
                    .attrs = dmi_processor_arm_id_attrs
                }),
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_SOC_ID, dmi_processor_t, soc_id, STRUCT, {
                    .attrs = dmi_processor_soc_id_attrs
                }),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE_VARIANT(dmi_processor_t, id_format, {
            .code     = "flags",
            .name     = "Flags",
            .variants = (const dmi_attribute_variant_t[]){
                DMI_VARIANT(DMI_PROCESSOR_ID_FORMAT_X86, dmi_processor_t, x86_id.features, SET, {
                    .values = &dmi_processor_x86_feature_names
                }),
                DMI_VARIANT_NULL
            }
        }),
        DMI_ATTRIBUTE(dmi_processor_t, version, STRING, {
            .code    = "version",
            .name    = "Version"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, voltage, DECIMAL, {
            .code    = "voltage",
            .name    = "Voltage",
            .unit    = DMI_UNIT_VOLT,
            .scale   = 1,
            .unspec  = dmi_value_ptr((uint8_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, supported_voltages, SET, {
            .code    = "supported-voltages",
            .name    = "Supported voltages",
            .unspec  = dmi_value_ptr((dmi_byte_t)0),
            .values  = &dmi_processor_voltage_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, external_clock, INTEGER, {
            .code    = "external-clock",
            .name    = "External Clock",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, maximum_speed, INTEGER, {
            .code    = "maximum-speed",
            .name    = "Maximum Speed",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, current_speed, INTEGER, {
            .code    = "current-speed",
            .name    = "Current Speed",
            .unit    = DMI_UNIT_MHZ,
            .unknown = dmi_value_ptr((uint16_t)0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, is_populated, BOOL, {
            .code    = "is-populated",
            .name    = "Socket populated"
        }),
        DMI_ATTRIBUTE(dmi_processor_t, status, ENUM, {
            .code    = "status",
            .name    = "Status",
            .unknown = dmi_value_ptr(DMI_PROCESSOR_STATUS_UNKNOWN),
            .values  = &dmi_processor_status_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, upgrade, ENUM, {
            .code    = "upgrade",
            .name    = "Upgrade",
            .unspec  = dmi_value_ptr(DMI_PROCESSOR_UPGRADE_UNSPEC),
            .unknown = dmi_value_ptr(DMI_PROCESSOR_UPGRADE_UNKNOWN),
            .values  = &dmi_processor_upgrade_names
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l1_cache_handle, HANDLE, {
            .code    = "l1-cache-handle",
            .name    = "L1 Cache handle",
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l2_cache_handle, HANDLE, {
            .code    = "l2-cache-handle",
            .name    = "L2 Cache handle",
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, l3_cache_handle, HANDLE, {
            .code    = "l3-cache-handle",
            .name    = "L3 Cache handle",
            .level   = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, serial_number, STRING, {
            .code    = "serial-number",
            .name    = "Serial Number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, asset_tag, STRING, {
            .code    = "asset-tag",
            .name    = "Asset Tag",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, part_number, STRING, {
            .code    = "part-number",
            .name    = "Part Number",
            .level   = DMI_VERSION(2, 3, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, features, SET, {
            .code    = "features",
            .name    = "Features",
            .values  = &dmi_processor_features_names,
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, core_count, INTEGER, {
            .code    = "core-count",
            .name    = "Core Count",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, core_enabled, INTEGER, {
            .code    = "core-enabled",
            .name    = "Core Enabled",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, thread_count, INTEGER, {
            .code    = "thread-count",
            .name    = "Thread Count",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(2, 5, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, thread_enabled, INTEGER, {
            .code    = "thread-enabled",
            .name    = "Thread Enabled",
            .unknown = dmi_value_ptr((uint16_t)0),
            .level   = DMI_VERSION(3, 6, 0)
        }),
        DMI_ATTRIBUTE(dmi_processor_t, socket_type, STRING, {
            .code    = "socket-type",
            .name    = "Socket Type"
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers      = {
        .decode = dmi_processor_decode,
        .link   = dmi_processor_link
    }
};

const char *dmi_processor_type_name(dmi_processor_type_t value)
{
    return dmi_name_lookup(&dmi_processor_type_names, (int)value);
}

const char *dmi_processor_family_name(dmi_processor_family_t value)
{
    return dmi_name_lookup(&dmi_processor_family_names, (int)value);
}

const char *dmi_processor_upgrade_name(dmi_processor_upgrade_t value)
{
    return dmi_name_lookup(&dmi_processor_upgrade_names, (int)value);
}

const char *dmi_processor_status_name(dmi_processor_status_t value)
{
    return dmi_name_lookup(&dmi_processor_status_names, (int)value);
}

static bool dmi_processor_decode(dmi_entity_t *entity)
{
    // Processor ID is interpreted after the family and characteristics are
    // known, and these are decoded depending on the structure version
    if (not dmi_processor_decode_fields(entity))
        return false;

    dmi_processor_decode_id(entity, dmi_entity_info(entity, DMI_TYPE(PROCESSOR)));

    return true;
}

static bool dmi_processor_decode_fields(dmi_entity_t *entity)
{
    dmi_processor_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    info->l1_cache_handle = DMI_HANDLE_INVALID;
    info->l2_cache_handle = DMI_HANDLE_INVALID;
    info->l3_cache_handle = DMI_HANDLE_INVALID;

    // SMBIOS 2.0 fields
    dmi_byte_t voltage_value = 0;
    dmi_byte_t status_value  = 0;

    bool status =
        dmi_stream_decode_str(stream, &info->socket_designation) and
        dmi_stream_decode(stream, dmi_byte_t, &info->type) and
        dmi_stream_decode(stream, dmi_byte_t, &info->family) and
        dmi_stream_decode_str(stream, &info->vendor) and
        dmi_stream_decode_bin(stream, sizeof(dmi_qword_t), &info->id) and
        dmi_stream_decode_str(stream, &info->version) and
        dmi_stream_decode(stream, dmi_byte_t, &voltage_value) and
        dmi_stream_decode(stream, dmi_word_t, &info->external_clock) and
        dmi_stream_decode(stream, dmi_word_t, &info->maximum_speed) and
        dmi_stream_decode(stream, dmi_word_t, &info->current_speed) and
        dmi_stream_decode(stream, dmi_byte_t, &status_value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->upgrade);
    if (not status)
        return false;

    dmi_processor_voltage_data_t voltage_data = {
        .__value = voltage_value
    };

    // Either current voltage or supported voltages are specified
    if (voltage_data.is_current) {
        info->voltage = voltage_data.value;
    } else {
        info->supported_voltages.__value    = voltage_data.value;
        info->supported_voltages.__reserved = 0;
    }

    dmi_processor_status_data_t status_data = {
        .__value = status_value
    };

    info->is_populated = status_data.is_populated;
    info->status       = status_data.status;

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    status =
        dmi_stream_decode(stream, dmi_handle_t, &info->l1_cache_handle) and
        dmi_stream_decode(stream, dmi_handle_t, &info->l2_cache_handle) and
        dmi_stream_decode(stream, dmi_handle_t, &info->l3_cache_handle);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.3 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 3, 0);

    status =
        dmi_stream_decode_str(stream, &info->serial_number) and
        dmi_stream_decode_str(stream, &info->asset_tag) and
        dmi_stream_decode_str(stream, &info->part_number);
    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.5 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 5, 0);

    dmi_word_t features = 0;

    status =
        dmi_stream_decode(stream, dmi_byte_t, &info->core_count) and
        dmi_stream_decode(stream, dmi_byte_t, &info->core_enabled) and
        dmi_stream_decode(stream, dmi_byte_t, &info->thread_count) and
        dmi_stream_decode(stream, dmi_word_t, &features);

    info->features.__value = features;

    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 2.6 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 6, 0);

    dmi_word_t family_ex = 0;
    if (not dmi_stream_decode(stream, dmi_word_t, &family_ex))
        return dmi_entity_incomplete(entity);

    // Actual family is stored in extended field
    if (info->family == DMI_PROCESSOR_FAMILY_EXTENDED)
        info->family = family_ex;

    // SMBIOS 3.0 fields, actual counts are stored in extended fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 0, 0);

    // Missing extended counts keep the original values
    dmi_word_t core_count_ex   = info->core_count;
    dmi_word_t core_enabled_ex = info->core_enabled;
    dmi_word_t thread_count_ex = info->thread_count;

    status =
        dmi_stream_decode(stream, dmi_word_t, &core_count_ex) and
        dmi_stream_decode(stream, dmi_word_t, &core_enabled_ex) and
        dmi_stream_decode(stream, dmi_word_t, &thread_count_ex);

    if (info->core_count == 0xFFu)
        info->core_count = core_count_ex;
    if (info->core_enabled == 0xFFu)
        info->core_enabled = core_enabled_ex;
    if (info->thread_count == 0xFFu)
        info->thread_count = thread_count_ex;

    if (not status)
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.6 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 6, 0);

    if (not dmi_stream_decode(stream, dmi_word_t, &info->thread_enabled))
        return dmi_entity_incomplete(entity);

    // SMBIOS 3.8 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(3, 8, 0);

    if (not dmi_stream_decode_str(stream, &info->socket_type))
        return dmi_entity_incomplete(entity);

    return true;
}

static bool dmi_processor_link(dmi_entity_t *entity)
{
    dmi_processor_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return false;

    dmi_registry_t *registry = entity->context->state.registry;
    bool success = true;

    if (not dmi_registry_resolve(registry, info->l1_cache_handle, DMI_TYPE(CACHE), &info->l1_cache))
        success = false;
    if (not dmi_registry_resolve(registry, info->l2_cache_handle, DMI_TYPE(CACHE), &info->l2_cache))
        success = false;
    if (not dmi_registry_resolve(registry, info->l3_cache_handle, DMI_TYPE(CACHE), &info->l3_cache))
        success = false;

    return success;
}

static void dmi_processor_decode_id(dmi_entity_t *entity, dmi_processor_t *info)
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
static void dmi_processor_decode_id_x86(dmi_entity_t *entity, dmi_processor_t *info,
                                        uint32_t low, uint32_t high)
{
    // Some firmware stores feature flags before the signature, which is told
    // by reserved bits of the signature
    const uint32_t reserved = 0xF000C000u;

    if ((low & reserved) and not (high & reserved)) {
        dmi_log_notice(entity->context->logger,
                       "Handle 0x%04hx (%s): Processor ID words are swapped",
                       entity->handle, dmi_type_name(entity->context, entity->type));

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

