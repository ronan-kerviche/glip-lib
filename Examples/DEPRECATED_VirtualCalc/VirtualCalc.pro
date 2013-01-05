HEADERS       = ./src/WindowRendering.hpp \
				./src/header.hpp
SOURCES       = ./src/main.cpp \
				./src/source.cpp \
				./src/WindowRendering.cpp
QT           += opengl
INCLUDEPATH  += /usr/include/ImageMagick/ \
				/usr/local/lib \
                /usr/include/unicap/ \
                ../../GLIP-Lib/include               
LIBS         += ../GLIP-Lib/lib/libglProcessing.a \
				-lunicap \
				-lMagickCore \
