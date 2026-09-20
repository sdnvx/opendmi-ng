//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <config.h>

#include <string.h>

#include <opendmi/format.h>
#include <opendmi/format/text.h>
#include <opendmi/format/json.h>
#include <opendmi/format/xml.h>
#include <opendmi/format/yaml.h>

const dmi_format_t *dmi_formats[] =
{
    &dmi_text_format,
#   if defined(ENABLE_XML)
        &dmi_xml_format,
#   endif // ENABLE_XML
#   if defined(ENABLE_YAML)
        &dmi_yaml_format,
#   endif
#   if defined(ENABLE_JSON)
        &dmi_json_format,
#   endif // ENABLE_JSON
    nullptr
};

#if defined(ENABLE_YAML)
    const dmi_format_t *dmi_format_default = &dmi_yaml_format;
#elif defined(ENABLE_JSON)
    const dmi_format_t *dmi_format_default = &dmi_json_format;
#elif defined(ENABLE_XML)
    const dmi_format_t *dmi_format_default = &dmi_xml_format;
#else
    const dmi_format_t *dmi_format_default = &dmi_text_format;
#endif

const dmi_format_t *dmi_format_get(const char *code)
{
    if (code == nullptr)
        return nullptr;

    for (size_t i = 0; dmi_formats[i] != nullptr; i++) {
        if (strcmp(dmi_formats[i]->code, code) == 0)
            return dmi_formats[i];
    }

    return nullptr;
}
