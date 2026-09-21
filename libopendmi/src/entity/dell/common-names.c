//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <opendmi/utils/name.h>
#include <opendmi/entity/dell/common.h>

const dmi_name_set_t dmi_dell_enable_state_names =
{
    .code  = "dell-enable-state",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_DELL_ENABLE_STATE_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_ENABLE_STATE_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_ENABLE_STATE_UNKNOWN),
        {
            .id   = DMI_DELL_ENABLE_STATE_ENABLED,
            .code = "enabled",
            .name = "Enabled"
        },
        {
            .id   = DMI_DELL_ENABLE_STATE_DISABLED,
            .code = "disabled",
            .name = "Disabled"
        },
        {}
    })
};

const dmi_name_set_t dmi_dell_port_security_names =
{
    .code  = "dell-port-security-setting",
    .names = DMI_NAMES({
        DMI_NAME_UNSPEC(DMI_DELL_PORT_SECURITY_UNSPEC),
        DMI_NAME_OTHER(DMI_DELL_PORT_SECURITY_OTHER),
        DMI_NAME_UNKNOWN(DMI_DELL_PORT_SECURITY_UNKNOWN),
        DMI_NAME_NONE(DMI_DELL_PORT_SECURITY_NONE),
        {
            .id   = DMI_DELL_PORT_SECURITY_EXT_IFACE_LOCKED,
            .code = "external-iface-locked",
            .name = "External interface locked out"
        },
        {
            .id   = DMI_DELL_PORT_SECURITY_EXT_IFACE_ENABLED,
            .code = "external-iface-enabled",
            .name = "External interface enabled"
        },
        {
            .id   = DMI_DELL_PORT_SECURITY_BOOT_BYPASS,
            .code = "boot-bypass",
            .name = "Boot-bypass"
        },
        {}
    })
};

const dmi_name_set_t dmi_dell_check_type_names =
{
    .code  = "dell-check-type",
    .names = DMI_NAMES({
        {
            .id   = DMI_DELL_CHECK_TYPE_WORD_CHECKSUM,
            .code = "word-checksum",
            .name = "Word checksum"
        },
        {
            .id   = DMI_DELL_CHECK_TYPE_BYTE_CHECKSUM,
            .code = "byte-checksum",
            .name = "Byte checksum"
        },
        {
            .id   = DMI_DELL_CHECK_TYPE_WORD_CRC,
            .code = "word-crc",
            .name = "Word CRC"
        },
        {
            .id   = DMI_DELL_CHECK_TYPE_WORD_CHECKSUM_N,
            .code = "word-checksum-negated",
            .name = "Negated word checksum"
        },
        {}
    })
};

const dmi_name_set_t dmi_dell_value_format_names =
{
    .code  = "dell-value-format",
    .names = DMI_NAMES({
        {
            .id   = DMI_DELL_VALUE_FORMAT_SCAN_CODE,
            .code = "scan-code",
            .name = "Alphanumeric, scan codes"
        },
        {
            .id   = DMI_DELL_VALUE_FORMAT_ASCII,
            .code = "ascii",
            .name = "Alphanumeric, ASCII"
        },
        {
            .id   = DMI_DELL_VALUE_FORMAT_SCAN_CODE_NS,
            .code = "scan-code-ns",
            .name = "Alphanumeric, scan codes (NS)"
        },
        {
            .id   = DMI_DELL_VALUE_FORMAT_ASCII_NS,
            .code = "ascii-ns",
            .name = "Alphanumeric, ASCII (NS)"
        },
        {}
    })
};

const char *dmi_dell_enable_state_name(dmi_dell_enable_state_t value)
{
    return dmi_name_lookup(&dmi_dell_enable_state_names, (int)value);
}

const char *dmi_dell_port_security_name(dmi_dell_port_security_t value)
{
    return dmi_name_lookup(&dmi_dell_port_security_names, (int)value);
}
