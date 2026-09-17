//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/utf8.h>

#include <opendmi/format/xml/helpers.h>

static bool dmi_xml_is_char(uint32_t code);

bool dmi_xml_data(dmi_xml_session_t *session, const dmi_data_t *data, size_t length)
{
    assert(session != nullptr);
    assert(data != nullptr);
    assert(length > 0);

    bool success = false;

    do {
        if (xmlTextWriterStartCDATA(session->writer) < 0)
            break;
        if (xmlTextWriterWriteBase64(session->writer, (const char *)data, 0, length) < 0)
            break;
        if (xmlTextWriterEndCDATA(session->writer) < 0)
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_xml_text(dmi_xml_session_t *session, const char *str)
{
    assert(session != nullptr);
    assert(str != nullptr);

    char *repaired = nullptr;

    if (not dmi_utf8_is_valid_ex(str, dmi_xml_is_char)) {
        repaired = dmi_utf8_repair_ex(session->context, str, dmi_xml_is_char);
        if (repaired == nullptr)
            return false;

        str = repaired;
    }

    int rv = xmlTextWriterWriteString(session->writer, dmi_xml_string(str));
    dmi_free(repaired);

    return rv >= 0;
}

//
// Check if character is allowed in XML 1.0 documents. Surrogates and code
// points beyond U+10FFFF are already rejected as invalid UTF-8.
//
static bool dmi_xml_is_char(uint32_t code)
{
    if (code < 0x20u)
        return (code == '\t') or (code == '\n') or (code == '\r');

    return (code != 0xFFFEu) and (code != 0xFFFFu);
}
