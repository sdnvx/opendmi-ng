//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <assert.h>

#include <opendmi/module.h>
#include <opendmi/module/acer.h>
#include <opendmi/module/ami.h>
#include <opendmi/module/apple.h>
#include <opendmi/module/dell.h>
#include <opendmi/module/hpe.h>
#include <opendmi/module/intel.h>
#include <opendmi/module/lenovo.h>
#include <opendmi/module/sun.h>

const dmi_module_t *const dmi_builtin_modules[] =
{
    &dmi_acer_module,
    &dmi_ami_module,
    &dmi_apple_module,
    &dmi_dell_module,
    &dmi_hpe_module,
    &dmi_intel_module,
    &dmi_lenovo_module,
    &dmi_sun_module,
    nullptr
};

// Registered external modules
static dmi_module_t *dmi_registered_modules = nullptr;

bool dmi_module_register(dmi_module_t *module)
{
    assert(module != nullptr);
    assert(module->code != nullptr);

    if (dmi_module_find(module->code) != nullptr)
        return false;

    dmi_module_t **plast = &dmi_registered_modules;
    while (*plast != nullptr)
        plast = &(*plast)->next;

    module->next = nullptr;
    *plast = module;

    return true;
}

const dmi_module_t *dmi_module_next(const dmi_module_t *module)
{
    if (module == nullptr)
        return (dmi_builtin_modules[0] != nullptr) ? dmi_builtin_modules[0] : dmi_registered_modules;

    // Built-in modules are not linked with each other
    for (size_t i = 0; dmi_builtin_modules[i] != nullptr; i++) {
        if (dmi_builtin_modules[i] != module)
            continue;

        if (dmi_builtin_modules[i + 1] != nullptr)
            return dmi_builtin_modules[i + 1];

        return dmi_registered_modules;
    }

    return module->next;
}

const dmi_module_t *dmi_module_find(const char *code)
{
    assert(code != nullptr);

    for (const dmi_module_t *module = dmi_module_next(nullptr); module != nullptr; module = dmi_module_next(module)) {
        if (strcmp(module->code, code) == 0)
            return module;
    }

    return nullptr;
}
