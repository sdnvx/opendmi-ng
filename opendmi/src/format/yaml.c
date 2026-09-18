//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/format/yaml.h>
#include <opendmi/format/yaml/handlers.h>

const dmi_format_t dmi_yaml_format =
{
    .code     = "yaml",
    .name     = "YAML (Yet Another Markup Language)",
    .handlers = {
        .initialize         = (dmi_format_initialize_fn *)dmi_yaml_initialize,
        .dump_start         = (dmi_format_dump_start_fn *)dmi_yaml_dump_start,
        .entry              = (dmi_format_entry_fn *)dmi_yaml_entry,
        .table_start        = (dmi_format_table_start_fn *)dmi_yaml_table_start,
        .entity_start       = (dmi_format_entity_start_fn *)dmi_yaml_entity_start,
        .entity_attrs_start = (dmi_format_entity_attrs_start_fn *)dmi_yaml_entity_attrs_start,
        .entity_attr        = (dmi_format_entity_attr_fn *)dmi_yaml_entity_attr,
        .entity_attrs_end   = (dmi_format_entity_attrs_end_fn *)dmi_yaml_entity_attrs_end,
        .entity_properties  = (dmi_format_entity_properties_fn *)dmi_yaml_entity_properties,
        .entity_data        = (dmi_format_entity_data_fn *)dmi_yaml_entity_data,
        .entity_strings     = (dmi_format_entity_strings_fn *)dmi_yaml_entity_strings,
        .entity_end         = (dmi_format_entity_end_fn *)dmi_yaml_entity_end,
        .table_end          = (dmi_format_table_end_fn *)dmi_yaml_table_end,
        .dump_end           = (dmi_format_dump_end_fn *)dmi_yaml_dump_end,
        .finalize           = (dmi_format_finalize_fn *)dmi_yaml_finalize
    }
};
