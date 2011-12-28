/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
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
 * \version 0.6
 * \date    August 15th 2011
**/

#include "ShaderSource.hpp"
#include "Exception.hpp"

    using namespace Glip::CoreGL;

	/**
	\fn    ShaderSource::ShaderSource(const ShaderSource& ss)
	\brief ShaderSource Construtor.

	\param ss Source code of the shader, as a copy.
	**/
	ShaderSource::ShaderSource(const ShaderSource& ss)
	{
		source     = ss.source;
		sourceName = ss.sourceName;
		inVars     = ss.inVars;
		outVars    = ss.outVars;
	}

	/**
	\fn    ShaderSource::ShaderSource(const char** src, bool eol, int lines)
	\brief ShaderSource Construtor.

	\param src Pointer to table of lines.
	\param eol Set to true if the code contains End Of Line delimiters.
	\param lines Number of lines to consider.
	**/
	ShaderSource::ShaderSource(const char** src, bool eol, int lines)
	{
		if(src==NULL)
			throw Exception("ShaderSource::ShaderSource - Can't load from NULL", __FILE__, __LINE__);

		source.clear();
		sourceName = "<Inner String>";

		if(lines>1)
			{
				for(int i = 1; i<lines; i++)
				{
					if(src[i]!=NULL) source += src[i];
					if(eol)          source += "\n";
				}
			}
		else
		{
			int i = 0;
			while(src[i]!=NULL)
			{
				source += src[i];
				if(eol) source += "\n";
				i++;
			}
		}

		parseGlobals();
	}

	/**
	\fn    ShaderSource::ShaderSource(const std::string& src)
	\brief Constructor of ShaderSource, load from a standard string or a file.

	\param src Source data (must have at least one '\n') or filename (without '\n')
	**/
	ShaderSource::ShaderSource(const std::string& src)
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
			sourceName = "<Inner String>";
			source.clear();
			source = src;
		}

		parseGlobals();
	}

	/**
	\fn    std::string ShaderSource::getLine(int l)
	\brief Get a line from the source code (delemiter : \\n)

	\param l Index of desired line (start at 0).

	\return Standard string or <error> in case of failure
	**/
	std::string ShaderSource::getLine(int l)
	{
		std::istringstream iss(source);
		std::string res="<error>";
		int i = 1;

		while( std::getline(iss, res) && i<l)
			i++;

		size_t pos = res.find_first_not_of(" \t\n");
		if(pos!=std::string::npos)
			res = res.substr(pos);

		return res;
	}

	int ShaderSource::removeKeyword(std::string& str, GLSL_KEYWORD kw)
	{
		size_t one = 0;
		int count = 0;
		std::string key(GLSLKeyword[static_cast<int>(kw)]);
		std::string separator(" \n\t,;");
		while((one=str.find(key, one))!=std::string::npos)
		{
			if(separator.find_first_of(str[one-1])!=std::string::npos && separator.find_first_of(str[one+key.length()])!=std::string::npos)
			{
				str.erase(one, key.length());
				count++;
			}
			else
				one += key.length();
		}

		return count;
	}

	int ShaderSource::removeAnyOfSampler(std::string& str)
	{
		GLSL_KEYWORD samplersKeywords[] = {	GLSL_KW_sampler1D,
							GLSL_KW_sampler2D,
							GLSL_KW_sampler3D,
							GLSL_KW_samplerCube,
							GLSL_KW_sampler1DShadow,
							GLSL_KW_sampler2DShadow,
							GLSL_KW_samplerCubeShadow,
							GLSL_KW_sampler1DArray,
							GLSL_KW_sampler2DArray,
							GLSL_KW_sampler1DArrayShadow,
							GLSL_KW_sampler2DArrayShadow,
							GLSL_KW_isampler1D,
							GLSL_KW_isampler2D,
							GLSL_KW_isampler3D,
							GLSL_KW_isamplerCube,
							GLSL_KW_isampler1DArray,
							GLSL_KW_isampler2DArray,
							GLSL_KW_usampler1D,
							GLSL_KW_usampler2D,
							GLSL_KW_usampler3D,
							GLSL_KW_usamplerCube,
							GLSL_KW_usampler1DArray,
							GLSL_KW_usampler2DArray,
							GLSL_KW_sampler2DRect,
							GLSL_KW_sampler2DRectShadow,
							GLSL_KW_isampler2DRect,
							GLSL_KW_usampler2DRect,
							GLSL_KW_samplerBuffer,
							GLSL_KW_isamplerBuffer,
							GLSL_KW_usamplerBuffer,
							GLSL_KW_sampler2DMS,
							GLSL_KW_isampler2DMS,
							GLSL_KW_usampler2DMS,
							GLSL_KW_sampler2DMSArray,
							GLSL_KW_isampler2DMSArray,
							GLSL_KW_usampler2DMSArray,
							GLSL_KW_samplerCubeArray,
							GLSL_KW_samplerCubeArrayShadow,
							GLSL_KW_isamplerCubeArray,
							GLSL_KW_usamplerCubeArray,
							GLSL_KW_END};
		int i  = 0,
		ct = 0;

		while(samplersKeywords[i]!=GLSL_KW_END)
		{
			ct += removeKeyword(str,samplersKeywords[i]);
			i++;
		}

		return ct;
	}

	void ShaderSource::parseGlobals(void)
	{
		std::string global1, global2, global3, global4;
		size_t one = 0,
		two = 0;
		inVars.clear();
		outVars.clear();

		// Remove all scopes {}
		for(int i=0; i<source.length(); i++)
		{
			if(source[i]=='{')
			{
				int level=1;
				while(level>0 && i<source.length())
				{
					if(source[i]=='{') level++;
					if(source[i]=='}') level--;
					i++;
				}
			}
			else
				global1 += source[i];
		}

		// Remove all parenthesis
		for(int i=0; i<global1.length(); i++)
		{
			if(global1[i]=='(')
			{
				int level=1;
				while(level>0 && i<global1.length())
				{
					if(global1[i]=='(') level++;
					else if(global1[i]==')') level--;
					i++;
				}
				global2 += ';'; // Add ; to the functions end (for the following parts)
			}
			else
				global2 += global1[i];
		}

		// Remove all commentaries
		for(int i=0; i<global2.length()-1; i++)
		{
			if(global2[i]=='/' && global2[i+1]=='*')
			{
				int level=1;
				while(level>0 && i<global2.length()-1)
				{
					if(global2[i]=='*' && global2[i+1]=='/') level--;
					i++;
				}
				i++;
			}
			else if(global2[i]=='/' && global2[i+1]=='/')
				while(global2[i]!='\n') i++;
			else
				global3 += global2[i];
		}

		// Remove all macros
		for(int i=0; i<global3.length()-1; i++)
		{
			if(global3[i]=='#')
				while(global3[i]!='\n') i++;
			else
				global4 += global3[i];
		}

		// cut in phrases along the ;
		std::vector<std::string> cutting;
		one = two = 0;
		while((two=global4.find_first_of(';', one))!=std::string::npos)
		{
			cutting.push_back(global4.substr(one, two-one));
			one = two+1;
		}

		for(std::vector<std::string>::iterator it=cutting.begin(); it!=cutting.end(); it++)
		{
			bool in  = false,
			out = false;
			// Find all descriptors :
			if(removeKeyword(*it, GLSL_KW_in)>0 || (removeKeyword(*it, GLSL_KW_uniform)>0 && removeAnyOfSampler(*it)>0))
				in = true;
			if(removeKeyword(*it, GLSL_KW_out)>0)
				out = true;
			for(int i = static_cast<int>(GLSL_KW_attribute); i<static_cast<int>(GLSL_KW_END); i++)
				removeKeyword(*it, static_cast<GLSL_KEYWORD>(i));

			// *it contains only variables name :
			if(in || out)
			{
				one = two = 0;
				while((one = (*it).find_first_not_of("\n\t ,;", two))!=std::string::npos)
				{
					two = (*it).find_first_of("\n\t ,;", one+1);

					if(in)
						inVars.push_back((*it).substr(one, two-one));
					if(out)
						outVars.push_back((*it).substr(one, two-one));
				}
			}
		}
	}

	/**
	\fn    std::string ShaderSource::errorLog(std::string log)
	\brief Add some source code information to the output shader compilation log

	\param log Input log to be mixed with source code.

	\return Enhanced log.
	**/
	std::string ShaderSource::errorLog(std::string log)
	{
		std::stringstream str("");
		std::istringstream iss(log);
		std::string line;

		str << "Shader Name : " << sourceName << std::endl;

		while( std::getline( iss, line) )
		{
			str << "    " << line << std::endl;
			size_t one = line.find('(') + 1;
			size_t two = line.find(')');
			int tmp;
			from_string(line.substr(one, two-one), tmp);
			str << "        >> " << getLine(tmp) << std::endl;
		}

		return std::string(str.str());
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
	\fn const std::vector<std::string>& ShaderSource::getInputVars(void)
	\brief Return a vector containing the name of all the input textures (uniform sampler*).

	\return A vector of standard string.
	**/
	const std::vector<std::string>& ShaderSource::getInputVars(void)
	{
		return inVars;
	}

	/**
	\fn const std::vector<std::string>& ShaderSource::getOutputVars(void)
	\brief Return a vector containing the name of all the output textures (uniform sampler*).

	\return A vector of standard string.
	**/
	const std::vector<std::string>& ShaderSource::getOutputVars(void)
	{
		return outVars;
	}

// Constant :
	const char* Glip::CoreGL::GLSLKeyword[] =
	{
		"attribute\0",
		"const\0",
		"uniform\0",
		"varying\0",
		"layout\0",
		"centroid\0",
		"flat\0",
		"smooth\0",
		"noperspective\0",
		"patch\0",
		"sample\0",
		"break\0",
		"continue\0",
		"do\0",
		"for\0",
		"while\0",
		"switch\0",
		"case\0",
		"default\0",
		"if\0",
		"else\0",
		"subroutine\0",
		"in\0",
		"out\0",
		"inout\0",
		"float\0",
		"double\0",
		"int\0",
		"void\0",
		"bool\0",
		"true\0",
		"false\0",
		"invariant\0",
		"discard\0",
		"return\0",
		"mat2\0",
		"mat3\0",
		"mat4\0",
		"dmat2\0",
		"dmat3\0",
		"dmat4\0",
		"mat2x2\0",
		"mat2x3\0",
		"mat2x4\0",
		"dmat2x2\0",
		"dmat2x3\0",
		"dmat2x4\0",
		"mat3x2\0",
		"mat3x3\0",
		"mat3x4\0",
		"dmat3x2\0",
		"dmat3x3\0",
		"dmat3x4\0",
		"mat4x2\0",
		"mat4x3\0",
		"mat4x4\0",
		"dmat4x2\0",
		"dmat4x3\0",
		"dmat4x4\0",
		"vec2\0",
		"vec3\0",
		"vec4\0",
		"ivec2\0",
		"ivec3\0",
		"ivec4\0",
		"bvec2\0",
		"bvec3\0",
		"bvec4\0",
		"dvec2\0",
		"dvec3\0",
		"dvec4\0",
		"uint\0",
		"uvec2\0",
		"uvec3\0",
		"uvec4\0",
		"lowp\0",
		"mediump\0",
		"highp\0",
		"precision\0",
		"sampler1D\0",
		"sampler2D\0",
		"sampler3D\0",
		"samplerCube\0",
		"sampler1DShadow\0",
		"sampler2DShadow\0",
		"samplerCubeShadow\0",
		"sampler1DArray\0",
		"sampler2DArray\0",
		"sampler1DArrayShadow\0",
		"sampler2DArrayShadow\0",
		"isampler1D\0",
		"isampler2D\0",
		"isampler3D\0",
		"isamplerCube\0",
		"isampler1DArray\0",
		"isampler2DArray\0",
		"usampler1D\0",
		"usampler2D\0",
		"usampler3D\0",
		"usamplerCube\0",
		"usampler1DArray\0",
		"usampler2DArray\0",
		"sampler2DRect\0",
		"sampler2DRectShadow\0",
		"isampler2DRect\0",
		"usampler2DRect\0",
		"samplerBuffer\0",
		"isamplerBuffer\0",
		"usamplerBuffer\0",
		"sampler2DMS\0",
		"isampler2DMS\0",
		"usampler2DMS\0",
		"sampler2DMSArray\0",
		"isampler2DMSArray\0",
		"usampler2DMSArray\0",
		"samplerCubeArray\0",
		"samplerCubeArrayShadow\0",
		"isamplerCubeArray\0",
		"usamplerCubeArray\0",
		"struct\0",
		"common\0",
		"partition\0",
		"active\0",
		"asm\0",
		"class\0",
		"union\0",
		"enum\0",
		"typedef\0",
		"template\0",
		"this\0",
		"packed\0",
		"goto\0",
		"inline\0",
		"noinline\0",
		"volatile\0",
		"public\0",
		"static\0",
		"extern\0",
		"external\0",
		"interface\0",
		"long\0",
		"short\0",
		"half\0",
		"fixed\0",
		"unsigned\0",
		"superp\0",
		"input\0",
		"output\0",
		"hvec2\0",
		"hvec3\0",
		"hvec4\0",
		"fvec2\0",
		"fvec3\0",
		"fvec4\0",
		"sampler3DRect\0",
		"filter\0",
		"image1D\0",
		"image2D\0",
		"image3D\0",
		"imageCube\0",
		"iimage1D\0",
		"iimage2D\0",
		"iimage3D\0",
		"iimageCube\0",
		"uimage1D\0",
		"uimage2D\0",
		"uimage3D\0",
		"uimageCube\0",
		"image1DArray\0",
		"image2DArray\0",
		"iimage1DArray\0",
		"iimage2DArray\0",
		"uimage1DArray\0",
		"uimage2DArray\0",
		"image1DShadow\0",
		"image2DShadow\0",
		"image1DArrayShadow\0",
		"image2DArrayShadow\0",
		"imageBuffer\0",
		"iimageBuffer\0",
		"uimageBuffer\0",
		"sizeof\0",
		"cast\0",
		"namespace\0",
		"using\0",
		"row_major\0" // 185 lines
	};
