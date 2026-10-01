//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_TEXT_TYPES_H
#define OPENDMI_FORMAT_TEXT_TYPES_H

#pragma once

#include <stdio.h>

#include <opendmi/types.h>
#include <opendmi/attribute.h>
#include <opendmi/format.h>

typedef struct dmi_text_session
{
    dmi_context_t *context;
    FILE *stream;
    dmi_format_options_t options;
    bool is_tty;

    // Code of the structure whose attributes are being written, which their
    // names are translated for, and the level of indentation they are
    // written at
    const char *owner;
    unsigned int depth;
} dmi_text_session_t;

#endif // !OPENDMI_FORMAT_TEXT_TYPES_H
