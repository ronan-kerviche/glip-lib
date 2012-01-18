CONFIG 		+= 	qt
QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS       	+= 	./src/WindowRendering.hpp \
			./src/header.hpp
SOURCES       	+= 	./src/main.cpp \
			./src/source.cpp \
			./src/WindowRendering.cpp
