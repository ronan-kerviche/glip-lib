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
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Core/Exception.hpp"
	#include <string>
	#include <vector>
	#include <map>
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
			class GLIP_API ShaderSource
			{
				public :
					/**
					\class LineInfo
					\brief Information about a specific line in ShaderSource.
					**/
					struct LineInfo
					{
						/// The number of the line (including the offset).
						int lineNumber;
						/// The name of the source (for example, the filename).
						std::string sourceName;

						LineInfo(void);
						LineInfo(const std::string& _sourceName, int _lineNumber);
					};

				private :
					// Data
					std::string 			source,
									sourceName;
					std::vector<size_t>		lineFirstChar,
									lineLength;
					std::vector<std::string> 	inSamplers2D;
					std::vector<std::string> 	uniformVars;
					std::vector<GLenum>		uniformVarsType;
					std::vector<std::string> 	outFragments;
					bool 				compatibilityRequest;
					int 				versionNumber,
									startLine;

					// Tools :
					void parseLines(void);
					bool removeBlock(std::string& line, const std::string& bStart, const std::string& bEnd, bool nested);
					void wordSplit(const std::string& line, std::vector<std::string>& split);
					GLenum parseUniformTypeCode(const std::string& str, const std::string& cpl);
					GLenum parseOutTypeCode(const std::string& str, const std::string& cpl);
					void parseCode(void);

				public :
					/// Name for the port mapping to gl_FragColor, if used.
					static std::string portNameForFragColor;

					/// Map containing information about specific lines of this source. The key represent the line number <b>starting at 1</b>.
					std::map<int, LineInfo>	linesInfo;

					ShaderSource(const std::string& src, const std::string& _sourceName="", int startLine=1, const std::map<int,LineInfo>& _linesInfo=std::map<int,LineInfo>());
					ShaderSource(const ShaderSource& ss);
					~ShaderSource(void);

					ShaderSource& operator=(const ShaderSource& c);

					bool empty(void) const;
					const std::string& getSource(void) const;
					const std::string& getSourceName(void) const;
					const char* getSourceCstr(void) const;
					int getNumLines(void) const;
					std::string getLine(int l, bool withNewLine=true) const;	
					LineInfo getLineInfo(int l) const;
					Exception errorLog(const std::string& log) const;
					bool requiresCompatibility(void) const;
					int getVersion(void) const;

					const std::vector<std::string>& getInputVars(void) const;
					const std::vector<std::string>& getOutputVars(void) const;
					const std::vector<std::string>& getUniformVars(void) const;
					const std::vector<GLenum>& getUniformTypes(void) const;
			};
		}
	}

#endif
