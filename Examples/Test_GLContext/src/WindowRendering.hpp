#ifndef __GLIPLIB_WINDOW_RENDERER__
#define __GLIPLIB_WINDOW_RENDERER__

	#include "GLIPLib.hpp"
	#include <QGLWidget>
	#include <QTimer>

	// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	// Class
	class WindowRenderer : public QGLWidget
	{
		Q_OBJECT

		private :
			double vp_x, vp_y, vp_w, vp_h, fmtImg;
			QTimer *timer;
			HdlVBO *vbo;

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

