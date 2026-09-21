//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>
#include <opendmi/context.h>
#include <opendmi/internal.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/processor-ex-internal.h>

//
// Processor-specific block declares a length of its own, and is missing from
// the structures which carry nothing of the kind.
//
bool dmi_processor_ex_decode_data(
        dmi_entity_t      *entity,
        const dmi_field_t *field,
        void              *value)
{
    dmi_unused(field);

    dmi_stream_t       *stream = dmi_entity_stream(entity);
    dmi_processor_ex_t *info   = value;

    if (dmi_stream_is_done(stream))
        return true;

    dmi_byte_t length = 0;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &length) and
        dmi_stream_decode(stream, dmi_byte_t, &info->arch);
    if (not status)
        return dmi_entity_incomplete(entity);

    // Data which does not fit into the structure is truncated
    if (length > dmi_stream_remaining(stream)) {
        dmi_stream_decode_bin(stream, dmi_stream_remaining(stream), &info->data);
        return dmi_entity_incomplete(entity);
    }

    return dmi_stream_decode_bin(stream, length, &info->data);
}
