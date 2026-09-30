//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Publishes the schemas of the documents the tool exports under /schemas/ of
// the site, which is where their identifiers and the namespace of the XML
// documents point, so that the identifiers resolve to the schemas themselves.
//
// The schemas are kept along with the rest of what the tool shares, and are
// not pages of any component, so they are added to the site as it is
// published rather than collected into the component.
//
'use strict'

const fs   = require('node:fs')
const path = require('node:path')

const root   = path.resolve(__dirname, '..')
const source = path.join(root, 'opendmi', 'share', 'opendmi')

// Schemas and the directory of the site they are published in
const schemas = ['opendmi.json', 'opendmi.xsd', 'opendmi.yml']
const target  = 'schemas'

module.exports.register = function ()
{
    this.on('beforePublish', ({ siteCatalog }) => {
        for (const schema of schemas) {
            siteCatalog.addFile({
                contents: fs.readFileSync(path.join(source, schema)),
                out:      { path: `${target}/${schema}` }
            })
        }
    })
}
