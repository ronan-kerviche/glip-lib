/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ShaderSource.cpp                                                                          */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Shader source and tools                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    ShaderSource.cpp
 * \brief   Shader source and tools
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include "Core/ShaderSource.hpp"
#include "Core/Exception.hpp"
#include "devDebugTools.hpp"
#include "Core/OglInclude.hpp"

    using namespace Glip::CoreGL;

	std::string ShaderSource::portNameForFragColor = "outputTexture";

	/**
	\fn    ShaderSource::ShaderSource(const ShaderSource& ss)
	\brief ShaderSource Construtor.
	\param ss Source code of the shader, as a copy.
	**/
	ShaderSource::ShaderSource(const ShaderSource& ss)
	{
		source     		= ss.source;
		sourceName 		= ss.sourceName;
		inSamplers2D    	= ss.inSamplers2D;
		uniformVars		= ss.uniformVars;
		uniformVarsType		= ss.uniformVarsType;
		outFragments    	= ss.outFragments;
		compatibilityRequest	= ss.compatibilityRequest;
		versionNumber		= ss.versionNumber;
		sourceInfo		= ss.sourceInfo;
		lineOffset		= ss.lineOffset;
	}

	/**
	\fn    ShaderSource::ShaderSource(const std::string& src, const std::string& _sourceInfo, int _lineOffset)
	\brief Constructor of ShaderSource, load from a standard string or a file.
	\param src Source data (must have at least one '\n') or filename (without '\n').
	\param _sourceInfo Some information about the source (if loaded from a Layout File for example).
	\param _lineOffset Correction added to the debugger info (also if loaded from a Layout File for example).
	**/
	ShaderSource::ShaderSource(const std::string& src, const std::string& _sourceInfo, int _lineOffset)
	 : compatibilityRequest(false), versionNumber(0), sourceInfo(_sourceInfo), lineOffset(_lineOffset)
	{
		size_t newline = src.find('\n');

		if(newline==std::string::npos)
		{
			// Open File
			std::fstream file;
			file.open(src.c_str());

			// Did it fail?
			if(!file.is_open())
				throw Exception("ShaderSource::ShaderSource - Failed to open file : " + src, __FILE__, __LINE__);

			source.clear();
			sourceName = src;

			// Set starting position
			file.seekg(0, std::ios::beg);

			std::string line;
			while(std::getline(file,line)) // loop while extraction from file is possible
			{
				source += line;
				source += "\n";
			}
		}
		else
		{
			// Inner string
			sourceName.clear();
			source.clear();
			source = src;
		}

		parseCode();
	}

	/**
	\fn    std::string ShaderSource::getLine(int l)
	\brief Get a line from the source code (delemiter : \\n).
	\param l Index of desired line (start at 0).
	\return Standard string or <error> in case of failure.
	**/
	std::string ShaderSource::getLine(int l)
	{
		bool	firstTime = true;
		size_t 	pre = 0,
			pos = 0;

		int i=0;

		while(i<=l)
		{
			if(firstTime)
				firstTime = false;
			else
				pre = pos+1;

			pos = source.find('\n',pre);

			if(pos==std::string::npos)
				return "<ShaderSource::getLine - error>";

			i++;
		}

		std::string res = source.substr(pre, pos-pre);

		size_t p2 = res.find_first_not_of(" \t");
		if(p2!=std::string::npos)
			return std::string(res.substr(p2));
		else
			return std::string(res);
	}

	bool ShaderSource::removeBloc(std::string& line, const std::string& bStart, const std::string& bEnd, bool nested)
	{
		if(!nested)
		{
			size_t pStart = line.find(bStart);

			if(pStart!=std::string::npos)
			{
				size_t pEnd = line.find( bEnd, pStart+bStart.size()-1);

				line.erase(pStart, pEnd-pStart+1);

				return true;
			}
			else
				return false;
		}
		else
		{
			size_t 	s = std::string::npos,
				e = std::string::npos;
			int 	level = 0;
			bool 	madeIt = false;
			for(size_t k=0; k<line.size()-std::max(bStart.size(),bEnd.size()); k++)
			{
				if( line.substr(k,bStart.size())==bStart )
				{
					if(!madeIt)
						s = k;

					level++;
					madeIt = true;
				}
				else if( line.substr(k,bEnd.size())==bEnd )
					level--;

				if(level==0 && madeIt)
				{
					e = k;
					break;
				}
			}

			if(s!=std::string::npos)
			{
				line.erase(s,e-s+1);
				return true;
			}
			else
				return false;
		}
	}

	void ShaderSource::wordSplit(const std::string& line, std::vector<std::string>& split)
	{
		const std::string 	delimiters = " \n\r\t\f\v.,;/\\?*+-:#'\"=",	// Will be used to split words
					wordsDelim = "=,;";				// Will also be appended to the list

		std::string current;
		bool recording=false;
		for(int i=0; i<line.size(); i++)
		{
			bool 	isDelimiter = (delimiters.find(line[i])!=std::string::npos),
				isWordDelim = (wordsDelim.find(line[i])!=std::string::npos);

			if(recording && isDelimiter)
			{
				split.push_back(current);
				recording = false;
				current.clear();

				if(isWordDelim) // some Delimiter are worth putting in the stream
				{
					split.push_back( "" );
					split.back() += line[i];
				}
			}
			else if(isWordDelim)
			{
				split.push_back( "" );
				split.back() += line[i];
			}
			else if(!isDelimiter)
			{
				current += line[i];
				recording = true;
			}
		}

		if(!current.empty())
			split.push_back(current);
	}

	GLenum ShaderSource::parseUniformTypeCode(const std::string& str, const std::string& cpl)
	{
		// Compare typename to known types :
		// (see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for more)
		GLenum typeCode;
		if(	str	== "float")				typeCode = GL_FLOAT;
		else if(str	== "vec2")				typeCode = GL_FLOAT_VEC2;
		else if(str	== "vec3")				typeCode = GL_FLOAT_VEC3;
		else if(str	== "vec4")				typeCode = GL_FLOAT_VEC4;
		else if(str	== "double")				typeCode = GL_DOUBLE;
		else if(str	== "dvec2")				typeCode = GL_DOUBLE_VEC2;
		else if(str	== "dvec3")				typeCode = GL_DOUBLE_VEC3;
		else if(str	== "dvec4")				typeCode = GL_DOUBLE_VEC4;
		else if(str	== "int")				typeCode = GL_INT;
		else if(str	== "ivec2")				typeCode = GL_INT_VEC2;
		else if(str	== "ivec3")				typeCode = GL_INT_VEC3;
		else if(str	== "ivec4")				typeCode = GL_INT_VEC4;
		else if(str	== "uvec2")				typeCode = GL_UNSIGNED_INT_VEC2;
		else if(str	== "uvec3")				typeCode = GL_UNSIGNED_INT_VEC3;
		else if(str	== "uvec4")				typeCode = GL_UNSIGNED_INT_VEC4;
		else if(str	== "bool")				typeCode = GL_BOOL;
		else if(str	== "bvec2")				typeCode = GL_BOOL_VEC2;
		else if(str	== "bvec3")				typeCode = GL_BOOL_VEC3;
		else if(str	== "bvec4")				typeCode = GL_BOOL_VEC4;
		else if(str	== "mat2" || str=="mat2x2")		typeCode = GL_FLOAT_MAT2;
		else if(str	== "mat3" || str=="mat3x3")		typeCode = GL_FLOAT_MAT3;
		else if(str	== "mat4" || str=="mat4x4")		typeCode = GL_FLOAT_MAT4;
		/*else if(str	== "mat2")				typeCode = GL_DOUBLE_MAT2;
		else if(str	== "mat3")				typeCode = GL_DOUBLE_MAT3;
		else if(str	== "mat4")				typeCode = GL_DOUBLE_MAT4;*/
		else if(str	== "sampler2D")				typeCode = GL_SAMPLER_2D;
		else if(str	== "usampler2D")			typeCode = GL_SAMPLER_2D;
		else if(str	== "isampler2D")			typeCode = GL_SAMPLER_2D;
		else if(str	== "sampler2DRect​")			typeCode = GL_SAMPLER_2D;
		else if(str	== "usampler2DRect​")			typeCode = GL_SAMPLER_2D;
		else if(str	== "isampler2DRect​")			typeCode = GL_SAMPLER_2D;
		else if(str	== "unsigned" && cpl=="int")		typeCode = GL_UNSIGNED_INT;
 		else
			throw Exception("ShaderSource::parseUniformLine - Unknown or unsupported uniform type \"" + str + "\".", __FILE__, __LINE__);

		return typeCode;
	}

	GLenum ShaderSource::parseOutTypeCode(const std::string& str, const std::string& cpl)
	{
		// Compare typename to known types :
		// (see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for more)
		GLenum typeCode;
		if(	str	== "float")				typeCode = GL_FLOAT;
		else if(str	== "vec2")				typeCode = GL_FLOAT_VEC2;
		else if(str	== "vec3")				typeCode = GL_FLOAT_VEC3;
		else if(str	== "vec4")				typeCode = GL_FLOAT_VEC4;
		else if(str	== "double")				typeCode = GL_DOUBLE;
		else if(str	== "dvec2")				typeCode = GL_DOUBLE_VEC2;
		else if(str	== "dvec3")				typeCode = GL_DOUBLE_VEC3;
		else if(str	== "dvec4")				typeCode = GL_DOUBLE_VEC4;
		else if(str	== "int")				typeCode = GL_INT;
		else if(str	== "ivec2")				typeCode = GL_INT_VEC2;
		else if(str	== "ivec3")				typeCode = GL_INT_VEC3;
		else if(str	== "ivec4")				typeCode = GL_INT_VEC4;
		else if(str	== "uvec2")				typeCode = GL_UNSIGNED_INT_VEC2;
		else if(str	== "uvec3")				typeCode = GL_UNSIGNED_INT_VEC3;
		else if(str	== "uvec4")				typeCode = GL_UNSIGNED_INT_VEC4;
		else if(str	== "bool")				typeCode = GL_BOOL;
		else if(str	== "bvec2")				typeCode = GL_BOOL_VEC2;
		else if(str	== "bvec3")				typeCode = GL_BOOL_VEC3;
		else if(str	== "bvec4")				typeCode = GL_BOOL_VEC4;
		else if(str	== "unsigned" && cpl=="int")		typeCode = GL_UNSIGNED_INT;
		else
			throw Exception("ShaderSource::parseOutLin - Unknown or unsupported out type \"" + str + "\".", __FILE__, __LINE__);

		return typeCode;
	}

	void ShaderSource::parseCode(void)
	{
		const std::string 	wordsDelim 	= ".,;/\\?*+-:#'\"",
					endOfCodeLine 	= ";";

		std::string line = source;

		inSamplers2D.clear();
		uniformVars.clear();
		uniformVarsType.clear();
		outFragments.clear();

		// Test compatibility needed :
		bool hasGl_FragColor = line.find("gl_FragColor");

		// Clean the code :
		while( removeBloc(line, "//", "\n", false) ) ;
		while( removeBloc(line, "/*", "*/", false) ) ;
		while( removeBloc(line, "{", "}", true) ) ;
		while( removeBloc(line, "(", ")", true) ) ;

		// Clean what remains of the code :
		std::vector<std::string> split;
		wordSplit(line, split);

		// Read it :
		const std::string 	versionKeyword 	= "#version",
					uniformKeyword 	= "uniform",
					outKeyword 	= "out";

		bool 	previousWasVersion 	= false,
			previousWasUniform 	= false,
			previousWasOut		= false,
			readingVarNames		= false,
			waitComa		= false;
		GLenum typeCode;
		for(int k=0; k<split.size(); k++)
		{
			if(endOfCodeLine.find(split[k])!=std::string::npos)
			{
				previousWasVersion 	= false;
				previousWasUniform 	= false;
				previousWasOut		= false;
				readingVarNames		= false;
				waitComa		= false;
			}
			else if(split[k]=="=")
				waitComa=true;
			else if(split[k]==",")
				waitComa=false;
			else if(split[k]==versionKeyword)
				previousWasVersion = true;
			else if(split[k]==uniformKeyword)
				previousWasUniform = true;
			else if(split[k]==outKeyword)
				previousWasOut = true;
			else if(previousWasVersion)
			{
				if(!fromString(split[k], versionNumber))
					throw Exception("ShaderSource::parseCode - GLSL version number cannot be read from string \"" + split[k] + "\".", __FILE__, __LINE__);
			}
			else if(previousWasUniform && !readingVarNames && k<split.size()-1)
			{
				typeCode = parseUniformTypeCode(split[k], split[k+1]);
				readingVarNames = true;
			}
			else if(!waitComa && previousWasUniform && readingVarNames)
			{
				if(typeCode != GL_SAMPLER_2D)
				{
					uniformVars.push_back(split[k]);
					uniformVarsType.push_back(typeCode);
				}
				else
					inSamplers2D.push_back(split[k]);
			}
			else if(previousWasOut && !readingVarNames && k<split.size()-1)
			{
				typeCode = parseOutTypeCode(split[k], split[k+1]);
				readingVarNames = true;
			}
			else if(!waitComa && previousWasOut && readingVarNames && split[k]!="," && split[k]!="=")
			{
				outFragments.push_back(split[k]);
			}
			// else
				// discard
		}

		if(outFragments.empty() && hasGl_FragColor)
		{
			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "ShaderSource::parseGlobals - Shader " << getSourceName() << " has no out vec4 variable gl_FragColor, falling into compatibility mode." << std::endl;
			#endif
			outFragments.push_back(portNameForFragColor);
			compatibilityRequest = true;
		}
	}

	/**
	\fn    std::string ShaderSource::errorLog(std::string log)
	\brief Add some source code information to the output shader compilation log.
	\param log Input log to be mixed with source code.
	\return Enhanced log.
	**/
	std::string ShaderSource::errorLog(std::string log)
	{
		bool 	firstLine = true,
			prevLineEnhancement = false,
			atLeastOneInfo = false;
		int	incriminatedLine = 0;

		const std::string 	delimAMDATI = "ERROR: 0:",
					delimINTEL = "0:";
		std::string buggyLine;
		HandleOpenGL::SupportedVendor v = HandleOpenGL::getVendorID();
		std::stringstream str("");
		std::istringstream iss(log);
		std::string line;

		if(!sourceName.empty())
		{
			str << "Shader Name : " << sourceName << std::endl;
			atLeastOneInfo = true;
		}

		if(!sourceInfo.empty())
		{
			str << "Shader Info : " << sourceInfo << std::endl;
			atLeastOneInfo = true;
		}

		if(!atLeastOneInfo)
			str << "Shader compilation log : " << std::endl;

		while( std::getline( iss, line) )
		{
			size_t	one = 0,
				two = 0;

			if(firstLine)
				firstLine = false;
			else if(v!=HandleOpenGL::vd_UNKNOWN && prevLineEnhancement)
			{
				str << std::endl;
				prevLineEnhancement = true;
			}

			str << "    " << line << std::endl;

			// Get the corresponding line :
			buggyLine.clear();
			switch(v)
			{
				case HandleOpenGL::vd_NVIDIA :
					one = line.find('(') + 1;
					two = line.find(')');
					if(fromString(line.substr(one, two-one), incriminatedLine))
					{
						incriminatedLine++;
						buggyLine = getLine(incriminatedLine-2);
					}
					break;
				case HandleOpenGL::vd_INTEL :
					one = line.find(delimINTEL);
					if(one!=std::string::npos)
					{
						two = line.find('(',one+delimINTEL.size()+1);
						if(two!=std::string::npos)
						{
							if(fromString(line.substr(one+delimINTEL.size(), two-one-delimINTEL.size()), incriminatedLine))
							{
								incriminatedLine++;
								buggyLine = getLine(incriminatedLine-2);
								prevLineEnhancement = true;
							}
						}
					}
					break;
				case HandleOpenGL::vd_AMDATI :
					one = line.find(delimAMDATI);
					if(one!=std::string::npos)
					{
						two = line.find(':',one+delimAMDATI.size()+1);
						if(two!=std::string::npos)
						{
							if(fromString(line.substr(one+delimAMDATI.size(), two-one-delimAMDATI.size()), incriminatedLine))
							{
								buggyLine = getLine(incriminatedLine-1);
								prevLineEnhancement = true;
							}
						}
					}
					break;
				case HandleOpenGL::vd_UNKNOWN :
					break;
			}

			// Print log :
			if(!buggyLine.empty())
			{
				str << "        At line ";
				if(lineOffset>=0)
					str << lineOffset + incriminatedLine;
				else
					str << incriminatedLine;

				str << " : \"" << buggyLine << "\"" << std::endl;
			}
		}

		return std::string(str.str());
	}

	/**
	\fn    bool ShaderSource::requiresCompatibility(void) const
	\brief Returns true if this Shader is using gl_FragColor and no out vec4 variables (e.g. Mesa <9.0 compatibility for Intel Core I7 with HD Graphics (>2nd Generation)); no call to glBindFragDataLocation is needed). If true, the input vars are indexed on their order of appearance in the shader source.
	\return Returns true if this Shader is using gl_FragColor and no out vec4 variables.
	**/
	bool ShaderSource::requiresCompatibility(void) const
	{
		return compatibilityRequest;
	}

	/**
	\fn    int ShaderSource::getVersion(void) const
	\brief Returns the shader version defined in the source (with #version) as an integer.
	\return Returns the version as an integer (e.g. 130 for 1.30) or 0 if no version was defined.
	**/
	int ShaderSource::getVersion(void) const
	{
		return versionNumber;
	}

	/**
	\fn    const std::string& ShaderSource::getSource(void) const
	\brief Returns the source as a standard string.
	\return A standard string.
	**/
	const std::string& ShaderSource::getSource(void) const
	{
		return source;
	}

	/**
	\fn    const std::string& ShaderSource::getSourceName(void) const
	\brief Return the name of the source : the filename if it was loaded from a file or <Inner String> otherwise.
	\return A standard string.
	**/
	const std::string& ShaderSource::getSourceName(void) const
	{
		return sourceName;
	}

	/**
	\fn    const char* ShaderSource::getSourceCstr(void) const
	\brief Returns the source as a characters table.
	\return A const char* string (including \0).
	**/
	const char* ShaderSource::getSourceCstr(void) const
	{
		return source.c_str();
	}

	/**
	\fn const std::vector<std::string>& ShaderSource::getInputVars(void) const
	\brief Return a vector containing the name of all the input textures (uniform sampler*).
	\return A vector of standard string.
	**/
	const std::vector<std::string>& ShaderSource::getInputVars(void) const
	{
		return inSamplers2D;
	}

	/**
	\fn const std::vector<std::string>& ShaderSource::getOutputVars(void) const
	\brief Return a vector containing the name of all the output textures (uniform {vec2, vec3, vec4, ...}).
	\return A vector of standard string.
	**/
	const std::vector<std::string>& ShaderSource::getOutputVars(void) const
	{
		return outFragments;
	}

	/**
	\fn const std::vector<std::string>& ShaderSource::getUniformVars(void) const
	\brief Return a vector containing the name of all the uniform variables which are not 2D samplers.
	\return A vector of standard string.
	**/
	const std::vector<std::string>& ShaderSource::getUniformVars(void) const
	{
		return uniformVars;
	}

	/**
	\fn const std::vector<GLenum>& ShaderSource::getUniformTypes(void) const
	\brief Return a vector containing the type of the uniform variables which are not 2D samplers (see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for a table of possible values).
	\return A vector of GLenum.
	**/
	const std::vector<GLenum>& ShaderSource::getUniformTypes(void) const
	{
		return uniformVarsType;
	}

