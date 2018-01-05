glip-filtervideo
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
Command line tool for video processing on GPU with GLIP-Lib.

Requirements
--------------
Requires cmake for building, X server and ffmpeg to read and write video streams.

Compiling
--------------
Using cmake, cmake-gui or equivalent generate the Makefile or SLN file for building the project :

> cmake . -DCMAKE_BUILD_TYPE=release

Then compile with Make or via Visual Studio depending on the platform :

> make

And install :

> make install

To generate the source package, use :

> make package_source

To generate packages (on *NIX, DEB and RPM), use :

> make package

Compiling against GLES (3.1, experimental) : first remove the existing cache :

> rm CMakeCache.txt && make clean

Regenerate the Makefile, before recompiling, as usual :

> cmake . -DCMAKE_BUILD_TYPE=release -DGLIP_USE_GLES3=1
