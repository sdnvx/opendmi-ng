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
     *
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
 * @param vector  The vector to initialize. Must not be @c nullptr.
 * @param matcher Optional callback for `dmi_vector_find()` and `dmi_vector_exists()`.
 *                May be @c nullptr if those functions will not be used.
 *
 * @return true on success, false if @p vector is @c nullptr.
 */
__dmi_api bool dmi_vector_init(dmi_vector_t *vector, dmi_vector_match_fn *matcher);

/**
 * @brief Retrieve an element by index.
 *
 * @param vector The vector to query. Must not be @c nullptr.
 * @param index  Zero-based index of the element to retrieve.
 * @param value  Output parameter that receives the element value. Must not be @c nullptr.
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
 * @param vector The vector to search. Must not be @c nullptr and must have a
 *               matcher set.
 * @param key    The search key passed to the matcher callback.
 * @param value  Output parameter that receives the matching element value.
 *               Must not be @c nullptr.
 *
 * @return true if a matching element was found, false otherwise or on error.
 */
__dmi_api bool dmi_vector_find(const dmi_vector_t *vector, uintptr_t key, uintptr_t *value);

/**
 * @brief Check whether any element matches a key.
 *
 * Equivalent to `dmi_vector_find()` but discards the matched value.
 *
 * @param vector The vector to search. Must not be @c nullptr and must have a
 *               matcher set.
 * @param key    The search key passed to the matcher callback.
 *
 * @return true if a matching element exists, false otherwise or on error.
 */
__dmi_api bool dmi_vector_exists(const dmi_vector_t *vector, uintptr_t key);

/**
 * @brief Append an element to the end of the vector.
 *
 * The internal buffer is grown automatically when capacity is exhausted.
 *
 * @param vector The vector to append to. Must not be @c nullptr.
 * @param value  The value to append.
 *
 * @return true on success, false if @p vector is @c nullptr or memory allocation fails.
 */
__dmi_api bool dmi_vector_push(dmi_vector_t *vector, uintptr_t value);

/**
 * @brief Remove and return the last element.
 *
 * @param vector The vector to pop from. Must not be @c nullptr and must not be empty.
 * @param value  Output parameter that receives the removed element value.
 *               Must not be @c nullptr.
 *
 * @return true on success, false if @p vector or @p value is @c nullptr or the vector
 * is empty.
 */
__dmi_api bool dmi_vector_pop(dmi_vector_t *vector, uintptr_t *value);

/**
 * @brief Remove all elements and release allocated memory.
 *
 * After this call the vector is in the same state as after `dmi_vector_init().`
 *
 * @param vector The vector to clear. Must not be @c nullptr.
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
