//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LOCALE_H
#define OPENDMI_LOCALE_H

#pragma once

#include <opendmi/types.h>

typedef struct dmi_resource dmi_resource_t;

/**
 * @brief Kind of a message argument.
 */
typedef enum dmi_message_arg_type
{
    DMI_MESSAGE_ARG_TEXT,  ///< Text, taken as it is
    DMI_MESSAGE_ARG_NUMBER ///< Number, which also selects plural forms
} dmi_message_arg_type_t;

/**
 * @brief Argument substituted into a message.
 */
typedef struct dmi_message_arg
{
    dmi_message_arg_type_t type;

    union
    {
        const char *text;
        intmax_t    number;
    };
} dmi_message_arg_t;

#define DMI_MESSAGE_TEXT(value) \
    (dmi_message_arg_t){ .type = DMI_MESSAGE_ARG_TEXT, .text = (value) }

#define DMI_MESSAGE_NUMBER(value) \
    (dmi_message_arg_t){ .type = DMI_MESSAGE_ARG_NUMBER, .number = (value) }

__BEGIN_DECLS

/**
 * @brief Initialize the locale of the program from the environment.
 *
 * Takes the messages and the character classification of the system from the
 * locale of the environment, and sets the locale of the library to the same
 * one, so that printable names are translated as well.
 *
 * Numbers are deliberately left in the C locale, so that the decimal point of
 * formatted values does not depend on the environment, and machine-readable
 * output stays the same everywhere.
 *
 * Programs which handle the locale themselves do not need the helper, and
 * call `setlocale`(3) and `dmi_set_locale`(3) on their own.
 */
__dmi_api void dmi_locale_init(void);

/**
 * @brief Set locale used to translate printable strings.
 *
 * The locale is process-wide, since printable names are looked up by
 * functions which take no DMI context, e.g. `dmi_name_lookup`(3). Resources
 * opened before the call are reopened with the new locale.
 *
 * Translations are available only if the library is built with ICU4C support;
 * otherwise the function does nothing and reports failure.
 *
 * @param[in] locale Locale name, e.g. `ru_RU`, or @c nullptr to use the
 *                   locale of the environment.
 *
 * @return `true` if the locale has been set, `false` otherwise.
 */
__dmi_api bool dmi_set_locale(const char *locale);

/**
 * @brief Get locale used to translate printable strings.
 *
 * @return Name of the locale the resources are opened with, or @c nullptr if
 *         the library is built without ICU4C support.
 */
__dmi_api const char *dmi_get_locale(void);

/**
 * @brief Open a resource package.
 *
 * Packages are registered by the code which embeds them, so the name is the
 * one passed to `pkgdata --name` at build time, e.g. `libopendmi` for the
 * resources of the library itself. Strings of locales with no bundle of their
 * own are taken from the `root` bundle of the package.
 *
 * @param[in] package Package name.
 * @param[in] data    Package data, embedded into the program as the
 *                    `<package>_dat` symbol.
 *
 * @return Resource handle, or @c nullptr if the package cannot be opened or
 *         the library is built without ICU4C support.
 */
__dmi_api dmi_resource_t *dmi_resource_open(const char *package, const void *data);

/**
 * @brief Get a string from a resource package.
 *
 * @param[in] resource Resource handle.
 * @param[in] table    Path of the table holding the string, e.g. a name set
 *                     code, or `processor/attributes` for a nested table.
 * @param[in] key      Key of the string within the table.
 *
 * @return String in UTF-8, which is kept until the locale is changed or the
 *         resource is closed, or @c nullptr if there is no such string.
 */
__dmi_api const char *dmi_resource_string(dmi_resource_t *resource, const char *table, const char *key);

/**
 * @brief Format a message, substituting its arguments.
 *
 * Placeholders of the pattern are substituted with the arguments:
 *
 * - `{N}` is the argument @p N itself, a number or a text;
 * - `{N:word}` is the plural form of `word` selected by the number of the
 *   argument @p N, taken from the `plural/word` table of @p resource;
 * - `{{` is a literal opening brace.
 *
 * Plural forms follow the rules of the locale the resource is opened with,
 * e.g. `{0} {0:byte}` gives `1 byte` and `2 bytes` in English, and
 * `1 байт`, `2 байта` and `5 байт` in Russian. Patterns with plural forms
 * need @p resource and ICU4C support; other patterns are formatted in any
 * case.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param[in] resource Resource handle holding the plural forms, which may be
 *                     @c nullptr for patterns with no plural forms.
 * @param[in] pattern  Message pattern.
 * @param[in] args     Arguments of the message.
 * @param[in] count    Number of the arguments.
 *
 * @return Formatted message, or @c nullptr on failure.
 */
__dmi_api char *dmi_message_format(
        dmi_resource_t          *resource,
        const char              *pattern,
        const dmi_message_arg_t *args,
        size_t                   count);

/**
 * @brief Format a message taken from a resource package.
 *
 * Same as `dmi_message_format`(3), but the pattern is the string @p key of
 * the table @p table, so that the message is translated.
 *
 * The caller is responsible for freeing the returned string.
 *
 * @param[in] resource Resource handle.
 * @param[in] table    Path of the table holding the pattern.
 * @param[in] key      Key of the pattern within the table.
 * @param[in] args     Arguments of the message.
 * @param[in] count    Number of the arguments.
 *
 * @return Formatted message, or @c nullptr if there is no such pattern, or
 *         on failure.
 */
__dmi_api char *dmi_resource_message(
        dmi_resource_t          *resource,
        const char              *table,
        const char              *key,
        const dmi_message_arg_t *args,
        size_t                   count);

/**
 * @brief Close a resource package.
 *
 * @param[in] resource Resource handle.
 */
__dmi_api void dmi_resource_close(dmi_resource_t *resource);

__END_DECLS

#endif // !OPENDMI_LOCALE_H
