//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/locale.h>

dmi_resource_t *dmi_tool_resource(void)
{
#ifdef ENABLE_ICU
    extern const char opendmi_dat[];

    static dmi_resource_t *resource;
    static bool failed;

    if ((resource == nullptr) and not failed) {
        resource = dmi_resource_open("opendmi", opendmi_dat);
        failed   = (resource == nullptr);
    }

    return resource;
#else
    return nullptr;
#endif
}

const char *dmi_tool_string(const char *text)
{
    if (text == nullptr)
        return nullptr;

    return dmi_tool_text("text", text, text);
}

const char *dmi_tool_text(const char *table, const char *key, const char *fallback)
{
    const char *text = dmi_resource_string(dmi_tool_resource(), table, key);

    return (text != nullptr) ? text : fallback;
}

char *dmi_tool_message(
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count)
{
    dmi_resource_t *resource = dmi_tool_resource();

    char *text = dmi_resource_message(resource, table, key, args, count);
    if (text != nullptr)
        return text;

    return dmi_message_format(resource, fallback, args, count);
}
