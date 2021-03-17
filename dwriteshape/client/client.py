
# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

if __name__ == '__main__':
    import sys
    import dwriteshape as dw 
    from pathlib import Path

    print('DWrite Shape Test Client')

    TESTDATA = Path(__file__).parent / "data"
    ADOBE_BLANK_TTF_PATH = TESTDATA / "AdobeBlank.subset.met.ttf"

    face = dw.Face(ADOBE_BLANK_TTF_PATH.read_bytes())
    font = dw.Font(face)
    upm = font.upem
    print(upm)

    buf = dw.Buffer()
    buf.add_utf8("aбç💩e".encode("utf-8"))
    dw.shape(font, buf)
    glyph_names = [font.glyph_to_string(g.codepoint) for g in buf.glyph_infos]
    infos = [(g.codepoint, g.cluster) for g in buf.glyph_infos]
    print(glyph_names)
    print(infos)

    advance = [(adv.x_advance, adv.y_advance, adv.x_offset, adv.y_offset) for adv in buf.glyph_positions]
    print(advance)

    infos = buf.glyph_infos
    positions = buf.glyph_positions

    for info, pos in zip(infos, positions):
        gid = info.codepoint
        cluster = info.cluster
        x_advance = pos.x_advance
        x_offset = pos.x_offset
        y_offset = pos.y_offset
        print(f"gid{gid}={cluster}@{x_advance},{x_offset}+{y_offset}")
  

    print('Success')