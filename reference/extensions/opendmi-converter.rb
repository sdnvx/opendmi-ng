#
# OpenDMI: Cross-platform DMI/SMBIOS framework
# Copyright (c) 2025-2026, The OpenDMI contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Converter of the reference manual into PDF, which opens the parts of the book
# on pages laid out after the logo.
#
# The theme gives the fonts and the colors of the headings, but no layout of
# the page a part opens on, so the part title is inked here: the signifier and
# the number of the part go above its name, lower on the page, and the stripes
# of the logo go below it, broken the way they are in the logo and running off
# the edge of the page.
#
# The legal notice, which is an open block of the legal role, is set in the
# type the theme gives the role and goes to the foot of its page.
#
# The contents list the parts by their signifiers too, e.g. "Part I:
# Introduction" rather than "I: Introduction", the way the headings of the
# parts and the outline of the document name them.
#
# Loaded with the --require option of asciidoctor-pdf.
#
class OpenDMIConverter < (Asciidoctor::Converter.for 'pdf')
  register_for 'pdf'

  # Colors of the stripes of the logo, from the lightest to the darkest
  STRIPE_COLORS = %w(9FE3D4 4FC4B8 1FA0A3 147A8C 0F5470).freeze

  # Geometry of the stripes, in the units of the logo: each stripe is as thick
  # as the step between them, and breaks down into a notch as wide as the
  # break of the logo and as deep as it
  STRIPE_STEP   = 20
  NOTCH_WIDTH   = 120
  NOTCH_DEPTH   = 50

  # Size of the unit of the logo on the page
  STRIPE_SCALE  = 0.4

  # Share of the height of the page the title of a part is inked below
  PART_TITLE_TOP = 0.3

  def ink_part_title node, title, opts = {}
    # Title of a part is its signifier and number, followed by its name, e.g.
    # "Part II: Getting started"
    label, separator, name = title.partition ': '

    return super if separator.empty?

    move_down bounds.height * PART_TITLE_TOP

    theme_font :heading_part_label do
      character_spacing (@theme.heading_part_label_character_spacing || 0) do
        ink_prose label, align: opts[:align], margin: 0
      end
    end

    super node, name, opts

    ink_part_stripes
  end

  def convert_open node
    return super unless node.role? 'legal'

    theme_font :role_legal do
      with_dry_run do |extent|
        if (height = extent&.single_page_height) && (delta = cursor - height - 0.0001) > 0
          move_down delta
        end

        super
      end
    end
  end

  def ink_toc_level entries, *args
    entries.each do |entry|
      next unless entry.context == :section && entry.sectname == 'part'
      next if entry.singleton_class.method_defined? :numbered_title, false

      entry.define_singleton_method :numbered_title do |opts = {}|
        super opts.merge formal: true
      end
    end

    super
  end

  private

  def ink_part_stripes
    top    = y - (@theme.heading_part_stripes_margin_top || 0)
    left   = bounds.absolute_left
    height = (NOTCH_DEPTH + STRIPE_STEP * STRIPE_COLORS.size) * STRIPE_SCALE

    canvas do
      right = bounds.right

      STRIPE_COLORS.each_with_index do |color, index|
        offset = STRIPE_STEP * index

        # Stripe goes down into the notch and back up, then straight on to the
        # edge of the page, as a band of the thickness of a step
        points = [
          [0,               offset],
          [NOTCH_WIDTH / 2, offset + NOTCH_DEPTH],
          [NOTCH_WIDTH,     offset],
          [nil,             offset],
          [nil,             offset + STRIPE_STEP],
          [NOTCH_WIDTH,     offset + STRIPE_STEP],
          [NOTCH_WIDTH / 2, offset + NOTCH_DEPTH + STRIPE_STEP],
          [0,               offset + STRIPE_STEP]
        ].map do |px, py|
          [px ? left + px * STRIPE_SCALE : right, top - py * STRIPE_SCALE]
        end

        fill_color color
        fill_polygon(*points)
      end
    end

    move_down (@theme.heading_part_stripes_margin_top || 0) + height
  end
end
