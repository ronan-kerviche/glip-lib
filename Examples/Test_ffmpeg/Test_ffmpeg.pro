CONFIG 		+= 	qt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../ExternalTools/ffmpeg \
			../../GLIP-Lib/include
unix: LIBS        += 	-lavutil \
			-lavformat \
			-lavcodec \
			-lavutil \
			-lswscale \
			../../GLIP-Lib/lib/libglip.so

win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/ResourceLoader.hpp \
			../ExternalTools/Qt/VideoControls.hpp \
			../ExternalTools/ffmpeg/InterfaceFFMPEG.hpp \
			../ExternalTools/ffmpeg/VideoStream.hpp \
			../ExternalTools/ffmpeg/VideoRecorder.hpp \
			./src/Test_ffmpeg.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/ResourceLoader.cpp \
			../ExternalTools/Qt/VideoControls.cpp \
			../ExternalTools/ffmpeg/InterfaceFFMPEG.cpp \
			../ExternalTools/ffmpeg/VideoStream.cpp \
			../ExternalTools/ffmpeg/VideoRecorder.cpp \
			./src/Test_ffmpeg.cpp
