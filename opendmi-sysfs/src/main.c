//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//

// Kernel headers are included before any source, see prelude.h, so the
// default prefix of messages is replaced rather than defined beforehand
#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/printk.h>

#include <opendmi/attribute.h>
#include <opendmi/context.h>
#include <opendmi/entity.h>
#include <opendmi/error.h>
#include <opendmi/registry.h>
#include <opendmi/utils.h>

MODULE_DESCRIPTION("Decoded DMI/SMBIOS data");
MODULE_AUTHOR("The OpenDMI contributors");

// GPL-only symbols of the kernel are used, and the BSD license of the
// library is compatible with GPL
MODULE_LICENSE("Dual BSD/GPL");

static bool verbose;
module_param(verbose, bool, 0444);
MODULE_PARM_DESC(verbose, "Log library debug messages and decoded attributes of all structures");

/**
 * @brief Context the structures are decoded by, held while the module is
 * loaded.
 */
static dmi_context_t *dmi_sysfs_context;

static void dmi_sysfs_log_handler(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args);

static dmi_log_t dmi_sysfs_logger =
{
    .handler = dmi_sysfs_log_handler
};

/**
 * @brief Log the errors queued in the context and remove them.
 *
 * @param is_fatal Whether the errors prevented the context from being opened,
 *                 they are warnings otherwise.
 */
static void dmi_sysfs_report_errors(dmi_context_t *context, bool is_fatal);

/**
 * @brief Log the header of a structure and, in verbose mode, its decoded
 * attributes.
 */
static void dmi_sysfs_print_entity(const dmi_entity_t *entity);

static void dmi_sysfs_print_attr(const dmi_entity_t *entity, const dmi_attribute_t *attr);

static int __init dmi_sysfs_init(void)
{
    dmi_context_t *context;

    context = dmi_create(DMI_CONTEXT_FLAG_LINK | DMI_CONTEXT_FLAG_AUTO_MODULES);
    if (context == NULL)
        return -ENOMEM;

    dmi_set_logger(context, &dmi_sysfs_logger);
    dmi_set_log_level(context, verbose ? DMI_LOG_DEBUG : DMI_LOG_INFO);

    if (!dmi_open(context, NULL)) {
        dmi_sysfs_report_errors(context, true);
        dmi_destroy(context);
        return -ENODEV;
    }

    // Structures which are not decoded completely do not prevent the context
    // from being opened in relaxed mode
    dmi_sysfs_report_errors(context, false);

    dmi_registry_iter_t iter;
    dmi_registry_iter_init(&iter, dmi_get_registry(context), NULL);

    size_t count = 0;
    const dmi_entity_t *entity;
    while ((entity = dmi_registry_iter_next(&iter)) != NULL) {
        dmi_sysfs_print_entity(entity);
        count++;
    }

    pr_info("%zu structures decoded\n", count);

    dmi_sysfs_context = context;

    return 0;
}

static void __exit dmi_sysfs_exit(void)
{
    dmi_destroy(dmi_sysfs_context);
}

module_init(dmi_sysfs_init);
module_exit(dmi_sysfs_exit);

static void dmi_sysfs_log_handler(
        dmi_log_t       *target,
        dmi_log_level_t  level,
        const char      *format,
        va_list          args)
{
    struct va_format vaf;
    va_list copy;

    (void)target;

    // Arguments are passed to printk() by pointer, and va_list is an array
    // on some architectures, so the pointer is taken to a copy
    va_copy(copy, args);
    vaf.fmt = format;
    vaf.va  = &copy;

    switch (level) {
    case DMI_LOG_ERROR:
        pr_err("%pV\n", &vaf);
        break;
    case DMI_LOG_WARNING:
        pr_warn("%pV\n", &vaf);
        break;
    case DMI_LOG_NOTICE:
        pr_notice("%pV\n", &vaf);
        break;
    default:
        // Debug messages are only requested in verbose mode, so they are
        // logged as the informational ones rather than hidden by pr_debug()
        pr_info("%pV\n", &vaf);
        break;
    }

    va_end(copy);
}

static void dmi_sysfs_report_errors(dmi_context_t *context, bool is_fatal)
{
    const dmi_error_t *error;

    while ((error = dmi_error_get_first(context)) != NULL) {
        const char *reason = dmi_error_message(error->reason);

        if (is_fatal) {
            if (error->message != NULL)
                pr_err("%s: %s\n", reason, error->message);
            else
                pr_err("%s\n", reason);
        } else {
            if (error->message != NULL)
                pr_warn("%s: %s\n", reason, error->message);
            else
                pr_warn("%s\n", reason);
        }
    }
}

static void dmi_sysfs_print_entity(const dmi_entity_t *entity)
{
    if (!verbose)
        return;

    pr_info("Handle 0x%04X, DMI type %d, %zu bytes: %s\n",
            dmi_entity_handle(entity), (int)dmi_entity_type_id(entity),
            entity->total_length, dmi_entity_name(entity));

    // Structures which have not been decoded have no attributes to show
    if ((entity->info == NULL) || (entity->spec == NULL) || (entity->spec->attributes == NULL))
        return;

    const dmi_attribute_t *attr;
    for (attr = entity->spec->attributes; attr->params.name != NULL; attr++) {
        // Attributes added by later versions are not decoded for earlier ones
        if ((attr->params.level != DMI_VERSION_NONE) && (entity->level < attr->params.level))
            continue;

        dmi_sysfs_print_attr(entity, attr);
    }
}

static void dmi_sysfs_print_attr(const dmi_entity_t *entity, const dmi_attribute_t *attr)
{
    // Value of variant attribute is described by the variant
    const dmi_attribute_t *variant = dmi_attribute_resolve(attr, entity->info);
    if (variant == NULL)
        return;

    const char *name  = dmi_attribute_name(attr, entity->spec->code);
    const void *value = dmi_member_ptr(entity->info, variant->value, dmi_data_t);

    // Nested values are left for the SysFS tree
    if (dmi_attribute_is_array(variant)) {
        pr_info("    %s: <array>\n", name);
        return;
    }
    if (variant->type == DMI_ATTRIBUTE_TYPE_STRUCT) {
        pr_info("    %s: <struct>\n", name);
        return;
    }

    if (dmi_attribute_is_unspecified(variant, value)) {
        pr_info("    %s: <unspecified>\n", name);
        return;
    }
    if (dmi_attribute_is_unknown(variant, value)) {
        pr_info("    %s: <unknown>\n", name);
        return;
    }

    char *text = dmi_attribute_format(entity->context, variant, value, true);
    if (text == NULL) {
        pr_info("    %s: <error>\n", name);
        return;
    }

    if (variant->params.unit != DMI_UNIT_NONE)
        pr_info("    %s: %s %s\n", name, text, dmi_unit_name(variant->params.unit));
    else
        pr_info("    %s: %s\n", name, text);

    dmi_free(text);
}
