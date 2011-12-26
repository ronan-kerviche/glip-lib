/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlShader.hpp                                                                             */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel and Fragment Shader Handle                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlShader.hpp
 * \brief   OpenGL Pixel and Fragment Shader Handle
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 7th 2010
**/

#ifndef __HDLSHADER_INCLUDE__
#define __HDLSHADER_INCLUDE__

	// GL include
        #include "OglInclude.hpp"
        #include "ShaderSource.hpp"
	#include <string>
        #include <fstream>

namespace Glip
{
	namespace CoreGL
	{

		// Prototype
		//class HdlShader;

		// Shader Handle
		/**
		\class HdlShader
		\brief Object handle for OpenGL Shaders Objects (both vertex and fragment shaders)
		**/
		class HdlShader : public ShaderSource
		{
			private :
				// Data
				GLuint shader;
				GLenum type;

			public :
				// Functions
				HdlShader(GLenum _type, ShaderSource& src);
				~HdlShader(void);

				GLuint getShaderID(void) const;
				GLenum getType(void)     const;
		};

		// Program Handle
		/**
		\class HdlProgram
		\brief Object handle for OpenGL Program (A vertex and a fragment program)
		**/
		class HdlProgram
		{
			public :
				// Data
				// Enum / data
				enum SHADER_DATA_TYPE
				{
					SHADER_VAR,
					SHADER_VEC2,
					SHADER_VEC3,
					SHADER_VEC4
				};
			private :
				// Data
				bool valid;
				GLuint program;
				GLuint attachedVertexShader, attachedFragmentShader;

				// Functions
				bool link(void);
			public :
				// Functions
				HdlProgram(void);
				HdlProgram(const HdlShader& shd1, const HdlShader& shd2);
				~HdlProgram(void);

				bool isValid(void);

				void update(const HdlShader& shader, bool lnk = true);
				void use(void);

				bool setFragmentLocation(const std::string& fragName, int frag);
				bool modifyVar(const std::string& varName, SHADER_DATA_TYPE type, int val1, int val2=0, int val3=0, int val4=0);
				bool modifyVar(const std::string& varName, SHADER_DATA_TYPE type, float val1, float val2=0, float val3=0, float val4=0);

				// Static tools :
				static int maxVaryingVar(void);
				static void stopProgram(void);
		};
	}
}

#endif

