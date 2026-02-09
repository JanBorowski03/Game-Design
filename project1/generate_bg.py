# Written by Codex
#TODO write a python script that will create 60 copies of the bg file properly numbered with pulsing brightness 

from __future__ import annotations

import math
from pathlib import Path

from PIL import Image, ImageEnhance

SRC = Path("assets/dungeon_bg.jpg")
OUT_DIR = SRC.parent
OUT_PREFIX = "dungeon_bg_"
COUNT = 60
MIN_BRIGHTNESS = 0.85
MAX_BRIGHTNESS = 1.15


def brightness_factor(index: int, total: int) -> float:
    phase = (index / total) * 2.0 * math.pi
    mid = (MIN_BRIGHTNESS + MAX_BRIGHTNESS) / 2.0
    amp = (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 2.0
    return mid + amp * math.sin(phase)


def main() -> None:
    img = Image.open(SRC).convert("RGB")
    for i in range(COUNT):
        factor = brightness_factor(i, COUNT)
        out = ImageEnhance.Brightness(img).enhance(factor)
        out_path = OUT_DIR / f"{OUT_PREFIX}{i:02d}.jpg"
        out.save(out_path, quality=95, optimize=True)


if __name__ == "__main__":
    main()