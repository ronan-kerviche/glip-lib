QT           += opengl
INCLUDEPATH  += /usr/local/lib \
                ../../GLIP-Lib/include
LIBS         += ../../GLIP-Lib/lib/libglip.a
HEADERS      += ./src/WindowRendering.hpp \
		./src/gameOfLife.hpp
SOURCES      += ./src/main.cpp \
                ./src/WindowRendering.cpp \
                ./src/gameOfLife.cpp
