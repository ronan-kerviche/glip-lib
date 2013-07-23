CONFIG 		+= 	qt
QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../ExternalTools/NetPBM \
			../../GLIP-Lib/include
LIBS        	+= 	../../GLIP-Lib/lib/libglip.a
HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/RessourceLoader.hpp \
			../ExternalTools/NetPBM/netpbm.hpp \
			./src/codeEditor.hpp \
			./src/keywords.hpp \
			./src/ressourcesTab.hpp \
			./src/compilationTab.hpp \
			./src/uniformsTab.hpp \
			./src/libraryInterface.hpp \
			./src/mainInterface.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/RessourceLoader.cpp \
			../ExternalTools/NetPBM/netpbm.cpp \
			./src/codeEditor.cpp \
			./src/keywords.cpp \
			./src/ressourcesTab.cpp \
			./src/compilationTab.cpp \
			./src/uniformsTab.cpp \
			./src/libraryInterface.cpp \
			./src/mainInterface.cpp

