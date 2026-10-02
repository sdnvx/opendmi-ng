//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <memory.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/vector.h>

/**
 * @internal
 * @brief Change the capacity of a vector, keeping its items.
 *
 * @param[in,out] vector   Vector to resize.
 * @param[in]     capacity New capacity, in items.
 *
 * @return `true` on success, `false` if memory cannot be allocated.
 */
static bool dmi_vector_resize(dmi_vector_t *vector, size_t capacity);

/**
 * @internal
 * @brief Number of items the capacity of a vector grows by when it is full.
 */
const size_t dmi_vector_delta_capacity = 16;

bool dmi_vector_init(dmi_vector_t *vector, dmi_context_t *context, dmi_vector_match_fn *matcher)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(context, vector);

    memset(vector, 0, sizeof(*vector));
    vector->context = context;
    vector->matcher = matcher;

    return true;
}

bool dmi_vector_get(const dmi_vector_t *vector, size_t index, uintptr_t *value)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(nullptr, vector);
    if (value == nullptr)
        return dmi_trace_argument_null(vector->context, value);
    if (index >= vector->length)
        return dmi_trace_argument_invalid(vector->context, index);

    *value = vector->data[index];

    return true;
}

bool dmi_vector_find(const dmi_vector_t *vector, uintptr_t key, uintptr_t *value)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(nullptr, vector);
    if (vector->matcher == nullptr)
        return dmi_trace_state_invalid(vector->context, "Vector has no matcher");

    for (size_t index = 0; index < vector->length; index++) {
        if (vector->matcher(vector->data[index], key)) {
            if (value != nullptr)
                *value = vector->data[index];
            return true;
        }
    }

    return false;
}

bool dmi_vector_exists(const dmi_vector_t *vector, uintptr_t key)
{
    return dmi_vector_find(vector, key, nullptr);
}

bool dmi_vector_push(dmi_vector_t *vector, uintptr_t value)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(nullptr, vector);

    if (vector->length >= vector->capacity) {
        if (not dmi_vector_resize(vector, vector->capacity + dmi_vector_delta_capacity))
            return false;
    }

    vector->data[vector->length++] = value;

    return true;
}

bool dmi_vector_pop(dmi_vector_t *vector, uintptr_t *value)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(nullptr, vector);

    // Empty vector is not an error, so that a vector can be emptied by
    // popping its items until there is none
    if (vector->length == 0)
        return false;

    vector->length--;

    if (value != nullptr)
        *value = vector->data[vector->length];

    if (vector->length == 0)
        dmi_vector_clear(vector);

    return true;
}

bool dmi_vector_clear(dmi_vector_t *vector)
{
    if (vector == nullptr)
        return dmi_trace_argument_null(nullptr, vector);

    free(vector->data);

    vector->data     = nullptr;
    vector->capacity = 0;
    vector->length   = 0;

    return true;
}

static bool dmi_vector_resize(dmi_vector_t *vector, size_t capacity)
{
    // Storage whose size does not fit in size_t cannot be allocated either
    if (capacity > SIZE_MAX / sizeof(vector->data[0]))
        return dmi_trace_out_of_memory(vector->context);

    uintptr_t *data = realloc(vector->data, sizeof(vector->data[0]) * capacity);
    if (data == nullptr)
        return dmi_trace_out_of_memory(vector->context);

    vector->data     = data;
    vector->capacity = capacity;

    return true;
}
