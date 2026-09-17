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
#include <assert.h>

#include <opendmi/error.h>
#include <opendmi/utils.h>
#include <opendmi/utils/base64.h>
#include <opendmi/utils/utf8.h>

#include <opendmi/format/yaml/helpers.h>

bool dmi_yaml_emit(dmi_yaml_session_t *session, yaml_event_t *event)
{
    assert(session != nullptr);
    assert(event != nullptr);

    bool result = yaml_emitter_emit(session->emitter, event);
    if (not result) {
        dmi_error_raise_ex(session->context, DMI_ERROR_INTERNAL,
                           "Unable to emit YAML event: %s", session->emitter->problem);
    }

    return result;
}

bool dmi_yaml_label(dmi_yaml_session_t *session, const char *value)
{
    return dmi_yaml_scalar(session, value, nullptr, YAML_PLAIN_SCALAR_STYLE);
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
            if (binary == nullptr) {
                dmi_error_raise(session->context, DMI_ERROR_OUT_OF_MEMORY);
                return false;
            }

            value = binary;
        } else {
            length = strlen(value);
        }

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
                               "Unable to initialize scalar event: %s",
                               session->emitter->problem);
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
                               "Unable to initialize sequence start event: %s",
                               session->emitter->problem);
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
        if (not yaml_sequence_end_event_initialize(&event))
            break;
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
        if (not yaml_mapping_start_event_initialize(&event, nullptr, (const yaml_char_t *)YAML_MAP_TAG, true, style))
            break;
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
        if (not yaml_mapping_end_event_initialize(&event))
            break;
        if (not dmi_yaml_emit(session, &event))
            break;

        success = true;
    } while (false);

    return success;
}
