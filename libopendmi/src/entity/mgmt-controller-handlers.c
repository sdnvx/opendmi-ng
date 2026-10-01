//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <string.h>

#include <opendmi/context.h>
#include <opendmi/field.h>
#include <opendmi/log.h>
#include <opendmi/encoder.h>
#include <opendmi/internal.h>
#include <opendmi/reader.h>
#include <opendmi/lint.h>
#include <opendmi/utils.h>
#include <opendmi/utils/codec.h>
#include <opendmi/utils/endian.h>

#include "mgmt-controller-internal.h"

/**
 * @internal
 * @brief Number of the bytes of the fields of Redfish over IP protocol record
 * data before the length of the hostname.
 */
#define DMI_MGMT_REDFISH_FIELDS_LENGTH 0x5A

/**
 * @internal
 * @brief Fields of the descriptor of a PCI/PCIe network interface.
 */
static const dmi_field_t dmi_mgmt_nhi_pci_fields[] = {
    DMI_FIELD(dmi_mgmt_nhi_pci_t, vendor_id,        dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_t, device_id,        dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_t, subsys_vendor_id, dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_t, subsys_id,        dmi_word_t),
    {}
};

/**
 * @internal
 * @brief Fields of the v2 descriptor of a USB network interface, after its
 * length.
 */
static const dmi_field_t dmi_mgmt_nhi_usb_v2_fields[] = {
    DMI_FIELD(dmi_mgmt_nhi_usb_v2_t, vendor_id,  dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_usb_v2_t, product_id, dmi_word_t),
    DMI_FIELD_STRING(dmi_mgmt_nhi_usb_v2_t, serial_number),
    DMI_FIELD_BINARY(dmi_mgmt_nhi_usb_v2_t, mac_address, DMI_MAC_ADDRESS_LENGTH),

    // Device characteristics are present since DSP0270 1.3
    DMI_FIELD_GROUP(),
    DMI_FIELD(dmi_mgmt_nhi_usb_v2_t, characteristics, dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_usb_v2_t, credential_handle, dmi_word_t,
              .absent = dmi_value_ptr((dmi_handle_t)DMI_HANDLE_INVALID)),
    {}
};

/**
 * @internal
 * @brief Fields of the v2 descriptor of a PCI/PCIe network interface, after
 * its length.
 */
static const dmi_field_t dmi_mgmt_nhi_pci_v2_fields[] = {
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, vendor_id,        dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, device_id,        dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, subsys_vendor_id, dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, subsys_id,        dmi_word_t),
    DMI_FIELD_BINARY(dmi_mgmt_nhi_pci_v2_t, mac_address, DMI_MAC_ADDRESS_LENGTH),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, segment_group, dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, bus_number,    dmi_byte_t),

    // Device and function numbers share a byte, the way PCI addresses do
    DMI_FIELD_BITS(dmi_mgmt_nhi_pci_v2_t, function_number, 3),
    DMI_FIELD_BITS(dmi_mgmt_nhi_pci_v2_t, device_number,   5),
    DMI_FIELD_PAD(dmi_byte_t),

    // Device characteristics are present since DSP0270 1.3
    DMI_FIELD_GROUP(),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, characteristics, dmi_word_t),
    DMI_FIELD(dmi_mgmt_nhi_pci_v2_t, credential_handle, dmi_word_t,
              .absent = dmi_value_ptr((dmi_handle_t)DMI_HANDLE_INVALID)),
    {}
};

/**
 * @internal
 * @brief Fields of Redfish over IP protocol record data, up to the length of
 * the hostname.
 */
static const dmi_field_t dmi_mgmt_redfish_fields[] = {
    DMI_FIELD_UUID(dmi_mgmt_redfish_over_ip_t, service_uuid),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, host_ip_assignment, dmi_byte_t),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, host_ip_format,     dmi_byte_t),
    DMI_FIELD_BINARY(dmi_mgmt_redfish_over_ip_t, host_ip_address, 16),
    DMI_FIELD_BINARY(dmi_mgmt_redfish_over_ip_t, host_ip_mask,    16),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, service_ip_discovery, dmi_byte_t),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, service_ip_format,    dmi_byte_t),
    DMI_FIELD_BINARY(dmi_mgmt_redfish_over_ip_t, service_ip_address, 16),
    DMI_FIELD_BINARY(dmi_mgmt_redfish_over_ip_t, service_ip_mask,    16),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, service_ip_port, dmi_word_t),
    DMI_FIELD(dmi_mgmt_redfish_over_ip_t, service_vlan_id, dmi_dword_t),
    {}
};

/**
 * @internal
 * @brief Convert UTF-16LE string to UTF-8.
 *
 * @details Unpaired surrogates are replaced with U+FFFD.
 *
 * @param[in] context Context to allocate the string in.
 * @param[in] data    UTF-16LE string.
 * @param[in] length  Length of the string in bytes.
 *
 * @return Allocated string, or `nullptr` if @p length is zero or on
 *         allocation failure.
 */
static char *dmi_mgmt_utf16_decode(dmi_context_t *context, const dmi_byte_t *data, size_t length);

/**
 * @internal
 * @brief Decode device descriptor of network host interface.
 *
 * @details Descriptors that do not fit into @p length are left in raw format.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[out]    nhi     Decoded network host interface.
 * @param[in]     length  Length of the interface data, which counts the
 *                        device type.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */
static bool dmi_mgmt_nhi_decode(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t length);

/**
 * @internal
 * @brief Decode the descriptor of a USB network interface, whose serial
 * number is a UTF-16 string descriptor following its fields.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[in,out] nhi     Network host interface holding the descriptor.
 * @param[in]     size    Size of the descriptor.
 *
 * @return `false` on allocation failure, `true` otherwise, the descriptor
 *         being left in raw format if it cannot be read.
 */
static bool dmi_mgmt_nhi_decode_usb(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t size);

/**
 * @internal
 * @brief Decode a v2 descriptor of a network interface, whose length, which
 * counts the device type and itself, follows the device type.
 *
 * @param[in,out] decoder    Decoder of the structure.
 * @param[in]     fields     Fields of the descriptor, after its length.
 * @param[in]     size       Size of the descriptor data.
 * @param[out]    descriptor Variable to store the decoded descriptor in.
 *
 * @return `true` if the descriptor has been decoded, `false` if it is too
 *         short or longer than the data of the interface.
 */
static bool dmi_mgmt_nhi_decode_v2(
        dmi_decoder_t     *decoder,
        const dmi_field_t *fields,
        size_t             size,
        void              *descriptor);

/**
 * @internal
 * @brief Decode the descriptor of an OEM-defined device, whose vendor IANA
 * code is stored with the most significant byte first.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[in,out] nhi     Network host interface holding the descriptor.
 * @param[in]     size    Size of the descriptor.
 *
 * @return `true` if the descriptor has been decoded, `false` otherwise.
 */
static bool dmi_mgmt_nhi_decode_oem(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t size);

/**
 * @internal
 * @brief Decode Redfish over IP protocol record data.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[in,out] record  Protocol record holding the data.
 *
 * @return `false` on allocation failure, `true` otherwise.
 */
static bool dmi_mgmt_redfish_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record);

/**
 * @internal
 * @brief Decode the type and the data of the interface, including the data
 * of a network host interface.
 *
 * @details Structure, which does not hold the whole interface data, is
 * marked incomplete, and is not decoded any further.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[out]    info    Decoded structure.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_mgmt_if_decode(dmi_decoder_t *decoder, dmi_mgmt_controller_t *info);

/**
 * @internal
 * @brief Decode the protocol records, which are present since SMBIOS 3.2.
 *
 * @details Records are counted only once they are completely decoded, and a
 * record the structure ends in the middle of marks the structure incomplete.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[out]    info    Decoded structure.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_mgmt_proto_records_decode(dmi_decoder_t *decoder, dmi_mgmt_controller_t *info);

/**
 * @internal
 * @brief Decode a single protocol record, including the data of Redfish over
 * IP.
 *
 * @details Record may be longer than the fields it is known to hold, so the
 * reader is left past the whole record.
 *
 * @param[in,out] decoder Decoder of the structure.
 * @param[out]    record  Decoded record.
 *
 * @return `true` on success, `false` otherwise.
 */
static bool dmi_mgmt_proto_record_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record);

bool dmi_mgmt_controller_decode(dmi_decoder_t *decoder)
{
    dmi_entity_t *entity = dmi_decoder_entity(decoder);

    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(mgmt_controller_host_if));
    if (info == nullptr)
        return false;

    if (not dmi_mgmt_if_decode(decoder, info))
        return false;
    if (dmi_entity_is_incomplete(entity))
        return true;

    // Protocol records are present since SMBIOS 3.2
    if (dmi_reader_is_done(dmi_decoder_reader(decoder)))
        return dmi_decoder_stop(decoder);

    return dmi_mgmt_proto_records_decode(decoder, info);
}

void dmi_mgmt_controller_cleanup(dmi_entity_t *entity)
{
    dmi_mgmt_controller_t *info;

    info = dmi_entity_info(entity, DMI_TYPE(mgmt_controller_host_if));
    if (info == nullptr)
        return;

    dmi_free(info->nhi.usb.serial_number);

    for (size_t i = 0; i < info->proto_records_count; i++)
        dmi_free(info->proto_records[i].redfish.service_hostname);

    dmi_free(info->proto_records);
}

bool dmi_mgmt_controller_encode(dmi_encoder_t *encoder)
{
    const dmi_mgmt_controller_t *info = dmi_entity_info(encoder->entity, DMI_TYPE(mgmt_controller_host_if));
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

static bool dmi_mgmt_nhi_decode(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t length)
{
    dmi_reader_t *reader = dmi_decoder_reader(decoder);

    dmi_byte_t device_type = 0;

    if (not dmi_decoder_get(decoder, dmi_byte_t, &device_type))
        return true;

    nhi->device_type = dmi_cast(nhi->device_type, device_type);
    nhi->format      = DMI_MGMT_NHI_FORMAT_RAW;

    // Descriptor is kept as a whole, and then read again by its type
    dmi_reader_mark_t start = dmi_reader_mark(reader);
    size_t            size  = length - 1;

    if (not dmi_decoder_get_binary(decoder, size, &nhi->descriptor))
        return true;

    dmi_reader_rewind(reader, start);

    switch (nhi->device_type) {
    case DMI_MGMT_NHI_DEVICE_TYPE_USB:
        return dmi_mgmt_nhi_decode_usb(decoder, nhi, size);

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI:
        if (dmi_fields_decode_into(decoder, dmi_mgmt_nhi_pci_fields, size, &nhi->pci))
            nhi->format = DMI_MGMT_NHI_FORMAT_PCI;
        break;

    case DMI_MGMT_NHI_DEVICE_TYPE_USB_V2:
        if (dmi_mgmt_nhi_decode_v2(decoder, dmi_mgmt_nhi_usb_v2_fields, size, &nhi->usb_v2))
            nhi->format = DMI_MGMT_NHI_FORMAT_USB_V2;
        break;

    case DMI_MGMT_NHI_DEVICE_TYPE_PCI_V2:
        if (dmi_mgmt_nhi_decode_v2(decoder, dmi_mgmt_nhi_pci_v2_fields, size, &nhi->pci_v2))
            nhi->format = DMI_MGMT_NHI_FORMAT_PCI_V2;
        break;

    default:
        if (dmi_mgmt_nhi_decode_oem(decoder, nhi, size))
            nhi->format = DMI_MGMT_NHI_FORMAT_OEM;
        break;
    }

    return true;
}

static bool dmi_mgmt_nhi_decode_usb(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t size)
{
    dmi_context_t      *context = dmi_entity_context(dmi_decoder_entity(decoder));
    dmi_mgmt_nhi_usb_t *usb     = &nhi->usb;
    dmi_byte_t          serial_length = 0;

    bool status =
        (size >= 6) and
        dmi_decoder_get(decoder, dmi_word_t, &usb->vendor_id) and
        dmi_decoder_get(decoder, dmi_word_t, &usb->product_id) and
        dmi_decoder_get(decoder, dmi_byte_t, &serial_length);
    if (not status)
        return true;

    // Serial number descriptor length includes its length and type
    if ((serial_length < 2) or (serial_length > size - 4))
        return true;

    // Serial number is a UTF-16 string, that follows the descriptor length
    // and type
    if (serial_length > 2) {
        usb->serial_number = dmi_mgmt_utf16_decode(context, nhi->descriptor.data + 6, serial_length - 2);
        if (usb->serial_number == nullptr)
            return false;
    }

    nhi->format = DMI_MGMT_NHI_FORMAT_USB;

    return true;
}

static bool dmi_mgmt_nhi_decode_v2(
        dmi_decoder_t     *decoder,
        const dmi_field_t *fields,
        size_t             size,
        void              *descriptor)
{
    dmi_byte_t v2_length = 0;

    if (not dmi_decoder_get(decoder, dmi_byte_t, &v2_length))
        return false;

    // Length counts the device type, which has been read, and itself, and
    // the descriptor ends where the data of the interface does at most
    if ((v2_length < 2) or (v2_length > size + 1))
        return false;

    return dmi_fields_decode_into(decoder, fields, v2_length - 2, descriptor);
}

static bool dmi_mgmt_nhi_decode_oem(dmi_decoder_t *decoder, dmi_mgmt_nhi_t *nhi, size_t size)
{
    if ((nhi->device_type < DMI_MGMT_NHI_DEVICE_TYPE_OEM_START) or (size < 4))
        return false;

    dmi_mgmt_nhi_oem_t *oem = &nhi->oem;
    dmi_dword_t vendor_iana = 0;

    bool status =
        dmi_decoder_get_bytes(decoder, &vendor_iana, sizeof(vendor_iana)) and
        dmi_decoder_get_binary(decoder, size - 4, &oem->vendor_data);
    if (not status)
        return false;

    oem->vendor_iana = dmi_ntoh(vendor_iana);

    return true;
}

static bool dmi_mgmt_redfish_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record)
{
    dmi_entity_t               *entity  = dmi_decoder_entity(decoder);
    dmi_mgmt_redfish_over_ip_t *redfish = &record->redfish;

    dmi_byte_t hostname_length = 0;

    // Hostname is the only variable-length field, which follows its length
    size_t length = record->data.length;
    if (length < DMI_MGMT_REDFISH_FIELDS_LENGTH + 1)
        return true;

    bool status =
        dmi_fields_decode_into(decoder, dmi_mgmt_redfish_fields, DMI_MGMT_REDFISH_FIELDS_LENGTH, redfish) and
        dmi_decoder_get(decoder, dmi_byte_t, &hostname_length) and
        (hostname_length <= length - DMI_MGMT_REDFISH_FIELDS_LENGTH - 1);
    if (not status)
        return true;

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
    const char *hostname = (const char *)record->data.data + DMI_MGMT_REDFISH_FIELDS_LENGTH + 1;
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

static bool dmi_mgmt_if_decode(dmi_decoder_t *decoder, dmi_mgmt_controller_t *info)
{
    dmi_reader_t *reader = dmi_decoder_reader(decoder);

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

    return true;
}

static bool dmi_mgmt_proto_records_decode(dmi_decoder_t *decoder, dmi_mgmt_controller_t *info)
{
    dmi_entity_t *entity = dmi_decoder_entity(decoder);

    dmi_byte_t proto_records_count = 0;
    if (not dmi_decoder_get(decoder, dmi_byte_t, &proto_records_count))
        return dmi_decoder_incomplete(decoder);

    entity->level = dmi_version(3, 2, 0);

    if (proto_records_count == 0)
        return true;

    info->proto_records = dmi_alloc_array(dmi_entity_context(entity), sizeof(*info->proto_records),
                                          proto_records_count);
    if (info->proto_records == nullptr)
        return false;

    // Records count is incremented only for completely decoded records
    for (size_t i = 0; i < proto_records_count; i++) {
        if (not dmi_mgmt_proto_record_decode(decoder, &info->proto_records[i]))
            return false;
        if (dmi_entity_is_incomplete(entity))
            return true;

        info->proto_records_count++;
    }

    return true;
}

static bool dmi_mgmt_proto_record_decode(dmi_decoder_t *decoder, dmi_mgmt_proto_record_t *record)
{
    dmi_reader_t *reader = dmi_decoder_reader(decoder);

    dmi_byte_t type   = 0;
    dmi_byte_t length = 0;

    bool status =
        dmi_decoder_get(decoder, dmi_byte_t, &type) and
        dmi_decoder_get(decoder, dmi_byte_t, &length) and
        dmi_reader_has(reader, length);
    if (not status)
        return dmi_decoder_incomplete(decoder);

    dmi_reader_mark_t data_start = dmi_reader_mark(reader);

    record->type = dmi_cast(record->type, type);
    if (not dmi_decoder_get_binary(decoder, length, &record->data))
        return false;

    if (record->type == DMI_MGMT_PROTO_REDFISH_OVER_IP) {
        dmi_reader_rewind(reader, data_start);

        if (not dmi_mgmt_redfish_decode(decoder, record))
            return false;
    }

    // Record may be longer than the fields it is known to hold, so the next
    // one is found by the length rather than by counting
    dmi_reader_skip_ex(reader, data_start, length);

    return true;
}
