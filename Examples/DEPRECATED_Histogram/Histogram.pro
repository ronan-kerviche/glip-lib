CONFIG 		+= 	qt release
QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS       	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/RessourceLoader.hpp \
			./src/header.hpp
SOURCES       	+= 	./src/main.cpp \
			./src/source.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/RessourceLoader.cpp
