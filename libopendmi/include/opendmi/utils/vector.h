//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_UTILS_VECTOR_H
#define OPENDMI_UTILS_VECTOR_H

#pragma once

#include <opendmi/types.h>

typedef struct dmi_vector dmi_vector_t;

/**
 * @brief Callback type used to match a vector entry against a search key.
 *
 * @param entry The value stored in the vector.
 * @param key   The search key to compare against.
 *
 * @return true if @p entry matches @p key, false otherwise.
 */
typedef bool dmi_vector_match_fn(uintptr_t entry, uintptr_t key);

/**
 * @brief Dynamic array of opaque pointer-sized values.
 */
struct dmi_vector
{
    /**
     * @brief Context the errors of the vector are raised against, or
     * @c nullptr if there is none.
     */
    dmi_context_t *context;

    /**
     * @brief Pointer to the allocated element buffer.
     */
    uintptr_t *data;

    /**
     * @brief Number of elements the buffer can hold without reallocation.
     */
    size_t capacity;

    /**
     * @brief Number of elements currently stored.
     */
    size_t length;

    /**
     * @brief Callback used by `dmi_vector_find()` and `dmi_vector_exists()`.
     */
    dmi_vector_match_fn *matcher;
};

__BEGIN_DECLS

/**
 * @brief Initialize a vector.
 *
 * Sets up an empty vector with no allocated storage. The vector must be
 * destroyed with `dmi_vector_clear()` when no longer needed.
 *
 * @param vector  The vector to initialize.
 * @param context Context to raise the errors of the vector against, or
 *                @c nullptr if there is none, in which case the errors are
 *                not recorded.
 * @param matcher Optional callback for `dmi_vector_find()` and `dmi_vector_exists()`.
 *                May be @c nullptr if those functions will not be used.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 *
 * @return true on success, false if @p vector is @c nullptr.
 */
__dmi_api bool dmi_vector_init(dmi_vector_t *vector, dmi_context_t *context, dmi_vector_match_fn *matcher);

/**
 * @brief Retrieve an element by index.
 *
 * @param vector The vector to query.
 * @param index  Zero-based index of the element to retrieve.
 * @param value  Output parameter that receives the element value.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector or value pointer is `nullptr`
 * @error DMI_ERROR_ARGUMENT_INVALID Index is out of range
 *
 * @return true on success, false if @p vector or @p value is @c nullptr or @p index is
 *         out of range.
 */
__dmi_api bool dmi_vector_get(const dmi_vector_t *vector, size_t index, uintptr_t *value);

/**
 * @brief Find the first element matching a key.
 *
 * Iterates over all elements and calls the matcher callback set during
 * `dmi_vector_init()` to locate the first match.
 *
 * @param vector The vector to search, which must have a matcher set.
 * @param key    The search key passed to the matcher callback.
 * @param value  Output parameter that receives the matching element value,
 *               or @c nullptr if the value is not needed.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 * @error DMI_ERROR_STATE_INVALID Vector has no matcher
 *
 * @return true if a matching element was found, false if none matches, which
 *         is not an error, or on error.
 */
__dmi_api bool dmi_vector_find(const dmi_vector_t *vector, uintptr_t key, uintptr_t *value);

/**
 * @brief Check whether any element matches a key.
 *
 * Equivalent to `dmi_vector_find()` but discards the matched value.
 *
 * @param vector The vector to search, which must have a matcher set.
 * @param key    The search key passed to the matcher callback.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 * @error DMI_ERROR_STATE_INVALID Vector has no matcher
 *
 * @return true if a matching element exists, false otherwise or on error.
 */
__dmi_api bool dmi_vector_exists(const dmi_vector_t *vector, uintptr_t key);

/**
 * @brief Append an element to the end of the vector.
 *
 * The internal buffer is grown automatically when capacity is exhausted.
 *
 * @param vector The vector to append to.
 * @param value  The value to append.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 * @error DMI_ERROR_OUT_OF_MEMORY Buffer cannot grow
 *
 * @return true on success, false if @p vector is @c nullptr or memory allocation fails.
 */
__dmi_api bool dmi_vector_push(dmi_vector_t *vector, uintptr_t value);

/**
 * @brief Remove and return the last element.
 *
 * The storage is released once the last element is removed.
 *
 * @param vector The vector to pop from.
 * @param value  Output parameter that receives the removed element value,
 *               or @c nullptr if the value is not needed.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 *
 * @return true on success, false if @p vector is @c nullptr or the vector is
 *         empty, which is not an error, so that a vector can be emptied by
 *         popping its elements until there is none.
 */
__dmi_api bool dmi_vector_pop(dmi_vector_t *vector, uintptr_t *value);

/**
 * @brief Remove all elements and release allocated memory.
 *
 * After this call the vector is in the same state as after `dmi_vector_init()`,
 * with the context and the matcher kept.
 *
 * @param vector The vector to clear.
 *
 * @error DMI_ERROR_ARGUMENT_NULL Vector is `nullptr`
 *
 * @return true on success, false if @p vector is @c nullptr.
 */
__dmi_api bool dmi_vector_clear(dmi_vector_t *vector);

__END_DECLS

/**
 * @brief Return the number of elements currently stored in the vector.
 *
 * @param vector The vector to query. May be @c nullptr.
 * @return Number of elements, or 0 if @p vector is @c nullptr.
 */
static inline size_t dmi_vector_length(const dmi_vector_t *vector)
{
    if (vector == nullptr)
        return 0;

    return vector->length;
}

/**
 * @brief Check whether the vector contains no elements.
 *
 * @param vector The vector to query. May be @c nullptr.
 *
 * @return true if @p vector is @c nullptr or contains no elements, false otherwise.
 */
static inline bool dmi_vector_is_empty(const dmi_vector_t *vector)
{
    if (vector == nullptr)
        return true;

    return vector->length == 0;
}

#endif // !OPENDMI_UTILS_VECTOR_H
