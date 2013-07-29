SOURCES		=	./src/main.cpp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT		+= 	opengl

INCLUDEPATH	+= 	/usr/local/lib \
               		../../GLIP-Lib/include

unix: LIBS      += 	../../GLIP-Lib/lib/libglip.a
win32: LIBS	+=	../../Project_VS/GLIP-Lib/x64/Debug/GLIP-Lib.lib

