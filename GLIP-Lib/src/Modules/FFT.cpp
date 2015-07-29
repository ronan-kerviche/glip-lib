/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : FFT.cpp                                                                                   */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : FFT pipeline generators for gray level input (real and complex).                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT.cpp
 * \brief   FFT pipeline generators for gray level input (real and complex).
 * \author  R. KERVICHE
 * \date    August 20th 2012
**/

	// Includes
	#include <cmath>
	#include "Core/Exception.hpp"
	#include "Modules/FFT.hpp"
	#include "devDebugTools.hpp"
	#include "Core/ShaderSource.hpp"
	#include "Core/HdlShader.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::FFTModules;
	using namespace Glip::Modules::VanillaParserSpace;

// FFTModules :
namespace Glip
{
	namespace Modules
	{
		namespace FFTModules
		{
			/**
			\fn Flag Glip::Modules::FFTModules::getFlag(const std::string& str)
			\brief Get the flag corresponding to a string.
			\param str Name of the flag.
			\return The flag corresponding to the string or raise an Exception otherwise.
			**/ 
			Flag getFlag(const std::string& str)
			{
				#define TEST(a, b, c) if(str== a || str== b) return c ;
	
				TEST("Shifted",			"SHIFTED", 		Shifted);
				TEST("Inversed",		"INVERSED",		Inversed);
				TEST("ZeroPadding",		"ZERO_PADDING",		ZeroPadding);
				TEST("CompatibilityMode",	"COMPATIBILITY_MODE",	CompatibilityMode);
				TEST("NoInput",			"NO_INPUT",		NoInput);

				#undef TEST
		
				throw Exception("GenerateFFT1DPipeline::getFlag - Unknown flag name : \"" + str + "\".", __FILE__, __LINE__, Exception::ModuleException);
			}
		}
	}
}

// GenerateFFT1DPipeline :
	/**
	\fn GenerateFFT1DPipeline::GenerateFFT1DPipeline(void)
	\brief Module constructor.

	This object can be added to a LayoutLoader via LayoutLoader::addModule().
	**/
	GenerateFFT1DPipeline::GenerateFFT1DPipeline(void)
	 :	LayoutLoaderModule(	"GENERATE_FFT1D_PIPELINE", 
					"DESCRIPTION{Generate the 1D FFT Pipeline transformation.}"
					"ARGUMENT:width{Width, can be either a numeral or the name of an existing format.}"
					"ARGUMENT:name{Name of the new pipeline.}"
					"ARGUMENT:options...{Options to be used by the FFT process : SHIFTED, INVERSED, ZERO_PADDING, COMPATIBILITY_MODE, NO_INPUT.}"
					"BODY_DESCRIPTION{PRE{...} block contains a filtering function to be applied before the FFT. It must define a function vec4 pre(in vec4 colorFromTexture, in float x). POST{...} block contains a filtering function to be applied after the FFT. It must implement a function vec4 post(in vec4 colorAfterFFT, in float x). Both of these block can declare their own uniform variables.}",
					2,
					7, //2 base + 5 arguments
					0)
	{ }

	const std::string GenerateFFT1DPipeline::getInputPortName(void)
	{
		return "inputTexture";
	}

	const std::string GenerateFFT1DPipeline::getOutputPortName(void)
	{
		return "outputTexture";
	}

	ShaderSource GenerateFFT1DPipeline::generateRadix2Code(int width, int currentLevel, int flags, const ShaderSource& pre)
	{
		// with		: the width of the texture.
		// currentLevel : the current decimation level, between width (first pass) and 1 (last pass).

		// Position testing : 
		// f = @(x,l) mod((mod(x,l)-l/2),l)+floor(x/l)*l;
		// f(0:7, 8)
		// f(0:7, 4)

		// Modulation testing : 
		// g = @(x,l,w) floor(2*mod(x,l)/l).*mod(x,l/2)*(w/l);
		// g(0:7, 8, 8)
		// g(0:7, 4, 8)

		int lineCounter = 1;
		#define PUSH_LINE_INFO { linesInfo[lineCounter] = ShaderSource::LineInfo(__FILE__, __LINE__); lineCounter++; }

		std::string str;
		std::map<int,ShaderSource::LineInfo> linesInfo;

		str +="#version 130 \n";													PUSH_LINE_INFO
		str += "const float twoPi = 6.28318530718; \n";											PUSH_LINE_INFO
		
		if(width!=currentLevel || (flags & NoInput)==0) // Not First pass or has an input
		{
			str += "uniform sampler2D " + getInputPortName() + "; \n";								PUSH_LINE_INFO
		}

		if((flags & CompatibilityMode)==0)
		{
			str += "out vec4 " + getOutputPortName() + "; \n";									PUSH_LINE_INFO
		}
		else
		{
			str += "vec4 " + getOutputPortName() + "; \n";										PUSH_LINE_INFO
		}

		if(width==currentLevel && !pre.empty()) // First pass and pre-function
		{
			str += pre.getSource();
			for(int k=0; k<pre.getNumLines(); k++)
				linesInfo[lineCounter+k] = pre.getLineInfo(k+1);

			lineCounter += pre.getNumLines();
		}

		str += "\n";															PUSH_LINE_INFO
		str += "void main() \n";													PUSH_LINE_INFO
		str += "{ \n";															PUSH_LINE_INFO
		str += "    const int w = " + toString(width) + ", \n";										PUSH_LINE_INFO
		str += "              l = " + toString(currentLevel) + "; \n";									PUSH_LINE_INFO
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";										PUSH_LINE_INFO
		str += "    int posB = int(mod((mod(pos.x, l) - l/2), l) + int(pos.x/l)*l); \n";						PUSH_LINE_INFO

		if(width==currentLevel) // First pass
		{
			str += "    float p = floor((2*mod(pos.x+w/2,w))/w)*floor(mod(pos.x+w/2, w/2)); \n";					PUSH_LINE_INFO

			if((flags & Shifted)!=0 && (flags & Inversed)!=0)
			{
				str += "    pos.x = int(mod(pos.x + w/2, w)); \n";								PUSH_LINE_INFO
				str += "    posB = int(mod(posB + w/2, w)); \n";								PUSH_LINE_INFO
			}
			
			str += "    vec2 ivA = vec2((float(pos.x)+0.5)/float(w),0), \n";							PUSH_LINE_INFO
			str += "         ivB = vec2((float(posB)+0.5)/float(w),0); \n";								PUSH_LINE_INFO

			if((flags & NoInput)==0)
			{
				str += "    vec4 A = texture(" + getInputPortName() + ", ivA); \n";						PUSH_LINE_INFO
				str += "    vec4 B = texture(" + getInputPortName() + ", ivB); \n";						PUSH_LINE_INFO
			}
			else
			{
				str += "    vec4 A = vec4(0.0, 0.0, 0.0, 0.0); \n";								PUSH_LINE_INFO
				str += "    vec4 B = vec4(0.0, 0.0, 0.0, 0.0); \n";								PUSH_LINE_INFO
			}

			// Apply the pre-function, if given : 
			if(!pre.empty())
			{
				str += "    A = pre(A, ivA.x); \n";										PUSH_LINE_INFO
				str += "    B = pre(B, ivB.x); \n";										PUSH_LINE_INFO
			}
		}		
		else
		{
			str += "    float p = floor((2*mod(pos.x,l))/l)*floor(mod(pos.x, l/2))*(w/l); \n";					PUSH_LINE_INFO
			str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(pos.x,0), 0); \n";					PUSH_LINE_INFO
			str += "    vec4 B = texelFetch(" + getInputPortName() + ", ivec2(posB,0), 0); \n";					PUSH_LINE_INFO
		}

		str += "    float c = cos(-twoPi*p/float(w)), \n";										PUSH_LINE_INFO
		str += "          s = sin(-twoPi*p/float(w)); \n"; 										PUSH_LINE_INFO
		

		if(width==currentLevel) // First pass
		{
			if((flags & Inversed)!=0)
			{
				str += "    A.g = -A.g; \n"; /* real */										PUSH_LINE_INFO
				str += "    B.g = -B.g; \n"; /* imaginary */									PUSH_LINE_INFO
			}

			str += "    " + getOutputPortName() + ".r  = A.r + B.r; \n"; /* real */							PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".g  = A.g + B.g; \n"; /* imaginary */						PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".b  = (A.r - B.r)*c - (A.g - B.g)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".a  = (A.r - B.r)*s + (A.g - B.g)*c; \n"; /* imaginary */			PUSH_LINE_INFO
		}
		else
		{
			str += "    float g = float(posB>pos.x)*2.0 - 1.0; \n";
			str += "    " + getOutputPortName() + ".r  = (g*A.r + B.r)*c - (g*A.g + B.g)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".g  = (g*A.r + B.r)*s + (g*A.g + B.g)*c; \n"; /* imaginary */			PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".b  = (g*A.b + B.b)*c - (g*A.a + B.a)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".a  = (g*A.b + B.b)*s + (g*A.a + B.a)*c; \n"; /* imaginary */			PUSH_LINE_INFO
		}

		if((flags & CompatibilityMode)!=0)
		{
			str += "    gl_FragColor = " + getOutputPortName() + "; \n";								PUSH_LINE_INFO
		}

		str += "} \n";															PUSH_LINE_INFO

		//std::cout << "GenerateFFT1DPipeline::generateStepCode(" << width << ", " << currentLevel << ")" << std::endl;
		//std::cout << str << std::endl;

		#undef PUSH_LINE_INFO

		return ShaderSource(str, "<GenerateFFT1DPipeline::generate(" + toString(width) +")>", 1, linesInfo);
	}

	ShaderSource GenerateFFT1DPipeline::generateLastShuffleCode(int width, int flags, const ShaderSource& post)
	{
		std::string str;
		std::map<int,ShaderSource::LineInfo> linesInfo;

		int lineCounter = 1;
		#define PUSH_LINE_INFO { linesInfo[lineCounter] = ShaderSource::LineInfo(__FILE__, __LINE__); lineCounter++; }

		str +="#version 130 \n";													PUSH_LINE_INFO	
		str += "uniform sampler2D " + getInputPortName() + "; \n";									PUSH_LINE_INFO

		if((flags & CompatibilityMode)==0)
		{
			str += "out vec4 " + getOutputPortName() + "; \n";									PUSH_LINE_INFO
		}		
		else
		{
			str += "vec4 " + getOutputPortName() + "; \n";										PUSH_LINE_INFO
		}

		if(!post.empty())
		{
			str += post.getSource();
			for(int k=0; k<post.getNumLines(); k++)
				linesInfo[lineCounter+k] = post.getLineInfo(k+1);

			lineCounter += post.getNumLines();
		}

		str += "\n";															PUSH_LINE_INFO
		str += "void main() \n";													PUSH_LINE_INFO
		str += "{ \n";															PUSH_LINE_INFO
		str += "    const int w = " + toString(width) + "; \n";										PUSH_LINE_INFO
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";										PUSH_LINE_INFO

		if((flags & Shifted)!=0 && (flags & Inversed)==0)
		{
			str += "    pos.x = int(mod(pos.x + w/2, w)); \n";									PUSH_LINE_INFO
		}

		str += "    int a = 0; \n";													PUSH_LINE_INFO
		str += "    for(int k=w/2; k>=1; k=k/2) a = a + int(mod(int(pos.x/k),2))*(w/(2*k)); \n"; /* Bit reversal */ 			PUSH_LINE_INFO
		str += "    int p = int(mod(a, w/2)); \n";											PUSH_LINE_INFO
		str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(p,0), 0); \n";						PUSH_LINE_INFO
		str += "    if(p<a) A.rg = A.ba; \n";												PUSH_LINE_INFO
	
		if((flags & Inversed)!=0)
		{
			str += "    A.rg = A.rg * vec2(1.0, -1.0)/w; \n";									PUSH_LINE_INFO
		}

		str += "    A.ba = vec2(length(A.rg), 1.0); \n";										PUSH_LINE_INFO

		// Apply the post-function, if given : 
		if(!post.empty())
		{
			str += "    A = post(A, gl_FragCoord.x/float(width)); \n";								PUSH_LINE_INFO
		}

		str += "    " + getOutputPortName() + " = A; \n";										PUSH_LINE_INFO

		if((flags & CompatibilityMode)!=0)
		{
			str += "    gl_FragColor = " + getOutputPortName() + "; \n";								PUSH_LINE_INFO
		}

		str += "} \n";															PUSH_LINE_INFO

		//std::cout << "GenerateFFT1DPipeline::generateLastShuffleCode(" << width << ")" << std::endl;
		//std::cout << str << std::endl;

		#undef PUSH_LINE_INFO

		return ShaderSource(str, "<GenerateFFT1DPipeline::generate(" + toString(width) +")>", 1, linesInfo);
	}

	/**
	\fn PipelineLayout GenerateFFT1DPipeline::generate(int width, int flags, const std::string& pre, const std::string& post)
	\brief Construct a pipeline performing a 1D FFT.
	\param width Width of the signal.
	\param flags Possible flags associated to the transformation (see Glip::Modules::FFTModules::Flag).
	\param pre Add a filtering function before the transform.
	\param post Add a filtering function after the transform.
	\return A complete pipeline layout.

	The <b>pre-function</b> will be inserted before the transform. It should be a block of code declaring the function <i>pre</i> as in the following example : 
	\code
	vec4 pre(in vec4 colorFromTexture, in float x)
	{
		return modifiedColor;
	}
	\endcode

	The <b>post-function</b> will be inserted after the transform. It should be a block of code declaring the function <i>post</i> as in the following example : 
	\code
	vec4 post(in vec4 colorAfterFFT, in float x)
	{
		return modifiedColor;
	}
	\endcode

	Each time, the position is normalized from 0.0 to 1.0. You are allowed to declare uniform variables in these filters.
	**/
	PipelineLayout GenerateFFT1DPipeline::generate(int width, int flags, const ShaderSource& pre, const ShaderSource& post)
	{
		double 	test1 = std::log(width)/std::log(2),
			test2 = std::floor(test1);

		if(test1!=test2)
			throw Exception("Size must be a power of 2 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ModuleException);
		if(width<4)
			throw Exception("Size must be at least 4 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ModuleException);

		if((flags & NoInput)!=0 && pre.empty())
			throw Exception("A PRE-function must be provided when using the flag NoInput.", __FILE__, __LINE__, Exception::ModuleException);

		if(!pre.getOutputVars().empty() && !pre.requiresCompatibility())
			throw Exception("A PRE-function cannot have an output (its own output port(s)).", __FILE__, __LINE__, Exception::ModuleException);

		HdlTextureFormat format(width, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		HdlTextureFormat halfFormat(width/2, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		PipelineLayout pipelineLayout("FFT1D" + toString(width) + "Pipeline");

		if((flags & NoInput)==0)
			pipelineLayout.addInput(getInputPortName());

		pipelineLayout.addOutput(getOutputPortName());

		std::string 	previousName = "",
				firstFilterName = "";

		for(int l=width; l>1; l/=2)
		{
			ShaderSource shader = generateRadix2Code(width, l, flags, pre);
			std::string name = "Filter"+ toString(l);
			FilterLayout filterLayout(name, halfFormat, shader);
			pipelineLayout.add(filterLayout,name);

			if(previousName.empty()) // First element
			{
				if((flags & NoInput)==0)
					pipelineLayout.connectToInput(getInputPortName(), name, getInputPortName());

				if(!pre.empty())
				{
					// Add the ports declared in PRE :
					for(std::vector<std::string>::const_iterator it=pre.getInputVars().begin(); it!=pre.getInputVars().end(); it++)
					{
						if((*it)==getInputPortName() && (flags & NoInput)==0)
							throw Exception("The PRE function cannot declare the input port \"" + getInputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
						else
						{
							pipelineLayout.addInput(*it);
							pipelineLayout.connectToInput(*it, name, *it);
						}
					}
				}

				firstFilterName = name;
			}
			else
				pipelineLayout.connect(previousName, getOutputPortName(), name, getInputPortName());
			
			previousName = name;
		}

		// Last : 
		const std::string shuffleFilterName = "FilterShuffle";
		ShaderSource shader = generateLastShuffleCode(width, flags, post);
		FilterLayout filterLayout(shuffleFilterName, format, shader);
		pipelineLayout.add(filterLayout,shuffleFilterName);
		pipelineLayout.connect(previousName, getOutputPortName(), shuffleFilterName, getInputPortName());
		
		// Add the ports declared in post : 
		if(!post.empty())
		{
			for(std::vector<std::string>::const_iterator it=post.getInputVars().begin(); it!=post.getInputVars().end(); it++)
			{
				if((*it)==getInputPortName() && (flags & NoInput)==0)
					throw Exception("The POST function cannot declare the input port \"" + getInputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
				else
				{
					pipelineLayout.addInput(*it);
					pipelineLayout.connectToInput(*it, shuffleFilterName, *it);
				}
			}

			for(std::vector<std::string>::const_iterator it=post.getOutputVars().begin(); it!=post.getOutputVars().end(); it++)
			{
				if((*it)==getOutputPortName() && !post.requiresCompatibility())
					throw Exception("The POST function cannot declare the output port \"" + getOutputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
				else if((*it)!=getOutputPortName())
				{
					pipelineLayout.addOutput(*it);
					pipelineLayout.connectToOutput(shuffleFilterName, *it, *it);
				}
			}
		}

		// Connect to output :
		pipelineLayout.connectToOutput(shuffleFilterName, getOutputPortName(), getOutputPortName());

		return pipelineLayout;
	}
	
	void GenerateFFT1DPipeline::apply(LAYOUT_LOADER_ARGUMENTS_LIST)
	{
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(dynamicPaths)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(geometryList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(mainPipelineName)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredSourceList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(startLine)
		UNUSED_PARAMETER(executionSource)		
		UNUSED_PARAMETER(executionSourceName)
		UNUSED_PARAMETER(executionStartLine)
	
		PIPELINE_MUST_NOT_EXIST( arguments[1] )

		int width = 0;
		CONST_ITERATOR_TO_FORMAT( itFormat, arguments.front() )
		if(VALID_ITERATOR_TO_FORMAT( itFormat ))
			width = itFormat->second.getWidth();
		else
		{
			CAST_ARGUMENT(0, int, _width)
			width = _width;
		}

		// Read the flags : 
		int flags = 0;
		for(unsigned int k=2; k<arguments.size(); k++)
		{
			FFTModules::Flag f = FFTModules::getFlag(arguments[k]);
			flags = flags | static_cast<int>(f);
		}

		// Read PRE and POST functions : 
		ShaderSource pre(""), post("");

		if(!body.empty())
		{
			VanillaParser parser(body, sourceName, bodyLine);
			bool 	preAlreadySet = false,
				postAlreadySet = false;

			for(std::vector<Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
			{
				if(it->strKeyword=="PRE")
				{
					if(preAlreadySet)
						throw Exception("PRE code already set.", it->sourceName, it->startLine, Exception::ClientScriptException);
				
					if(it->arguments.size()==1 && it->noBody)
					{
						SOURCE_MUST_EXIST( it->arguments.front() )
						CONST_ITERATOR_TO_SOURCE( its, it->arguments.front() )
						pre = its->second;
					}
					else if(it->noArgument && !it->body.empty())
					{
						ShaderSource src(it->body, it->sourceName, it->bodyLine);
						pre = src;
					}
					else 
						throw Exception("The PRE code can have either one argument or one body.", it->sourceName, it->startLine, Exception::ClientScriptException);

					preAlreadySet = true;
				}
				else if(it->strKeyword=="POST")
				{
					if(postAlreadySet)
						throw Exception("POST code already set.", it->sourceName, it->startLine, Exception::ClientScriptException);
				
					if(it->arguments.size()==1 && it->noBody)
					{
						SOURCE_MUST_EXIST( it->arguments.front() )
						CONST_ITERATOR_TO_SOURCE( its, it->arguments.front() )
						post = its->second;
					}
					else if(it->noArgument && !it->body.empty())
					{
						ShaderSource src(it->body, it->sourceName, it->bodyLine);
						post = src;
					}
					else 
						throw Exception("The PRE code can have either one argument or one body.", it->sourceName, it->startLine, Exception::ClientScriptException);

					preAlreadySet = true;
				}				
				else
					throw Exception("Unknown keyword \"" + it->strKeyword + "\". Expected PRE or POST.", it->sourceName, it->startLine, Exception::ClientScriptException);
			}
		}
			
		APPEND_NEW_PIPELINE(arguments[1], generate(width, flags, pre, post))
	}

// GenerateFFT2DPipeline :
	/**
	\fn GenerateFFT2DPipeline::GenerateFFT2DPipeline(void)
	\brief Module constructor.

	This object can be added to a LayoutLoader via LayoutLoader::addModule().
	**/
	GenerateFFT2DPipeline::GenerateFFT2DPipeline(void)
	 :	LayoutLoaderModule(	"GENERATE_FFT2D_PIPELINE", 
					"DESCRIPTION{Generate the 2D FFT Pipeline transformation.}"
					"ARGUMENT:width{Width, can be either a numeral or the name of an existing format.}"
					"ARGUMENT:height{Height, can be either a numeral or the name of an existing format.}"
					"ARGUMENT:name{Name of the new pipeline.}"
					"ARGUMENT:options...{Options to be used by the FFT process : SHIFTED, INVERSED, ZERO_PADDING, COMPATIBILITY_MODE, NO_INPUT.}"
					"BODY_DESCRIPTION{PRE{...} block contains a filtering function to be applied before the FFT. It must define a function vec4 pre(in vec4 colorFromTexture, in vec2 x). POST{...} block contains a filtering function to be applied after the FFT. It must implement a function vec4 post(in vec4 colorAfterFFT, in vec2 x). Both of these block can declare their own uniform variables.}",
					2,
					8, //3 base + 5 arguments
					0)
	{ }

	const std::string GenerateFFT2DPipeline::getInputPortName(void)
	{
		return "inputTexture";
	}

	const std::string GenerateFFT2DPipeline::getOutputPortName(void)
	{
		return "outputTexture";
	}

	ShaderSource GenerateFFT2DPipeline::generateRadix2Code(int width, int oppositeWidth, int currentLevel, int flags, bool horizontal, const ShaderSource& pre)
	{
		// Note that 'width' is generic here, it can be either the width or the height. 'oppositeWidth' is given as the other dimension.
		// Horizontal must be first.

		int lineCounter = 1;
		#define PUSH_LINE_INFO { linesInfo[lineCounter] = ShaderSource::LineInfo(__FILE__, __LINE__); lineCounter++; }

		std::string str;
		std::map<int,ShaderSource::LineInfo> linesInfo;

		str +="#version 130 \n";													PUSH_LINE_INFO
		str += "const float twoPi = 6.28318530718; \n";											PUSH_LINE_INFO
		
		if(!horizontal || width!=currentLevel || (flags & NoInput)==0) // Not First pass or has an input
		{
			str += "uniform sampler2D " + getInputPortName() + "; \n";								PUSH_LINE_INFO
		}

		if((flags & CompatibilityMode)==0)
		{
			str += "out vec4 " + getOutputPortName() + "; \n";									PUSH_LINE_INFO
		}
		else
		{
			str += "vec4 " + getOutputPortName() + "; \n";										PUSH_LINE_INFO
		}

		if(horizontal && width==currentLevel && !pre.empty()) // First pass and pre-function
		{
			str += pre.getSource();
			for(int k=0; k<pre.getNumLines(); k++)
				linesInfo[lineCounter+k] = pre.getLineInfo(k+1);

			lineCounter += pre.getNumLines();
		}

		str += "\n";															PUSH_LINE_INFO
		str += "void main() \n";													PUSH_LINE_INFO
		str += "{ \n";															PUSH_LINE_INFO
		str += "    const int w = " + toString(width) + ", \n";										PUSH_LINE_INFO
		str += "              h = " + toString(oppositeWidth) + ", \n";									PUSH_LINE_INFO
		str += "              l = " + toString(currentLevel) + "; \n";									PUSH_LINE_INFO
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";										PUSH_LINE_INFO

		if(!horizontal)
		{
			str += "    pos.xy = pos.yx; \n";											PUSH_LINE_INFO
		}

		str += "    int posB = int(mod((mod(pos.x, l) - l/2), l) + int(pos.x/l)*l); \n";						PUSH_LINE_INFO

		if(width==currentLevel) // First pass in the current direction
		{
			str += "    float p = floor((2*mod(pos.x+w/2,w))/w)*floor(mod(pos.x+w/2, w/2)); \n";					PUSH_LINE_INFO

			if((flags & Shifted)!=0 && (flags & Inversed)!=0)
			{
				str += "    pos.x = int(mod(pos.x + w/2, w)); \n";								PUSH_LINE_INFO
				str += "    posB = int(mod(posB + w/2, w)); \n";								PUSH_LINE_INFO
			}
			
			if(horizontal)
			{
				// Also real first pass :
				str += "    vec2 ivA = vec2((float(pos.x)+0.5)/float(w),(float(pos.y)+0.5)/float(h)), \n";			PUSH_LINE_INFO
				str += "         ivB = vec2((float(posB)+0.5)/float(w),(float(pos.y)+0.5)/float(h)); \n";			PUSH_LINE_INFO

				if((flags & NoInput)==0)
				{
					str += "    vec4 A = texture(" + getInputPortName() + ", ivA); \n";					PUSH_LINE_INFO
					str += "    vec4 B = texture(" + getInputPortName() + ", ivB); \n";					PUSH_LINE_INFO
				}
				else
				{
					str += "    vec4 A = vec4(0.0, 0.0, 0.0, 0.0); \n";							PUSH_LINE_INFO
					str += "    vec4 B = vec4(0.0, 0.0, 0.0, 0.0); \n";							PUSH_LINE_INFO
				}

				// Apply the pre-function, if given : 
				if(!pre.empty())
				{
					str += "    A = pre(A, ivA); \n";									PUSH_LINE_INFO
					str += "    B = pre(B, ivB); \n";									PUSH_LINE_INFO
				}
			}
			else
			{
				// Second pass, read from first horizontal pass compressed format :
				if((flags & Shifted)!=0 && (flags & Inversed)==0)
				{
					str += "    pos.y = int(mod(pos.y + w/2, w)); \n";							PUSH_LINE_INFO
				}

				str += "    int a = 0; \n";											PUSH_LINE_INFO
				str += "    for(int k=w/2; k>=1; k=k/2) a = a + int(mod(int(pos.y/k),2))*(w/(2*k)); \n"; 			PUSH_LINE_INFO
				str += "    pos.y = int(mod(a, w/2));  \n";									PUSH_LINE_INFO

				// Read : 
				str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(pos.y,pos.x), 0); \n";			PUSH_LINE_INFO
				str += "    vec4 B = texelFetch(" + getInputPortName() + ", ivec2(pos.y,posB), 0); \n";				PUSH_LINE_INFO

				// read from first horizontal pass, second part : 
				str += "    if(pos.y<a) { A.rg = A.ba; B.rg = B.ba; } \n";							PUSH_LINE_INFO

				if((flags & Inversed)!=0)
				{
					str += "    A.rg = A.rg * vec2(1.0, -1.0)/h; \n";							PUSH_LINE_INFO
					str += "    B.rg = B.rg * vec2(1.0, -1.0)/h; \n";							PUSH_LINE_INFO
				}
			}
		}		
		else
		{
			str += "    float p = floor((2*mod(pos.x,l))/l)*floor(mod(pos.x, l/2))*(w/l); \n";					PUSH_LINE_INFO

			if(horizontal)
			{
				str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(pos.x,pos.y), 0); \n";			PUSH_LINE_INFO
				str += "    vec4 B = texelFetch(" + getInputPortName() + ", ivec2(posB,pos.y), 0); \n";				PUSH_LINE_INFO
			}
			else
			{
				str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(pos.y,pos.x), 0); \n";			PUSH_LINE_INFO
				str += "    vec4 B = texelFetch(" + getInputPortName() + ", ivec2(pos.y,posB), 0); \n";				PUSH_LINE_INFO
			}
		}

		str += "    float c = cos(-twoPi*p/float(w)), \n";										PUSH_LINE_INFO
		str += "          s = sin(-twoPi*p/float(w)); \n"; 										PUSH_LINE_INFO
		

		if(width==currentLevel) // First pass
		{
			if((flags & Inversed)!=0)
			{
				str += "    A.g = -A.g; \n"; /* real */										PUSH_LINE_INFO
				str += "    B.g = -B.g; \n"; /* imaginary */									PUSH_LINE_INFO
			}

			str += "    " + getOutputPortName() + ".r  = A.r + B.r; \n"; /* real */							PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".g  = A.g + B.g; \n"; /* imaginary */						PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".b  = (A.r - B.r)*c - (A.g - B.g)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".a  = (A.r - B.r)*s + (A.g - B.g)*c; \n"; /* imaginary */			PUSH_LINE_INFO
		}
		else
		{
			str += "    float g = float(posB>pos.x)*2.0 - 1.0; \n";									PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".r  = (g*A.r + B.r)*c - (g*A.g + B.g)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".g  = (g*A.r + B.r)*s + (g*A.g + B.g)*c; \n"; /* imaginary */			PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".b  = (g*A.b + B.b)*c - (g*A.a + B.a)*s; \n"; /* real */				PUSH_LINE_INFO
			str += "    " + getOutputPortName() + ".a  = (g*A.b + B.b)*s + (g*A.a + B.a)*c; \n"; /* imaginary */			PUSH_LINE_INFO
		}

		if((flags & CompatibilityMode)!=0)
		{
			str += "    gl_FragColor = " + getOutputPortName() + "; \n";								PUSH_LINE_INFO
		}

		str += "} \n";

		#undef PUSH_LINE_INFO

		//std::cout << "GenerateFFT2DPipeline::generateStepCode(" << width << ", " << currentLevel << ")" << std::endl;
		//std::cout << str << std::endl;

		return ShaderSource(str, "<GenerateFFT1DPipeline::generate(" + toString(width) +")>", 1, linesInfo);
	}

	ShaderSource GenerateFFT2DPipeline::generateLastShuffleCode(int width, int oppositeWidth, int flags, bool horizontal, const ShaderSource& post)
	{
		// Note that 'width' is generic here, it can be either the width or the height. 'oppositeWidth' is given as the other dimension.
		// Horizontal must be first.

		std::string str;
		std::map<int,ShaderSource::LineInfo> linesInfo;

		int lineCounter = 1;
		#define PUSH_LINE_INFO { linesInfo[lineCounter] = ShaderSource::LineInfo(__FILE__, __LINE__); lineCounter++; }

		str += "#version 130 \n";													PUSH_LINE_INFO
		str += "uniform sampler2D " + getInputPortName() + "; \n";									PUSH_LINE_INFO

		if((flags & CompatibilityMode)==0)
		{
			str += "out vec4 " + getOutputPortName() + "; \n";									PUSH_LINE_INFO
		}		
		else
		{
			str += "vec4 " + getOutputPortName() + "; \n";										PUSH_LINE_INFO
		}

		if(!horizontal && !post.empty())
		{
			str += post.getSource();
			for(int k=0; k<post.getNumLines(); k++)
				linesInfo[lineCounter+k] = post.getLineInfo(k+1);

			lineCounter += post.getNumLines();
		}

		str += "\n";															PUSH_LINE_INFO
		str += "void main() \n";													PUSH_LINE_INFO
		str += "{ \n";															PUSH_LINE_INFO
		str += "    const int w = " + toString(width) + ", \n";										PUSH_LINE_INFO
		str += "              h = " + toString(oppositeWidth) + "; \n";									PUSH_LINE_INFO
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";										PUSH_LINE_INFO

		if(!horizontal)
		{
			str += "    pos.xy = pos.yx; \n";											PUSH_LINE_INFO
		}

		if((flags & Shifted)!=0 && (flags & Inversed)==0)
		{
			str += "    pos.x = int(mod(pos.x + w/2, w)); \n";									PUSH_LINE_INFO
		}

		str += "    int a = 0; \n";													PUSH_LINE_INFO
		str += "    for(int k=w/2; k>=1; k=k/2) a = a + int(mod(int(pos.x/k),2))*(w/(2*k)); \n"; /* Bit reversal */			PUSH_LINE_INFO
		str += "    int p = int(mod(a, w/2));  \n";											PUSH_LINE_INFO

		if(horizontal)
		{
			str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(p,pos.y), 0); \n";					PUSH_LINE_INFO
		}
		else
		{
			str += "    vec4 A = texelFetch(" + getInputPortName() + ", ivec2(pos.y,p), 0); \n";					PUSH_LINE_INFO
		}

		str += "    if(p<a) A.rg = A.ba; \n";												PUSH_LINE_INFO
	
		if((flags & Inversed)!=0)
		{
			str += "    A.rg = A.rg * vec2(1.0, -1.0)/w; \n";									PUSH_LINE_INFO
		}

		str += "    A.ba = vec2(length(A.rg), 1.0); \n";										PUSH_LINE_INFO

		// Apply the post-function, if given : 
		if(!horizontal && !post.empty())
		{
			str += "    A = post(A, gl_FragCoord.xy/vec2(h, w)); \n";								PUSH_LINE_INFO
		}

		str += "    " + getOutputPortName() + " = A; \n";										PUSH_LINE_INFO

		if((flags & CompatibilityMode)!=0)
		{
			str += "    gl_FragColor = " + getOutputPortName() + "; \n";								PUSH_LINE_INFO
		}

		str += "} \n";															PUSH_LINE_INFO

		#undef PUSH_LINE_INFO

		//std::cout << "GenerateFFT2DPipeline::generateLastShuffleCode(" << width << ")" << std::endl;
		//std::cout << str << std::endl;

		return ShaderSource(str, "<GenerateFFT1DPipeline::generate(" + toString(width) +")>", 1, linesInfo);
	}

	/**
	\fn PipelineLayout GenerateFFT2DPipeline::generate(int width, int height, int flags)
	\brief Construct a pipeline performing a 2D FFT.
	\param width Width of the signal.
	\param height Height of the signal.
	\param flags Possible flags associated to the transformation (see Glip::Modules::FFTModules::Flag).
	\param pre Add a filtering function before the transform.
	\param post Add a filtering function after the transform.
	\return A complete pipeline layout.

	The <b>pre-function</b> will be inserted before the transform. It should be a block of code declaring the function <i>pre</i> as in the following example : 
	\code
	vec4 pre(in vec4 colorFromTexture, in vec2 pos)
	{
		return modifiedColor;
	}
	\endcode

	The <b>post-function</b> will be inserted after the transform. It should be a block of code declaring the function <i>post</i> as in the following example : 
	\code
	vec4 post(in vec4 colorAfterFFT, in vec2 pos)
	{
		return modifiedColor;
	}
	\endcode

	Each time, the position is normalized from 0.0 to 1.0. You are allowed to declare uniform variables in these filters.
	**/
	PipelineLayout GenerateFFT2DPipeline::generate(int width, int height, int flags, const ShaderSource& pre, const ShaderSource& post)
	{
		double 	test1w = std::log(width)/std::log(2),
			test2w = std::floor(test1w),
			test1h = std::log(height)/std::log(2),
			test2h = std::floor(test1h);

		if(test1w!=test2w)
			throw Exception("Width must be a power of 2 (current value : " + toString(width) + ").", __FILE__, __LINE__, Exception::ModuleException);
		if(width<4)
			throw Exception("Width must be at least 4 (current value : " + toString(width) + ").", __FILE__, __LINE__, Exception::ModuleException);

		if(test1h!=test2h)
			throw Exception("Height must be a power of 2 (current value : " + toString(height) + ").", __FILE__, __LINE__, Exception::ModuleException);
		if(height<4)
			throw Exception("Height must be at least 4 (current value : " + toString(height) + ").", __FILE__, __LINE__, Exception::ModuleException);

		if((flags & NoInput)!=0 && pre.empty())
			throw Exception("A PRE-function must be provided when using the flag NoInput.", __FILE__, __LINE__, Exception::ModuleException);

		if(!pre.getOutputVars().empty() && !pre.requiresCompatibility())
			throw Exception("A PRE-function cannot have an output (its own output port(s)).", __FILE__, __LINE__, Exception::ModuleException);

		HdlTextureFormat format(width, height, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		HdlTextureFormat halfWidthFormat(width/2, height, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		HdlTextureFormat halfHeightFormat(width, height/2, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		PipelineLayout pipelineLayout("FFT2D" + toString(width) + "x" + toString(height) + "Pipeline");

		if((flags & NoInput)==0)
			pipelineLayout.addInput(getInputPortName());

		pipelineLayout.addOutput(getOutputPortName());

		std::string 	previousName = "",
				firstFilterName = "";

		// Horizontal : 
		for(int l=width; l>1; l/=2)
		{
			ShaderSource shader = generateRadix2Code(width, height, l, flags, true, pre);
			std::string name = "FilterH"+ toString(l);
			FilterLayout filterLayout(name, halfWidthFormat, shader);
			pipelineLayout.add(filterLayout,name);

			if(previousName.empty())
			{
				if((flags & NoInput)==0)
					pipelineLayout.connectToInput(getInputPortName(), name, getInputPortName());

				if(!pre.empty())
				{
					// Add the ports declared in PRE :
					for(std::vector<std::string>::const_iterator it=pre.getInputVars().begin(); it!=pre.getInputVars().end(); it++)
					{
						if((*it)==getInputPortName() && (flags & NoInput)==0)
							throw Exception("The PRE function cannot declare the input port \"" + getInputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
						else
						{
							pipelineLayout.addInput(*it);
							pipelineLayout.connectToInput(*it, name, *it);
						}
					}
				}

				firstFilterName = name;
			}
			else
				pipelineLayout.connect(previousName, getOutputPortName(), name, getInputPortName());
			
			previousName = name;
		}

		// Intermediate shuffle (not needed anymore) : 
		/*const std::string intermediateShuffleFilterName = "FilterIntermediateShuffle";
		ShaderSource intermediateShader = generateLastShuffleCode(width, height, flags, true, post);
		FilterLayout intermediateFilterLayout(intermediateShuffleFilterName, format, intermediateShader);
		pipelineLayout.add(intermediateFilterLayout,intermediateShuffleFilterName);
		pipelineLayout.connect(previousName, getOutputPortName(), intermediateShuffleFilterName, inputPortName);
		previousName = intermediateShuffleFilterName;*/

		// Vertical : 
		for(int l=height; l>1; l/=2)
		{
			ShaderSource shader = generateRadix2Code(height, width, l, flags, false, pre);
			std::string name = "FilterV"+ toString(l);
			FilterLayout filterLayout(name, halfHeightFormat, shader);
			pipelineLayout.add(filterLayout,name);

			pipelineLayout.connect(previousName, getOutputPortName(), name, getInputPortName());
			previousName = name;
		}

		// Last : 
		const std::string finalShuffleFilterName = "FilterFinalShuffle";
		ShaderSource finalShader = generateLastShuffleCode(height, width, flags, false, post);
		FilterLayout finalFilterLayout(finalShuffleFilterName, format, finalShader);
		pipelineLayout.add(finalFilterLayout, finalShuffleFilterName);
		pipelineLayout.connect(previousName, getOutputPortName(), finalShuffleFilterName, getInputPortName());

		// Add the ports declared in post : 
		if(!post.empty())
		{
			for(std::vector<std::string>::const_iterator it=post.getInputVars().begin(); it!=post.getInputVars().end(); it++)
			{
				if((*it)==getInputPortName() && (flags & NoInput)==0)
					throw Exception("The POST function cannot declare the input port \"" + getInputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
				else
				{
					pipelineLayout.addInput(*it);
					pipelineLayout.connectToInput(*it, finalShuffleFilterName, *it);
				}
			}

			for(std::vector<std::string>::const_iterator it=post.getOutputVars().begin(); it!=post.getOutputVars().end(); it++)
			{
				if((*it)==getOutputPortName() && !post.requiresCompatibility())
					throw Exception("The POST function cannot declare the output port \"" + getOutputPortName() + "\" as it is already used.", __FILE__, __LINE__, Exception::ModuleException);
				else if((*it)!=getOutputPortName())
				{
					pipelineLayout.addOutput(*it);
					pipelineLayout.connectToOutput(finalShuffleFilterName, *it, *it);
				}
			}
		}

		// Connect to output :
		pipelineLayout.connectToOutput(finalShuffleFilterName, getOutputPortName(), getOutputPortName());

		return pipelineLayout;
	}
	
	void GenerateFFT2DPipeline::apply(LAYOUT_LOADER_ARGUMENTS_LIST)
	{
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(dynamicPaths)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(geometryList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(mainPipelineName)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredSourceList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(startLine)
		UNUSED_PARAMETER(executionSource)		
		UNUSED_PARAMETER(executionSourceName)
		UNUSED_PARAMETER(executionStartLine)

		PIPELINE_MUST_NOT_EXIST( arguments[2] )
		//CAST_ARGUMENT(0, int, width)
		//CAST_ARGUMENT(1, int, height)

		int 	width = 0,
			height = 0;

		CONST_ITERATOR_TO_FORMAT( itFormatWidth, arguments[0] )
		if(VALID_ITERATOR_TO_FORMAT( itFormatWidth ))
			width = itFormatWidth->second.getWidth();
		else
		{
			CAST_ARGUMENT(0, int, _width)
			width = _width;
		}

		CONST_ITERATOR_TO_FORMAT( itFormatHeight, arguments[1] )
		if(VALID_ITERATOR_TO_FORMAT( itFormatHeight ))
			height = itFormatHeight->second.getHeight();
		else
		{
			CAST_ARGUMENT(0, int, _height)
			height = _height;
		}

		// Read the flags : 
		int flags = 0;
		for(unsigned int k=3; k<arguments.size(); k++)
		{
			FFTModules::Flag f = FFTModules::getFlag(arguments[k]);
			flags = flags | static_cast<int>(f);
		}

		// Read PRE and POST functions : 
		ShaderSource pre(""), post("");

		if(!body.empty())
		{
			VanillaParser parser(body, sourceName, bodyLine);
			bool 	preAlreadySet = false,
				postAlreadySet = false;

			for(std::vector<Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
			{
				if(it->strKeyword=="PRE")
				{
					if(preAlreadySet)
						throw Exception("PRE code already set.", it->sourceName, it->startLine, Exception::ClientScriptException);
				
					if(it->arguments.size()==1 && it->noBody)
					{
						SOURCE_MUST_EXIST( it->arguments.front() )
						CONST_ITERATOR_TO_SOURCE( its, it->arguments.front() )
						pre = its->second;
					}
					else if(it->noArgument && !it->body.empty())
					{
						ShaderSource src(it->body, it->sourceName, it->bodyLine);
						pre = src;
					}
					else 
						throw Exception("The PRE code can have either one argument or one body.", it->sourceName, it->startLine, Exception::ClientScriptException);

					preAlreadySet = true;
				}
				else if(it->strKeyword=="POST")
				{
					if(postAlreadySet)
						throw Exception("POST code already set.", it->sourceName, it->startLine, Exception::ClientScriptException);
				
					if(it->arguments.size()==1 && it->noBody)
					{
						SOURCE_MUST_EXIST( it->arguments.front() )
						CONST_ITERATOR_TO_SOURCE( its, it->arguments.front() )
						post = its->second;
					}
					else if(it->noArgument && !it->body.empty())
					{
						ShaderSource src(it->body, it->sourceName, it->bodyLine);
						post = src;
					}
					else 
						throw Exception("The PRE code can have either one argument or one body.", it->sourceName, it->startLine, Exception::ClientScriptException);

					preAlreadySet = true;
				}				
				else
					throw Exception("Unknown keyword \"" + it->strKeyword + "\". Expected PRE or POST.", it->sourceName, it->startLine, Exception::ClientScriptException);
			}
		}

		APPEND_NEW_PIPELINE(arguments[2], generate(width, height, flags, pre, post))
	}

