SOURCES		= 	./src/main.cpp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT		+= 	opengl

INCLUDEPATH	+= 	/usr/include/ImageMagick/ \
			/usr/local/lib \
			../../GLIP-Lib/include

unix: LIBS      += 	../../GLIP-Lib/lib/libglip.so
win32:Debug:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib
win32:Release:	LIBS +=	../../Project_VS/GLIP-Lib/x64/Release/GLIP-Lib.lib

