CONFIG 		+= 	qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../../GLIP-Lib/include

unix: LIBS      += 	../../GLIP-Lib/lib/libglip.so
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/UniformsVarsLoaderInterface.hpp \
			../ExternalTools/Qt/ResourceLoader.hpp \
			./src/imageContentInfo.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/UniformsVarsLoaderInterface.cpp \
			../ExternalTools/Qt/ResourceLoader.cpp \
			./src/imageContentInfo.cpp
