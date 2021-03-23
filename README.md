# Project

DWriteShapePy is a Python extension built using Cython. It provides streamlined bindings for
the DirectWrite shaping engine. The model is similar to the model provided by uHarfBuzz but some 
differences because of differences in the API models between DirectWrite and HarfBuzz. 

DWriteShapePy was initially developed to support a Python based font testing environment where similar tests are 
executed using both DirectWrite and HarfBuzz. 

The repo contains two methods to build the extension. The standard Python/Cython method using setup.py and a 
Visual Studio solution. Setup.py is used to build, install and create distribution packages. The Visual Studio solution 
is for cross mode debugging of the extension as well as the client. 

Setup.py builds the package with the extension name “dwriteshapepy”. 
Useful command lines when using setup.py to build, install and create distribution packages are:

Build the extension on local machine.
Python setup.py build 

Install the built extension into current Python environment.
Python setup.py install

Create distribution package.
Python setup.py bdist_wheel

The dwriteshape directory in the repo contains the Visual Studio solution and projects. The dwriteshape solution contains two 
projects. The dwriteshape project and the client project. The dwriteshape project builds the package as the extension name “dwriteshape”. 
Note the difference between extension build using setup.py. This is necessary because Visual Studio does not have same level 
of control as setup.py and seems to require the extension name be the same as that of the .pyx file. It could also be useful in 
avoiding name collisions. The Visual Studio project should only be used to create debug builds of the extension for development. 
The client project is a simple test client for the dwriteshape project. 

In order to build the extension with the Visual Studio project you must first use Cython to create the .cpp file from the .pyx and .pxd files. 
There is a build.bat in the src\dwriteshapepy directory to do this.  The generated file dwriteshape.cpp should be deleted if switching 
from building with Visual Studio or setup.py or visa versa. When building with setup.py the .cpp file is generated automatically so build.bat 
is not necessary. 

The directory src\cpp contains the DWrite abstraction layer of the extension basically plumbing between the exported API and DWrite. 
This code is common between build methods. 

### Example

```python
import sys
import dwriteshapepy as dw 
from pathlib import Path

with open(sys.argv[1], 'rb') as fontfile:
    fontdata = fontfile.read()

text = sys.argv[2]

face = hb.Face(fontdata)
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

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.
