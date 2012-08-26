/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : WindowRendering.hpp                                                                       */
/*     Original Date : September 1st 2011                                                                        */
/*                                                                                                               */
/*     Description   : Qt Widget for rendering textures                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

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
	class WindowRenderer : public QGLWidget, public OutputDevice
	{
		Q_OBJECT

		private :
			double vp_x, vp_y, vp_w, vp_h, fmtImg;
			HdlVBO *vbo;

		public :
			bool xFlip, yFlip;

			WindowRenderer(QWidget* parent, int w, int h, double _fmtImg=-1);
			~WindowRenderer(void);

			void resizeGL(int width, int height);
			void process(HdlTexture& t);

		signals :
			void resized(void);
	};

#endif

