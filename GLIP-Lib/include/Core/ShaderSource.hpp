/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ShaderSource.hpp                                                                          */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Shader source and tools                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ShaderSource.hpp
 * \brief   Shader source and tools
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_SHADERSOURCE__
#define __GLIP_SHADERSOURCE__

	// Includes
	#include "OglInclude.hpp"
        #include <string>
        #include <vector>
        #include <fstream>

	namespace Glip
	{
		namespace CoreGL
		{
			// Objects
/**
\class ShaderSource
\brief Shader source code and infos.

Loading a shader from a standard string or a file :
\code
// From a file :
ShaderSource src("./Filters/game.glsl");
// From a string, which must contain at least one new line character '\n' :
ShaderSource src(myShader);
\endcode

The shader source will be parsed to find input and output ports name. In the case that you are using gl_FragColor, the name of the output port will be the one contained by string ShaderSource::portNameForFragColor.
**/
			class ShaderSource
			{
				private :
					// Data
					std::string 			source,
									sourceName,
									sourceInfo;
					std::vector<std::string> 	inSamplers2D;
					std::vector<std::string> 	uniformVars;
					std::vector<GLenum>		uniformVarsType;
					std::vector<std::string> 	outFragments;
					bool 				compatibilityRequest;
					int 				versionNumber,
									lineOffset;

					// Tools
					std::string getLine(int l);
					bool removeBloc(std::string& line, const std::string& bStart, const std::string& bEnd, bool nested);
					void wordSplit(const std::string& line, std::vector<std::string>& split);
					GLenum parseUniformTypeCode(const std::string& str, const std::string& cpl);
					GLenum parseOutTypeCode(const std::string& str, const std::string& cpl);
					void parseCode(void);

				public :
					// Static data
					static std::string portNameForFragColor;

					// Tools
					//ShaderSource(const char** src, bool eol = true, int lines=-1);
					ShaderSource(const std::string& src, const std::string& _sourceInfo="", int _lineOffset=-1);
					ShaderSource(const ShaderSource& ss);

					const std::string& getSource(void)     const;
					const std::string& getSourceName(void) const;
					const char*        getSourceCstr(void) const;
					std::string        errorLog(std::string log);
					bool		   requiresCompatibility(void) const;
					int		   getVersion(void) const;

					const std::vector<std::string>& getInputVars(void) const;
					const std::vector<std::string>& getOutputVars(void) const;
					const std::vector<std::string>& getUniformVars(void) const;
					const std::vector<GLenum>&	getUniformTypes(void) const;
			};
		}
	}

#endif
