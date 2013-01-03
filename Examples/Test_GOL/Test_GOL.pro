QT           += opengl
INCLUDEPATH  += /usr/local/lib \
		../ExternalTools/Qt \
                ../../GLIP-Lib/include
LIBS         += ../../GLIP-Lib/lib/libglip.a
HEADERS      += ../ExternalTools/Qt/WindowRendering.hpp \
		./src/gameOfLife.hpp
SOURCES      += ./src/main.cpp \
                ../ExternalTools/Qt/WindowRendering.cpp \
                ./src/gameOfLife.cpp
