//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_COMMAND_H
#define OPENDMI_COMMAND_H

#pragma once

#include <opendmi/types.h>
#include <opendmi/option.h>
#include <opendmi/log.h>

/**
 * @brief Command line usage error.
 * 
 * @note Status `2` is what command line utilities actually return when
 * called improperly. The `EX_USAGE`status from `<sysexits.h>` doesn't
 * reflect reality and common practice.
 */
#define EXIT_USAGE 2

typedef struct dmi_global_config dmi_global_config_t;

typedef struct dmi_command_config dmi_command_config_t;
typedef struct dmi_command dmi_command_t;
typedef struct dmi_command_ops dmi_command_ops_t;

/**
 * @brief Command usage handler.
 *
 * @param[in] context Context handle.
 */
typedef void dmi_command_usage_fn(void);

/**
 * @brief Command handler function.
 *
 * @param[in] context Context handle.
 * @param[in] argc    Number of arguments.
 * @param[in] argv    Arguments array.
 */
typedef int dmi_command_main_fn(dmi_context_t *context, int argc, char *argv[]);

/**
 * @brief Command cleanup handler
 *
 * @param[in] context Context handle.
 */
typedef void dmi_command_cleanup_fn(dmi_context_t *context);

struct dmi_global_config
{
    bool  show_version;
    bool  show_usage;
    bool  quiet;
    bool  log_enable;
    const char *log_path;
    dmi_log_level_t log_level;
    const char *device_path;
    const char *input_path;
};

struct dmi_command_config
{
    bool show_usage;
};

typedef enum dmi_command_flags
{
    DMI_COMMAND_FLAG_DETACHED = 1 << 0,
    DMI_COMMAND_FLAG_PAGER    = 1 << 1
} dmi_command_flags_t;

/**
 * @brief Command handlers.
 */
struct dmi_command_ops
{
    dmi_command_usage_fn *usage;
    dmi_command_main_fn *main;
    dmi_command_cleanup_fn *cleanup;
};

/**
 * @brief Command specification.
 */
struct dmi_command
{
    /**
     * @brief Command name.
     */
    const char *name;

    /**
     * @brief Short description.
     */
    const char *description;

    /**
     * @brief Command options specification.
     */
    const dmi_option_set_t **options;

    /**
     * @brief Command arguments specification.
     */
    const dmi_argument_t *arguments;

    /**
     * @brief Flags.
     */
    unsigned flags;

    /**
     * @brief Command handler.
     */
    dmi_command_ops_t handlers;
};

extern dmi_global_config_t dmi_global_config;
extern dmi_command_config_t dmi_command_config;

extern const dmi_option_set_t dmi_global_options;

/**
 * @brief Available commands, terminated by `nullptr`.
 */
extern const dmi_command_t *dmi_commands[];

__BEGIN_DECLS

void dmi_command_init(const char *process);

void dmi_command_list(void);

const dmi_command_t *dmi_command_find(const char *name);

void dmi_command_banner(void);
void dmi_command_usage(const dmi_command_t *command);

void dmi_command_message(const char *format, ...);
void dmi_command_message_ex(const dmi_command_t *command, const char *format, ...);

void dmi_command_trace(dmi_context_t *context);

int dmi_command_run(
        const dmi_command_t *command,
        dmi_context_t       *context,
        int                  argc,
        char                *argv[]);

__END_DECLS

#endif // !OPENDMI_COMMAND_H
