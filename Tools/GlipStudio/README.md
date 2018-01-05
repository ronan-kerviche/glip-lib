GlipStudio
==============

Author
--------------
Ronan Kerviche

License
--------------
The MIT License (MIT)

Copyright (c) 2010-2017 Ronan Kerviche

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Description
--------------
GUI tool for image processing on GPU with GLIP-Lib. Requires Qt toolkit, possibly libraw for raw image loading.

Platforms : *NIX, Windows 7+

More on http://glip-lib.net

Build options
--------------
In the file `GlipStudio.pro`, the variable `GLIP_OPTIONS` contain the options of the module options. You can remove any flag to disable and bypass requirements.

| Option name          | Description |
| -------------------- | ----- |
| "__USE_QVGL__"       | Mandatory |
| "__MAKE_VARIABLES__" | Mandatory |
| "__USE_NETPBM__"     | Enable NetPBM image formats support (PGM, PPM) |
| "__USE_LIBRAW__"     | Enable RAW file format suport (as .CR2, .NEF, etc.) via LibRAW. |

On Windows platforms, you will have to set the location of the LibRAW library to compile with its support (see the `contains(DEFINES, __USE_LIBRAW__)` block) or disable it otherwise.

Compiling
--------------
Using Qt toolchain on *NIX :

> qmake CONFIG+="release"

Compile :

> make

Install :

> make install

On Windows, use Qt Creator and target Qt <= 5.3, with OpenGL support.

