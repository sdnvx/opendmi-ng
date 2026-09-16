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

/**
 * @internal
 */
static bool dmi_registry_put(dmi_registry_t *registry, dmi_entity_t *entity);

dmi_registry_t *dmi_registry_create(dmi_context_t *context, size_t capacity)
{
    bool success = false;
    dmi_registry_t *registry = nullptr;

    dmi_log_debug(context->logger, "Creating registry...");

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

dmi_entity_t *dmi_registry_get(
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

    if (handle == DMI_HANDLE_UNSUPPORTED) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "handle: 0x%04x", handle);
        return nullptr;
    }
    if ((handle == DMI_HANDLE_INVALID) and (type == DMI_TYPE_INVALID)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "type: %d", type);
        return nullptr;
    }

    if (handle != DMI_HANDLE_INVALID) {
        entry = registry->index[(size_t)handle % registry->capacity];

        while (entry != nullptr) {
            if (entry->entity->handle == handle)
                break;
            entry = entry->next;
        }
    } else {
        entry = registry->head;

        while (entry != nullptr) {
            if (entry->entity->type == type)
                break;
            entry = entry->seq_next;
        }
    }

    if (entry == nullptr) {
        if (not optional) {
            if (handle != DMI_HANDLE_INVALID)
                dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND, "handle 0x%04x", handle);
            else
                dmi_error_raise_ex(context, DMI_ERROR_ENTITY_NOT_FOUND, "type %d", type);
        }

        return nullptr;
    }

    entity = entry->entity;

    if ((type != DMI_TYPE_INVALID) and (entity->type != type)) {
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

dmi_entity_t *dmi_registry_get_any(
        dmi_registry_t   *registry,
        dmi_handle_t      handle,
        const dmi_type_t *type,
        bool              optional)
{
    dmi_context_t *context = nullptr;
    dmi_entity_t  *entity  = nullptr;

    if (registry == nullptr)
        return nullptr;

    context = registry->context;

    if ((handle == DMI_HANDLE_INVALID) or (handle == DMI_HANDLE_UNSUPPORTED)) {
        dmi_error_raise_ex(context, DMI_ERROR_INVALID_ARGUMENT, "handle: 0x%04x", handle);
        return nullptr;
    }

    entity = dmi_registry_get(registry, handle, DMI_TYPE_INVALID, optional);
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
    dmi_log_debug(context->logger, "Destroying registry...");

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
    dmi_log_debug(context->logger, "Scanning SMBIOS structures...");

    bool success = false;
    const dmi_data_t *ptr = context->state.table_data;
    size_t index = 0;

    // Scan table area
    while ((context->state.entity_count == 0) or (index < context->state.entity_count)) {
        dmi_entity_t *entity = nullptr;

        // Get remaining table area size
        size_t remaining = 0;
        size_t offset = (size_t)(ptr - context->state.table_data);
        if (context->state.table_area_size > 0)
            remaining = context->state.table_area_size - offset;
        else if (context->state.table_area_max_size > 0)
            remaining = context->state.table_area_max_size - offset;
        else
            assert(false);

        // Check for the end of table area
        if (remaining < sizeof(dmi_header_t) + 2) {
            dmi_log_warning(context->logger, "Truncated table area, stopping before end-of-table");
            registry->status |= DMI_REGISTRY_STATUS_TRUNCATED;
            break;
        }

        // Create entity for the structure
        entity = dmi_entity_create(context, ptr, remaining);
        if (entity == nullptr) {
            const dmi_error_t *error = dmi_error_peek_last(context);

            if (error->reason == DMI_ERROR_ENTITY_TRUNCATED) {
                dmi_log_warning(context->logger, "Truncated structure, stopping before end-of-table");
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

        // Stop at the end of table
        if (entity->type == DMI_TYPE(END_OF_TABLE))
            break;

        // Update structure pointer and index
        ptr += entity->total_length;
        index++;
    }

    // Set entity count and status
    registry->count   = index + 1;
    registry->status |= DMI_REGISTRY_STATUS_SCANNED;

    dmi_log_debug(context->logger, "Found %zu structures", registry->count);
    success = true;

exit:
    return success;
}

bool dmi_registry_decode(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    if (registry->status & DMI_REGISTRY_STATUS_DECODED)
        return true;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context->logger, "Decoding SMBIOS structures...");

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, registry, nullptr);

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        dmi_log_debug(context->logger, "%p: Handle 0x%04hx, length %zu, type %d (%s)",
                      entity->data,
                      entity->handle,
                      entity->body_length,
                      entity->type,
                      dmi_type_name(context, entity->type));

        if (not dmi_entity_decode(entity)) {
            dmi_error_raise_ex(context, DMI_ERROR_ENTITY_DECODE, "0x%04hx", entity->handle);
            return false;
        }
    }

    registry->status |= DMI_REGISTRY_STATUS_DECODED;

    return true;
}

bool dmi_registry_link(dmi_registry_t *registry)
{
    assert(registry != nullptr);

    if (registry->status & DMI_REGISTRY_STATUS_LINKED)
        return true;

    dmi_context_t *context = registry->context;
    dmi_log_debug(context->logger, "Linking SMBIOS structures...");

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, registry, nullptr);

    dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != nullptr) {
        if ((entity->spec == nullptr) or (entity->spec->handlers.link == nullptr))
            continue;

        dmi_log_debug(context->logger, "%p: Handle 0x%04hx, length %zu, type %d (%s)",
                      entity->data,
                      entity->handle,
                      entity->body_length,
                      entity->type,
                      dmi_type_name(context, entity->type));

        if (not dmi_entity_link(entity))
            return false;
    }

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
            if (last->entity->data == entity->data) {
                dmi_free(entry);
                dmi_error_raise_ex(registry->context, DMI_ERROR_DUPLICATE_ENTRY,
                                   "%p (0x%04x)", entity->data, entity->handle);
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
