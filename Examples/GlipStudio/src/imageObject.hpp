#ifndef __GLIPSTUDIO_IMAGEOBJECT__
#define __GLIPSTUDIO_IMAGEOBJECT__

	#include "GLIPLib.hpp"
	#include "ResourceLoader.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	
	class ImageObject
	{
		private : 
			bool 			virtualImage,
						saved;
			QString 		filename;
			ImageBuffer*		imageBuffer;
			HdlTextureFormat 	textureFormat;
			HdlTexture* 		textureData;

		public : 
			ImageObject(const QString& _filename);
			ImageObject(HdlTexture& texture);
			~ImageObject(void);

			bool isVirtual(void) const;
			bool wasSaved(void) const;
			bool isOnDevice(void) const;
			void loadToDevice(void);
			void unloadFromDevice(void);
			void setMinFilter(GLenum mf);
			void setMagFilter(GLenum mf);
			void setSWrapping(GLint m);
			void setTWrapping(GLint m);
			const QString& getFilename(void) const;
			QString getName(void) const;
			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			HdlTexture& texture(void);
			void save(const std::string& filename);
			void save(void);
	};

#endif
