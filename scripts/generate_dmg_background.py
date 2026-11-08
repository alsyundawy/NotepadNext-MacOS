#!/usr/bin/env python3
"""
Generate high-DPI background images for NotepadNext macOS DMG installer.
Creates both standard (680x440) and Retina @2x (1360x880) PNG assets.
"""

import math
import os

from PIL import Image, ImageDraw, ImageFont


def draw_bezier_curve(draw, p0, p1, p2, width, color, steps=100):
    """Draw a smooth quadratic bezier curve."""
    points = []
    for i in range(steps + 1):
        t = i / float(steps)
        # B(t) = (1-t)^2 * P0 + 2*(1-t)*t * P1 + t^2 * P2
        x = (1 - t) ** 2 * p0[0] + 2 * (1 - t) * t * p1[0] + (t**2) * p2[0]
        y = (1 - t) ** 2 * p0[1] + 2 * (1 - t) * t * p1[1] + (t**2) * p2[1]
        points.append((x, y))

    for i in range(len(points) - 1):
        draw.line([points[i], points[i + 1]], fill=color, width=width)
    return points[-1]


def draw_arrow_head(draw, tip, angle_deg, size, color):
    """Draw an arrowhead pointing in the specified angle."""
    rad = math.radians(angle_deg)
    left_angle = rad + math.radians(150)
    right_angle = rad - math.radians(150)

    p1 = (tip[0] + size * math.cos(left_angle), tip[1] + size * math.sin(left_angle))
    p2 = (tip[0] + size * math.cos(right_angle), tip[1] + size * math.sin(right_angle))

    draw.polygon([tip, p1, p2], fill=color)


def get_system_font(size, bold=False):
    """Locate appropriate system font."""
    font_candidates = [
        "/System/Library/Fonts/SFPro.ttf",
        (
            "/System/Library/Fonts/SFProDisplay-Bold.otf"
            if bold
            else "/System/Library/Fonts/SFProDisplay-Regular.otf"
        ),
        "/System/Library/Fonts/HelveticaNeue.ttc",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
    ]
    for path in font_candidates:
        if os.path.exists(path):
            try:
                idx = 1 if (bold and "Helvetica" in path) else 0
                return ImageFont.truetype(path, size, index=idx)
            except (OSError, ValueError):
                pass
    return ImageFont.load_default()


def create_dmg_background(scale=2, output_path="deploy/macos/dmg-background@2x.png"):
    base_w, base_h = 680, 440
    width = base_w * scale
    height = base_h * scale

    # Base image with crisp white
    img = Image.new("RGBA", (width, height), (255, 255, 255, 255))
    draw = ImageDraw.Draw(img)

    # 1. Background gradient (pure crisp white into very subtle mac aesthetic)
    top_color = (248, 250, 252, 255)
    bottom_color = (255, 255, 255, 255)
    for y in range(height):
        ratio = y / float(height)
        r = int(top_color[0] + (bottom_color[0] - top_color[0]) * ratio)
        g = int(top_color[1] + (bottom_color[1] - top_color[1]) * ratio)
        b = int(top_color[2] + (bottom_color[2] - top_color[2]) * ratio)
        draw.line([(0, y), (width, y)], fill=(r, g, b, 255))

    # 2. Modern subtle geometric polygonal accents (cool slate & blue)
    poly_overlay = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    poly_draw = ImageDraw.Draw(poly_overlay)

    poly1 = [
        (0, 0),
        (int(290 * scale), 0),
        (int(360 * scale), int(170 * scale)),
        (int(160 * scale), int(390 * scale)),
        (0, int(300 * scale)),
    ]
    poly_draw.polygon(poly1, fill=(225, 238, 250, 70))

    poly2 = [
        (int(260 * scale), 0),
        (int(470 * scale), 0),
        (int(420 * scale), int(250 * scale)),
        (int(200 * scale), int(170 * scale)),
    ]
    poly_draw.polygon(poly2, fill=(215, 230, 246, 55))

    poly3 = [
        (int(440 * scale), 0),
        (width, 0),
        (width, int(320 * scale)),
        (int(490 * scale), int(280 * scale)),
    ]
    poly_draw.polygon(poly3, fill=(228, 240, 252, 65))

    poly4 = [
        (0, int(260 * scale)),
        (int(220 * scale), int(360 * scale)),
        (int(440 * scale), int(430 * scale)),
        (width, int(370 * scale)),
        (width, height),
        (0, height),
    ]
    poly_draw.polygon(poly4, fill=(240, 246, 254, 85))

    # Composite geometric polygons
    img = Image.alpha_composite(img, poly_overlay)
    draw = ImageDraw.Draw(img)

    # 3. Title & Tagline in Header Area
    # Title: "NotepadNext"
    title_font = get_system_font(int(26 * scale), bold=True)
    title_text = "NotepadNext"
    title_bbox = draw.textbbox((0, 0), title_text, font=title_font)
    title_w = title_bbox[2] - title_bbox[0]
    title_x = int(base_w * scale / 2 - title_w / 2)
    title_y = int(28 * scale)
    draw.text((title_x, title_y), title_text, fill=(30, 41, 59, 255), font=title_font)

    # Tagline: "Lightweight, blazing-fast native Scintilla & Qt text and source code editor for macOS."
    tagline_font = get_system_font(int(11.5 * scale), bold=False)
    tagline_text = "Lightweight, blazing-fast native Scintilla & Qt text and source code editor for macOS."
    tagline_bbox = draw.textbbox((0, 0), tagline_text, font=tagline_font)
    tagline_w = tagline_bbox[2] - tagline_bbox[0]
    tagline_x = int(base_w * scale / 2 - tagline_w / 2)
    tagline_y = int(66 * scale)
    draw.text(
        (tagline_x, tagline_y), tagline_text, fill=(71, 85, 105, 235), font=tagline_font
    )

    # Subtle horizontal divider line
    div_y = int(92 * scale)
    draw.line(
        [(int(60 * scale), div_y), (int((base_w - 60) * scale), div_y)],
        fill=(226, 232, 240, 200),
        width=max(1, int(1 * scale)),
    )

    # 4. Icon Guide Glows (Left: NotepadNext.app, Right: Applications)
    pedestal_r = int(76 * scale)
    left_center = (int(170 * scale), int(245 * scale))
    right_center = (int(510 * scale), int(245 * scale))

    halo = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    halo_draw = ImageDraw.Draw(halo)
    halo_draw.ellipse(
        [
            left_center[0] - pedestal_r,
            left_center[1] - pedestal_r,
            left_center[0] + pedestal_r,
            left_center[1] + pedestal_r,
        ],
        fill=(255, 255, 255, 140),
    )
    halo_draw.ellipse(
        [
            right_center[0] - pedestal_r,
            right_center[1] - pedestal_r,
            right_center[0] + pedestal_r,
            right_center[1] + pedestal_r,
        ],
        fill=(255, 255, 255, 140),
    )
    img = Image.alpha_composite(img, halo)
    draw = ImageDraw.Draw(img)

    # 5. Drag and Drop curved arrow
    # Curves from ~255 to ~425
    arrow_color = (30, 41, 59, 230)
    p0 = (int(255 * scale), int(260 * scale))
    p1 = (int(340 * scale), int(230 * scale))  # curve peak
    p2 = (int(425 * scale), int(260 * scale))

    curve_w = max(2, int(3.5 * scale))
    draw_bezier_curve(draw, p0, p1, p2, curve_w, arrow_color, steps=80)

    # Tangent at p2 for arrowhead
    t = 0.98
    x_prev = (1 - t) ** 2 * p0[0] + 2 * (1 - t) * t * p1[0] + (t**2) * p2[0]
    y_prev = (1 - t) ** 2 * p0[1] + 2 * (1 - t) * t * p1[1] + (t**2) * p2[1]
    angle = math.degrees(math.atan2(p2[1] - y_prev, p2[0] - x_prev))
    head_size = int(14 * scale)
    draw_arrow_head(draw, p2, angle, head_size, arrow_color)

    # 6. "drag and drop" text
    dnd_font = get_system_font(int(16 * scale), bold=False)
    dnd_text = "drag and drop"
    dnd_bbox = draw.textbbox((0, 0), dnd_text, font=dnd_font)
    dnd_w = dnd_bbox[2] - dnd_bbox[0]
    dnd_x = int(340 * scale - dnd_w / 2)
    dnd_y = int(200 * scale)
    draw.text((dnd_x, dnd_y), dnd_text, fill=(30, 41, 59, 235), font=dnd_font)

    # Ensure output directory exists and save
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    img.save(output_path, "PNG", optimize=True)
    print(f"Generated {output_path} ({width}x{height})")


if __name__ == "__main__":
    create_dmg_background(scale=1, output_path="deploy/macos/dmg-background.png")
    create_dmg_background(scale=2, output_path="deploy/macos/dmg-background@2x.png")
