CONFIG 		+= 	qt
QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/RessourceLoader.hpp \
			./src/codeEditor.hpp \
			./src/keywords.hpp \
			./src/libraryInterface.hpp \
			./src/mainInterface.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/RessourceLoader.cpp \
			./src/codeEditor.cpp \
			./src/keywords.cpp \
			./src/libraryInterface.cpp \
			./src/mainInterface.cpp

