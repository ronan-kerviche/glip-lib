CONFIG 	+= qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += opengl

INCLUDEPATH += 	/usr/local/lib \
		../ExternalSrc/Qt \
		../../GLIP-Lib/include

unix: 		LIBS += ../../GLIP-Lib/lib/libglip.so
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

GLIP_OPTIONS	= "__USE_QVGL__" \ 		# Define GUI elements.
		  "__MAKE_VARIABLES__" \	# Allow modules to distribut some of their variables.
		  "__USE_QSETTINGS__" \ 	# To read/write settings, use QSettings.
		  "__USE_NETPBM__" \		# Load PPM / PGM files.
		  "__USE_LIBRAW__"		# Load Raw files (such as CR2 and NEF via LibRaw (required)).

unix:DEFINES +=	$${GLIP_OPTIONS}

win32:Debug:DEFINES +=	"WIN32_LEAN_AND_MEAN" \ # Avoid symbol redefinition if using libraw
			$${GLIP_OPTIONS}		
	
win32:Release:DEFINES +="WIN32_LEAN_AND_MEAN" \ # Avoid symbol redefinition if using libraw
			$${GLIP_OPTIONS}

INCLUDEPATH +=	../ExternalSrc/Qt

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
	INCLUDEPATH += ../ExternalSrc/NetPBM
	HEADERS += ../ExternalSrc/NetPBM/NetPBM.hpp
	SOURCES += ../ExternalSrc/NetPBM/NetPBM.cpp
}

contains(DEFINES, __USE_LIBRAW__){
	INCLUDEPATH += ../ExternalSrc/LibRaw
	HEADERS	+= ../ExternalSrc/LibRaw/LibRawInterface.hpp
	SOURCES += ../ExternalSrc/LibRaw/LibRawInterface.cpp

	# Specifics to *NIX platforms :	
	unix: LIBS += -lraw
	
	# Specifics to Windows platform :
	win32 : 	INCLUDEPATH += path/to/LibRaw/include
	win32:Debug: 	LIBS += path/to/LibRaw/debug/libraw.lib
	win32:Release:	LIBS += path/to/LibRaw/release/libraw.lib
}

# Where to output :
DESTDIR = .
OBJECTS_DIR = ./obj
MOC_DIR = ./mocs

# For the installation on *NIX platform :
unix:installPath = /usr/local/GlipStudio

unix:target.files = GlipStudio
unix:target.path = $${installPath}
unix:target.extra = ln -sf $${installPath}/GlipStudio /usr/bin/GlipStudio
unix:stylesheet.files = stylesheet.css
unix:stylesheet.path = $${installPath}
unix:fonts.files = Fonts/*
unix:fonts.path = $${installPath}/Fonts/
unix:resources.files = Resources/*
unix:resources.path = $${installPath}/Resources/

unix:INSTALLS += target stylesheet fonts resources
