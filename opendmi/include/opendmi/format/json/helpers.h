//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_JSON_HELPERS_H
#define OPENDMI_FORMAT_JSON_HELPERS_H

#pragma once

#include <opendmi/format/json/types.h>

__BEGIN_DECLS

bool dmi_json_label(dmi_json_session_t *session, const char *value);

bool dmi_json_scalar_str(dmi_json_session_t *session, const char *value);
bool dmi_json_scalar_int(dmi_json_session_t *session, intmax_t value);
bool dmi_json_scalar_number(dmi_json_session_t *session, const char *value);
bool dmi_json_scalar_bool(dmi_json_session_t *session, bool value);
bool dmi_json_scalar_null(dmi_json_session_t *session);

bool dmi_json_sequence_start(dmi_json_session_t *session);
bool dmi_json_sequence_end(dmi_json_session_t *session);

bool dmi_json_mapping_start(dmi_json_session_t *session);
bool dmi_json_mapping_end(dmi_json_session_t *session);

__END_DECLS

#define dmi_json_scalar(session, value) (_Generic((value),                           \
                                                  char *:       dmi_json_scalar_str, \
                                                  const char *: dmi_json_scalar_str, \
                                                  int:          dmi_json_scalar_int, \
                                                  size_t:       dmi_json_scalar_int, \
                                                  bool:         dmi_json_scalar_bool \
                                                 )(session, value))

#endif // !OPENDMI_FORMAT_JSON_HELPERS_H
