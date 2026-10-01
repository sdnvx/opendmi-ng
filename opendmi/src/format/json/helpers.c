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
#include <errno.h>
#include <assert.h>

#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/utf8.h>
#include <opendmi/format/iter.h>

#include <opendmi/format/json/helpers.h>

/**
 * @internal
 * @brief Raise an error if the generator has failed. Document is generated in
 * memory and is written as a whole, so these are errors of the generator usage.
 */
static bool dmi_json_check(dmi_json_session_t *session, yajl_gen_status status);

static const char *const dmi_json_messages[] = {
    [yajl_gen_keys_must_be_strings]     = "keys must be strings",
    [yajl_max_depth_exceeded]           = "maximum depth exceeded",
    [yajl_gen_in_error_state]           = "generator is in error state",
    [yajl_gen_generation_complete]      = "document is already complete",
    [yajl_gen_invalid_number]           = "invalid number",
    [yajl_gen_no_buf]                   = "no buffer",
    [yajl_gen_invalid_string]           = "invalid string"
};

bool dmi_json_label(dmi_json_session_t *session, const char *value)
{
    return dmi_json_scalar_str(session, value);
}

bool dmi_json_scalar_str(dmi_json_session_t *session, const char *value)
{
    yajl_gen_status status;
    char *repaired = nullptr;

    assert(session != nullptr);
    assert(value != nullptr);

    // JSON has no binary strings, so invalid bytes are replaced
    if (not dmi_utf8_is_valid(value)) {
        // Allocation failure is raised by the allocator
        repaired = dmi_utf8_repair(session->context, value);
        if (repaired == nullptr)
            return false;

        value = repaired;
    }

    status = yajl_gen_string(session->generator, (const unsigned char *)value, strlen(value));
    dmi_free(repaired);

    return dmi_json_check(session, status);
}

bool dmi_json_scalar_int(dmi_json_session_t *session, intmax_t value)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_integer(session->generator, value));
}

bool dmi_json_scalar_number(dmi_json_session_t *session, const char *value)
{
    assert(session != nullptr);
    assert(value != nullptr);
    assert(dmi_format_is_number(value));

    char decimal[24];
    const char *text = value;

    // JSON has no hexadecimal numbers, so they are written as decimal ones,
    // while decimal ones are written as they are, since they may not fit into
    // the integers of the generator (e.g. the unsigned ones of 64 bits)
    if ((value[0] == '0') and (value[1] == 'x')) {
        errno = 0;
        uintmax_t number = strtoumax(value + 2, nullptr, 16);

        // Numbers beyond the integers of the platform are kept as strings
        if (errno == ERANGE)
            return dmi_json_scalar_str(session, value);

        snprintf(decimal, sizeof(decimal), "%" PRIuMAX, number);
        text = decimal;
    }

    return dmi_json_check(session, yajl_gen_number(session->generator, text, strlen(text)));
}

bool dmi_json_scalar_bool(dmi_json_session_t *session, bool value)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_bool(session->generator, value));
}

bool dmi_json_scalar_null(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_null(session->generator));
}

bool dmi_json_sequence_start(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_array_open(session->generator));
}

bool dmi_json_sequence_end(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_array_close(session->generator));
}

bool dmi_json_mapping_start(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_map_open(session->generator));
}

bool dmi_json_mapping_end(dmi_json_session_t *session)
{
    assert(session != nullptr);

    return dmi_json_check(session, yajl_gen_map_close(session->generator));
}

static bool dmi_json_check(dmi_json_session_t *session, yajl_gen_status status)
{
    if (status == yajl_gen_status_ok)
        return true;

    const char *message = nullptr;

    if ((size_t)status < countof(dmi_json_messages))
        message = dmi_json_messages[status];
    else
        message = "unknown error";

    dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to generate JSON: %s", message);

    return false;
}
