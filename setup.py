# -*- coding: utf-8 -*-
from io import open
import os
import sys
import platform
from setuptools import Extension, setup
from Cython.Build import cythonize

here = os.path.abspath(os.path.dirname(__file__))

# Get the long description from the pypi.md file
with open(os.path.join(here, 'pypi.md'), encoding='utf-8') as f:
    long_description = f.read()

define_macros = [('UNICODE', 1), ('_UNICODE', 1)]
linetrace = False
if int(os.environ.get('CYTHON_LINETRACE', '0')):
    linetrace = True
    define_macros.append(('CYTHON_TRACE_NOGIL', '1'))

extra_compile_args = ['/Zc:wchar_t']

if platform.system() != 'Windows':
    extra_compile_args.append('-std=c++11')

extension = Extension(
    'dwriteshapepy.dwriteshape',
    define_macros=define_macros,
    include_dirs=["."],
    sources=['src/dwriteshapepy/dwriteshape.pyx','src/cpp/hb-common.cc','src/cpp/hb-number.cc','src/cpp/dwriteshapeInternal.cpp','src/cpp/dwriteshapelib.cpp', 'src/cpp/locale.cpp','src/cpp/posttable.cpp','src/cpp/textanalysis.cpp', 'src/cpp/textrun.cpp' ],
    language='c++',
    libraries=['dwrite'],
    extra_compile_args=extra_compile_args,
)

setup(
    name="dwriteshapepy", 
    version= '1.0.0',
    description="Python extension for Windows DirectWrite shaping, modeled after uharfbuzz ",
    long_description=long_description,
    long_description_content_type='text/markdown',
    author="Paul Linnerud",
    author_email="paulli@microsoft.com",
    url="https://github.com/microsoft/DWriteShapePy",
    license="MIT",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Microsoft :: Windows :: Windows 10",
    ],
    package_dir={"": "src"},
    packages=["dwriteshapepy"],
    zip_safe=False,
    python_requires=">=3.5",
    ext_modules = cythonize(
        extension,
        annotate=bool(int(os.environ.get('CYTHON_ANNOTATE', '0'))),
        compiler_directives={"linetrace": linetrace},
    ),
)
