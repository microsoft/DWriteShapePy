
# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import dwriteshapepy as dw
from pathlib import Path
import pytest

TESTDATA = Path(__file__).parent / "data"
ADOBE_BLANK_TTF_PATH = TESTDATA / "AdobeBlank.subset.met.ttf"

@pytest.fixture
def blankfont():
    """Return a subset of AdobeBlank.ttf containing the following glyphs/characters:
    [
        {gid=0, name=".notdef"},
        {gid=1, name="a", code=0x61},
        {gid=2, name="b", code=0x62},
        {gid=3, name="c", code=0x63},
        {gid=4, name="d", code=0x64},
        {gid=5, name="e", code=0x65},
        {gid=6, name="ccedilla", code=0x62},
        {gid=7, name="uni0431", code=0x0431},  # CYRILLIC SMALL LETTER BE
        {gid=8, name="u1F4A9", code=0x1F4A9},  # PILE OF POO
    ]
    """
    face = dw.Face(ADOBE_BLANK_TTF_PATH.read_bytes())
    font = dw.Font(face)
    upm = font.upem
    print(upm)
    return font

class TestShape:
    @pytest.mark.parametrize(
            "string, expected",
            [
                ("abcde", [(1, 0), (2, 1), (3, 2), (4, 3), (5, 4)]),
                ("abçde", [(1, 0), (2, 1), (6, 2), (4, 3), (5, 4)]),
                ("aбcde", [(1, 0), (7, 1), (3, 2), (4, 3), (5, 4)]),
                ("abc💩e", [(1, 0), (2, 1), (3, 2), (8, 3), (5, 3)]),
            ],
            ids=["ascii", "latin1", "ucs2", "ucs4"],
        )

    def test_add_str_gid_and_cluster_no_features(self, blankfont, string, expected):
        buf = dw.Buffer()
        buf.add_str(string)
        dw.shape(blankfont, buf)
        infos = [(g.codepoint, g.cluster) for g in buf.glyph_infos]
        assert infos == expected

    def test_add_utf8(self, blankfont):
        buf = dw.Buffer()
        buf.add_utf8("aбç💩e".encode("utf-8"))
        dw.shape(blankfont, buf)
        infos = [(g.codepoint, g.cluster) for g in buf.glyph_infos]
        assert infos == [(1, 0), (7, 1), (6, 2), (8, 3), (5, 3)]

    def test_add_codepoints(self, blankfont):
        buf = dw.Buffer()
        buf.add_codepoints([0x61, 0x431, 0xE7, 0x1F4A9, 0x65])
        dw.shape(blankfont, buf)
        infos = [(g.codepoint, g.cluster) for g in buf.glyph_infos]
        assert infos == [(1, 0), (7, 1), (6, 2), (8, 3), (5, 3)]

    def test_positions(self, blankfont):
        string = "abcde"
        expected = []
        buf = dw.Buffer()
        buf.add_str(string)
        dw.shape(blankfont, buf)
        pos = [g.position for g in buf.glyph_positions]
        expected = [(0.0, 0.0, 1100.0, 0.0), (0.0, 0.0, 1200.0, 0.0), (0.0, 0.0, 1300.0, 0.0), (0.0, 0.0, 1400.0, 0.0), (0.0, 0.0, 1500.0, 0.0)]
        assert pos == expected

    @pytest.mark.parametrize(
        "string, expected",
        [
            ("abcde", ["a", "b", "c", "d", "e"]),
            ("abçde", ["a", "b", "ccedilla", "d", "e"]),
            ("aбcde", ["a", "uni0431", "c", "d", "e"]),
            ("abc💩e", ["a", "b", "c", "u1F4A9", "e"]),
        ],
        ids=["ascii", "latin1", "ucs2", "ucs4"],
         )

    def test_glyh_name(self, blankfont, string, expected):
        buf = dw.Buffer()
        buf.add_str(string)
        dw.shape(blankfont, buf)
        # font.glyph_to_string() return "gidN" if the font does not contain glyph names
        # or if the glyph ID does not exist.
        glyph_names = [blankfont.glyph_to_string(g.codepoint) for g in buf.glyph_infos]
        assert glyph_names == expected
        assert blankfont.glyph_to_string(1000) == 'gid1000'

    @pytest.mark.parametrize(
        "string, features, expected",
        [
            # The calt feature replaces c by a in the context e, d, c', b, a.
            ("edcbaedcba", {}, ["e", "d", "a", "b", "a", "e", "d", "a", "b", "a"]),
            ("edcbaedcba", {"calt": True, "calt[2]": False}, ["e", "d", "c", "b", "a", "e", "d", "a", "b", "a"]),
            ("edcbaedcba", {"calt": [(0, 10, True), (7, 8, False)]}, ["e", "d", "a", "b", "a", "e", "d", "c", "b", "a"]),
            ("edcbaedcba", {"calt": [(0, 10, False), (7, 8, True)]}, ["e", "d", "c", "b", "a", "e", "d", "a", "b", "a"]),
        ],
    )

    def test_features_slice(self, blankfont, string, features, expected):
        buf = dw.Buffer()
        buf.add_str(string)
        dw.shape(blankfont, buf, features)
        glyph_names = [blankfont.glyph_to_string(g.codepoint) for g in buf.glyph_infos]
        assert glyph_names == expected