//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdbool.h>
#include <cmocka.h>

#include <opendmi/internal.h>

#include <opendmi/entity/chassis.h>
#include <opendmi/entity/dell/common.h>
#include <opendmi/entity/intel/rsd-fpga.h>
#include <opendmi/entity/intel/rsd-phys-device-mapping.h>
#include <opendmi/entity/intel/rsd-processor-cpuid.h>
#include <opendmi/entity/mgmt-controller.h>
#include <opendmi/entity/processor-ex.h>
#include <opendmi/entity/system-boot.h>
#include <opendmi/entity/system-event-log.h>

static void test_entity_value_names(void **pstate);

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_entity_value_names)
    };

    return cmocka_run_group_tests(tests, nullptr, nullptr);
}

//
// Values of the enumerations are named by the functions of their own, which
// know every value their name sets do and nothing else.
//
static void test_entity_value_names(void **pstate)
{
    dmi_unused(pstate);

    assert_string_equal(dmi_boot_status_name(DMI_BOOT_STATUS_NO_ERRORS_DETECTED), "No errors detected");
    assert_string_equal(dmi_dell_check_type_name(DMI_DELL_CHECK_TYPE_WORD_CHECKSUM), "Word checksum");
    assert_string_equal(dmi_dell_value_format_name(DMI_DELL_VALUE_FORMAT_SCAN_CODE), "Alphanumeric, scan codes");
    assert_string_equal(dmi_event_log_data_format_name(DMI_EVENT_LOG_DATA_FORMAT_NONE), "None");
    assert_string_equal(dmi_event_log_type_name(DMI_EVENT_LOG_TYPE_SINGLE_BIT_ECC), "Single-bit ECC memory error");
    assert_string_equal(dmi_intel_rsd_cpuid_subtype_name(DMI_INTEL_RSD_CPUID_SUBTYPE_BASIC), "Leaves 00h to 10h");
    assert_string_equal(dmi_intel_rsd_fpga_hps_isa_name(DMI_INTEL_RSD_FPGA_HPS_ISA_X86), "x86");
    assert_string_equal(dmi_intel_rsd_fpga_hssi_config_name(DMI_INTEL_RSD_FPGA_HSSI_CONFIG_NETWORKING), "Networking");
    assert_string_equal(dmi_intel_rsd_fpga_memory_tech_name(DMI_INTEL_RSD_FPGA_MEMORY_TECH_NONE), "None");
    assert_string_equal(dmi_intel_rsd_fpga_status_name(DMI_INTEL_RSD_FPGA_STATUS_DISABLED), "Disabled");
    assert_string_equal(dmi_intel_rsd_fpga_type_name(DMI_INTEL_RSD_FPGA_TYPE_INTEGRATED), "Integrated");
    assert_string_equal(dmi_intel_rsd_phys_device_type_name(DMI_INTEL_RSD_PHYS_DEVICE_TYPE_INVALID), "Undefined or invalid");
    assert_string_equal(dmi_mgmt_if_type_name(DMI_MGMT_IF_TYPE_MCTP_KCS), "Keyboard Controller Style");
    assert_string_equal(dmi_mgmt_nhi_characteristic_name(DMI_MGMT_NHI_CHAR_CREDENTIAL_BOOTSTRAPPING), "Credential bootstrapping via IPMI commands");
    assert_string_equal(dmi_mgmt_nhi_device_type_name(DMI_MGMT_NHI_DEVICE_TYPE_USB), "USB network interface");
    assert_string_equal(dmi_mgmt_proto_name(DMI_MGMT_PROTO_IPMI), "IPMI");
    assert_string_equal(dmi_mgmt_redfish_ip_assignment_name(DMI_MGMT_REDFISH_IP_ASSIGNMENT_UNKNOWN), "Unknown");
    assert_string_equal(dmi_mgmt_redfish_ip_format_name(DMI_MGMT_REDFISH_IP_FORMAT_UNKNOWN), "Unknown");
    assert_string_equal(dmi_processor_arch_name(DMI_PROCESSOR_ARCH_RESERVED), "Reserved");
    assert_string_equal(dmi_rack_type_name(DMI_RACK_TYPE_OPEN), "Open Rack");
    assert_string_equal(dmi_system_log_access_method_name(DMI_SYSTEM_LOG_ACCESS_METHOD_INDEXED_IO_8BIT_1_1), "Indexed I/O: 1 8-bit index port, 1 8-bit data port");
    assert_string_equal(dmi_system_log_header_fmt_name(DMI_SYSTEM_LOG_HEADER_FMT_NO_HEADER), "No header");

    assert_null(dmi_boot_status_name((dmi_boot_status_t)0x100));
    assert_null(dmi_dell_check_type_name((dmi_dell_check_type_t)0x100));
    assert_null(dmi_dell_value_format_name((dmi_dell_value_format_t)0x100));
    assert_null(dmi_event_log_data_format_name((dmi_event_log_data_format_t)0x100));
    assert_null(dmi_event_log_type_name((dmi_event_log_type_t)0x100));
    assert_null(dmi_intel_rsd_cpuid_subtype_name((dmi_intel_rsd_cpuid_subtype_t)0x100));
    assert_null(dmi_intel_rsd_fpga_hps_isa_name((dmi_intel_rsd_fpga_hps_isa_t)0x100));
    assert_null(dmi_intel_rsd_fpga_hssi_config_name((dmi_intel_rsd_fpga_hssi_config_t)0x100));
    assert_null(dmi_intel_rsd_fpga_memory_tech_name((dmi_intel_rsd_fpga_memory_tech_t)0x100));
    assert_null(dmi_intel_rsd_fpga_status_name((dmi_intel_rsd_fpga_status_t)0x100));
    assert_null(dmi_intel_rsd_fpga_type_name((dmi_intel_rsd_fpga_type_t)0x100));
    assert_null(dmi_intel_rsd_phys_device_type_name((dmi_intel_rsd_phys_device_type_t)0x100));
    assert_null(dmi_mgmt_if_type_name((dmi_mgmt_if_type_t)0x100));
    assert_null(dmi_mgmt_nhi_characteristic_name((dmi_mgmt_nhi_characteristic_t)0x100));
    assert_null(dmi_mgmt_nhi_device_type_name((dmi_mgmt_nhi_device_type_t)0x100));
    assert_null(dmi_mgmt_proto_name((dmi_mgmt_proto_t)0x100));
    assert_null(dmi_mgmt_redfish_ip_assignment_name((dmi_mgmt_redfish_ip_assignment_t)0x100));
    assert_null(dmi_mgmt_redfish_ip_format_name((dmi_mgmt_redfish_ip_format_t)0x100));
    assert_null(dmi_processor_arch_name((dmi_processor_arch_t)0x100));
    assert_null(dmi_rack_type_name((dmi_rack_type_t)0x100));
    assert_null(dmi_system_log_access_method_name((dmi_system_log_access_method_t)0x100));
    assert_null(dmi_system_log_header_fmt_name((dmi_system_log_header_fmt_t)0x100));
}
