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

typedef struct dmi_module dmi_module_t;

/**
 * @brief DMI extension module.
 */
struct dmi_module
{
    const char *code;
    const char *name;
    const dmi_entity_spec_t **entities;

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
 * @param module Extension module to register; must not be @c nullptr.
 *
 * @return `true` on success, `false` if a module with the same code is
 *         already available.
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
 * @brief Looks up a registered extension module by its code.
 *
 * Searches built-in and registered modules for the module whose `code` field
 * equals @p code.
 *
 * @param code Null-terminated module identifier string; must not be @c nullptr.
 * @return Pointer to the matching module, or @c nullptr if no module with the
 *         given code is registered.
 */
__dmi_api const dmi_module_t *dmi_module_find(const char *code);

__END_DECLS

#endif // !OPENDMI_MODULE_H
