//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/log.h>
#include <opendmi/encoder.h>
#include <opendmi/internal.h>
#include <opendmi/reader.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/utils/endian.h>

#include <opendmi/entity/mgmt-controller-internal.h>

static char *dmi_mgmt_utf16_decode(dmi_context_t *context, const dmi_byte_t *data, size_t length);
static bool dmi_mgmt_nhi_decode(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t length);
static bool dmi_mgmt_redfish_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record);

/**
 * @brief Convert UTF-16LE string to UTF-8.
 *
 * Unpaired surrogates are replaced with U+FFFD.
 *
 * @return Allocated string, or @c nullptr if @p length is zero or on
 *         allocation failure.
 */
static char *dmi_mgmt_utf16_decode(dmi_context_t *context, const dmi_byte_t *data, size_t length)
{
    size_t count = length / 2;
    if (count == 0)
        return nullptr;

    // Every UTF-16 code unit takes at most 3 bytes in UTF-8
    char *str = dmi_alloc(context, count * 3 + 1);
    if (str == nullptr)
        return nullptr;

    unsigned char *pos = (unsigned char *)str;

    for (size_t i = 0; i < count; i++) {
        uint32_t code = (uint32_t)(data[i * 2] | (data[i * 2 + 1] << 8));

        if ((code >= 0xD800) and (code <= 0xDBFF) and (i + 1 < count)) {
            uint32_t low = (uint32_t)(data[i * 2 + 2] | (data[i * 2 + 3] << 8));
            if ((low >= 0xDC00) and (low <= 0xDFFF)) {
                code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                i++;
            }
        }
        if ((code >= 0xD800) and (code <= 0xDFFF))
            code = 0xFFFD;

        // Surrogate pairs take 4 bytes in UTF-8, as two code units do
        if (code < 0x80) {
            *pos++ = (unsigned char)code;
        } else if (code < 0x800) {
            *pos++ = (unsigned char)(0xC0 | (code >> 6));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        } else if (code < 0x10000) {
            *pos++ = (unsigned char)(0xE0 | (code >> 12));
            *pos++ = (unsigned char)(0x80 | ((code >> 6) & 0x3F));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        } else {
            *pos++ = (unsigned char)(0xF0 | (code >> 18));
            *pos++ = (unsigned char)(0x80 | ((code >> 12) & 0x3F));
            *pos++ = (unsigned char)(0x80 | ((code >> 6) & 0x3F));
            *pos++ = (unsigned char)(0x80 | (code & 0x3F));
        }
    }

    *pos = 0;

    return str;
}

/**
 * @brief Decode device descriptor of network host interface.
 *
 * Descriptors that do not fit into @p length are left in raw format.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */

static bool dmi_mgmt_nhi_decode(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t length)
{
    dmi_entity_t  *entity  = dmi_decoder_entity(decoder);
    dmi_context_t *context = dmi_entity_context(entity);
    dmi_reader_t  *reader  = dmi_decoder_reader(decoder);

    dmi_byte_t device_type = 0;

    if (not dmi_decoder_get(decoder, dmi_byte_t, &device_type))
        return true;

    nhi->device_type = dmi_cast(nhi->device_type, device_type);
    nhi->format      = DMI_MGMT_NHI_FORMAT_RAW;

    size_t start = reader->position;
    size_t size  = length - 1;

    if (not dmi_decoder_get_binary(decoder, size, &nhi->descriptor))
        return true;

    dmi_reader_seek(reader, start);

    // Length of v2 descriptors includes device type and length fields
    dmi_byte_t v2_length = 0;
    bool status = true;

    switch (nhi->device_type) {
    case DMI_MGMT_NHI_DEVICE_TYPE_USB: {
        dmi_mgmt_nhi_usb_t *usb = &nhi->usb;
        dmi_byte_t serial_length = 0;

        status =
            (size >= 6) and
            dmi_decoder_get(decoder, dmi_word_t, &usb->vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &usb->product_id) and
            dmi_decoder_get(decoder, dmi_byte_t, &serial_length);
        if (not status)
            break;

        // Serial number descriptor length includes its length and type
        if ((serial_length < 2) or (serial_length > size - 4))
            break;

        // Serial number is a UTF-16 string, that follows the descriptor
        // length and type
        if (serial_length > 2) {
            usb->serial_number = dmi_mgmt_utf16_decode(context, nhi->descriptor.data + 6,
                                                       serial_length - 2);
            if (usb->serial_number == nullptr)
                return false;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_USB;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI: {
        dmi_mgmt_nhi_pci_t *pci = &nhi->pci;

        status =
            (size >= 8) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->device_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->subsys_vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->subsys_id);
        if (status)
            nhi->format = DMI_MGMT_NHI_FORMAT_PCI;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_USB_V2: {
        dmi_mgmt_nhi_usb_v2_t *usb    = &nhi->usb_v2;
        dmi_string_t           number = 0;

        status =
            dmi_decoder_get(decoder, dmi_byte_t, &v2_length) and
            (v2_length >= 0x0D) and
            (v2_length <= length) and
            dmi_decoder_get(decoder, dmi_word_t, &usb->vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &usb->product_id) and
            dmi_decoder_get(decoder, dmi_string_t, &number) and
            dmi_decoder_get_binary(decoder, DMI_MAC_ADDRESS_LENGTH, &usb->mac_address);
        if (not status)
            break;

        usb->serial_number = dmi_entity_string(entity, number);

        // Device characteristics are present since DSP0270 1.3
        usb->credential_handle = DMI_HANDLE_INVALID;
        if (v2_length >= 0x11) {
            status =
                dmi_decoder_get(decoder, dmi_word_t, &usb->characteristics) and
                dmi_decoder_get(decoder, dmi_word_t, &usb->credential_handle);
            if (not status)
                break;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_USB_V2;
        break;
    }

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI_V2: {
        dmi_mgmt_nhi_pci_v2_t *pci = &nhi->pci_v2;
        dmi_byte_t devfn = 0;

        status =
            dmi_decoder_get(decoder, dmi_byte_t, &v2_length) and
            (v2_length >= 0x14) and
            (v2_length <= length) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->device_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->subsys_vendor_id) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->subsys_id) and
            dmi_decoder_get_binary(decoder, DMI_MAC_ADDRESS_LENGTH, &pci->mac_address) and
            dmi_decoder_get(decoder, dmi_word_t, &pci->segment_group) and
            dmi_decoder_get(decoder, dmi_byte_t, &pci->bus_number) and
            dmi_decoder_get(decoder, dmi_byte_t, &devfn);
        if (not status)
            break;

        pci->device_number   = devfn >> 3;
        pci->function_number = devfn & 0x07;

        // Device characteristics are present since DSP0270 1.3
        pci->credential_handle = DMI_HANDLE_INVALID;
        if (v2_length >= 0x18) {
            status =
                dmi_decoder_get(decoder, dmi_word_t, &pci->characteristics) and
                dmi_decoder_get(decoder, dmi_word_t, &pci->credential_handle);
            if (not status)
                break;
        }

        nhi->format = DMI_MGMT_NHI_FORMAT_PCI_V2;
        break;
    }

    default: {
        if ((nhi->device_type < DMI_MGMT_NHI_DEVICE_TYPE_OEM_START) or (size < 4))
            break;

        dmi_mgmt_nhi_oem_t *oem = &nhi->oem;
        dmi_dword_t vendor_iana = 0;

        // Vendor IANA code is stored with the most significant byte first
        status =
            dmi_decoder_get_bytes(decoder, &vendor_iana, sizeof(vendor_iana)) and
            dmi_decoder_get_binary(decoder, size - 4, &oem->vendor_data);
        if (not status)
            break;

        oem->vendor_iana = dmi_ntoh(vendor_iana);
        nhi->format = DMI_MGMT_NHI_FORMAT_OEM;
        break;
    }
    }

    return true;
}

/**
 * @brief Decode Redfish over IP protocol record data.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */

static bool dmi_mgmt_redfish_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record)
{
    dmi_entity_t               *entity  = dmi_decoder_entity(decoder);
    dmi_mgmt_redfish_over_ip_t *redfish = &record->redfish;

    dmi_byte_t host_ip_assignment   = 0;
    dmi_byte_t host_ip_format       = 0;
    dmi_byte_t service_ip_discovery = 0;
    dmi_byte_t service_ip_format    = 0;
    dmi_byte_t hostname_length      = 0;

    // Hostname is the only variable-length field
    size_t length = record->data.length;
    if (length < 0x5B)
        return true;

    bool status =
        dmi_decoder_get_uuid(decoder, &redfish->service_uuid) and
        dmi_decoder_get(decoder, dmi_byte_t, &host_ip_assignment) and
        dmi_decoder_get(decoder, dmi_byte_t, &host_ip_format) and
        dmi_decoder_get_binary(decoder, 16, &redfish->host_ip_address) and
        dmi_decoder_get_binary(decoder, 16, &redfish->host_ip_mask) and
        dmi_decoder_get(decoder, dmi_byte_t, &service_ip_discovery) and
        dmi_decoder_get(decoder, dmi_byte_t, &service_ip_format) and
        dmi_decoder_get_binary(decoder, 16, &redfish->service_ip_address) and
        dmi_decoder_get_binary(decoder, 16, &redfish->service_ip_mask) and
        dmi_decoder_get(decoder, dmi_word_t, &redfish->service_ip_port) and
        dmi_decoder_get(decoder, dmi_dword_t, &redfish->service_vlan_id) and
        dmi_decoder_get(decoder, dmi_byte_t, &hostname_length) and
        (hostname_length <= length - 0x5B);
    if (not status)
        return true;

    redfish->host_ip_assignment   = dmi_cast(redfish->host_ip_assignment, host_ip_assignment);
    redfish->host_ip_format       = dmi_cast(redfish->host_ip_format, host_ip_format);
    redfish->service_ip_discovery = dmi_cast(redfish->service_ip_discovery, service_ip_discovery);
    redfish->service_ip_format    = dmi_cast(redfish->service_ip_format, service_ip_format);

    // IPv4 addresses take the first 4 bytes of the fields
    if (redfish->host_ip_format == DMI_MGMT_REDFISH_IP_FORMAT_IPV4) {
        redfish->host_ip_address.length = 4;
        redfish->host_ip_mask.length    = 4;
    }
    if (redfish->service_ip_format == DMI_MGMT_REDFISH_IP_FORMAT_IPV4) {
        redfish->service_ip_address.length = 4;
        redfish->service_ip_mask.length    = 4;
    }

    // Hostname is not an SMBIOS string, and may be padded with NULL
    // characters
    const char *hostname = (const char *)record->data.data + 0x5B;
    size_t hostname_size = strnlen(hostname, hostname_length);

    if (hostname_size > 0) {
        redfish->service_hostname = dmi_alloc(dmi_entity_context(entity), hostname_size + 1);
        if (redfish->service_hostname == nullptr)
            return false;

        memcpy(redfish->service_hostname, hostname, hostname_size);
        redfish->service_hostname[hostname_size] = 0;
    }

    record->has_redfish = true;

    return true;
}

bool dmi_mgmt_controller_decode(dmi_decoder_t *decoder)
{
    dmi_entity_t *entity = dmi_decoder_entity(decoder);

    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    if (info == nullptr)
        return false;

    dmi_context_t *context = dmi_entity_context(entity);
    dmi_reader_t  *reader  = dmi_decoder_reader(decoder);

    dmi_byte_t if_type        = 0;
    dmi_byte_t if_data_length = 0;

    bool status =
        dmi_decoder_get(decoder, dmi_byte_t, &if_type) and
        dmi_decoder_get(decoder, dmi_byte_t, &if_data_length);
    if (not status)
        return false;

    info->if_type = dmi_cast(info->if_type, if_type);

    // Some implementations use different structure layout (e.g. the one from
    // SMBIOS versions prior to 3.2), so stop decoding there as dmidecode does
    if (not dmi_reader_has(reader, if_data_length))
        return dmi_decoder_incomplete(decoder);

    // Interface data is read as a whole, and then read again by its type, so
    // the cursor ends up past it either way
    dmi_reader_mark_t if_data_start = dmi_reader_mark(reader);

    if (not dmi_decoder_get_binary(decoder, if_data_length, &info->if_data))
        return false;

    if ((info->if_type == DMI_MGMT_IF_TYPE_NETWORK_HOST_IF) and (if_data_length > 0)) {
        dmi_reader_rewind(reader, if_data_start);

        if (not dmi_mgmt_nhi_decode(decoder, &info->nhi, if_data_length))
            return false;

        info->has_nhi = true;

        dmi_reader_skip_ex(reader, if_data_start, if_data_length);
    }

    // Protocol records are present since SMBIOS 3.2
    if (dmi_reader_is_done(reader))
        return dmi_decoder_stop(decoder);

    dmi_byte_t proto_records_count = 0;
    if (not dmi_decoder_get(decoder, dmi_byte_t, &proto_records_count))
        return dmi_decoder_incomplete(decoder);

    entity->level = dmi_version(3, 2, 0);

    if (proto_records_count == 0)
        return true;

    info->proto_records = dmi_alloc_array(context, sizeof(*info->proto_records),
                                          proto_records_count);
    if (info->proto_records == nullptr)
        return false;

    // Records count is incremented only for completely decoded records
    for (size_t i = 0; i < proto_records_count; i++) {
        dmi_byte_t type   = 0;
        dmi_byte_t length = 0;

        status =
            dmi_decoder_get(decoder, dmi_byte_t, &type) and
            dmi_decoder_get(decoder, dmi_byte_t, &length) and
            dmi_reader_has(reader, length);
        if (not status)
            return dmi_decoder_incomplete(decoder);

        dmi_mgmt_proto_record_t *record = &info->proto_records[i];
        dmi_reader_mark_t data_start = dmi_reader_mark(reader);

        record->type = dmi_cast(record->type, type);
        if (not dmi_decoder_get_binary(decoder, length, &record->data))
            return false;

        if (record->type == DMI_MGMT_PROTO_REDFISH_OVER_IP) {
            dmi_reader_rewind(reader, data_start);

            if (not dmi_mgmt_redfish_decode(decoder, record))
                return false;
        }

        // Record may be longer than the fields it is known to hold, so the
        // next one is found by the length rather than by counting
        dmi_reader_skip_ex(reader, data_start, length);

        info->proto_records_count++;
    }

    return true;
}

void dmi_mgmt_controller_cleanup(dmi_entity_t *entity)
{
    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    if (info == nullptr)
        return;

    dmi_free(info->nhi.usb.serial_number);

    for (size_t i = 0; i < info->proto_records_count; i++)
        dmi_free(info->proto_records[i].redfish.service_hostname);

    dmi_free(info->proto_records);
}

//
// Interface data and protocol records are written as the structure holds
// them, since the ways they are read again by their types are derived from
// them. Protocol records are present since SMBIOS 3.2.
//
bool dmi_mgmt_controller_encode(dmi_encoder_t *encoder)
{
    const dmi_mgmt_controller_t *info = dmi_entity_info(encoder->entity, DMI_TYPE(MGMT_CONTROLLER_HOST_IF));
    if (info == nullptr)
        return false;

    if (not dmi_encoder_put(encoder, dmi_byte_t, info->if_type))
        return false;

    // Interface data longer than the structure is not read at all, and the
    // length the source data declares for it is kept
    dmi_byte_t original = 0;

    if ((info->if_data.length == 0) and dmi_encoder_peek(encoder, &original, sizeof(original)) and
        (original > dmi_encoder_remaining(encoder) - sizeof(original)))
        return dmi_encoder_put(encoder, dmi_byte_t, original);

    bool status =
        dmi_encoder_put(encoder, dmi_byte_t, info->if_data.length) and
        dmi_encoder_put_bytes(encoder, info->if_data.data, info->if_data.length);
    if (not status)
        return false;

    bool has_records = (encoder->mode == DMI_ENCODE_MODE_PRESERVE)
                     ? (dmi_encoder_remaining(encoder) > 0)
                     : (encoder->version >= DMI_VERSION(3, 2, 0));

    if (not has_records)
        return true;

    if (not dmi_encoder_put(encoder, dmi_byte_t, info->proto_records_count))
        return false;

    for (size_t i = 0; i < info->proto_records_count; i++) {
        const dmi_mgmt_proto_record_t *record = &info->proto_records[i];

        status =
            dmi_encoder_put(encoder, dmi_byte_t, record->type) and
            dmi_encoder_put(encoder, dmi_byte_t, record->data.length) and
            dmi_encoder_put_bytes(encoder, record->data.data, record->data.length);
        if (not status)
            return false;
    }

    return true;
}
