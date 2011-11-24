/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : TextureReader.hpp                                                                        **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Texture Reader                                                                           **/
/**                                                                                                             **/
/*****************************************************************************************************************/

#ifndef __TEXTURE_READER_INCLUDE__
#define __TEXTURE_READER_INCLUDE__

	/* Info :
		Implement a slow readback of GL textures
		Based on glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* img);
	*/

	// Include :
	#include "OglInclude.hpp"

	// Prototypes
	namespace Glip
	{
	    namespace CoreGL
	    {
	        class __HdlTextureFormat_OnlyData;
	    }

	    namespace CorePipeline
	    {
	        class InputModule;
	    }
	}

	using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

	// Structure
	class TextureReader : public InputModule
	{
		private :
		public :
			// Functions
				TextureReader(int _nSocket);

				__HdlTextureFormat_OnlyData& getFormat(int i=0);
				bool read(int i, GLint level, GLenum format, GLenum type, GLvoid* img);
	};

#endif

