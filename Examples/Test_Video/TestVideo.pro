CONFIG 		+= 	qt
QT           	+= 	opengl \
			phonon
INCLUDEPATH  	+= 	/usr/local/lib \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS		+=	./src/VideoPlayer.hpp \
			./src/WindowRendering.hpp
SOURCES      	+= 	./src/main.cpp \
			./src/VideoPlayer.cpp \
			./src/WindowRendering.cpp

