//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_XML_HELPERS_H
#define OPENDMI_FORMAT_XML_HELPERS_H

#pragma once

#include <opendmi/format/xml/types.h>

__BEGIN_DECLS

bool dmi_xml_data(dmi_xml_session_t *session, const dmi_data_t *data, size_t length);

/**
 * @brief Write text content.
 *
 * Invalid UTF-8 bytes and characters not allowed in XML 1.0 (control
 * characters except tab, line feed and carriage return, U+FFFE and U+FFFF)
 * are replaced with U+FFFD replacement character.
 *
 * @param[in] session XML session.
 * @param[in] str     NUL-terminated string.
 *
 * @return `true` on success, `false` otherwise.
 */
bool dmi_xml_text(dmi_xml_session_t *session, const char *str);

__END_DECLS

static inline const xmlChar *dmi_xml_string(const char *str)
{
    return (const xmlChar *)str;
}

#endif // !OPENDMI_FORMAT_XML_HELPERS_H
