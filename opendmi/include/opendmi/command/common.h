//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMMAND_COMMON_H
#define OPENDMI_COMMAND_COMMON_H

#pragma once

#include <opendmi/option.h>
#include <opendmi/filter.h>
#include <opendmi/format.h>

typedef struct dmi_filter_config dmi_filter_config_t;

struct dmi_filter_config
{
    dmi_filter_t filter;
};

/**
 * @brief Entity filtering options.
 */
extern const dmi_option_set_t dmi_filter_options;
extern dmi_filter_config_t dmi_filter_config;

__BEGIN_DECLS

dmi_handle_t dmi_parse_handle(const char *str);
dmi_type_t dmi_parse_type(dmi_context_t *context, const char *str);

/**
 * @brief Print all entities matching the entity filter.
 *
 * @param[in] context DMI context handle.
 * @param[in] stream  Output stream.
 * @param[in] format  Output format.
 * @param[in] dump    Print raw entity data instead of decoded attributes.
 *
 * @return `true` on success, `false` if output fails. Error details are
 *         raised in the context.
 */
bool dmi_print_all(
        dmi_context_t      *context,
        FILE               *stream,
        const dmi_format_t *format,
        bool                dump);

/**
 * @brief Print single entity.
 *
 * @return `true` on success, `false` if any format handler fails.
 */
bool dmi_print_entity(
        const dmi_format_t *format,
        const dmi_entity_t *entity,
        void               *session,
        bool                dump);

__END_DECLS

#endif // !OPENDMI_COMMAND_COMMON_H
