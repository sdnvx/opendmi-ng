//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <assert.h>

#include <opendmi/registry.h>
#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/log.h>
#include <opendmi/utils.h>
#include <opendmi/internal.h>

#include <opendmi/entity/additional-info.h>

/**
 * @internal
 */
static bool dmi_registry_put(dmi_registry_t *registry, dmi_entity_t *entity);

dmi_registry_t *dmi_registry_create(dmi_context_t *context, size_t capacity)
{
    bool success = false;
    dmi_registry_t *registry = nullptr;

    dmi_log_debug(context, "Creating registry...");

    if (capacity == 0)
        capacity = DMI_REGISTRY_CAPACITY;

    registry = dmi_alloc(context, sizeof(*registry));
    if (registry == nullptr)
        return nullptr;

    registry->context  = context;
    registry->capacity = capacity;

    do {
        registry->index = dmi_alloc_array(context, sizeof(dmi_registry_entry_t *), capacity);
        if (registry->index == nullptr)
            break;

        success = true;
    } while (false);

    if (not success) {
        dmi_registry_destroy(registry);
        return nullptr;
    }

    return registry;
}

dmi_entity_t *dmi_registry_lookup(
        dmi_registry_t *registry,
        dmi_handle_t    handle,
        dmi_type_t      type,
        bool            optional)
{
    if (registry == nullptr)
        return nullptr;

    dmi_context_t *context = registry->context;
    const dmi_registry_entry_t *entry = nullptr;
    dmi_entity_t *entity = nullptr;

    // Reserved handle values mean that the reference is not set
    if ((handle == DMI_HANDLE_INVALID) or (handle == DMI_HANDLE_UNSUPPORTED))
        return nullptr;

    entry = registry->index[(size_t)handle % registry->capacity];

    while (entry != nullptr) {
        if (entry->entity->handle == handle)
            break;
        entry = entry->next;
    }

    if (entry == nullptr) {
        if (not optional)
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND, "handle 0x%04x", handle);

        return nullptr;
    }

    entity = entry->entity;

    if ((type != DMI_TYPE_ANY) and (entity->type != type)) {
        //
        // Some SMBIOS vendors report 0x0000u instead of 0xFFFFu as
        // unspecified handle value, even if there is a structure with
        // this handle.
        //
        if ((handle == 0x0000u) and (context->flags & DMI_CONTEXT_FLAG_STRICT) == 0)
            return nullptr;

        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_TYPE,
                           "0x%04x: %d (expected %d)", handle,
                           entity->type, type);
        return nullptr;
    }

    return entity;
}

dmi_entity_t *dmi_registry_lookup_any(
        dmi_registry_t   *registry,
        dmi_handle_t      handle,
        const dmi_type_t *type,
        bool              optional)
{
    const dmi_context_t *context = nullptr;
    dmi_entity_t        *entity  = nullptr;

    if (registry == nullptr)
        return nullptr;

    context = registry->context;

    entity = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, optional);
    if (entity == nullptr)
        return nullptr;

    if (type != nullptr) {
        while (*type != DMI_TYPE_INVALID) {
            if (entity->type == *type)
                break;
            type++;
        }

        if (*type == DMI_TYPE_INVALID) {
            //
            // Some SMBIOS vendors report 0x0000u instead of 0xFFFFu as
            // unspecified handle value, even if there is a structure with
            // this handle.
            //
            if ((handle == 0x0000u) and (context->flags & DMI_CONTEXT_FLAG_STRICT) == 0)
                return nullptr;

            dmi_error_raise_ex(registry->context, DMI_ERROR_INVALID_ENTITY_TYPE,
                               "0x%04x: %d", handle, entity->type);
            return nullptr;
        }
    }

    return entity;
}

bool dmi_registry_resolve(
        dmi_registry_t  *registry,
        dmi_handle_t     handle,
        dmi_type_t       type,
        dmi_entity_t   **pentity)
{
    const dmi_type_t types[] = { type, DMI_TYPE_INVALID };

    return dmi_registry_resolve_any(registry, handle, (type != DMI_TYPE_ANY) ? types : nullptr, pentity);
}

bool dmi_registry_resolve_any(
        dmi_registry_t    *registry,
        dmi_handle_t       handle,
        const dmi_type_t  *types,
        dmi_entity_t     **pentity)
{
    assert(pentity != nullptr);

    *pentity = nullptr;

    if (registry == nullptr)
        return false;

    // Reserved handle values mean that the reference is not set
    if ((handle == DMI_HANDLE_INVALID) or (handle == DMI_HANDLE_UNSUPPORTED))
        return true;

    dmi_context_t *context = registry->context;

    dmi_entity_t *entity = dmi_registry_lookup(registry, handle, DMI_TYPE_ANY, false);
    if (entity == nullptr)
        return false;

    if (types != nullptr) {
        const dmi_type_t *type = types;

        while ((*type != DMI_TYPE_INVALID) and (entity->type != *type))
            type++;

        if (*type == DMI_TYPE_INVALID) {
            // Some SMBIOS vendors report 0x0000u instead of 0xFFFFu as
            // unspecified handle value
            if ((handle == 0x0000u) and (context->flags & DMI_CONTEXT_FLAG_STRICT) == 0)
                return true;

            if (types[1] == DMI_TYPE_INVALID) {
                dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_TYPE,
                                   "0x%04x: %s instead of %s", handle,
                                   dmi_type_name(context, entity->type),
                                   dmi_type_name(context, types[0]));
            } else {
                dmi_error_raise_ex(context, DMI_ERROR_INVALID_ENTITY_TYPE,
                                   "0x%04x: unexpected %s", handle,
                                   dmi_type_name(context, entity->type));
            }
            return false;
        }
    }

    *pentity = entity;

    return true;
}

dmi_entity_t *dmi_registry_lookup_first(
        dmi_registry_t *registry,
        dmi_type_t      type,
        bool            optional)
{
    if (registry == nullptr)
        return nullptr;

    dmi_context_t *context = registry->context;

    if (type == DMI_TYPE_INVALID) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "type: %d", type);
        return nullptr;
    }

    for (const dmi_registry_entry_t *entry = registry->head; entry != nullptr; entry = entry->seq_next) {
        if (entry->entity->type == type)
            return entry->entity;
    }

    if (not optional)
        dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND, "type %d", type);

    return nullptr;
}

unsigned dmi_registry_status(const dmi_registry_t *registry)
{
    if (registry == nullptr)
        return 0;

    return registry->status;
}

void dmi_registry_destroy(dmi_registry_t *registry)
{
    if (registry == nullptr)
        return;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context, "Destroying registry...");

    if (registry->index) {
        dmi_registry_entry_t *entry, *next;

        for (size_t i = 0; i < registry->capacity; i++) {
            entry = registry->index[i];

            while (entry != nullptr) {
                next = entry->next;

                dmi_entity_destroy(entry->entity);
                dmi_free(entry);

                entry = next;
            }
        }

        dmi_free(registry->index);
    }

    dmi_free(registry);
}

bool dmi_registry_scan(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    if (registry->status & DMI_REGISTRY_STATUS_SCANNED)
        return true;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context, "Scanning SMBIOS structures...");

    bool   success = false;
    size_t offset  = 0;
    size_t index   = 0;
    size_t count   = 0;

    // Scan table area
    while ((context->state.entity_count == 0) or (index < context->state.entity_count)) {
        dmi_entity_t *entity = nullptr;

        // Get remaining table data size. Actual table data size is always
        // known here, since table data may be shorter than the maximum size
        // specified in the entry point.
        size_t remaining = context->state.table->length - offset;

        // Check for the end of table area
        if (remaining < sizeof(dmi_header_t) + 2) {
            dmi_log_warning(context, "Truncated table area, stopping before end-of-table");
            registry->status |= DMI_REGISTRY_STATUS_TRUNCATED;
            break;
        }

        // Create entity for the structure
        entity = dmi_entity_create(context, context->state.table, offset);
        if (entity == nullptr) {
            const dmi_error_t *error = dmi_error_peek_last(context);

            if (error->reason == DMI_ERROR_ENTITY_TRUNCATED) {
                dmi_log_warning(context, "Truncated structure, stopping before end-of-table");
                registry->status |= DMI_REGISTRY_STATUS_TRUNCATED;
                break;
            }

            // Next structure cannot be located after a structure with invalid
            // length, so the rest of the table is treated as truncated in
            // relaxed mode
            if ((error->reason == DMI_ERROR_INVALID_ENTITY_LENGTH) and
                ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0))
            {
                dmi_log_warning(context, "Invalid structure length, stopping before end-of-table");
                registry->status |= DMI_REGISTRY_STATUS_TRUNCATED;
                break;
            }

            dmi_error_raise(context, DMI_ERROR_ENTITY_DECODE);
            goto exit;
        }

        // Add entity to registry
        if (not dmi_registry_put(registry, entity)) {
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_REGISTER,
                               "0x%04x (%s)", entity->handle, entity->spec->name);
            dmi_entity_destroy(entity);
            goto exit;
        }

        count++;

        // Stop at the end of table
        if (entity->type == DMI_TYPE(END_OF_TABLE))
            break;

        // Update structure offset and index
        offset += entity->total_length;
        index++;
    }

    // Set entity count and status
    registry->count   = count;
    registry->status |= DMI_REGISTRY_STATUS_SCANNED;

    dmi_log_debug(context, "Found %zu structures", registry->count);
    success = true;

exit:
    return success;
}

bool dmi_registry_overlay(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    dmi_context_t *context = registry->context;
    dmi_log_debug(context, "Applying additional information...");

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, registry, nullptr);

    bool success = true;

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (entity->type != DMI_TYPE(ADDITIONAL_INFO))
            continue;

        // Additional information does not depend on other structures
        if (not dmi_entity_decode(entity)) {
            const dmi_error_t *error = dmi_error_peek_last(context);
            if ((error != nullptr) and (error->reason == DMI_ERROR_OUT_OF_MEMORY))
                return false;

            success = false;
            continue;
        }

        const dmi_additional_info_t *info = dmi_entity_info(entity, DMI_TYPE(ADDITIONAL_INFO));

        // All entries are processed, so that every invalid one is reported
        for (size_t i = 0; i < info->entry_count; i++) {
            const dmi_additional_info_entry_t *entry = &info->entries[i];

            dmi_entity_t *target = dmi_registry_lookup(registry, entry->ref_handle, DMI_TYPE_ANY, true);
            if (target == nullptr) {
                dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND,
                                   "Additional information 0x%04x[%zu]: structure 0x%04x not found",
                                   entity->handle, i, entry->ref_handle);
                success = false;
                continue;
            }

            if (not dmi_entity_add_overlay(target, entity, i)) {
                const dmi_error_t *error = dmi_error_peek_last(context);
                if ((error != nullptr) and (error->reason == DMI_ERROR_OUT_OF_MEMORY))
                    return false;

                success = false;
            }
        }
    }

    // Invalid entries are fatal only in strict mode, otherwise they are
    // skipped, and the errors are left in the error queue
    if ((not success) and (context->flags & DMI_CONTEXT_FLAG_STRICT))
        return false;

    return true;
}

bool dmi_registry_decode(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    if (registry->status & DMI_REGISTRY_STATUS_DECODED)
        return true;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context, "Decoding SMBIOS structures...");

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, registry, nullptr);

    bool success = true;

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        dmi_log_debug(context, "0x%04zx: Handle 0x%04hx, length %zu, type %d (%s)",
                      entity->offset,
                      entity->handle,
                      entity->body_length,
                      entity->type,
                      dmi_type_name(context, entity->type));

        if (dmi_entity_decode(entity))
            continue;

        // Nothing else can be decoded without memory, regardless of the mode
        const dmi_error_t *error = dmi_error_peek_last(context);
        if ((error != nullptr) and (error->reason == DMI_ERROR_OUT_OF_MEMORY))
            return false;

        // Structure boundaries have been checked while scanning, so a failure
        // concerns this structure only. Decoding goes on, so that all
        // malformed structures in the table are reported at once.
        success = false;

        if ((context->flags & DMI_CONTEXT_FLAG_STRICT) == 0) {
            dmi_log_warning(context, "Unable to decode structure 0x%04hx (%s), skipping",
                            entity->handle, dmi_type_name(context, entity->type));
        }
    }

    // Malformed structures are fatal only in strict mode, otherwise they are
    // left undecoded, and the rest of the table remains usable
    if ((not success) and (context->flags & DMI_CONTEXT_FLAG_STRICT))
        return false;

    registry->status |= DMI_REGISTRY_STATUS_DECODED;

    return true;
}

bool dmi_registry_link(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    if (registry->status & DMI_REGISTRY_STATUS_LINKED)
        return true;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context, "Linking SMBIOS structures...");

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, registry, nullptr);

    bool success = true;

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if (not dmi_entity_is_linkable(entity))
            continue;

        // Undecoded entities have no data to link
        if ((entity->state & DMI_ENTITY_STATE_DECODED) == 0)
            continue;

        dmi_log_debug(context, "0x%04zx: Handle 0x%04hx, length %zu, type %d (%s)",
                      entity->offset,
                      entity->handle,
                      entity->body_length,
                      entity->type,
                      dmi_type_name(context, entity->type));

        // Linking goes on after a failure, so that all broken references in
        // the table are reported at once
        if (not dmi_entity_link(entity))
            success = false;
    }

    // Broken references are fatal only in strict mode, otherwise the rest of
    // the table remains usable and the errors are left in the error queue
    if ((not success) and (context->flags & DMI_CONTEXT_FLAG_STRICT))
        return false;

    registry->status |= DMI_REGISTRY_STATUS_LINKED;

    return true;
}

static bool dmi_registry_put(dmi_registry_t *registry, dmi_entity_t *entity)
{
    size_t hash;
    dmi_registry_entry_t *entry;
    dmi_registry_entry_t *last;

    assert(registry != nullptr);
    assert(entity != nullptr);

    // Allocate new entry
    entry = dmi_alloc(registry->context, sizeof(dmi_registry_entry_t));
    if (entry == nullptr)
        return false;

    // Initialize entry
    memset(entry, 0, sizeof(dmi_registry_entry_t));
    entry->entity = entity;

    hash = (size_t)entity->handle % registry->capacity;
    last = registry->index[hash];

    // Add entry to index
    if (last == nullptr) {
        registry->index[hash] = entry;
    } else {
        while (true) {
            if ((last->entity->buffer == entity->buffer) and
                (last->entity->offset == entity->offset))
            {
                dmi_free(entry);
                dmi_error_raise_ex(registry->context, DMI_ERROR_DUPLICATE_ENTRY,
                                   "0x%04zx (0x%04x)", entity->offset, entity->handle);
                return false;
            }

            if (last->entity->handle == entity->handle) {
                dmi_error_raise_ex(registry->context, DMI_ERROR_DUPLICATE_HANDLE,
                                   "0x%04x", entity->handle);
            }

            if (last->next == nullptr)
                break;

            last = last->next;
        }

        last->next = entry;
    }

    entry->seq_prev = registry->tail;

    if (registry->tail != nullptr)
        registry->tail->seq_next = entry;
    registry->tail = entry;

    if (registry->head == nullptr)
        registry->head = entry;

    return true;
}

bool dmi_registry_iter_init(
        dmi_registry_iter_t *iter,
        dmi_registry_t *registry,
        dmi_filter_t *filter)
{
    assert(iter != nullptr);
    assert(registry != nullptr);

    if ((iter == nullptr) or (registry == nullptr))
        return false;

    iter->registry = registry;
    iter->position = nullptr;
    iter->filter   = filter;
    iter->is_done  = false;

    return true;
}

bool dmi_registry_iter_has_next(dmi_registry_iter_t *iter)
{
    const dmi_registry_entry_t *next;

    if ((iter == nullptr) or iter->is_done)
        return false;

    if (iter->position == nullptr)
        next = iter->registry->head;
    else
        next = iter->position->seq_next;

    if (iter->filter != nullptr) {
        while ((next != nullptr) and
               (not dmi_filter_match(iter->filter, next->entity)))
        {
            next = next->seq_next;
        }
    }

    return next != nullptr;
}

dmi_entity_t *dmi_registry_iter_next(dmi_registry_iter_t *iter)
{
    if ((iter == nullptr) or iter->is_done)
        return nullptr;

    if (iter->position == nullptr)
        iter->position = iter->registry->head;
    else
        iter->position = iter->position->seq_next;

    if (iter->filter != nullptr) {
        while ((iter->position != nullptr) and
               (not dmi_filter_match(iter->filter, iter->position->entity)))
        {
            iter->position = iter->position->seq_next;
        }
    }

    if (iter->position == nullptr) {
        iter->is_done = true;
        return nullptr;
    }

    return iter->position->entity;
}
