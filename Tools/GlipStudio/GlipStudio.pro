CONFIG 	+= 	qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl

INCLUDEPATH += 	/usr/local/lib \
		../ExternalSrc/Qt \
		../../GLIP-Lib/include

unix: 		LIBS += ../../GLIP-Lib/lib/libglip.so -fopenmp
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

unix:DEFINES +=	"__USE_QVGL__" \		# Define GUI elements
		"__MAKE_VARIABLES__" \		# Allow modules to distribut some of their variables
		"__USE_NETPBM__" \		# Load PPM / PGM files
		"__USE_LIBRAW__"		# Load Raw files (such as CR2 and NEF with the help of LibRaw

win32:Debug:DEFINES +=	"__USE_QVGL__" \
			"__MAKE_VARIABLES__" \
			"__USE_NETPBM__"

win32:Release:DEFINES +="__USE_QVGL__" \
			"__MAKE_VARIABLES__" \
			"__USE_NETPBM__"

INCLUDEPATH +=	../ExternalSrc/Qt \
		../ExternalSrc/NetPBM \
		../ExternalSrc/LibRaw 

HEADERS += 	../ExternalSrc/Qt/GLSceneWidget.hpp \	
		../ExternalSrc/Qt/QMenuTools.hpp \	
		../ExternalSrc/Qt/ImageItem.hpp \ 
		../ExternalSrc/Qt/PipelineManager.hpp \
		../ExternalSrc/Qt/UniformsLoaderInterface.hpp \
		../ExternalSrc/Qt/GLSLKeywords.hpp \
		../ExternalSrc/Qt/CodeEditor.hpp \
		./src/GlipStudio.hpp
SOURCES += 	../ExternalSrc/Qt/GLSceneWidget.cpp \	
		../ExternalSrc/Qt/QMenuTools.cpp \	
		../ExternalSrc/Qt/ImageItem.cpp \
		../ExternalSrc/Qt/PipelineManager.cpp \
		../ExternalSrc/Qt/UniformsLoaderInterface.cpp \
		../ExternalSrc/Qt/GLSLKeywords.cpp \
		../ExternalSrc/Qt/CodeEditor.cpp \
		./src/GlipStudio.cpp \
		./src/main.cpp

# Specific addons :
contains(DEFINES, __USE_NETPBM__){
	HEADERS += ../ExternalSrc/NetPBM/NetPBM.hpp
	SOURCES += ../ExternalSrc/NetPBM/NetPBM.cpp
}

contains(DEFINES, __USE_LIBRAW__){
	unix: LIBS += -lraw
	HEADERS	+= ../ExternalSrc/LibRaw/LibRawInterface.hpp
	SOURCES += ../ExternalSrc/LibRaw/LibRawInterface.cpp
}

DESTDIR=.
OBJECTS_DIR=./obj
MOC_DIR=./mocs

#QMAKE_CXXFLAGS += -g
