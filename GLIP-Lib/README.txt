	GLIP-Lib
	OpenGL Image Processing Library
	Copyright (C) 2010-2013

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
		In GLIP-Lib : 
			sh ./compile
		or
			cmake . -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release

	Windows : 
		Create a Visual Studio project for a static library.
		Set the platform setting to match yours (x86/x64).
		Add in the include path the following ones : 
			include/
			include/Core/
		Compile the solution.

		With QtCreator, compile the projects.

		Tested with VS2012 x64, Qt5 x64.

Support
	Email the authors for any question, bug fixes or improvement you may want to discuss.
