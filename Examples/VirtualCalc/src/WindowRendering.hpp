
#ifndef __WINDOW_RENDERER__
#define __WINDOW_RENDERER__
	
	#include "../GLIP-Lib/include/GLIPLib.hpp"
	#include <QtOpenGL/QGLWidget>
	#include <QtCore/QTimer>

	#define DISP_MIROR // miror the display
	
	// Class
	class WindowRenderer : public QGLWidget, public InputModule
	{
		Q_OBJECT

		private :
			double vp_x, vp_y, vp_w, vp_h, fmtImg;		
			QTimer     *timer;

			void quad(float aX, float aY, float bX, float bY);
		public :
			WindowRenderer(int w, int h, double _fmtImg=-1);  
			~WindowRenderer(void);

			void resizeGL(int width, int height); 

			bool process(void);

		private slots : 
			void draw(void);
	};

#endif 

