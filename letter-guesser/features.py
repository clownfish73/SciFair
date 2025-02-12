import os
import csv
import math
import random
import numpy as np
from svgpathtools import svg2paths
from shapely.geometry import Polygon, Point
from shapely.ops import unary_union
from shapely import affinity

OUTPUT_DIR = "output_svgs"
LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
HOLE_COUNTS = {'A': 1, 'B': 2, 'D': 1, 'O': 1, 'P': 1, 'Q': 1, 'R': 1}
DEFAULT_HOLE_COUNT = 0
STANDARD_SIZE = 100
SAMPLES_PER_SEGMENT = 50

def count_sides(svg_file):
    try:
        paths, _ = svg2paths(svg_file)
        total = 0
        for path in paths:
            for subpath in path.continuous_subpaths():
                total += len(subpath)
        return total
    except Exception:
        return 0

def normalize_polygon(polygon, standard_size=STANDARD_SIZE):
    minx, miny, maxx, maxy = polygon.bounds
    width = maxx - minx
    height = maxy - miny
    if width == 0 and height == 0:
        return polygon
    scale_factor = standard_size / max(width, height)
    scaled = affinity.scale(polygon, xfact=scale_factor, yfact=scale_factor, origin=(0, 0))
    minx_s, miny_s, maxx_s, maxy_s = scaled.bounds
    return affinity.translate(scaled, xoff=-minx_s, yoff=-miny_s)

def compute_perimeter_and_area(svg_file, samples_per_segment=SAMPLES_PER_SEGMENT):
    try:
        paths, _ = svg2paths(svg_file)
        polygons = []
        for path in paths:
            for subpath in path.continuous_subpaths():
                pts = []
                for segment in subpath:
                    for i in range(samples_per_segment + 1):
                        t = i / samples_per_segment
                        pt = segment.point(t)
                        pts.append((pt.real, pt.imag))
                if len(pts) < 3:
                    continue
                if pts[0] != pts[-1]:
                    pts.append(pts[0])
                poly = Polygon(pts)
                if poly.is_valid and poly.area > 1e-6:
                    polygons.append(poly)
        if not polygons:
            return None, None, None
        unioned = unary_union(polygons)
        if unioned.is_empty:
            return None, None, None
        norm_poly = normalize_polygon(unioned)
        return norm_poly.length, norm_poly.area, norm_poly
    except Exception:
        return None, None, None

def compute_symmetry(svg_file, axis='horizontal', samples=200):
    try:
        paths, _ = svg2paths(svg_file)
        polygons = []
        for path in paths:
            for subpath in path.continuous_subpaths():
                pts = []
                for segment in subpath:
                    pts.extend([(segment.point(t).real, segment.point(t).imag)
                                for t in np.linspace(0, 1, 51)])
                if len(pts) < 3:
                    continue
                if pts[0] != pts[-1]:
                    pts.append(pts[0])
                poly = Polygon(pts)
                if poly.is_valid and poly.area > 1e-6:
                    polygons.append(poly)
        if not polygons:
            return False
        unioned = unary_union(polygons)
        if unioned.is_empty:
            return False
        minx, miny, maxx, maxy = unioned.bounds
        if axis == 'horizontal':
            cx = 0.5 * (minx + maxx)
            reflect = lambda p: Point(2 * cx - p.x, p.y)
        elif axis == 'vertical':
            cy = 0.5 * (miny + maxy)
            reflect = lambda p: Point(p.x, 2 * cy - p.y)
        else:
            return False
        inside = 0
        match = 0
        for _ in range(samples):
            rx = random.uniform(minx, maxx)
            ry = random.uniform(miny, maxy)
            p = Point(rx, ry)
            if unioned.contains(p):
                inside += 1
                if unioned.contains(reflect(p)):
                    match += 1
        if inside == 0:
            return False
        return (match / inside) >= 1.0
    except Exception:
        return False

def compute_normalized_features(svg_file, letter):
    P, A, norm_poly = compute_perimeter_and_area(svg_file)
    if P is None or A is None or norm_poly is None or A <= 0 or P <= 0:
        return None
    P_n = round(P / math.sqrt(A), 3)
    SF = round((4 * math.pi * A) / (P ** 2), 3)
    convex_hull_area = norm_poly.convex_hull.area
    Solidity = round(A / convex_hull_area, 3) if convex_hull_area > 0 else 0
    H = HOLE_COUNTS.get(letter, DEFAULT_HOLE_COUNT)
    S = count_sides(svg_file)
    if norm_poly.geom_type == 'Polygon':
        components = 1
    elif norm_poly.geom_type == 'MultiPolygon':
        components = len(norm_poly.geoms)
    else:
        components = 1
    chi = components - H
    HorizSym = compute_symmetry(svg_file, axis='horizontal')
    VertSym = compute_symmetry(svg_file, axis='vertical')
    return {
        "Letter": letter,
        "P_n": P_n,
        "SF": SF,
        "Solidity": Solidity,
        "H": H,
        "S": S,
        "chi": chi,
        "HorizSym": HorizSym,
        "VertSym": VertSym
    }

def main():
    output_csv = "normalized_metrics.csv"
    fieldnames = ["Letter", "P_n", "SF", "Solidity", "H", "S", "chi", "HorizSym", "VertSym"]
    with open(output_csv, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for letter in LETTERS:
            svg_path = os.path.join(OUTPUT_DIR, f"{letter}.svg")
            if not os.path.isfile(svg_path):
                writer.writerow({
                    "Letter": letter,
                    "P_n": None,
                    "SF": None,
                    "Solidity": None,
                    "H": None,
                    "S": None,
                    "chi": None,
                    "HorizSym": None,
                    "VertSym": None
                })
                continue
            features = compute_normalized_features(svg_path, letter)
            if features is not None:
                writer.writerow(features)
                print(f"Processed {letter}: {features}")
            else:
                writer.writerow({
                    "Letter": letter,
                    "P_n": None,
                    "SF": None,
                    "Solidity": None,
                    "H": None,
                    "S": None,
                    "chi": None,
                    "HorizSym": None,
                    "VertSym": None
                })
                print(f"Failed to process {letter}")
    print(f"Feature extraction complete. True values saved in {output_csv}")

if __name__ == "__main__":
    main()
