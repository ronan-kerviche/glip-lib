CONFIG 	+= 	qt \
		debug

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl

INCLUDEPATH += 	/usr/local/lib \
		../ExternalTools/Qt \
		../../GLIP-Lib/include

unix: 		LIBS += ../../GLIP-Lib/lib/libglip.so
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

INCLUDEPATH +=	../ExternalTools/Qt \
		../ExternalTools/NetPBM

HEADERS += 	../ExternalTools/Qt/GLSceneWidget.hpp \	
		../ExternalTools/NetPBM/NetPBM.hpp \ 	
		../ExternalTools/Qt/ImageItem.hpp \ 
		../ExternalTools/Qt/UniformsVarsLoaderInterface.hpp \
		../ExternalTools/Qt/ResourceLoader.hpp \
		../ExternalTools/Qt/GLSLKeywords.hpp \
		../ExternalTools/Qt/CodeEditor.hpp \
		./src/imageContentInfo.hpp
SOURCES += 	./src/main.cpp \
		../ExternalTools/Qt/GLSceneWidget.cpp \	
		../ExternalTools/NetPBM/NetPBM.cpp \	
		../ExternalTools/Qt/ImageItem.cpp \
		../ExternalTools/Qt/UniformsVarsLoaderInterface.cpp \
		../ExternalTools/Qt/ResourceLoader.cpp \
		../ExternalTools/Qt/GLSLKeywords.cpp \
		../ExternalTools/Qt/CodeEditor.cpp \
		./src/imageContentInfo.cpp

