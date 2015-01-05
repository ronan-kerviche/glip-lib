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
#include "devDebugTools.hpp"
#include "Core/OglInclude.hpp"

using namespace Glip;
using namespace Glip::CoreGL;

	std::string ShaderSource::portNameForFragColor = "outputTexture";

	/**
	\fn    ShaderSource::ShaderSource(const ShaderSource& ss)
	\brief ShaderSource Construtor.
	\param ss Source code of the shader, as a copy.
	**/
	ShaderSource::ShaderSource(const ShaderSource& ss)
	 :	source(ss.source),
		sourceName(ss.sourceName),
		lineFirstChar(ss.lineFirstChar),
		lineLength(ss.lineLength),
		inSamplers2D(ss.inSamplers2D),
		uniformVars(ss.uniformVars),
		uniformVarsType(ss.uniformVarsType),
		outFragments(ss.outFragments),
		compatibilityRequest(ss.compatibilityRequest),
		versionNumber(ss.versionNumber),
		startLine(ss.startLine),
		linesInfo(ss.linesInfo)
	{ }

	/**
	\fn    ShaderSource::ShaderSource(const std::string& src, const std::string& _sourceName, int _startLine, const std::map<int,LineInfo>& _linesInfo)
	\brief Constructor of ShaderSource, load from a standard string or a file.
	\param src Source data (must have at least one '\n') or filename (without '\n').
	\param _sourceName Name of the source.
	\param _startLine Correction added to the debugger info (also if loaded from a Layout File for example).
	\param _linesInfo Specific information about lines, see ShaderSource::linesInfo and ShaderSource::LineInfo.

	If the source name is left blank and the src parameter is actually a filename, it will be copied as the source name.
	**/
	ShaderSource::ShaderSource(const std::string& src, const std::string& _sourceName, int _startLine, const std::map<int,LineInfo>& _linesInfo)
	 :	sourceName(_sourceName),
		compatibilityRequest(false), 
		versionNumber(0),  
		startLine(_startLine),
		linesInfo(_linesInfo)
	{
		size_t newline = src.find('\n');

		if(newline==std::string::npos)
		{
			// Open File
			std::fstream file;
			file.open(src.c_str());

			// Did it fail?
			if(!file.is_open())
				throw Exception("ShaderSource::ShaderSource - Failed to open file : " + src, __FILE__, __LINE__, Exception::CoreException);

			source.clear();
			
			if(sourceName.empty())
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
			source = src;

		parseCode();
	}

	ShaderSource::~ShaderSource(void)
	{ }
	
	void ShaderSource::parseLines(void)
	{
		if(!source.empty())
		{
			size_t 	pre = 0,
				pos = source.find('\n');

			while(pre<source.length())
			{
				lineFirstChar.push_back(pre);

				if(pos==std::string::npos)
					pos = source.length()-1;

				lineLength.push_back(pos-pre+1);

				pre = pos+1;
				pos = source.find('\n', pre);	
			}
		}
	}

	bool ShaderSource::removeBlock(std::string& line, const std::string& bStart, const std::string& bEnd, bool nested)
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
		for(unsigned int i=0; i<line.size(); i++)
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
		if(	str	== "float")			typeCode = GL_FLOAT;
		else if(str	== "vec2")			typeCode = GL_FLOAT_VEC2;
		else if(str	== "vec3")			typeCode = GL_FLOAT_VEC3;
		else if(str	== "vec4")			typeCode = GL_FLOAT_VEC4;
		else if(str	== "double")			typeCode = GL_DOUBLE;
		else if(str	== "dvec2")			typeCode = GL_DOUBLE_VEC2;
		else if(str	== "dvec3")			typeCode = GL_DOUBLE_VEC3;
		else if(str	== "dvec4")			typeCode = GL_DOUBLE_VEC4;
		else if(str	== "int")			typeCode = GL_INT;
		else if(str	== "ivec2")			typeCode = GL_INT_VEC2;
		else if(str	== "ivec3")			typeCode = GL_INT_VEC3;
		else if(str	== "ivec4")			typeCode = GL_INT_VEC4;
		else if(str	== "uvec2")			typeCode = GL_UNSIGNED_INT_VEC2;
		else if(str	== "uvec3")			typeCode = GL_UNSIGNED_INT_VEC3;
		else if(str	== "uvec4")			typeCode = GL_UNSIGNED_INT_VEC4;
		else if(str	== "bool")			typeCode = GL_BOOL;
		else if(str	== "bvec2")			typeCode = GL_BOOL_VEC2;
		else if(str	== "bvec3")			typeCode = GL_BOOL_VEC3;
		else if(str	== "bvec4")			typeCode = GL_BOOL_VEC4;
		else if(str	== "mat2" || str=="mat2x2")	typeCode = GL_FLOAT_MAT2;
		else if(str	== "mat3" || str=="mat3x3")	typeCode = GL_FLOAT_MAT3;
		else if(str	== "mat4" || str=="mat4x4")	typeCode = GL_FLOAT_MAT4;
		/*else if(str	== "mat2")			typeCode = GL_DOUBLE_MAT2;
		else if(str	== "mat3")			typeCode = GL_DOUBLE_MAT3;
		else if(str	== "mat4")			typeCode = GL_DOUBLE_MAT4;*/
		else if(str	== "sampler2D")			typeCode = GL_SAMPLER_2D;
		else if(str	== "usampler2D")		typeCode = GL_SAMPLER_2D;
		else if(str	== "isampler2D")		typeCode = GL_SAMPLER_2D;
		else if(str   == "sampler2DRect")               typeCode = GL_SAMPLER_2D;
		else if(str   == "usampler2DRect")              typeCode = GL_SAMPLER_2D;
		else if(str   == "isampler2DRect")              typeCode = GL_SAMPLER_2D;
		//else if(str	== "sampler2DRect​")	typeCode = GL_SAMPLER_2D;
		//else if(str	== "usampler2DRect​")	typeCode = GL_SAMPLER_2D;
		//else if(str	== "isampler2DRect​")	typeCode = GL_SAMPLER_2D;
		else if(str	== "unsigned" && cpl=="int")	typeCode = GL_UNSIGNED_INT;
 		else
			throw Exception("ShaderSource::parseUniformLine - Unknown or unsupported uniform type \"" + str + "\".", __FILE__, __LINE__, Exception::GLException);

		return typeCode;
	}

	GLenum ShaderSource::parseOutTypeCode(const std::string& str, const std::string& cpl)
	{
		// Compare typename to known types :
		// (see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for more)
		GLenum typeCode;
		if(	str	== "float")			typeCode = GL_FLOAT;
		else if(str	== "vec2")			typeCode = GL_FLOAT_VEC2;
		else if(str	== "vec3")			typeCode = GL_FLOAT_VEC3;
		else if(str	== "vec4")			typeCode = GL_FLOAT_VEC4;
		else if(str	== "double")			typeCode = GL_DOUBLE;
		else if(str	== "dvec2")			typeCode = GL_DOUBLE_VEC2;
		else if(str	== "dvec3")			typeCode = GL_DOUBLE_VEC3;
		else if(str	== "dvec4")			typeCode = GL_DOUBLE_VEC4;
		else if(str	== "int")			typeCode = GL_INT;
		else if(str	== "ivec2")			typeCode = GL_INT_VEC2;
		else if(str	== "ivec3")			typeCode = GL_INT_VEC3;
		else if(str	== "ivec4")			typeCode = GL_INT_VEC4;
		else if(str	== "uvec2")			typeCode = GL_UNSIGNED_INT_VEC2;
		else if(str	== "uvec3")			typeCode = GL_UNSIGNED_INT_VEC3;
		else if(str	== "uvec4")			typeCode = GL_UNSIGNED_INT_VEC4;
		else if(str	== "bool")			typeCode = GL_BOOL;
		else if(str	== "bvec2")			typeCode = GL_BOOL_VEC2;
		else if(str	== "bvec3")			typeCode = GL_BOOL_VEC3;
		else if(str	== "bvec4")			typeCode = GL_BOOL_VEC4;
		else if(str	== "unsigned" && cpl=="int")	typeCode = GL_UNSIGNED_INT;
		else
			throw Exception("ShaderSource::parseOutLin - Unknown or unsupported out type \"" + str + "\".", __FILE__, __LINE__, Exception::GLException);

		return typeCode;
	}

	void ShaderSource::parseCode(void)
	{
		const std::string 	wordsDelim 	= ".,;/\\?*+-:#'\"",
					endOfCodeLine 	= ";";

		std::string tmpSource = source;

		inSamplers2D.clear();
		uniformVars.clear();
		uniformVarsType.clear();
		outFragments.clear();

		// Parse the lines :
		parseLines();

		// Test compatibility needed :
		bool hasGl_FragColor = tmpSource.find("gl_FragColor");

		// Clean the code :
		while( removeBlock(tmpSource, "//", "\n", false) ) ;
		while( removeBlock(tmpSource, "/*", "*/", false) ) ;
		while( removeBlock(tmpSource, "{", "}", true) ) ;
		while( removeBlock(tmpSource, "(", ")", true) ) ;

		// Clean what remains of the code :
		std::vector<std::string> split;
		wordSplit(tmpSource, split);

		if(split.empty())
			return ;

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
		for(unsigned int k=0; k<split.size(); k++)
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
					throw Exception("ShaderSource::parseCode - GLSL version number cannot be read from string \"" + split[k] + "\".", __FILE__, __LINE__, Exception::GLException);
			}
			else if(previousWasUniform && !readingVarNames && k<(split.size()-1))
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
			else if(previousWasOut && !readingVarNames && k<(split.size()-1))
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
	\fn ShaderSource& ShaderSource::operator=(const ShaderSource& c)
	\brief Copy operator.
	\param c Source.
	\return A reference to this.
	**/
	ShaderSource& ShaderSource::operator=(const ShaderSource& c)
	{
		source 			= c.source;
		sourceName 		= c.sourceName;
		lineFirstChar 		= c.lineFirstChar;
		lineLength 		= c.lineLength;
		inSamplers2D 		= c.inSamplers2D;
		uniformVars 		= c.uniformVars;
		uniformVarsType 	= c.uniformVarsType;
		outFragments 		= c.outFragments;
		compatibilityRequest 	= c.compatibilityRequest;
		versionNumber 		= c.versionNumber;
		linesInfo		= c.linesInfo;

		return (*this);
	}

	/**
	\fn Exception ShaderSource::errorLog(const std::string& log) const
	\brief Add some source code information to the output shader compilation log.
	\param log Input log to be mixed with source code.
	\return Enhanced log.
	**/
	Exception ShaderSource::errorLog(const std::string& log) const
	{	
		int incriminatedLine = 0;

		const std::string 	delimAMDATI = "ERROR: 0:",
					delimINTEL = "0:";
		std::string buggyLine;
		HandleOpenGL::SupportedVendor v = HandleOpenGL::getVendorID();
		Exception result(">> Shader Compilation Log :", getSourceName(), startLine, Exception::ClientScriptException);
		std::istringstream iss(log);
		std::string line;

		while(std::getline(iss, line))
		{
			size_t	one = 0,
				two = 0;

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
						buggyLine = getLine(incriminatedLine-1, false);
					}
					break;
				case HandleOpenGL::vd_INTEL :
					one = line.find(delimINTEL);
					if(one!=std::string::npos)
					{
						two = line.find('(',one+delimINTEL.size()+1);
						if(two!=std::string::npos && fromString(line.substr(one+delimINTEL.size(), two-one-delimINTEL.size()), incriminatedLine))
							buggyLine = getLine(incriminatedLine, false);
					}
					break;
				case HandleOpenGL::vd_AMDATI :
					one = line.find(delimAMDATI);
					if(one!=std::string::npos)
					{
						two = line.find(':',one+delimAMDATI.size()+1);
						if(two!=std::string::npos && fromString(line.substr(one+delimAMDATI.size(), two-one-delimAMDATI.size()), incriminatedLine))
							buggyLine = getLine(incriminatedLine, false);
					}
					break;
				case HandleOpenGL::vd_UNKNOWN :
					break;
			}

			// Print log :
			if(!buggyLine.empty())
			{
				LineInfo info = getLineInfo(incriminatedLine);
				Exception a(line, info.sourceName, info.lineNumber, Exception::ClientScriptException);
				Exception b(buggyLine, info.sourceName, info.lineNumber, Exception::ClientScriptException);
				result << b << a; // Must be in reverse here!
			}
			else if(incriminatedLine>1)
			{
				LineInfo info = getLineInfo(incriminatedLine);
				Exception e(line, info.sourceName, info.lineNumber, Exception::ClientScriptException);
				result << e;
			}
			else
			{
				Exception e(line, getSourceName(), startLine, Exception::ClientScriptException);
				result << e;
			}
		}

		return result;
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
	\brief Return the name of the source : the one given with the constructor if any, the filename if it was loaded from a file or a blank string otherwise.
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
	\fn int ShaderSource::getNumLines(void) const
	\brief Get the number of lines in this source.
	\return The number of lines in the source.
	**/
	int ShaderSource::getNumLines(void) const
	{
		return lineFirstChar.size();
	}

	/**
	\fn std::string ShaderSource::getLine(int l, bool withNewLine) const
	\brief Get a specific line from the source.
	\param l The line index. <b>The indices start at 1</b>.
	\param withNewLine Set to true if the returned string must contain the new-line character.
	\return A string containing the right excerpt, or an empty line if the index is out of bounds.
	**/
	std::string ShaderSource::getLine(int l, bool withNewLine) const
	{
		l--; // to 0 based indexing.
		if(l<0 || l>=static_cast<int>(lineFirstChar.size()))
			return "";
		else if(!withNewLine && source[lineFirstChar[l]+lineLength[l]-1]=='\n')
			return source.substr(lineFirstChar[l], lineLength[l]-1);
		else
			return source.substr(lineFirstChar[l], lineLength[l]);
	}

	/**
	\fn ShaderSource::LineInfo ShaderSource::getLineInfo(int l) const
	\brief Get information about the line at the given index.
	\param l The line index. <b>The indices start at 1</b>.

	See ShaderSource::linesInfo on how to supply specific line information.

	\return A ShaderSource::LineInfo containing the required information, especially the real line number.
	**/
	ShaderSource::LineInfo ShaderSource::getLineInfo(int l) const
	{
		std::map<int, LineInfo>::const_iterator it=linesInfo.find(l);

		if(it!=linesInfo.end())
			return it->second;
		else
		{
			LineInfo info;
			info.sourceName = sourceName;
			info.lineNumber = startLine+l-1;

			return info;
		}
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

