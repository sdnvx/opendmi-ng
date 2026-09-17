//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>

#include <opendmi/internal.h>
#include <opendmi/utils.h>
#include <opendmi/utils/vector.h>

static bool dmi_vector_resize(dmi_vector_t *vector, size_t capacity);

const size_t dmi_vector_delta_capacity = 16;

bool dmi_vector_init(dmi_vector_t *vector, dmi_vector_match_fn *matcher)
{
    if (vector == nullptr) {
        errno = EINVAL;
        return false;
    }

    memset(vector, 0, sizeof(*vector));
    vector->matcher = matcher;

    return true;
}

bool dmi_vector_get(const dmi_vector_t *vector, size_t index, uintptr_t *value)
{
    if ((vector == nullptr) or (value == nullptr)) {
        errno = EINVAL;
        return false;
    }

    if (index >= vector->length) {
        errno = ENOENT;
        return false;
    }

    *value = vector->data[index];

    return true;
}

bool dmi_vector_find(const dmi_vector_t *vector, uintptr_t key, uintptr_t *value)
{
    if (vector == nullptr) {
        errno = EINVAL;
        return false;
    }

    if (vector->matcher == nullptr) {
        errno = ENOTSUP;
        return false;
    }

    for (size_t index = 0; index < vector->length; index++) {
        if (vector->matcher(vector->data[index], key)) {
            if (value != nullptr)
                *value = vector->data[index];
            return true;
        }
    }

    errno = ENOENT;
    return false;
}

bool dmi_vector_exists(const dmi_vector_t *vector, uintptr_t key)
{
    return dmi_vector_find(vector, key, nullptr);
}

bool dmi_vector_push(dmi_vector_t *vector, uintptr_t value)
{
    if (vector == nullptr) {
        errno = EINVAL;
        return false;
    }

    if (vector->length >= vector->capacity) {
        if (not dmi_vector_resize(vector, vector->capacity + dmi_vector_delta_capacity))
            return false;
    }

    vector->data[vector->length++] = value;

    return true;
}

bool dmi_vector_pop(dmi_vector_t *vector, uintptr_t *value)
{
    if (vector == nullptr) {
        errno = EINVAL;
        return false;
    }

    if (vector->length == 0) {
        errno = ENOENT;
        return false;
    }

    vector->length--;

    if (value != nullptr)
        *value = vector->data[vector->length];

    if (vector->length == 0)
        dmi_vector_clear(vector);

    return true;
}

bool dmi_vector_clear(dmi_vector_t *vector)
{
    if (vector == nullptr) {
        errno = EINVAL;
        return false;
    }

    free(vector->data);

    vector->data     = nullptr;
    vector->capacity = 0;
    vector->length   = 0;

    return true;
}

static bool dmi_vector_resize(dmi_vector_t *vector, size_t capacity)
{
    uintptr_t *data = realloc(vector->data, sizeof(vector->data[0]) * capacity);
    if (data == nullptr)
        return false;

    vector->data     = data;
    vector->capacity = capacity;

    return true;
}
