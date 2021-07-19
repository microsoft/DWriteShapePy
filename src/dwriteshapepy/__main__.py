import dwriteshapepy as dw

import argparse
import re
import sys


def main(args=None):
    if args is None:
        args = sys.argv[1:]
    parser = argparse.ArgumentParser()

    fgroup = parser.add_argument_group("Font options")
    fgroup.add_argument("--font-file", metavar="filename", help="Set font file-name")
    fgroup.add_argument(
        "--font-index",
        metavar="index",
        help="Set face index (default: 0)",
        type=int,
        default=0,
    )
    fgroup.add_argument(
        "--font-size",
        metavar="1/2 integers or 'upem'",
        help="Font size (default: upem)",
        default="upem",
    )

    vgroup = parser.add_argument_group("Variations options")
    vgroup.add_argument(
        "--variations", metavar="list", help="Comma-separated list of font variations"
    )

    tgroup = parser.add_argument_group("Text options")
    tgroup.add_argument("--text", metavar="string", help="Set input text")
    tgroup.add_argument(
        "--text-file", metavar="filename", help="Set input text file-name"
    )
    tgroup.add_argument(
        "-u",
        "--unicodes",
        metavar="list of hex numbers",
        help="Set input Unicode codepoints",
    )

    sgroup = parser.add_argument_group("Shape options")
    # sgroup.add_argument(
    #     "--direction", metavar="ltr/rtl/ttb/btt", help="Set text direction (default: auto)"
    # )
    sgroup.add_argument(
        "--language", metavar="langstr", help="Set text language (default: $LANG)"
    )
    # sgroup.add_argument(
    #     "--script", metavar="ISO-15924 tag", help="Set text script (default: auto)"
    # )

    ffgroup = parser.add_argument_group("Feature options")
    ffgroup.add_argument(
        "--features", metavar="list", help="Comma-separated list of font features"
    )

    osgroup = parser.add_argument_group("Output syntax options")
    osgroup.add_argument(
        "--no-glyph-names",
        help="Output glyph indices instead of names",
        dest="glyph_names",
        action="store_false",
    )
    osgroup.add_argument(
        "--no-positions",
        help="Do not output glyph positions",
        dest="positions",
        action="store_false",
    )
    osgroup.add_argument(
        "--no-advances",
        help="Do not output glyph advances",
        dest="advances",
        action="store_false",
    )
    osgroup.add_argument(
        "--no-clusters",
        help="Do not output cluster indices",
        dest="clusters",
        action="store_false",
    )
    parser.add_argument(dest="font_file", metavar="FONT-FILE", nargs="?")
    parser.add_argument(dest="text", metavar="TEXT", nargs="?")

    args = parser.parse_args(args)

    if args.text:
        text = args.text
    elif args.text_file:
        with open(args.text_file, "rb") as textfile:
            text = textfile.read()
    elif args.unicodes:
        codes = re.split(r"[,\s]", args.unicodes)
        text = "".join([chr(int(c, 16)) for c in codes])

    with open(args.font_file, "rb") as fontfile:
        fontdata = fontfile.read()

    face = dw.Face(fontdata)
    font = dw.Font(face)
    if args.font_size != "upem":
        sizes = re.split(r"[,\s]", args.font_size)
        font.upem = sizes[0]

    buf = dw.Buffer()
    buf.add_str(text)
    if args.language:
        buf.language = args.language

    if args.variations:
        axes = re.split(r"\s*,\s*", args.variations)
        variations = {}
        for axis in axes:
            axis_tag, value = axis.split("=")
            variations[axis_tag] = float(value)
        font.set_variations(variations)

    feature_dict = {}

    if args.features:
        features = re.split(r"\s*,\s*", args.features)
        for f in features:
            m = re.match(r"([+-]?)(\w+)(\[[\d:]+\])?(?:=(\d+))?", f)
            tag = m[2]
            indices = m[3]
            value = m[4]
            if indices:
                if not tag in feature_dict:
                    feature_dict[tag] = []
                l, r = re.match(r"^(\d+)?:?(\d+)?$", indices)
                if value:
                    value = int(value)
                feature_dict[tag].append((int(l or 0), int(r or -1), value or True))
            elif value is not None:
                feature_dict[tag] = int(value)
            elif m[1] == "-":
                feature_dict[tag] = False
            else:
                feature_dict[tag] = True

    dw.shape(font, buf, feature_dict)

    glyphs = []
    for info, pos in zip(buf.glyph_infos, buf.glyph_positions):
        thisglyph = ""
        if args.glyph_names:
            thisglyph += font.glyph_to_string(info.codepoint)
        else:
            thisglyph += str(info.codepoint)
        if args.clusters:
            thisglyph += f"={info.cluster}"
        if args.positions and (pos.x_offset or pos.y_offset):
            thisglyph += f"@{pos.x_offset},{pos.y_offset}"
        if args.advances:
            thisglyph += f"+{pos.x_advance}"
        glyphs.append(thisglyph)

    print("[" + "|".join(glyphs) + "]")


if __name__ == "__main__":
    sys.exit(main())
