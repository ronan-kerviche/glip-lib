HEADERS       = ./src/WindowRendering.hpp \
				./src/header.hpp
SOURCES       = ./src/main.cpp \
				./src/source.cpp \
				./src/WindowRendering.cpp
QT           += opengl
INCLUDEPATH  += /usr/local/lib \
                /usr/include/unicap       
LIBS         += ../../GLIP-Lib/lib/libglip.a \
				-lunicap \
