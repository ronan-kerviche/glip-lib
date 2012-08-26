	GLIP-Lib
	OpenGL Image Processing Library
	Copyright (C) 2010-2011-2012

===============================================================================

Web
	http://glip-lib.sourceforge.net/index.html
	http://sourceforge.net/projects/glip-lib/

Authors
	Ronan    KERVICHE - ronan.kerviche@free.fr
	Corentin DERBOIS  -

License and disclaimers
	MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
	THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Aim
	The aim of this library is to provide a framework of video processing
	pipelines on GPU architectures. It uses the OpenGL library
	(Khronos Group, Copyright 2010) and the shading language GLSL.

Plateforms
	Developped on Linux Debian, tested on Fedora 17 and on Windows 7 (MingW).

Documentation
	See : http://glip-lib.sourceforge.net/index.html

How to compile
	Linux :
	In GLIP-Lib : sh ./compile

	Windows : (description is for MingW compiler but it should work with cl)
		Download and install CMake from : http://www.cmake.org/
		In CMake-GUI :
			Where is the source code    : <your path>/glip-lib/GLIP-Lib
			Where to build the binaries : <your path>/glip-lib/GLIP-Lib
		Add the following entries :
			CMAKE_BUILD_TYPE <leave blank>
			CMAKE_GNUtoMS <leave unticked>
			CMAKE_INSTALL_PREFIX C:/Program Files (x86)/GLIP

		Then configure and choose your compiler (tested with MingW via Makefiles) and "use default native compiler".
		Then generate.

		Once generation is done, the Makefile has been updated and you can compile with command : mingw32-make.

		Compiling the Qt examples :

		qmake

		Then edit the Makefile and make sure that ../../GLIP-Lib/lib/libglip.a appears first in LIBS.

		mingw32-make

		The application might require to have some DLL in the same folder as the executable file. For a release target, you might want to copy :
			liggcc_s_dw2-1.dll
			mingwm10.dll
			qgif4.dll
			qico4.dll
			qjpeg4.dll
			qmng4.dll
			qsvg4.dll
			QtCore4.dll
			QtGui4.dll
			qtiff4.dll
			QtOpenGL4.dll

Support
	Email the authors for any question, bug fixes or improvement you may want to discuss.
