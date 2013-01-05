CONFIG 		+= 	qt release
QT           	+= 	opengl \
			phonon
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS		+=	./src/VideoPlayer.hpp \
			../ExternalTools/Qt/WindowRendering.hpp
SOURCES      	+= 	./src/main.cpp \
			./src/VideoPlayer.cpp \
			../ExternalTools/Qt/WindowRendering.cpp

