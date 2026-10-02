//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright (c) 2017-2020 Ingy döt Net
// Copyright (c) 2006-2016 Kirill Simonov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/error.h>
#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>
#include <opendmi/utils/utf8.h>

#include <opendmi/format/yaml/helpers.h>

/**
 * @internal
 * @brief Check if string may be written as a plain scalar.
 *
 * @details A plain scalar must be one which YAML 1.1 readers resolve as a
 * string: empty strings, booleans, nulls, merge and value keys, and anything
 * looking like a number or a timestamp are resolved as values of other types.
 *
 * @param[in] value String to check.
 *
 * @return `true` if the string may be written as a plain scalar, `false` if
 *         it has to be quoted.
 */
static bool dmi_yaml_is_plain_string(const char *value);

bool dmi_yaml_emit(dmi_yaml_session_t *session, yaml_event_t *event)
{
    assert(session != nullptr);
    assert(event != nullptr);

    bool result = yaml_emitter_emit(session->emitter, event);
    if (not result)
        dmi_yaml_raise(session);

    return result;
}

bool dmi_yaml_flush(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    bool result = yaml_emitter_flush(session->emitter);
    if (not result)
        dmi_yaml_raise(session);

    return result;
}

void dmi_yaml_raise(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    const yaml_emitter_t *emitter = session->emitter;
    const char *problem = (emitter->problem != nullptr) ? emitter->problem : "unknown error";

    // Writer reports failures of the stream, which leave the reason in errno
    if (emitter->error == YAML_WRITER_ERROR)
        dmi_error_raise_ex(session->context, DMI_ERROR_FILE_WRITE_FAILED, "%s", strerror(errno));
    else if (emitter->error == YAML_MEMORY_ERROR)
        dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
    else
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL, "Unable to emit YAML: %s", problem);
}

bool dmi_yaml_label(dmi_yaml_session_t *session, const char *value)
{
    return dmi_yaml_scalar(session, value, YAML_STR_TAG, YAML_PLAIN_SCALAR_STYLE);
}

bool dmi_yaml_scalar(
        dmi_yaml_session_t  *session,
        const char          *value,
        const char          *tag,
        yaml_scalar_style_t  style)
{
    assert(session != nullptr);

    bool success = false;
    yaml_event_t event = {};
    char *binary = nullptr;
    size_t length;

    do {
        // Encode incorrect UTF-8 strings as binary data
        if ((style == YAML_DOUBLE_QUOTED_SCALAR_STYLE) and
            (not dmi_utf8_is_valid(value)))
        {
            tag   = YAML_BINARY_TAG;
            style = YAML_LITERAL_SCALAR_STYLE;

            binary = dmi_base64_encode(dmi_data(value), strlen(value), &length);
            if (binary == nullptr)
                return dmi_trace_out_of_memory(session->context);

            value = binary;
        } else {
            length = strlen(value);
        }

        // Plain strings, which readers would resolve as values of other types
        // (e.g. "no" as boolean in YAML 1.1), are quoted
        if ((style == YAML_PLAIN_SCALAR_STYLE) and (tag != nullptr) and
            (strcmp(tag, YAML_STR_TAG) == 0) and not dmi_yaml_is_plain_string(value))
            style = YAML_DOUBLE_QUOTED_SCALAR_STYLE;

        // Write explicit tags for literal scalars only. Quoted scalars are
        // always strings, and a non-specific tag would make readers resolve
        // them as other types (e.g. "yes" as boolean).
        bool implicit = (style != YAML_LITERAL_SCALAR_STYLE);

        bool result = yaml_scalar_event_initialize(&event, nullptr,
                                                   (const yaml_char_t *)tag,
                                                   (const yaml_char_t *)value,
                                                   length,
                                                   implicit, implicit, style);

        if (not result) {
            dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                               "Unable to initialize YAML scalar event");
            break;
        }
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    dmi_free(binary);

    return success;
}

bool dmi_yaml_sequence_start(dmi_yaml_session_t *session, yaml_sequence_style_t style)
{
    assert(session != nullptr);

    bool success = false;
    yaml_event_t event = {};

    do {
        bool result = yaml_sequence_start_event_initialize(&event, nullptr,
                                                           (const yaml_char_t *)YAML_SEQ_TAG,
                                                           true, style);

        if (not result) {
            dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                               "Unable to initialize YAML sequence start event");
            break;
        }
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_yaml_sequence_end(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    bool success = false;
    yaml_event_t event = {};

    do {
        if (not yaml_sequence_end_event_initialize(&event)) {
            dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                               "Unable to initialize YAML sequence end event");
            break;
        }
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_yaml_mapping_start(dmi_yaml_session_t *session, yaml_mapping_style_t style)
{
    assert(session != nullptr);

    bool success = false;
    yaml_event_t event = {};

    do {
        if (not yaml_mapping_start_event_initialize(&event, nullptr, (const yaml_char_t *)YAML_MAP_TAG, true, style)) {
            dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                               "Unable to initialize YAML mapping start event");
            break;
        }
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    return success;
}

bool dmi_yaml_mapping_end(dmi_yaml_session_t *session)
{
    assert(session != nullptr);

    bool success = false;
    yaml_event_t event = {};

    do {
        if (not yaml_mapping_end_event_initialize(&event)) {
            dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                               "Unable to initialize YAML mapping end event");
            break;
        }
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    return success;
}

static bool dmi_yaml_is_plain_string(const char *value)
{
    static const char *const special[] = {
        "y", "Y", "yes", "Yes", "YES", "n", "N", "no", "No", "NO",
        "true", "True", "TRUE", "false", "False", "FALSE",
        "on", "On", "ON", "off", "Off", "OFF",
        "null", "Null", "NULL", "~", "<<", "="
    };

    if (*value == '\0')
        return false;

    for (size_t i = 0; i < countof(special); i++) {
        if (strcmp(value, special[i]) == 0)
            return false;
    }

    // Numbers (including infinities and NaNs) and timestamps start with a
    // digit, a sign or a dot, and consist of a few characters only
    if (strchr("0123456789+-.", *value) == nullptr)
        return true;

    return strspn(value, "0123456789abcdefABCDEF_.:+-xobeEtTzZ iInNaA") != strlen(value);
}
