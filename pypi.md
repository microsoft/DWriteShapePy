# Project

DWriteShapePy is a Python extension built using Cython. It provides streamlined bindings for
the DirectWrite shaping engine. The model is similar to the model provided by uHarfBuzz but some 
differences because of differences in the API models between DirectWrite and HarfBuzz. 

DWriteShapePy was initially developed to support a Python based font testing environment where similar tests are 
executed using both DirectWrite and HarfBuzz. 

### Example

```python
import sys
import dwriteshapepy as dw 
from pathlib import Path

with open(sys.argv[1], 'rb') as fontfile:
    fontdata = fontfile.read()

text = sys.argv[2]

face = dw.Face(fontdata)
font = dw.Font(face)
upm = font.upem
print(upm)

buf = dw.Buffer()
buf.add_str(text)
    
features = {"kern": True, "liga": True}
dw.shape(font, buf, features)
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

```

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.
