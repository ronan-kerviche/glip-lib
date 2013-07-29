CONFIG 		+= 	qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../../GLIP-Lib/include

unix: LIBS      += 	../../GLIP-Lib/lib/libglip.a
win32: LIBS	+=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib

HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/RessourceLoader.hpp \
			./src/imageContentInfo.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/RessourceLoader.cpp \
			./src/imageContentInfo.cpp
