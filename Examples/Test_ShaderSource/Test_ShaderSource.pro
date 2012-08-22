SOURCES       = ./src/main.cpp
QT           += opengl
INCLUDEPATH  += /usr/include/ImageMagick/ \
		/usr/local/lib \
                ../../GLIP-Lib/include
LIBS         += ../../GLIP-Lib/lib/libglip.a
