//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
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
