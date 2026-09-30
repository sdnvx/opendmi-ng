#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Links the references to the manual pages in the reference manual.
#
# Manual pages refer to each other the way manual pages do, e.g.
# `dmi_open`(3), which the book turns into links to the pages it includes, as
# the documents of the book are included. Pages of the book are named without
# the sections, the way their titles name them, and the references of a page to
# itself are not linked. The manual pages the book does not include, e.g. the
# ones of the system, are left as they are. The site links the pages the same
# way, see website/collect.js.
#
# A manual page describing several functions or types is referred to by the
# name of each: its other names are symbolic links to its source, which are
# looked up next to it.
#
# Loaded with the --require option of asciidoctor-pdf.
#
require 'asciidoctor/extensions'

class OpenDMIManPages < Asciidoctor::Extensions::IncludeProcessor
  # Reference to a manual page, e.g. `dmi_open`(3)
  REFERENCE = /`([\w.-]+)`\((\d\w*)\)/

  # Source of a manual page, e.g. dmi_open.3.adoc
  SOURCE = /\A(.+)\.(\d\w*)\.adoc\z/

  # Document a book or a part includes
  INCLUDE = /^include::([^\[]+\.adoc)\[.*\]\s*$/

  # Only the documents are included this way, while the files of other kinds,
  # e.g. the license the legal notice is made of, are included for their text,
  # with the lines they are included by
  def handles? target
    target.end_with? '.adoc'
  end

  def process doc, reader, target, attributes
    file = doc.normalize_system_path target, reader.dir, nil, target_name: 'include file'

    unless ::File.file? file
      logger.error message_with_context %(include file not found: #{file}), source_location: reader.cursor
      return
    end

    pages = (@pages ||= list_pages doc.attr 'docfile')
    path  = ::File.realpath file
    text  = link_pages (::File.read file, mode: 'r:UTF-8'), path, pages

    # Title of a manual page is the anchor the references to it link to
    text = text.gsub(/^= /) { %([##{page_id path}]\n= ) } if pages.value? path

    reader.push_include text, file, (doc.path_resolver.relative_path file, doc.base_dir), 1, attributes
  end

  private

  # Sources of the manual pages the book includes, by the names and sections
  # the manual pages are referred to by, e.g. dmi_open(3)
  def list_pages book
    included = list_includes book
    pages    = {}

    included.map {|file| ::File.dirname file }.uniq.each do |dir|
      ::Dir.each_child dir do |entry|
        next unless (name = SOURCE.match entry) && (::File.exist? (::File.join dir, entry))

        path = ::File.realpath ::File.join dir, entry
        pages[%(#{name[1]}(#{name[2]}))] = path if included.include? path
      end
    end

    pages
  end

  # Documents a book includes, and the ones they include in turn
  def list_includes file
    ::File.foreach(file, mode: 'r:UTF-8').flat_map do |line|
      next [] unless (include = INCLUDE.match line)

      path = ::File.realpath ::File.expand_path include[1], (::File.dirname file)
      [path, *(list_includes path)]
    end
  end

  def link_pages text, path, pages
    text.gsub REFERENCE do
      ref, name, section = $&, $1, $2
      target = pages[%(#{name}(#{section}))]

      if target.nil?
        ref
      elsif target == path
        %(`#{name}`)
      else
        %(xref:#{page_id target}[`#{name}`])
      end
    end
  end

  # Anchor of a manual page, e.g. man-dmi_open-3
  def page_id path
    %(man-#{(::File.basename path, '.adoc').tr '.', '-'})
  end
end

Asciidoctor::Extensions.register do
  include_processor OpenDMIManPages
end
