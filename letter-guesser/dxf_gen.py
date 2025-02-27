import os
import sys
import ezdxf
from fontTools.ttLib import TTFont

# --------------------------- Configuration Variables --------------------------- #
FONT_PATH = "/path2font"
OUTPUT_DIR = "output_dxfs"
LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
CANVAS_SIZE_MM = 100  
PADDING = 10
# --------------------------------------------------------------------------------- #

def extract_and_save_glyphs_as_dxf(font_path, output_dir, letters, canvas_size, padding=10):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    try:
        font = TTFont(font_path)
    except Exception as e:
        print(f"Error loading font '{font_path}': {e}")
        sys.exit(1)

    cmap = font["cmap"].getBestCmap()
    glyf_table = font["glyf"]

    for char in letters:
        try:
            unicode_val = ord(char)
            glyph_name = cmap.get(unicode_val)
            if not glyph_name:
                print(f"Character '{char}' not found in the font.")
                continue

            glyph = glyf_table[glyph_name]
            if glyph.isComposite():
                print(f"Skipping composite glyph for '{char}'")
                continue

            xMin, yMin, xMax, yMax = glyph.xMin, glyph.yMin, glyph.xMax, glyph.yMax
            width = xMax - xMin
            height = yMax - yMin

            # Calculate scaling factors to fit within the canvas (with padding)
            scale_x = (canvas_size - 2 * padding) / width if width else 1.0
            scale_y = (canvas_size - 2 * padding) / height if height else 1.0
            scale_factor = min(scale_x, scale_y)

            # After scaling, center the glyph within the canvas
            scaled_width = width * scale_factor
            scaled_height = height * scale_factor
            translate_x = (canvas_size - scaled_width) / 2 - xMin * scale_factor
            translate_y = (canvas_size - scaled_height) / 2 - yMin * scale_factor

            # Extract contours directly from the glyph coordinates
            # The glyph.coordinates is a list of (x, y) pairs and glyph.endPtsOfContours marks the contour boundaries
            coords = glyph.coordinates
            end_pts = glyph.endPtsOfContours
            start = 0
            contours = []
            for end in end_pts:
                contour = []
                for i in range(start, end + 1):
                    x, y = coords[i]
                    x = x * scale_factor + translate_x
                    y = y * scale_factor + translate_y
                    contour.append((x, y))
                # Ensure the contour is closed (first point == last point)
                if contour[0] != contour[-1]:
                    contour.append(contour[0])
                contours.append(contour)
                start = end + 1

            doc = ezdxf.new(dxfversion='R2010')
            # Set drawing units to millimeters
            doc.header['$INSUNITS'] = 4  # 4 corresponds to millimeters
            msp = doc.modelspace()
            for contour in contours:
                msp.add_lwpolyline(contour, close=True)

            dxf_filename = os.path.join(output_dir, f"{char}.dxf")
            doc.saveas(dxf_filename)
            print(f"Saved DXF for letter '{char}' to '{dxf_filename}'")

        except Exception as e:
            print(f"Error processing letter '{char}': {e}")
            continue

if __name__ == "__main__":
    extract_and_save_glyphs_as_dxf(
        font_path=FONT_PATH,
        output_dir=OUTPUT_DIR,
        letters=LETTERS,
        canvas_size=CANVAS_SIZE_MM,
        padding=PADDING
    )
