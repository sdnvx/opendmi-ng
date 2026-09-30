//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/utf8.h>

#include <opendmi/format/json/helpers.h>

bool dmi_json_label(dmi_json_session_t *session, const char *value)
{
    return dmi_json_scalar_str(session, value);
}

bool dmi_json_scalar_str(dmi_json_session_t *session, const char *value)
{
    int rv;
    char *repaired = nullptr;

    assert(session != nullptr);
    assert(value != nullptr);

    // JSON has no binary strings, so invalid bytes are replaced
    if (not dmi_utf8_is_valid(value)) {
        repaired = dmi_utf8_repair(session->context, value);
        if (repaired == nullptr)
            return false;

        value = repaired;
    }

    rv = yajl_gen_string(session->generator, (const unsigned char *)value, strlen(value));
    dmi_free(repaired);

    if (rv != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_scalar_int(dmi_json_session_t *session, intmax_t value)
{
    assert(session != nullptr);

    if (yajl_gen_integer(session->generator, value) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_scalar_number(dmi_json_session_t *session, const char *value)
{
    assert(session != nullptr);
    assert(value != nullptr);

    char decimal[24];
    const char *text = value;

    // JSON has no hexadecimal numbers, so they are written as decimal ones,
    // which are written as they are, since they may not fit into the integers
    // of the generator (e.g. the unsigned ones of 64 bits)
    if ((value[0] == '0') and ((value[1] == 'x') or (value[1] == 'X'))) {
        char *end;

        errno = 0;
        uintmax_t number = strtoumax(value + 2, &end, 16);
        if ((value[2] == '\0') or (*end != '\0') or (errno == ERANGE))
            return false;

        snprintf(decimal, sizeof(decimal), "%" PRIuMAX, number);
        text = decimal;
    } else {
        // Integers and fixed-point numbers, which may be negative
        const char *ptr = (*text == '-') ? text + 1 : text;
        const char *digits = ptr;

        while (isdigit((unsigned char)*ptr))
            ptr++;
        if (ptr == digits)
            return false;

        if (*ptr == '.') {
            digits = ++ptr;
            while (isdigit((unsigned char)*ptr))
                ptr++;
            if (ptr == digits)
                return false;
        }

        if (*ptr != '\0')
            return false;
    }

    if (yajl_gen_number(session->generator, text, strlen(text)) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_scalar_bool(dmi_json_session_t *session, bool value)
{
    assert(session != nullptr);

    if (yajl_gen_bool(session->generator, value) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_scalar_null(dmi_json_session_t *session)
{
    assert(session != nullptr);

    if (yajl_gen_null(session->generator) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_sequence_start(dmi_json_session_t *session)
{
    assert(session != nullptr);

    if (yajl_gen_array_open(session->generator) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_sequence_end(dmi_json_session_t *session)
{
    assert(session != nullptr);

    if (yajl_gen_array_close(session->generator) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_mapping_start(dmi_json_session_t *session)
{
    assert(session != nullptr);

    if (yajl_gen_map_open(session->generator) != yajl_gen_status_ok)
        return false;

    return true;
}

bool dmi_json_mapping_end(dmi_json_session_t *session)
{
    assert(session != nullptr);

    if (yajl_gen_map_close(session->generator) != yajl_gen_status_ok)
        return false;

    return true;
}
