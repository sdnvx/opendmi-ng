//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_TOOL_UTILS_LOCALE_H
#define OPENDMI_TOOL_UTILS_LOCALE_H

#pragma once

#include <opendmi/locale.h>

/**
 * @brief Get the resources of the tool, opening them on the first call.
 *
 * @return Resource handle, or @c nullptr if the resources cannot be opened,
 *         or the tool is built without ICU4C support.
 */
dmi_resource_t *dmi_tool_resource(void);

/**
 * @brief Translate a text of the tool.
 *
 * Texts are translated by their English text, which is the key of the
 * translation in the `text` table of the resources, so that the strings of
 * the tool stay readable in the code.
 *
 * @param[in] text English text.
 *
 * @return Translated text, or @p text if the resources have no translation
 *         for it.
 */
const char *dmi_tool_string(const char *text);

/**
 * @brief Get a string of the tool resources.
 *
 * @param[in] table    Path of the table holding the string.
 * @param[in] key      Key of the string within the table.
 * @param[in] fallback String used if there is no translation.
 *
 * @return Translated string, or @p fallback if the resources have none, so
 *         that the tool prints strings in English when it is built without
 *         ICU4C support.
 */
const char *dmi_tool_text(const char *table, const char *key, const char *fallback);

/**
 * @brief Format a message of the tool resources.
 *
 * The message is translated if the resources have a pattern for @p key,
 * and is formatted from @p fallback otherwise, so that the tool prints
 * messages in English when it is built without ICU4C support.
 *
 * Patterns of the resources may use plural forms, e.g. `{0} {0:byte}`, while
 * @p fallback may not, since the forms come from the resources.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param[in] table    Path of the table holding the pattern.
 * @param[in] key      Key of the pattern within the table.
 * @param[in] fallback Pattern used if there is no translation.
 * @param[in] args     Arguments of the message.
 * @param[in] count    Number of the arguments.
 *
 * @return Formatted message, or @c nullptr on failure.
 */
char *dmi_tool_message(
        const char              *table,
        const char              *key,
        const char              *fallback,
        const dmi_message_arg_t *args,
        size_t                   count);

#endif // !OPENDMI_TOOL_UTILS_LOCALE_H
