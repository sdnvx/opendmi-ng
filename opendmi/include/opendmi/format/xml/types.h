//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef OPENDMI_FORMAT_XML_TYPES_H
#define OPENDMI_FORMAT_XML_TYPES_H

#pragma once

#include <stdio.h>

#include <libxml/xmlIO.h>
#include <libxml/xmlwriter.h>

#include <opendmi/types.h>
#include <opendmi/attribute.h>
#include <opendmi/format.h>

#define DMI_XML_PREFIX    "dmi"
#define DMI_XML_NAMESPACE "https://opendmi.org/schemas/opendmi.xsd"

// Schema of the documents is published at the address of their namespace
#define DMI_XML_SCHEMA_LOCATION DMI_XML_NAMESPACE " " DMI_XML_NAMESPACE

#define DMI_XSI_PREFIX    "xsi"
#define DMI_XSI_NAMESPACE "http://www.w3.org/2001/XMLSchema-instance"

typedef struct dmi_xml_session
{
    /**
     * @brief Context handle.
     */
    dmi_context_t *context;

    /**
     * @brief Output options.
     */
    dmi_format_options_t options;

    /**
     * @brief Output stream.
     */
    FILE *stream;

    /**
     * @brief XML output buffer.
     */
    xmlOutputBuffer *buffer;

    /**
     * @brief XML writer handle.
     */
    xmlTextWriter *writer;
} dmi_xml_session_t;

#endif // !OPENDMI_FORMAT_XML_TYPES_H
