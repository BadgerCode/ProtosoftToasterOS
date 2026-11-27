"""Draws src/guides/images/power-splitting.svg, the power-splitting diagram in the
Toaster OS "Setting up" guide (Power section).

Run it from anywhere with `python scripts/power-splitting.py`. It needs only Python 3.
Change the parts list or the layout numbers below, then run it again.
"""
from pathlib import Path

OUTPUT = Path(__file__).resolve().parent.parent / 'src' / 'guides' / 'images' / 'power-splitting.svg'

W, H = 1000, 620
BG = '#120e1a'; SURFACE = '#1c1726'; BORDER = '#3f3a4d'; TEXT = '#f1f2f6'; MUTED = '#9a93aa'
RED = '#e5484d'; BLACK = '#111014'; CASING = '#7b748d'; ACCENT = '#3d8bff'
FONT = "system-ui, -apple-system, 'Segoe UI', Roboto, sans-serif"
out = []
def add(s): out.append(s)
def text(x, y, s, size=15, fill=TEXT, weight=400, anchor='start'):
    add(f'<text x="{x}" y="{y}" font-size="{size}" font-weight="{weight}" fill="{fill}" text-anchor="{anchor}">{s}</text>')
def box(x, y, w, h, r=10, fill=SURFACE, stroke=BORDER, sw=2):
    add(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{r}" fill="{fill}" stroke="{stroke}" stroke-width="{sw}"/>')
def wire(d, colour):
    if colour == BLACK:  # Ground: black, with a light casing so it shows on the dark background
        add(f'<path d="{d}" fill="none" stroke="{CASING}" stroke-width="7" stroke-linecap="round"/>')
        add(f'<path d="{d}" fill="none" stroke="{BLACK}" stroke-width="4" stroke-linecap="round"/>')
    else:
        add(f'<path d="{d}" fill="none" stroke="{colour}" stroke-width="5" stroke-linecap="round"/>')
def curve(x1, y1, x2, y2):
    mx = (x1 + x2) / 2
    return f'M {x1} {y1} C {mx} {y1}, {mx} {y2}, {x2} {y2}'

add(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" font-family="{FONT}">')
add(f'<rect width="{W}" height="{H}" rx="16" fill="{BG}"/>')

# --- Power bank -------------------------------------------------------------
pbx, pby = 40, 230
box(pbx, pby, 130, 170, r=18)
add(f'<rect x="{pbx+40}" y="{pby+40}" width="50" height="80" rx="6" fill="none" stroke="{MUTED}" stroke-width="3"/>')
add(f'<rect x="{pbx+57}" y="{pby+33}" width="16" height="8" rx="2" fill="{MUTED}"/>')
for i in range(3):
    add(f'<rect x="{pbx+46}" y="{pby+98 - i*22}" width="38" height="16" rx="3" fill="#2fcd03"/>')
text(pbx + 65, pby + 150, 'Power bank', 15, TEXT, 600, 'middle')
# USB port + plug
add(f'<rect x="{pbx+130}" y="{pby+75}" width="22" height="20" rx="3" fill="{MUTED}"/>')

# --- USB cable to the cut end ------------------------------------------------
cable_y = pby + 85
add(f'<path d="M {pbx+152} {cable_y} H 300" stroke="#4a4458" stroke-width="16" stroke-linecap="round" fill="none"/>')
add(f'<path d="M {pbx+152} {cable_y} H 300" stroke="#2b2735" stroke-width="10" stroke-linecap="round" fill="none"/>')
text(235, cable_y - 26, 'USB cable', 15, TEXT, 600, 'middle')
text(235, cable_y + 40, 'End cut off, or a', 13, MUTED, 400, 'middle')
text(235, cable_y + 58, 'cable with bare wires', 13, MUTED, 400, 'middle')

# Exposed wires from the cut end to the splitter
split_x = 420
v_in_y, g_in_y = 150, 430
wire(curve(300, cable_y - 6, split_x + 8, v_in_y - 38), RED)
wire(curve(300, cable_y + 6, split_x + 8, g_in_y - 38), BLACK)
text(352, 205, '5V (red)', 15, RED, 700, 'end')
text(385, 412, 'Ground (black)', 15, TEXT, 700, 'end')

# --- Splitter: two lever-connector blocks ----------------------------------
def splitter(y, label, colour):
    box(split_x, y - 60, 70, 150, r=8, fill='#e8e6ee', stroke='#bdb7ca')
    for i in range(5):  # Five slots: one in, four out (like a WAGO 221-415)
        ly = y - 48 + i * 28
        add(f'<rect x="{split_x+44}" y="{ly}" width="18" height="20" rx="3" fill="{colour}"/>')
        add(f'<rect x="{split_x+8}" y="{ly+6}" width="30" height="8" rx="2" fill="#bdb7ca"/>')
    text(split_x + 35, y - 72, label, 14, TEXT, 700, 'middle')
    text(split_x + 78, y - 33, 'in', 12, MUTED, 600)
splitter(v_in_y, '5V splitter', '#f3a5a7')
splitter(g_in_y, 'Ground splitter', '#5a546b')
text(split_x + 35, 560, 'Splitters, e.g. WAGO lever', 13, MUTED, 400, 'middle')
text(split_x + 35, 578, 'connectors or JST Y cables', 13, MUTED, 400, 'middle')

# --- Parts ------------------------------------------------------------------
parts = [
    ('Microcontroller', '5V and GND pins'),
    ('LED panels', 'VCC and GND of the first panel'),
    ('Side LED strips', '5V and GND of the strips'),
    ('Fan', 'Red and black wires'),
]
px, pw, ph, gap = 680, 280, 80, 36
top = (H - (len(parts) * ph + (len(parts) - 1) * gap)) / 2
for i, (name, pins) in enumerate(parts):
    y = top + i * (ph + gap)
    box(px, y, pw, ph)
    text(px + 22, y + 34, name, 16, TEXT, 600)
    text(px + 22, y + 56, pins, 13, MUTED)
    vy, gy = y + 24, y + 56
    # Two terminals on the left edge
    add(f'<circle cx="{px}" cy="{vy}" r="5" fill="{RED}"/>')
    add(f'<circle cx="{px}" cy="{gy}" r="5" fill="{CASING}"/>')
    # Outputs from each splitter, spread over the blocks' slots
    out_v = v_in_y - 10 + i * 28
    out_g = g_in_y - 10 + i * 28
    wire(curve(split_x + 70, out_v, px, vy), RED)
    wire(curve(split_x + 70, out_g, px, gy), BLACK)

text(px + pw / 2, top - 18, 'Every part gets its own 5V and ground', 13, MUTED, 400, 'middle')
text(px + pw / 2, top + 4 * ph + 3 * gap + 30, 'Add more outputs for other parts', 13, MUTED, 400, 'middle')
add('</svg>')
OUTPUT.write_text('\n'.join(out) + '\n', encoding='utf-8')
print(f'Wrote {OUTPUT}')
