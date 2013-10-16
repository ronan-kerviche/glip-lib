CONFIG 		+= 	qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../ExternalTools/NetPBM \
			../../GLIP-Lib/include

unix: LIBS      += 	../../GLIP-Lib/lib/libglip.a
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/ResourceLoader.hpp \
			../ExternalTools/NetPBM/netpbm.hpp \
			./src/dataModules.hpp \
			./src/codeEditor.hpp \
			./src/keywords.hpp \
			./src/imageObject.hpp \
			./src/imagesCollection.hpp \
			./src/resourcesTab.hpp \
			./src/compilationTab.hpp \
			./src/uniformsTab.hpp \
			./src/libraryInterface.hpp \
			./src/mainInterface.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/ResourceLoader.cpp \
			../ExternalTools/NetPBM/netpbm.cpp \
			./src/dataModules.cpp \
			./src/codeEditor.cpp \
			./src/keywords.cpp \
			./src/imageObject.cpp \
			./src/imagesCollection.cpp \
			./src/resourcesTab.cpp \
			./src/compilationTab.cpp \
			./src/uniformsTab.cpp \
			./src/libraryInterface.cpp \
			./src/mainInterface.cpp

