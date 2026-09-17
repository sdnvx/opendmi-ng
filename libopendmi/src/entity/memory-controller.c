//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <assert.h>

#include <opendmi/context.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/memory-controller.h>

static bool dmi_memory_controller_decode(dmi_entity_t *entity);
static bool dmi_memory_controller_link(dmi_entity_t *entity);
static void dmi_memory_controller_cleanup(dmi_entity_t *entity);

static const dmi_name_set_t dmi_error_detect_method_names =
{
    .code  = "error-detect-methods",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_ERROR_DETECT_METHOD_UNSPEC),
        DMI_NAME_OTHER(DMI_ERROR_DETECT_METHOD_OTHER),
        DMI_NAME_UNKNOWN(DMI_ERROR_DETECT_METHOD_UNKNOWN),
        DMI_NAME_NONE(DMI_ERROR_DETECT_METHOD_NONE),
        {
            .id   = DMI_ERROR_DETECT_METHOD_PARITY,
            .code = "parity",
            .name = "8-bit Parity"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_32,
            .code = "ecc-32",
            .name = "32-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_64,
            .code = "ecc-64",
            .name = "64-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_ECC_128,
            .code = "ecc-128",
            .name = "128-bit ECC"
        },
        {
            .id   = DMI_ERROR_DETECT_METHOD_CRC,
            .code = "crc",
            .name = "CRC"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_error_correct_caps_names =
{
    .code  = "error-correct-caps",
    .names = (dmi_name_t[]){
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        DMI_NAME_NONE(2),
        {
            .id   = 3,
            .code = "single-bit",
            .name = "Single-bit error correcting"
        },
        {
            .id   = 4,
            .code = "double-bit",
            .name = "Double-bit error correcting"
        },
        {
            .id   = 5,
            .code = "scrubbing",
            .name = "Error scrubbing"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_module_speed_names =
{
    .code  = "memory-module-speeds",
    .names = (dmi_name_t[]){
        DMI_NAME_OTHER(0),
        DMI_NAME_UNKNOWN(1),
        {
            .id   = 2,
            .code = "70ns",
            .name = "70 ns"
        },
        {
            .id   = 3,
            .code = "60ns",
            .name = "60 ns"
        },
        {
            .id   = 4,
            .code = "50ns",
            .name = "50 ns"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_interleave_names =
{
    .code  = "memory-interleaves",
    .names = (dmi_name_t[]){
        DMI_NAME_UNSPEC(DMI_MEMORY_INTERLEAVE_UNSPEC),
        DMI_NAME_OTHER(DMI_MEMORY_INTERLEAVE_OTHER),
        DMI_NAME_UNKNOWN(DMI_MEMORY_INTERLEAVE_UNKNOWN),
        {
            .id   = DMI_MEMORY_INTERLEAVE_1WAY,
            .code = "1-way",
            .name = "One-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_2WAY,
            .code = "2-way",
            .name = "Two-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_4WAY,
            .code = "4-way",
            .name = "Four-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_8WAY,
            .code = "8-way",
            .name = "Eight-Way Interleave"
        },
        {
            .id   = DMI_MEMORY_INTERLEAVE_16WAY,
            .code = "16-way",
            .name = "Sixteen-Way Interleave"
        },
        DMI_NAME_NULL
    }
};

static const dmi_name_set_t dmi_memory_module_voltage_names =
{
    .code  = "memory-module-voltages",
    .names = (dmi_name_t[]){
        {
            .id   = 0,
            .code = "5v",
            .name = "5V"
        },
        {
            .id   = 1,
            .code = "3v3",
            .name = "3.3V"
        },
        {
            .id   = 2,
            .code = "2v9",
            .name = "2.9V"
        },
        DMI_NAME_NULL
    }
};

const dmi_entity_spec_t dmi_memory_controller_spec =
{
    .code            = "memory-controller",
    .name            = "Memory controller information",
    .type            = DMI_TYPE(MEMORY_CONTROLLER),
    .minimum_version = DMI_VERSION(2, 0, 0),
    .minimum_length  = 0x0F,
    .decoded_length  = sizeof(dmi_memory_controller_t),
    .attributes      = (const dmi_attribute_t[]){
        DMI_ATTRIBUTE(dmi_memory_controller_t, error_detection, ENUM, {
            .code   = "error-detection",
            .name   = "Error detecting method",
            .values = &dmi_error_detect_method_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, error_correction, SET, {
            .code   = "error-correction",
            .name   = "Error correcting capabilities",
            .values = &dmi_error_correct_caps_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_interleave, ENUM, {
            .code   = "supported-interleave",
            .name   = "Supported interleave",
            .values = &dmi_memory_interleave_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, current_interleave, ENUM, {
            .code   = "current-interleave",
            .name   = "Current interleave",
            .values = &dmi_memory_interleave_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, maximum_module_size, SIZE, {
            .code   = "maximum-module-size",
            .name   = "Maximum module size"
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, maximum_memory_size, SIZE, {
            .code   = "maximum-memory-size",
            .name   = "Maximum memory size"
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_speeds, SET, {
            .code   = "supported-speeds",
            .name   = "Supported speeds",
            .values = &dmi_memory_module_speed_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, supported_types, SET, {
            .code   = "supported-types",
            .name   = "Supported types",
            .values = &dmi_memory_module_type_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, required_voltages, SET, {
            .code   = "required-voltages",
            .name   = "Required voltages",
            .values = &dmi_memory_module_voltage_names
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, slot_count, INTEGER, {
            .code   = "slot-count",
            .name   = "Slot count"
        }),
        DMI_ATTRIBUTE_ARRAY(dmi_memory_controller_t, module_handles, slot_count, HANDLE, {
            .code   = "module-handles",
            .name   = "Module handles"
        }),
        DMI_ATTRIBUTE(dmi_memory_controller_t, enabled_error_correction, SET, {
            .code   = "enabled-error-correction",
            .name   = "Enabled error correcting capabilities",
            .values = &dmi_error_correct_caps_names,
            .level  = DMI_VERSION(2, 1, 0)
        }),
        DMI_ATTRIBUTE_NULL
    },
    .handlers = {
        .decode  = dmi_memory_controller_decode,
        .link    = dmi_memory_controller_link,
        .cleanup = dmi_memory_controller_cleanup
    }
};

const char *dmi_error_detect_method_name(dmi_error_detect_method_t value)
{
    return dmi_name_lookup(&dmi_error_detect_method_names, (int)value);
}

const char *dmi_memory_interleave_name(dmi_memory_interleave_t value)
{
    return dmi_name_lookup(&dmi_memory_interleave_names, (int)value);
}

static bool dmi_memory_controller_decode(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    dmi_stream_t *stream = &entity->stream;

    dmi_byte_t maximum_module_size;

    bool status =
        dmi_stream_decode(stream, dmi_byte_t, &info->error_detection) and
        dmi_stream_decode(stream, dmi_byte_t, &info->error_correction.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->supported_interleave) and
        dmi_stream_decode(stream, dmi_byte_t, &info->current_interleave) and
        dmi_stream_decode(stream, dmi_byte_t, &maximum_module_size) and
        dmi_stream_decode(stream, dmi_word_t, &info->supported_speeds.__value) and
        dmi_stream_decode(stream, dmi_word_t, &info->supported_types.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->required_voltages.__value) and
        dmi_stream_decode(stream, dmi_byte_t, &info->slot_count);
    if (not status)
        return false;

    info->maximum_module_size = 1uLL << (maximum_module_size + 20);
    info->maximum_memory_size = info->maximum_module_size * info->slot_count;

    size_t slot_count = info->slot_count;

    info->module_handles = dmi_alloc_array(entity->context, sizeof(dmi_handle_t), slot_count);
    if (info->module_handles == nullptr)
        return false;

    // Only completely present module handles are counted
    info->slot_count = 0;

    for (size_t i = 0; i < slot_count; i++) {
        if (not dmi_stream_decode(stream, dmi_word_t, &info->module_handles[i]))
            return dmi_entity_incomplete(entity);

        info->slot_count++;
    }

    // SMBIOS 2.1 fields
    if (dmi_stream_is_done(stream))
        return dmi_entity_stop(entity);

    entity->level = dmi_version(2, 1, 0);

    dmi_byte_t enabled_error_correction = 0;
    if (not dmi_stream_decode(stream, dmi_byte_t, &enabled_error_correction))
        return dmi_entity_incomplete(entity);

    info->enabled_error_correction.__value = enabled_error_correction;

    return true;
}

static bool dmi_memory_controller_link(dmi_entity_t *entity)
{
    dmi_registry_t *registry;
    dmi_memory_controller_t *info;

    assert(entity != nullptr);

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return false;

    info->modules = dmi_alloc_array(entity->context, sizeof(dmi_entity_t *), info->slot_count);
    if (info->modules == nullptr)
        return false;

    registry = entity->context->state.registry;

    for (size_t i = 0; i < info->slot_count; i++) {
        info->modules[i] = dmi_registry_get(registry, info->module_handles[i], DMI_TYPE(MEMORY_MODULE), false);

        if (info->modules[i] == nullptr)
            continue;

        dmi_memory_module_t *module = dmi_entity_info(info->modules[i], DMI_TYPE(MEMORY_MODULE));

        // Bind memory controller to module
        module->controller = entity;

        // Check module installed size value
        if (module->installed_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT) {
            if (module->installed_size.value > info->maximum_module_size)
                module->installed_size.status = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID;
        }

        // Check module enabled size value
        if (module->enabled_size.status == DMI_MEMORY_MODULE_SIZE_STATUS_PRESENT) {
            if (module->enabled_size.value > info->maximum_module_size)
                module->enabled_size.status = DMI_MEMORY_MODULE_SIZE_STATUS_INVALID;
        }
    }

    return true;
}

static void dmi_memory_controller_cleanup(dmi_entity_t *entity)
{
    dmi_memory_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MEMORY_CONTROLLER));
    if (info == nullptr)
        return;

    dmi_free(info->module_handles);
    dmi_free(info->modules);
}
