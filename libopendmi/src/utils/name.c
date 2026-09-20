//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/internal.h>
#include <opendmi/utils/name.h>

static bool dmi_name_find(
        const dmi_name_set_t  *dict,
        int                    id,
        const char           **pcode,
        const char           **pname,
        dmi_name_type_t       *ptype);

const char *dmi_code_lookup(const dmi_name_set_t *dict, int id)
{
    return dmi_code_lookup_ex(dict, id, nullptr);
}

const char *dmi_code_lookup_ex(const dmi_name_set_t *dict, int id, dmi_name_type_t *ptype)
{
    const char *code = nullptr;
    dmi_name_find(dict, id, &code, nullptr, ptype);

    return code;
}

int dmi_code_lookup_rev(const dmi_name_set_t *dict, const char *code)
{
    if ((dict == nullptr) or (dict->names == nullptr))
        return -1;

    for (const dmi_name_t *entry = dict->names; entry->code != nullptr; entry++) {
        if (strcmp(code, entry->code) == 0)
            return entry->id;
    }

    return -1;
}

const char *dmi_name_lookup(const dmi_name_set_t *dict, int id)
{
    return dmi_name_lookup_ex(dict, id, nullptr);
}

const char *dmi_name_lookup_ex(const dmi_name_set_t *dict, int id, dmi_name_type_t *ptype)
{
    const char *name = nullptr;
    dmi_name_find(dict, id, nullptr, &name, ptype);

    return name;
}

//
// Find entry for the identifier, exact entries take precedence over ranges.
// Returns false if there is no matching entry.
//
static bool dmi_name_find(
        const dmi_name_set_t  *dict,
        int                    id,
        const char           **pcode,
        const char           **pname,
        dmi_name_type_t       *ptype)
{
    const char *code = nullptr;
    const char *name = nullptr;
    dmi_name_type_t type = DMI_NAME_TYPE_NONE;

    do {
        if ((dict == nullptr) or (id < 0))
            break;

        // Names and ranges are optional
        if (dict->names != nullptr) {
            for (const dmi_name_t *entry = dict->names; entry->code != nullptr; entry++) {
                if (id != entry->id)
                    continue;

                type = DMI_NAME_TYPE_EXACT;
                code = entry->code, name = entry->name;

                break;
            }
        }

        // Exact entries take precedence over ranges
        if (type != DMI_NAME_TYPE_NONE)
            break;

        if (dict->ranges != nullptr) {
            for (const dmi_name_range_t *entry = dict->ranges; entry->code != nullptr; entry++) {
                if ((id < entry->start_id) or (id > entry->end_id))
                    continue;

                type = DMI_NAME_TYPE_RANGE;
                code = entry->code, name = entry->name;

                break;
            }
        }
    } while (false);

    // Printable names are translated, if the locale has a translation for
    // the entry, while codes are machine-readable and are never translated
    if ((pname != nullptr) and (code != nullptr) and (dict->code != nullptr)) {
        const char *translated = dmi_locale_string(dict->code, code);

        if (translated != nullptr)
            name = translated;
    }

    if (pcode != nullptr)
        *pcode = code;
    if (pname != nullptr)
        *pname = name;
    if (ptype != nullptr)
        *ptype = type;

    return type != DMI_NAME_TYPE_NONE;
}
