//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_MODULE_H
#define OPENDMI_MODULE_H

#pragma once

#include <opendmi/entity.h>
#include <opendmi/platform.h>

typedef struct dmi_module       dmi_module_t;
typedef struct dmi_relocation   dmi_relocation_t;
typedef struct dmi_module_group dmi_module_group_t;

/**
 * @brief Extension module flags.
 */
typedef enum dmi_module_flags
{
    /**
     * Specifications of the module give way to the specifications of the other
     * enabled modules of the same type numbers, instead of conflicting with
     * them. Meant for the modules of structures which platforms of any vendor
     * may carry, e.g. the ones of the Intel reference code, at type numbers
     * which vendors also give to structures of their own.
     */
    DMI_MODULE_FLAG_YIELD = (1 << 0)
} dmi_module_flags_t;

/**
 * @brief Structure a vendor places at a type number of its own.
 *
 * Structures defined by others, e.g. by the Intel reference code, are placed
 * by some vendors at type numbers other than the original ones, since the
 * original numbers are taken by their own structures. The data is laid out
 * as the original specification describes it. A specification is relocated
 * more than once by a vendor which places the structure at a type number or
 * another, e.g. on platforms of different generations.
 */
struct dmi_relocation
{
    /**
     * @brief Specification of the structure, @c nullptr to terminate a list
     * of relocations.
     */
    const dmi_entity_spec_t *spec;

    /**
     * @brief Type number the vendor places the structure at, or
     * `DMI_TYPE_ID_INVALID` if the platforms of the vendor never carry the
     * structure, and the type number is given to other structures.
     */
    dmi_type_id_t type;
};

/**
 * @brief List of relocations, terminated by an empty entry.
 */
#define DMI_RELOCATIONS(...) (const dmi_relocation_t[])__VA_ARGS__

/**
 * @brief Group association (type 14) whose members are structures of
 * a specification.
 *
 * Firmware often lists the structures of a kind by a group of a well-known
 * name, e.g. the Intel reference code lists its firmware version information
 * by the `Firmware Version Info` group. Members of such a group are checked
 * to be decoded by the specification, which tells the structures a vendor
 * places at a type number the module does not know of.
 */
struct dmi_module_group
{
    /**
     * @brief Name of the group, @c nullptr to terminate a list of groups.
     */
    const char *name;

    /**
     * @brief Specification the members of the group are decoded by.
     */
    const dmi_entity_spec_t *spec;
};

/**
 * @brief List of groups, terminated by an empty entry.
 */
#define DMI_GROUPS(...) (const dmi_module_group_t[])__VA_ARGS__

/**
 * @brief DMI extension module.
 */
struct dmi_module
{
    /**
     * @brief Module code.
     */
    const char *code;

    /**
     * @brief Module name.
     */
    const char *name;

    /**
     * @brief Specifications of the structure types the module brings,
     * terminated by @c nullptr.
     */
    const dmi_entity_spec_t **entities;

    /**
     * @brief Module flags, a combination of `dmi_module_flags_t` values.
     */
    unsigned flags;

    /**
     * @brief Platforms the module is enabled for automatically as the context
     * is opened, terminated by `DMI_PLATFORM_NULL`. @c nullptr for modules
     * which are only enabled explicitly.
     */
    const dmi_platform_match_t *platforms;

    /**
     * @brief Structures of other modules, which the platforms of the module
     * place at type numbers of their own, terminated by an entry with no
     * specification. Relocations apply while the module is enabled, to the
     * specifications of the enabled modules.
     */
    const dmi_relocation_t *relocations;

    /**
     * @brief Group associations whose members are structures of the module,
     * terminated by an entry with no name.
     */
    const dmi_module_group_t *groups;

    /**
     * @brief Next registered module. Used for modules registered with
     * `dmi_module_register()` only.
     */
    dmi_module_t *next;
};

/**
 * @brief Built-in extension modules, terminated by @c nullptr.
 *
 * Built-in modules are listed statically rather than registered at startup,
 * so that they are always linked in, including static builds.
 */
extern __dmi_api const dmi_module_t *const dmi_builtin_modules[];

__BEGIN_DECLS

/**
 * @brief Registers an external extension module.
 *
 * Appends @p module to the list of registered modules, which follow built-in
 * modules. The module must remain valid for the lifetime of the program; it
 * is not copied. Registration is not thread-safe, so modules should be
 * registered before they are used.
 *
 * @param module Extension module to register; must not be @c nullptr, and
 *               must have a code.
 *
 * @return `true` on success, `false` if @p module is @c nullptr, or if a
 *         module with the same code is already available, which is not
 *         reported.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Module is `nullptr`
 */
__dmi_api bool dmi_module_register(dmi_module_t *module);

/**
 * @brief Iterates over available extension modules.
 *
 * Built-in modules are returned first, followed by registered modules in
 * the order of registration.
 *
 * @param module Module returned by the previous call, or @c nullptr to get the
 *               first module.
 * @return Pointer to the next module, or @c nullptr if there are no more
 *         modules.
 */
__dmi_api const dmi_module_t *dmi_module_next(const dmi_module_t *module);

/**
 * @brief Looks up an available extension module by its code.
 *
 * Searches built-in and registered modules for the module whose `code` field
 * equals @p code.
 *
 * @param code Null-terminated module identifier string; must not be @c nullptr.
 * @return Pointer to the matching module, or @c nullptr if no module with the
 *         given code is available, or if @p code is @c nullptr.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Code is `nullptr`
 */
__dmi_api const dmi_module_t *dmi_module_find(const char *code);

__END_DECLS

#endif // !OPENDMI_MODULE_H
