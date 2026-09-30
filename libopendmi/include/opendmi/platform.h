//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_PLATFORM_H
#define OPENDMI_PLATFORM_H

#pragma once

#include <opendmi/vendor.h>

#ifndef DMI_PLATFORM_T
#   define DMI_PLATFORM_T
    typedef struct dmi_platform dmi_platform_t;
#endif // !DMI_PLATFORM_T

typedef struct dmi_platform_match dmi_platform_match_t;
typedef struct dmi_generations    dmi_generations_t;

/**
 * @brief Platform the SMBIOS data comes from.
 *
 * The platform is told as the context is opened: the vendor of the firmware
 * from the firmware information (type 0), the vendor and the product name of
 * the system from the system information (type 1), the vendor of the
 * baseboard from the baseboard information (type 2), and the vendor of the
 * processors from the processor information (type 4). The vendor of the
 * firmware tells the family and the generation of the platform from the
 * product name, since the layout of vendor-specific structures depends on
 * them.
 *
 * The vendors are told apart, since each of them adds structures of its own:
 * e.g. the firmware of AMI is found on the boards of many vendors, and the
 * structures of the Intel reference code on Intel platforms of any vendor.
 *
 * A platform owns the strings it holds, which are set with
 * `dmi_platform_set_product()` and `dmi_platform_set_family()`, and freed
 * along with it by `dmi_platform_destroy()`. The vendors and the generation
 * are plain values, which are set directly.
 */
struct dmi_platform
{
    /**
     * @brief Context the platform belongs to, whose error queue its errors are
     * raised against. May be @c nullptr, in which case errors are not
     * reported.
     */
    dmi_context_t *context;

    /**
     * @brief Vendor of the platform firmware, `DMI_VENDOR_OTHER` if unknown.
     */
    dmi_vendor_t firmware_vendor;

    /**
     * @brief Vendor of the system, `DMI_VENDOR_OTHER` if unknown.
     */
    dmi_vendor_t system_vendor;

    /**
     * @brief Vendor of the baseboard, `DMI_VENDOR_OTHER` if unknown.
     */
    dmi_vendor_t baseboard_vendor;

    /**
     * @brief Vendor of the processors, `DMI_VENDOR_OTHER` if unknown.
     */
    dmi_vendor_t processor_vendor;

    /**
     * @brief Product name, @c nullptr if unknown. Owned by the platform.
     */
    char *product;

    /**
     * @brief Code of the product family, as the vendor defines it, e.g.
     * `"server"` for HPE servers. @c nullptr if the vendor tells no family.
     * Owned by the platform.
     */
    char *family;

    /**
     * @brief Generation of the platform, as the vendor numbers it within its
     * families, e.g. `DMI_HPE_GEN10`. Generations are ordered, so that a later
     * generation has a greater number. Zero means that the generation is
     * unknown.
     */
    unsigned generation;
};

/**
 * @brief Condition on the platform, which a module is enabled for
 * automatically.
 *
 * A platform satisfies a condition if it satisfies all of its members. Members
 * left zeroed, i.e. `DMI_VENDOR_ANY` vendors and a @c nullptr family, are
 * satisfied by any platform, including one whose vendor is unknown.
 */
struct dmi_platform_match
{
    /**
     * @brief Vendor of the platform firmware. `DMI_VENDOR_INVALID` terminates
     * a list of conditions.
     */
    dmi_vendor_t firmware_vendor;

    /**
     * @brief Vendor of the system.
     */
    dmi_vendor_t system_vendor;

    /**
     * @brief Vendor of the baseboard.
     */
    dmi_vendor_t baseboard_vendor;

    /**
     * @brief Vendor of the processors.
     */
    dmi_vendor_t processor_vendor;

    /**
     * @brief Code of the product family.
     */
    const char *family;
};

/**
 * @brief Range of platform generations a structure specification applies to.
 *
 * Specifications which apply to all generations leave the range zeroed.
 * A bounded range never applies to a platform of unknown generation.
 */
struct dmi_generations
{
    /**
     * @brief First generation of the range, zero if unbounded.
     */
    unsigned minimum;

    /**
     * @brief Last generation of the range, zero if unbounded.
     */
    unsigned maximum;
};

/**
 * @brief List of platform conditions, terminated by an empty entry.
 */
#define DMI_PLATFORMS(...) (const dmi_platform_match_t[])__VA_ARGS__

/**
 * @brief Terminator of platform condition lists.
 */
#define DMI_PLATFORM_NULL { .firmware_vendor = DMI_VENDOR_INVALID }

__BEGIN_DECLS

/**
 * @brief Create a platform.
 *
 * The platform has the vendors `DMI_VENDOR_OTHER`, and nothing else.
 *
 * @param[in] context Context the platform belongs to, or @c nullptr.
 *
 * @return Platform on success, or @c nullptr on failure.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api dmi_platform_t *dmi_platform_create(dmi_context_t *context);

/**
 * @brief Create a copy of a platform, which belongs to the same context and
 * holds copies of its strings.
 *
 * @param[in] platform Platform to copy.
 *
 * @return Copy on success, or @c nullptr if @p platform is @c nullptr or
 *         memory is exhausted.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api dmi_platform_t *dmi_platform_clone(const dmi_platform_t *platform);

/**
 * @brief Set the product name of a platform.
 *
 * @param[in,out] platform Platform to update.
 * @param[in]     product  Product name, which is copied, or @c nullptr to
 *                         clear it.
 *
 * @return `true` on success, `false` if @p platform is @c nullptr or memory
 *         is exhausted, in which case the product name is left as it was.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api bool dmi_platform_set_product(dmi_platform_t *platform, const char *product);

/**
 * @brief Set the family code of a platform.
 *
 * @param[in,out] platform Platform to update.
 * @param[in]     family   Family code, which is copied, or @c nullptr to
 *                         clear it.
 *
 * @return `true` on success, `false` if @p platform is @c nullptr or memory
 *         is exhausted, in which case the family is left as it was.
 *
 * @error DMI_ERROR_OUT_OF_MEMORY Memory is exhausted.
 */
__dmi_api bool dmi_platform_set_family(dmi_platform_t *platform, const char *family);

/**
 * @brief Destroy a platform along with the strings it holds. Does nothing if
 * @p platform is @c nullptr.
 *
 * @param[in] platform Platform to destroy.
 */
__dmi_api void dmi_platform_destroy(dmi_platform_t *platform);

/**
 * @brief Check whether a platform satisfies a condition.
 *
 * @param[in] platform Platform to check.
 * @param[in] match    Condition to check against.
 *
 * @return `true` if each vendor of the condition is either `DMI_VENDOR_ANY`
 *         or the vendor of the platform, and the condition either names no
 *         family or the family of the platform; `false` otherwise or if either
 *         argument is @c nullptr.
 */
__dmi_api bool dmi_platform_match(const dmi_platform_t *platform, const dmi_platform_match_t *match);

/**
 * @brief Check whether a range of generations contains the generation of
 * a platform.
 *
 * @param[in] platform    Platform to check.
 * @param[in] generations Range of generations.
 *
 * @return `true` if the range is unbounded, or if the generation of the
 *         platform is known and lies within the range; `false` otherwise.
 */
__dmi_api bool dmi_platform_in_generations(
        const dmi_platform_t    *platform,
        const dmi_generations_t *generations);

__END_DECLS

#endif // !OPENDMI_PLATFORM_H
