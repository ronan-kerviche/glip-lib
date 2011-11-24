							GLIP-Lib 
				OpenGL Image Processing Library
					 Copyright (C) 2010-2011 

===============================================================================

Web
	http://sourceforge.net/projects/glip-lib/

Authors
	Ronan    KERVICHE - ronan.kerviche@free.fr
	Corentin DERBOIS  - 

License and disclaimers
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

Aim
	The aim of this library is to provide a framework of video processing
	pipelines on GPU architectures. It uses the OpenGL library
	(Khronos Group, Copyright 2010) and the shading language GLSL.

Plateforms
	The core library, except its modules, tends to be cross-plateform. 
	Please, refer to future documentation for further informations.

Documentation
	Not for the moment. (Doxygen)

How to compile
	In GLIP-Lib : sh ./compile

===============================================================================
	WHAT'S NEW?

0.51 - Base version, it inludes low level processing and pipelines management

0.55 - 
		* ADDED    : Technology presentation slides (in PDF - see SourceForge)
					 Support for Unicap2 library instead of V4L2, it includes
						new routines for 1394 cameras support
					 Multi-target rendering from a single shader (#version 130)
                     Starting documentation
		* UPGRADED : Hardware checking and support for both NVidia and AMD/ATI
		* REMOVED  : V4L2 support
		
0.60 - 
        * ADDED    : Support for histogram-like algorithms
					 Debugging tools and naming for filters and pipelines
                     New formats decoding for cameras
		* BUGFIX   : Some bugs in memory management for cameras
					 Floating point texture is now fully available.



