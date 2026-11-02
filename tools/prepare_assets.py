"""Key near-black to alpha and pad the menu icon to 25x25."""
from __future__ import annotations

from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
IMG = ROOT / "resources" / "images"
LAYERS = IMG / "layers"


def key_black(path: Path, threshold: int = 20) -> None:
    im = Image.open(path).convert("RGBA")
    px = im.load()
    w, h = im.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a > 0 and r <= threshold and g <= threshold and b <= threshold:
                px[x, y] = (0, 0, 0, 0)
    im.save(path)
    print(f"keyed {path.name} {w}x{h}")


def pad_menu(src: Path, dest: Path, size: int = 25) -> None:
    im = Image.open(src).convert("RGBA")
    out = Image.new("RGBA", (size, size), (5, 4, 10, 255))
    out.paste(im, (0, 0), im)
    out.save(dest)
    print(f"menu {dest.name} {size}x{size}")


def main() -> None:
    key_black(LAYERS / "mid_skyline.png", 22)
    key_black(LAYERS / "fg_rooftops.png", 22)
    raw = IMG / "menu_icon_raw.png"
    if raw.exists():
        pad_menu(raw, IMG / "menu_icon.png", 25)
        raw.unlink()
    for p in sorted(IMG.rglob("*.png")):
        im = Image.open(p)
        print(f"  {p.relative_to(IMG)} {im.size[0]}x{im.size[1]} mode={im.mode}")


if __name__ == "__main__":
    main()
