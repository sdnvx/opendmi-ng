//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_LINT_H
#define OPENDMI_LINT_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/utils/name.h>
#include <opendmi/utils/version.h>

typedef struct dmi_lint         dmi_lint_t;

#ifndef DMI_LINT_RULE_T
#   define DMI_LINT_RULE_T
    typedef struct dmi_lint_rule dmi_lint_rule_t;
#endif // !DMI_LINT_RULE_T

typedef struct dmi_lint_issue   dmi_lint_issue_t;
typedef struct dmi_lint_options dmi_lint_options_t;

/**
 * @brief Severity of an issue.
 */
typedef enum dmi_lint_severity
{
    DMI_LINT_SEVERITY_NONE,    ///< Rule is disabled
    DMI_LINT_SEVERITY_NOTE,    ///< Peculiarity of the data, not a defect
    DMI_LINT_SEVERITY_WARNING, ///< Recommendation of the specification is not followed
    DMI_LINT_SEVERITY_ERROR    ///< Requirement of the specification is violated
} dmi_lint_severity_t;

/**
 * @brief Profile the data is checked against.
 */
typedef enum dmi_lint_profile
{
    /**
     * Data has been read from a platform, and is expected to have the defects
     * of its firmware, which nobody is going to fix.
     */
    DMI_LINT_PROFILE_READER,

    /**
     * Data is being produced, and is expected to conform to the
     * specification, since it is written rather than read.
     */
    DMI_LINT_PROFILE_PRODUCER
} dmi_lint_profile_t;

/**
 * @brief Part of the data a rule is checked against.
 */
typedef enum dmi_lint_scope
{
    /**
     * Entry point, which is checked once, before the structures are read.
     */
    DMI_LINT_SCOPE_ENTRY,

    /**
     * Single structure, which is checked for every structure of the table.
     */
    DMI_LINT_SCOPE_ENTITY,

    /**
     * Table as a whole, which is checked once, after every structure has been
     * read, so that the rule has the totals of the table.
     */
    DMI_LINT_SCOPE_TABLE
} dmi_lint_scope_t;

/**
 * @brief Check performed by a rule.
 *
 * The rule reports the issues it finds with `dmi_lint_issue`(3), which takes
 * the rule being checked from @p lint, so that the rule does not name itself.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure being checked, which is @c nullptr for the
 *                   rules of the entry point and of the table.
 */
typedef void dmi_lint_check_fn(dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @brief Rule the data is checked against.
 */
struct dmi_lint_rule
{
    /**
     * @brief Code name of the rule, e.g. `entry.checksum`.
     */
    const char *code;

    /**
     * @brief Printable description of what the rule checks.
     */
    const char *name;

    /**
     * @brief Severity of the issues in the reader profile.
     */
    dmi_lint_severity_t severity;

    /**
     * @brief Severity of the issues in the producer profile.
     */
    dmi_lint_severity_t producer_severity;

    /**
     * @brief Part of the data the rule is checked against, which tells when
     * the check is performed.
     */
    dmi_lint_scope_t scope;

    /**
     * @brief Check performed by the rule.
     */
    dmi_lint_check_fn *check;

    /**
     * @brief Rule is checked only if all checks are enabled, either because
     * it is expensive, or because it is a matter of taste rather than of the
     * specification.
     */
    bool optional;
};

/**
 * @brief Issue found by a rule.
 *
 * Issues are passed to the handler while the data is checked, and are valid
 * until the handler returns, so the handler has to copy whatever it keeps.
 */
struct dmi_lint_issue
{
    /**
     * @brief Rule which has found the issue.
     */
    const dmi_lint_rule_t *rule;

    /**
     * @brief Severity of the issue, which is the severity of the rule in the
     * profile the data is checked against.
     */
    dmi_lint_severity_t severity;

    /**
     * @brief Handle of the structure the issue belongs to, or
     * `DMI_HANDLE_INVALID` if it belongs to the table as a whole.
     */
    dmi_handle_t handle;

    /**
     * @brief Type of the structure the issue belongs to.
     */
    dmi_type_t type;

    /**
     * @brief Code name of the attribute the issue belongs to, or @c nullptr
     * if it belongs to the whole structure.
     */
    const char *attribute;

    /**
     * @brief Offset of the issue within the data, or `DMI_LINT_NO_OFFSET` if
     * the issue has no offset of its own.
     */
    size_t offset;

    /**
     * @brief Details of the issue, e.g. the expected and the actual value.
     */
    const char *message;
};

/**
 * @brief Offset of an issue which has no offset of its own.
 */
#define DMI_LINT_NO_OFFSET SIZE_MAX

/**
 * @brief Handler called for every issue found.
 *
 * @param[in] data  Handler data.
 * @param[in] issue Issue found, valid until the handler returns.
 */
typedef void dmi_lint_issue_fn(void *data, const dmi_lint_issue_t *issue);

/**
 * @brief Decide whether a rule is checked.
 *
 * @param[in] data Filter data.
 * @param[in] rule Rule in question.
 *
 * @return `true` if the rule is checked, `false` if it is skipped.
 */
typedef bool dmi_lint_rule_fn(void *data, const dmi_lint_rule_t *rule);

/**
 * @brief Options of a check.
 */
struct dmi_lint_options
{
    /**
     * @brief Profile the data is checked against.
     */
    dmi_lint_profile_t profile;

    /**
     * @brief Check the rules which are disabled by default.
     */
    bool all;

    /**
     * @brief Version of the specification the data is checked against, or
     * `DMI_VERSION_NONE` to take the version from the entry point.
     */
    dmi_version_t version;

    /**
     * @brief Filter deciding which rules are checked, or @c nullptr to check
     * them all.
     */
    dmi_lint_rule_fn *rule_filter;

    /**
     * @brief Data passed to the rule filter.
     */
    void *rule_filter_data;
};

/**
 * @brief Totals of the table, gathered while its structures are read.
 *
 * Rules of the `DMI_LINT_SCOPE_TABLE` scope are checked once the totals are
 * complete, so that they see the whole table.
 */
typedef struct dmi_lint_totals
{
    /**
     * @brief Number of the structures of the table.
     */
    size_t entity_count;

    /**
     * @brief Length of the longest structure of the table.
     */
    size_t entity_max_size;

    /**
     * @brief Number of the structures of every type.
     */
    size_t type_counts[DMI_TYPE_MAX + 1];

    /**
     * @brief First end-of-table structure, or @c nullptr if the table has
     * none.
     */
    const dmi_entity_t *terminator;
} dmi_lint_totals_t;

__BEGIN_DECLS

/**
 * @brief Names of issue severities.
 */
extern __dmi_api const dmi_name_set_t dmi_lint_severity_names;

/**
 * @brief Check the data of an opened context against the rules.
 *
 * Issues are reported to @p handler as they are found, in the order of the
 * structures they belong to, and the ones of the table itself come first.
 * They are counted by the caller, since it has to tell their severities
 * apart anyway.
 *
 * @param[in] context DMI context, which has to be opened.
 * @param[in] options Options of the check, or @c nullptr for the defaults.
 * @param[in] handler Handler called for every issue found, which may be
 *                    @c nullptr to check the data without reporting.
 * @param[in] data    Data passed to the handler.
 *
 * @error DMI_ERROR_NULL_ARGUMENT Context is `nullptr`
 * @error DMI_ERROR_INVALID_STATE Context is not opened
 *
 * @return `true` if the data has been checked, `false` if it cannot be.
 */
__dmi_api bool dmi_lint(
        dmi_context_t            *context,
        const dmi_lint_options_t *options,
        dmi_lint_issue_fn        *handler,
        void                     *data);

/**
 * @brief Report an issue found by the rule being checked.
 *
 * @param[in] lint      Check in progress.
 * @param[in] entity    Structure the issue belongs to, or @c nullptr if it
 *                      belongs to the table as a whole.
 * @param[in] attribute Code name of the attribute the issue belongs to, or
 *                      @c nullptr if it belongs to the whole structure.
 * @param[in] offset    Offset of the issue within the data, or
 *                      `DMI_LINT_NO_OFFSET` if it has no offset of its own.
 * @param[in] format    `printf`-style format string of the details.
 * @param[in] ...       Format arguments.
 */
__dmi_api void dmi_lint_issue(
        dmi_lint_t         *lint,
        const dmi_entity_t *entity,
        const char         *attribute,
        size_t              offset,
        const char         *format,
        ...);

/**
 * @brief Get the DMI context the data of which is being checked.
 *
 * @param[in] lint Check in progress.
 *
 * @return DMI context, or @c nullptr if @p lint is @c nullptr.
 */
__dmi_api dmi_context_t *dmi_lint_context(dmi_lint_t *lint);

/**
 * @brief Get the version of the specification the data is checked against.
 *
 * @param[in] lint Check in progress.
 *
 * @return Version given in the options of the check, or the one of the entry
 *         point if the options name none.
 */
__dmi_api dmi_version_t dmi_lint_version(const dmi_lint_t *lint);

/**
 * @brief Get the offset of a structure from the beginning of the table.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure in question.
 *
 * @return Offset of the structure, or `DMI_LINT_NO_OFFSET` if it cannot be
 *         told.
 */
__dmi_api size_t dmi_lint_entity_offset(const dmi_lint_t *lint, const dmi_entity_t *entity);

/**
 * @brief Get the offset of a string of a structure from the beginning of the
 * table.
 *
 * @param[in] lint   Check in progress.
 * @param[in] entity Structure the string belongs to.
 * @param[in] num    One-based number of the string.
 *
 * @return Offset of the string data, or `DMI_LINT_NO_OFFSET` if it cannot be
 *         told.
 */
__dmi_api size_t dmi_lint_string_offset(
        const dmi_lint_t   *lint,
        const dmi_entity_t *entity,
        size_t              num);

/**
 * @brief Get the totals of the table being checked.
 *
 * The totals are complete for the rules of the table, and are gathered for
 * the rules of the structures, which are checked while the table is read.
 *
 * @param[in] lint Check in progress.
 *
 * @return Totals of the table, or @c nullptr if @p lint is @c nullptr.
 */
__dmi_api const dmi_lint_totals_t *dmi_lint_totals(const dmi_lint_t *lint);

/**
 * @brief Get the rules the data is checked against.
 *
 * @return Array of the rules, terminated with @c nullptr.
 */
__dmi_api const dmi_lint_rule_t *const *dmi_lint_rules(void);

/**
 * @brief Get printable name of a rule.
 *
 * The name is translated, if the locale the library is set to has a
 * translation for the rule, and is left as it is declared otherwise.
 *
 * @param[in] rule Rule in question.
 *
 * @return Printable name of the rule, or @c nullptr if @p rule is @c nullptr.
 */
__dmi_api const char *dmi_lint_rule_name(const dmi_lint_rule_t *rule);

/**
 * @brief Find a rule by its code name.
 *
 * Rules of the structure types belong to their specifications, so the ones
 * of the types an opened context knows are searched as well.
 *
 * @param[in] context DMI context, or @c nullptr to search the rules which
 *                    apply to any structure only.
 * @param[in] code    Code name of the rule.
 *
 * @return Rule, or @c nullptr if there is no rule with such code name.
 */
__dmi_api const dmi_lint_rule_t *dmi_lint_rule_find(dmi_context_t *context, const char *code);

/**
 * @brief Get the severity of a rule in a profile.
 *
 * @param[in] rule    Rule in question.
 * @param[in] profile Profile the data is checked against.
 *
 * @return Severity of the issues the rule reports, or
 *         `DMI_LINT_SEVERITY_NONE` if the rule is not checked in the profile.
 */
__dmi_api dmi_lint_severity_t dmi_lint_rule_severity(
        const dmi_lint_rule_t *rule,
        dmi_lint_profile_t     profile);

__END_DECLS

#endif // !OPENDMI_LINT_H
