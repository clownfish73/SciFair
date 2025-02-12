import os
import sys
import xml.etree.ElementTree as ET

from fontTools.pens.svgPathPen import SVGPathPen
from fontTools.ttLib import TTFont

# --------------------------- Configuration Variables --------------------------- #

FONT_PATH = "/Users/rafi/PycharmProjects/trees-are-real/downloaded_fonts/Press Start 2P.ttf"
OUTPUT_DIR = "output_svgs"
LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
SVG_WIDTH = 100  # Desired SVG canvas width
SVG_HEIGHT = 100  # Desired SVG canvas height
PADDING = 10  # Optional padding around the glyph

# --------------------------------------------------------------------------------- #


def extract_and_save_glyphs_auto_scale(font_path, output_dir, letters, svg_width, svg_height, padding=10):
    """Extract glyphs from a font, automatically scale them to fit the SVG canvas, and save as SVG files."""
    # Ensure output directory exists
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Load the font
    try:
        font = TTFont(font_path)
    except Exception as e:
        print(f"Error loading font '{font_path}': {e}")
        sys.exit(1)

    cmap = font["cmap"].getBestCmap()
    glyph_set = font.getGlyphSet()

    for char in letters:
        try:
            unicode_val = ord(char)
            glyph_name = cmap.get(unicode_val)
            if not glyph_name:
                print(f"Character '{char}' not found in the font.")
                continue
            glyph = glyph_set[glyph_name]

            # Create SVG path commands
            pen = SVGPathPen(glyph_set)
            glyph.draw(pen)
            path_commands = pen.getCommands()

            # Get glyph metrics
            glyph_metrics = font["glyf"][glyph_name]
            xMin, yMin, xMax, yMax = glyph_metrics.xMin, glyph_metrics.yMin, glyph_metrics.xMax, glyph_metrics.yMax
            width = xMax - xMin
            height = yMax - yMin

            # Calculate scaling factor to fit glyph within SVG canvas with padding
            scale_x = (svg_width - 2 * padding) / width if width != 0 else 1.0
            scale_y = (svg_height - 2 * padding) / height if height != 0 else 1.0
            scale_factor = min(scale_x, scale_y)

            # After scaling, recalculate glyph dimensions
            scaled_width = width * scale_factor
            scaled_height = height * scale_factor

            # Calculate translation to center the glyph within the SVG canvas
            translate_x = (svg_width - scaled_width) / 2 - xMin * scale_factor
            translate_y = (svg_height + scaled_height) / 2 + yMin * scale_factor  # + because y-axis is flipped

            # Define SVG element
            svg = ET.Element(
                "svg",
                xmlns="http://www.w3.org/2000/svg", # litterally copied from w3
                version="1.1",
                width=f"{svg_width}px",
                height=f"{svg_height}px",
                viewBox=f"0 0 {svg_width} {svg_height}"
            )

            # Define transformation: scale and translate
            transform = f"translate({translate_x}, {translate_y}) scale({scale_factor}, -{scale_factor})"
            path = ET.SubElement(svg, "path", d=path_commands, fill="black", transform=transform)

            # Save SVG to file
            svg_filename = os.path.join(output_dir, f"{char}.svg")
            ET.ElementTree(svg).write(svg_filename)
            print(f"Successfully processed and saved letter: '{char}' as '{svg_filename}'")

        except Exception as e:
            print(f"Error processing letter '{char}': {e}")
            continue


if __name__ == "__main__":
    extract_and_save_glyphs_auto_scale(
        font_path=FONT_PATH,
        output_dir=OUTPUT_DIR,
        letters=LETTERS,
        svg_width=SVG_WIDTH,
        svg_height=SVG_HEIGHT,
        padding=PADDING
    )
