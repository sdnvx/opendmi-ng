//
// OpenDMI: Cross-platform DMI/SMBIOS framework
// Copyright (c) 2025-2026, The OpenDMI contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Gathers what the site shows but does not hold itself.
//
// The reference manual is a book, which includes its parts, and the parts
// include the documents they are made of: its own pages, and the manual pages
// of the tool and the library, which live next to the sources they describe.
// Every document including no others becomes a page of the site as it is, and
// the navigation follows the way the book includes them. The book is built as
// a PDF too, which the site offers for download.
//
// The release the site offers for download is the latest one the changelog
// records, so that the site follows the releases without being edited. The
// version the documentation describes is the one of the build, which the site
// takes from the same place the build does.
//
// The contributors are listed on the site the way the repository lists them.
//
// The logos are kept along with the rest of what the components share, and
// are published as the images of the site: the full one for the home page,
// the one without the tagline for the header of every page, and the icon
// every page gives to the browser.
//
// Run by the Antora collector extension from the root of the repository,
// which scans the output directory into the component.
//
'use strict'

const fs   = require('node:fs')
const path = require('node:path')

const { execFileSync } = require('node:child_process')

const root      = path.resolve(__dirname, '..')
const source    = path.join(root, 'reference', 'src')
const collected = path.join(__dirname, 'build', 'collect')
const output    = path.join(collected, 'modules', 'reference')

// Name the PDF of the reference manual is published under
const pdf = 'opendmi-reference.pdf'

// Logos and icons the site shows
const logos = ['opendmi-logo.svg', 'opendmi-logo-compact.svg', 'opendmi-icon.svg', 'favicon.ico']

// Read the documents a book or a part includes, in the order it includes
// them, grouped by the headings between the includes
function readPart(file)
{
    const text  = fs.readFileSync(file, 'utf8')
    const nodes = []

    let group = null

    for (const line of text.split('\n')) {
        const heading = line.match(/^== (.+)$/)
        const include = line.match(/^include::([^[]+)\[.*\]\s*$/)

        if (heading) {
            group = { title: heading[1], children: [] }
            nodes.push(group)
        } else if (include && include[1].endsWith('.adoc')) {
            // Files of other kinds are included for their text rather than
            // as documents, e.g. the license the legal notice is made of
            const node = readDocument(path.resolve(path.dirname(file), include[1]), file)
            ;(group ? group.children : nodes).push(node)
        }
    }

    return nodes
}

// Documents including others are parts, and the rest are pages. Pages of the
// book are placed the way the book lays them out, and the ones it includes
// from elsewhere, e.g. the manual pages, are placed in the part including them
function readDocument(file, parent)
{
    const text  = fs.readFileSync(file, 'utf8')
    const title = text.match(/^= (.+)$/m)?.[1] ?? path.basename(file, '.adoc')

    if (/^include::/m.test(text))
        return { title, page: relativePath(file), children: readPart(file) }

    const page = file.startsWith(source + path.sep)
        ? relativePath(file)
        : path.join(path.dirname(relativePath(parent)), path.basename(file))

    return {
        title,
        file,
        page,
        summary: readSummary(text)
    }
}

// Purpose of a manual page, which its Name section gives after the name, and
// which a paragraph may carry over several lines, up to the end of the
// section or of the condition it is written under
function readSummary(text)
{
    const name = text.match(/^== Name\s*\n\s*\n((?:(?!endif::)[^\n]+\n)+)/m)?.[1]

    if (name === undefined)
        return undefined

    return name.replace(/\s+/g, ' ').trim().match(/^.+? - (.+?)\.?$/)?.[1]
}

// Pages of the manual pages the book includes, by the names and sections the
// manual pages are referred to by, e.g. dmi_open(3). A manual page describing
// several functions or types is installed under the name of each, the other
// names being links to its source, which are looked up next to it
function listManPages(pages)
{
    const byFile = new Map(pages.map((page) => [fs.realpathSync(page.file), page.page]))
    const dirs   = new Set(pages.map((page) => path.dirname(page.file)))
    const names  = new Map()

    for (const dir of dirs) {
        for (const entry of fs.readdirSync(dir)) {
            const name = entry.match(/^(.+)\.(\d\w*)\.adoc$/)

            if (name === null)
                continue

            const page = byFile.get(fs.realpathSync(path.join(dir, entry)))

            if (page !== undefined)
                names.set(`${name[1]}(${name[2]})`, page)
        }
    }

    return names
}

// Manual pages refer to each other the way manual pages do, e.g. `dmi_open`(3),
// which the pages of the site turn into links to the pages of the site. Pages
// of the site are named without the sections, the way their titles name them,
// and the references of a page to itself are not linked. The manual pages the
// book does not include, e.g. the ones of the system, are left as they are
function linkManPages(text, page, names)
{
    return text.replace(/`([\w.-]+)`\((\d\w*)\)/g, (ref, name, section) => {
        const target = names.get(`${name}(${section})`)

        if (target === undefined)
            return ref

        return (target !== page) ? `xref:${target}[\`${name}\`]` : `\`${name}\``
    })
}

function relativePath(file)
{
    return path.relative(source, file).split(path.sep).join('/')
}

function isPart(node)
{
    return node.children !== undefined
}

function isGroup(node)
{
    return isPart(node) && (node.page === undefined)
}

function listPages(nodes)
{
    return nodes.flatMap((node) => isPart(node) ? listPages(node.children) : [node])
}

function listParts(nodes)
{
    return nodes.flatMap((node) => isPart(node)
        ? [...(isGroup(node) ? [] : [node]), ...listParts(node.children)]
        : [])
}

// Groups are plain entries of the navigation, and are left out when they hold
// no pages yet
function renderNav(node, depth)
{
    const bullet = '*'.repeat(depth)

    if (!isPart(node))
        return [`${bullet} xref:${node.page}[]`]

    const children = node.children.flatMap((child) => renderNav(child, depth + 1))

    if (isGroup(node))
        return (children.length > 0) ? [`${bullet} ${node.title}`, ...children] : []

    return [`${bullet} xref:${node.page}[${node.title}]`, ...children]
}

// Anchor of a heading of a part, given explicitly rather than generated, so
// that the contents of the book can link to it
function anchor(title)
{
    return title.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '')
}

function hasPages(node)
{
    return listPages(node.children).length > 0
}

// Contents of a part: its pages, under the headings the part groups them by
function renderPart(part, top)
{
    const lines = [`= ${part.title}`]

    // Parts of the book itself have their titles set the way the PDF sets them
    if (top)
        lines.push(':page-role: part')

    const item = (node) => isPart(node)
        ? `* xref:${node.page}[${node.title}]`
        : `* xref:${node.page}[]` + (node.summary ? ` - ${node.summary}` : '')

    const loose = part.children.filter((node) => !isGroup(node))

    if (loose.length > 0)
        lines.push('', ...loose.map(item))

    for (const group of part.children.filter((node) => isGroup(node) && hasPages(node)))
        lines.push('', `[#${anchor(group.title)}]`, `== ${group.title}`, '', ...group.children.map(item))

    return lines
}

// Contents of the book: its parts, and the headings of each, the way the
// table of contents of the PDF shows them
function renderBook(title, nodes)
{
    const lines = [`= ${title}`, '']

    for (const node of nodes) {
        if (!isPart(node)) {
            lines.push(`* xref:${node.page}[]`)
            continue
        }

        lines.push(`* xref:${node.page}[${node.title}]`)

        for (const group of node.children.filter((child) => isGroup(child) && hasPages(child)))
            lines.push(`** xref:${node.page}#${anchor(group.title)}[${group.title}]`)
    }

    lines.push('', `This manual is available as a xref:attachment$${pdf}[PDF] too.`)

    return lines
}

// Version of the build, and the version the library is named after, which is
// the minor one before 1.0 and the major one after it, the way CMakeLists.txt
// in the root of the sources sets them
function readVersion()
{
    const cmake   = fs.readFileSync(path.join(root, 'CMakeLists.txt'), 'utf8')
    const version = cmake.match(/^project\([^)]*?\bVERSION\s+((\d+)\.(\d+)\.\d+)/m)

    if (version === null)
        throw new Error('CMakeLists.txt sets no version of the project')

    const [, full, major, minor] = version

    return { version: full, soversion: (major === '0') ? `${major}.${minor}` : major }
}

// Build the book as a PDF, the way the build of the reference manual does
function buildPdf(target, build)
{
    const reference = path.join(root, 'reference')

    fs.mkdirSync(path.dirname(target), { recursive: true })

    execFileSync(process.env.ASCIIDOCTOR_PDF || 'asciidoctor-pdf', [
        '--require', 'asciidoctor-pdf',
        '--require', path.join(reference, 'extensions', 'opendmi-converter.rb'),
        '--backend', 'pdf',
        '--attribute', `pdf-themesdir=${path.join(reference, 'themes')}`,
        '--attribute', `pdf-fontsdir=${path.join(reference, 'fonts')}`,
        '--theme', 'opendmi',
        '--attribute', `release-version=${build.version}`,
        '--attribute', `soversion=${build.soversion}`,
        '--out-file', target,
        path.join(source, 'index.adoc')
    ], { stdio: 'inherit' })
}

// Contributors page of the site, which is the list the repository keeps,
// turned from Markdown into AsciiDoc. The list is made of headings, links and
// nested list items only, which is all that is turned
function renderContributors()
{
    const markdown = fs.readFileSync(path.join(root, 'CONTRIBUTORS.md'), 'utf8')
    const lines    = []

    for (const line of markdown.split('\n')) {
        const heading = line.match(/^(#+) (.+)$/)
        const item    = line.match(/^( *)[*-] (.+)$/)

        // Title of the list names the project, which every page of the site
        // is about anyway
        if (heading && heading[1].length === 1)
            lines.push('= Contributors')
        else if (heading)
            lines.push('='.repeat(heading[1].length) + ' ' + heading[2].replace(/:$/, ''))
        else if (item)
            lines.push('*'.repeat(item[1].length / 2 + 1) + ' ' + item[2])
        else
            lines.push(line)
    }

    return lines.map((line) => line.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '$2[$1]'))
}

// Latest release the changelog records, which is the first version after the
// changes not released yet
function readRelease()
{
    const changelog = fs.readFileSync(path.join(root, 'CHANGELOG.md'), 'utf8')
    const release   = changelog.match(/^## \[(\d+\.\d+\.\d+)\] - (.+)$/m)

    if (release === null)
        throw new Error('CHANGELOG.md records no release')

    return { version: release[1], date: release[2].trim() }
}

function writeLines(file, lines)
{
    fs.mkdirSync(path.dirname(file), { recursive: true })
    fs.writeFileSync(file, lines.join('\n') + '\n')
}

const book  = readPart(path.join(source, 'index.adoc'))
const title = fs.readFileSync(path.join(source, 'index.adoc'), 'utf8').match(/^= (.+)$/m)[1]

fs.rmSync(collected, { recursive: true, force: true })

const pages    = listPages(book)
const manPages = listManPages(pages)

for (const page of pages) {
    const target = path.join(output, 'pages', page.page)
    const text   = fs.readFileSync(page.file, 'utf8')

    fs.mkdirSync(path.dirname(target), { recursive: true })
    fs.writeFileSync(target, linkManPages(text, page.page, manPages))
}

for (const part of listParts(book))
    writeLines(path.join(output, 'pages', part.page), renderPart(part, book.includes(part)))

writeLines(path.join(output, 'pages', 'index.adoc'), renderBook(title, book))
writeLines(path.join(output, 'nav.adoc'), [
    '.Reference manual',
    '* xref:index.adoc[Contents]',
    ...book.flatMap((node) => renderNav(node, 1))
])

const build = readVersion()

buildPdf(path.join(output, 'attachments', pdf), build)

// Attributes of the component, which the collector merges into its descriptor
writeLines(path.join(collected, 'antora.yml'), [JSON.stringify({
    asciidoc: {
        attributes: {
            'release-version': build.version,
            'soversion':       build.soversion
        }
    }
}, null, 2)])

for (const logo of logos) {
    const target = path.join(collected, 'modules', 'ROOT', 'images', logo)

    fs.mkdirSync(path.dirname(target), { recursive: true })
    fs.copyFileSync(path.join(root, 'common', logo), target)
}

writeLines(path.join(collected, 'modules', 'ROOT', 'pages', 'contributors.adoc'), renderContributors())

const release = readRelease()

writeLines(path.join(collected, 'modules', 'ROOT', 'partials', 'release.adoc'), [
    `:latest-release-version: ${release.version}`,
    `:latest-release-date: ${release.date}`
])
