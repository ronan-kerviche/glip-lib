#ifndef __HEADER_LIFE__
#define __HEADER_LIFE__

	/* Notes : 
		This is based on corellation calculus : 

		c = Sum( (a-Ma)*(b-Mb) )/SQRT( Sum(a-Ma)*Sum(b-Mb) )
		
		Where Mx is the arithmetic mean of x
	*/

	// Includes
	#include <iostream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QDesktopWidget>
	#include <QtCore/QTimer>
	#include <ImageMagick/magick/api.h>
	#include <ImageMagick/magick/ImageMagick.h>

	// Ressource define : 
	#define RESSOURCES_NUM 13

	// Modes
	enum
	{
		MODE_WAITING,
		MODE_DRAWING
	};

	// POV define : 
	//#define POV_OPERATOR

	class ApplicationWebcam : QObject, public OutputModule
	{
		Q_OBJECT 
	
		private :
			QTimer        *timer;
			HdlPipeline   *meanPipeline;
			HdlPipeline   *maxPipeline;
			HdlPipeline   *drawPipeline;
			HdlPipeline   *correlatePipeline;
			HdlPipeline   *roiPipeline;
			UnicapCamera  *cam; 
			TextureReader *reader;

			HdlTexture    *references[RESSOURCES_NUM];
			SocketID       refSockets[RESSOURCES_NUM];
			float          mean[RESSOURCES_NUM],
						   sum[RESSOURCES_NUM];
			float          backgroundMax, coeffThreshold, radius;
			int            currentMode;
			bool           enabled;
			
		public :
			ApplicationWebcam(int id, FOURCC code);
			~ApplicationWebcam(void); 

			bool isEnabled(void); 
		 
		private slots :
			void loop(void);
	};	

#endif  
