//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>
#include <ctype.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/value.h>
#include <opendmi/internal.h>
#include <opendmi/reader.h>
#include <opendmi/registry.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/codec.h>

#include <opendmi/entity/cache.h>
#include <opendmi/entity/processor-internal.h>

/**
 * @internal
 * @brief Offsets the rules read the raw data of a structure at.
 */
#define DMI_PROCESSOR_FAMILY_OFFSET 0x06
#define DMI_PROCESSOR_ID_OFFSET     0x08

void dmi_processor_lint_cores(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return;

    size_t offset = dmi_lint_entity_offset(lint, entity);

    if ((info->core_count != 0) and (info->core_enabled != 0) and
        (info->core_enabled > info->core_count)) {
        dmi_lint_issue(lint, entity, "core-enabled", offset,
                       "%u cores are enabled, while the processor has %u",
                       info->core_enabled, info->core_count);
    }

    if ((info->thread_count != 0) and (info->thread_enabled != 0) and
        (info->thread_enabled > info->thread_count)) {
        dmi_lint_issue(lint, entity, "thread-enabled", offset,
                       "%u threads are enabled, while the processor has %u",
                       info->thread_enabled, info->thread_count);
    }

    // Every core runs at least one thread
    if ((info->core_count != 0) and (info->thread_count != 0) and
        (info->thread_count < info->core_count)) {
        dmi_lint_issue(lint, entity, "thread-count", offset,
                       "processor has %u threads and %u cores",
                       info->thread_count, info->core_count);
    }
}

void dmi_processor_lint_speed(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));

    if ((info == nullptr) or (info->maximum_speed == 0) or (info->current_speed == 0))
        return;

    if (info->current_speed <= info->maximum_speed)
        return;

    dmi_lint_issue(lint, entity, "current-speed", dmi_lint_entity_offset(lint, entity),
                   "processor runs at %u MHz, while its maximum speed is %u MHz",
                   info->current_speed, info->maximum_speed);
}

void dmi_processor_lint_cache(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));
    if (info == nullptr)
        return;

    const struct
    {
        dmi_handle_t   handle;
        unsigned short level;
        const char    *code;
    } caches[] =
    {
        { info->l1_cache_handle, 1, "l1-cache-handle" },
        { info->l2_cache_handle, 2, "l2-cache-handle" },
        { info->l3_cache_handle, 3, "l3-cache-handle" }
    };

    dmi_registry_t *registry = dmi_get_registry(dmi_lint_context(lint));

    for (size_t i = 0; i < countof(caches); i++) {
        if ((caches[i].handle == DMI_HANDLE_INVALID) or
            (caches[i].handle == DMI_HANDLE_UNSUPPORTED))
            continue;

        const dmi_entity_t *cache =
                dmi_registry_lookup(registry, caches[i].handle, DMI_TYPE(CACHE), true);
        if (cache == nullptr)
            continue;

        const dmi_cache_t *data = dmi_entity_info(cache, DMI_TYPE(CACHE));

        if ((data == nullptr) or (data->level == caches[i].level))
            continue;

        dmi_lint_issue(lint, entity, caches[i].code, dmi_lint_entity_offset(lint, entity),
                       "handle 0x%04X refers to a cache of level %u",
                       (unsigned)caches[i].handle, data->level);
    }
}

void dmi_processor_lint_family(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    dmi_reader_t reader;
    dmi_byte_t value;

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return;

    if (not dmi_reader_get_bytes_at(&reader, &value, DMI_PROCESSOR_FAMILY_OFFSET, sizeof(value)))
        return;

    if (value != DMI_PROCESSOR_FAMILY_EXTENDED)
        return;

    // Extended family was added in SMBIOS 2.6, and the structures of the
    // earlier versions end before it
    if (entity->level >= dmi_version(2, 6, 0))
        return;

    dmi_lint_issue(lint, entity, "family", dmi_lint_entity_offset(lint, entity) +
                   DMI_PROCESSOR_FAMILY_OFFSET,
                   "family refers to the extended one, which the structure does not carry");
}

//
// Firmware of some vendors stores the feature flags before the signature,
// which the decoder puts back in place. The swap is told by the bits the
// signature reserves, since a valid signature has none of them set.
//
void dmi_processor_lint_id(dmi_lint_t *lint, const dmi_entity_t *entity)
{
    const dmi_processor_t *info = dmi_entity_info(entity, DMI_TYPE(PROCESSOR));

    if ((info == nullptr) or (info->id_format != DMI_PROCESSOR_ID_FORMAT_X86))
        return;

    dmi_reader_t reader;
    dmi_dword_t words[2];

    if (not dmi_reader_initialize(&reader, dmi_entity_buffer(entity),
                                  dmi_entity_offset(entity), entity->body_length))
        return;

    if (not dmi_reader_get_bytes_at(&reader, words, DMI_PROCESSOR_ID_OFFSET, sizeof(words)))
        return;

    uint32_t low  = dmi_decode(words[0]);
    uint32_t high = dmi_decode(words[1]);

    if (not (low & DMI_PROCESSOR_ID_RESERVED) or (high & DMI_PROCESSOR_ID_RESERVED))
        return;

    dmi_lint_issue(lint, entity, "id", dmi_lint_entity_offset(lint, entity) +
                   DMI_PROCESSOR_ID_OFFSET,
                   "signature 0x%08X and feature flags 0x%08X are stored the other way round",
                   high, low);
}
