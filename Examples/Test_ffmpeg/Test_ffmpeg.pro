CONFIG 		+= 	qt release
QT           	+= 	opengl
INCLUDEPATH  	+= 	/usr/local/lib \
			../ExternalTools/Qt \
			../ExternalTools/ffmpeg \
			../../GLIP-Lib/include
LIBS        	+= 	-lavutil \
			-lavformat \
			-lavcodec \
			-lavutil \
			-lswscale \
			../../GLIP-Lib/lib/libglip.a
HEADERS      	+= 	../ExternalTools/Qt/WindowRendering.hpp \
			../ExternalTools/Qt/RessourceLoader.hpp \
			../ExternalTools/Qt/VideoControls.hpp \
			../ExternalTools/ffmpeg/InterfaceFFMPEG.hpp \
			../ExternalTools/ffmpeg/VideoStream.hpp \
			../ExternalTools/ffmpeg/VideoRecorder.hpp \
			./src/Test_ffmpeg.hpp
SOURCES      	+= 	./src/main.cpp \
			../ExternalTools/Qt/WindowRendering.cpp \
			../ExternalTools/Qt/RessourceLoader.cpp \
			../ExternalTools/Qt/VideoControls.cpp \
			../ExternalTools/ffmpeg/InterfaceFFMPEG.cpp \
			../ExternalTools/ffmpeg/VideoStream.cpp \
			../ExternalTools/ffmpeg/VideoRecorder.cpp \
			./src/Test_ffmpeg.cpp
