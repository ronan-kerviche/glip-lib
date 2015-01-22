/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FFT1D.cpp                                                                                 */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : 1D FFT for gray level input (real or complex).                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT1D.cpp
 * \brief   1D FFT for gray level input (real or complex).
 * \author  R. KERVICHE
 * \date    August 20th 2012
**/

	// Includes
	#include <cmath>
	#include "Core/Exception.hpp"
	#include "Modules/FFT1D.hpp"
	#include "devDebugTools.hpp"
	#include "Core/ShaderSource.hpp"
	#include "Core/HdlShader.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::FFTModules;

// FFTModules :
	Flag getFlag(const std::string& str)
	{
		#define TEST(a, b, c) if(str== a || str== b) return c ;
	
		TEST("Shifted",			"SHIFTED", 		Shifted);
		TEST("Inversed",		"INVERSED",		Inversed);
		TEST("CompatibilityMode",	"COMPATIBILITY_MODE",	CompatibilityMode);

		#undef TEST
		
		throw Exception("GenerateFFT1DPipeline::getFlag - Unknown flag name : \"" + str + "\".", __FILE__, __LINE__, Exception::ModuleException);
	}

// GenerateFFT1DPipeline :
	GenerateFFT1DPipeline::GenerateFFT1DPipeline(void)
	 :	LayoutLoaderModule(	"GENERATE_FFT1D_PIPELINE", 
					"Generate the 1D FFT Pipeline transformation.\n"
					"Options : SHIFTED, INVERSED, COMPATIBILITY_MODE.\n"
					"Arguments : width, name [, option, ...].",
					2,
					5, //2 base + 3 arguments
					-1)
	{ }

	std::string GenerateFFT1DPipeline::generateRadix2Code(int width, int currentLevel, int flags)
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

		std::string str;

		str +="#version 130 \n";
		str += "const float twoPi = 6.28318530718; \n";
		str += "uniform sampler2D inputTexture; \n";

		if((flags & CompatibilityMode)==0)
			str += "out vec4 outputTexture; \n";
		else
			str += "vec4 outputTexture; \n";
		
		str += "\n";
		str += "void main() \n";
		str += "{ \n";
		str += "    const int w = " + toString(width) + ", \n";
		str += "              l = " + toString(currentLevel) + "; \n";
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";
		str += "    int posB = int(mod((mod(pos.x, l) - l/2), l) + int(pos.x/l)*l); \n";

		if(width==currentLevel) // First pass
		{
			str += "    float p = floor((2*mod(pos.x+w/2,w))/w)*floor(mod(pos.x+w/2, w/2)); \n";

			if((flags & Shifted)!=0 && (flags & Inversed)!=0)
			{
				str += "    pos.x = int(mod(pos.x + w/2, w)); \n";
				str += "    posB = int(mod(posB + w/2, w)); \n";
			}
			
			str += "    vec4 A = texture(inputTexture, vec2((float(pos.x)+0.5)/float(w),0)); \n";
			str += "    vec4 B = texture(inputTexture, vec2((float(posB)+0.5)/float(w),0)); \n";
		}		
		else
		{
			str += "    float p = floor((2*mod(pos.x,l))/l)*floor(mod(pos.x, l/2))*(w/l); \n";
			str += "    vec4 A = texelFetch(inputTexture, ivec2(pos.x,0), 0); \n";
			str += "    vec4 B = texelFetch(inputTexture, ivec2(posB,0), 0); \n";
		}

		str += "    float c = cos(-twoPi*p/float(w)), \n";
		str += "          s = sin(-twoPi*p/float(w)); \n"; 
		

		if(width==currentLevel) // First pass
		{
			if((flags & Inversed)!=0)
			{
				str += "    A.g = -A.g; \n"; // real
				str += "    B.g = -B.g; \n"; // imaginary
			}

			str += "    outputTexture.r  = A.r + B.r; \n"; // real
			str += "    outputTexture.g  = A.g + B.g; \n"; // imaginary
			str += "    outputTexture.b  = (A.r - B.r)*c - (A.g - B.g)*s; \n"; // real
			str += "    outputTexture.a  = (A.r - B.r)*s + (A.g - B.g)*c; \n"; // imaginary
		}
		else
		{
			str += "    float g = float(posB>pos.x)*2.0 - 1.0; \n";
			str += "    outputTexture.r  = (g*A.r + B.r)*c - (g*A.g + B.g)*s; \n"; // real
			str += "    outputTexture.g  = (g*A.r + B.r)*s + (g*A.g + B.g)*c; \n"; // imaginary
			str += "    outputTexture.b  = (g*A.b + B.b)*c - (g*A.a + B.a)*s; \n"; // real
			str += "    outputTexture.a  = (g*A.b + B.b)*s + (g*A.a + B.a)*c; \n"; // imaginary
		}

		if((flags & CompatibilityMode)!=0)
			str += "    gl_FragColor = outputTexture; \n";

		str += "} \n";

		std::cout << "GenerateFFT1DPipeline::generateStepCode(" << width << ", " << currentLevel << ")" << std::endl;
		std::cout << str << std::endl;

		return str;
	}

	std::string GenerateFFT1DPipeline::generateLastShuffleCode(int width, int flags)
	{
		std::string str;

		str +="#version 130 \n";
		str += "uniform sampler2D inputTexture; \n";

		if((flags & CompatibilityMode)==0)
			str += "out vec4 outputTexture; \n";
		else
			str += "vec4 outputTexture; \n";

		if((flags & CompatibilityMode)!=0)
			str += "    gl_FragColor = outputTexture; \n";

		str += "\n";
		str += "void main() \n";
		str += "{ \n";
		str += "    const int w = " + toString(width) + "; \n";
		str += "    ivec2 pos = ivec2(gl_FragCoord.xy); \n";

		if((flags & Shifted)!=0 && (flags & Inversed)==0)
			str += "    pos.x = int(mod(pos.x + w/2, w)); \n";

		str += "    int a = 0; \n";
		str += "    for(int k=w/2; k>=1; k=k/2) a = a + int(mod(int(pos.x/k),2))*(w/(2*k)); \n"; // Bit reversal
		str += "    int p = int(mod(a, w/2)); // Prepare for the folding. \n";
		str += "    vec4 A = texelFetch(inputTexture, ivec2(p,0), 0); \n";
		str += "    if(p<a) A.rg = A.ba; \n";
	
		if((flags & Inversed)!=0)
			str += "    A.rg = A.rg * vec2(1.0, -1.0)/w; \n";

		str += "    A.ba = vec2(length(A.rg), 1.0); \n";
		str += "    outputTexture = A; \n";

		if((flags & CompatibilityMode)!=0)
			str += "    gl_FragColor = outputTexture; \n";

		str += "} \n";

		std::cout << "GenerateFFT1DPipeline::generateLastShuffleCode(" << width << ")" << std::endl;
		std::cout << str << std::endl;

		return str;
	}

	PipelineLayout GenerateFFT1DPipeline::generate(int width, int flags)
	{
		double 	test1 = std::log(width)/std::log(2),
			test2 = std::floor(test1);

		if(test1!=test2)
			throw Exception("Size must be a power of 2 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ClientScriptException);
		if(width<4)
			throw Exception("Size must be at least 4 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ClientScriptException);

		HdlTextureFormat format(width, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		HdlTextureFormat halfFormat(width/2, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		PipelineLayout pipelineLayout("FFT1D" + toString(width) + "Pipeline");
		pipelineLayout.addInput("inputTexture");
		pipelineLayout.addOutput("outputTexture");

		std::string 	previousName = "",
				firstFilterName = "";

		for(int l=width; l>1; l/=2)
		{
			ShaderSource shader(generateRadix2Code(width, l, flags), "<GenerateFFT1DPipeline::generate()>");
			std::string name = "Filter"+ toString(l);
			FilterLayout filterLayout(name, halfFormat, shader);
			pipelineLayout.add(filterLayout,name);

			if(previousName=="")
			{
				pipelineLayout.connectToInput("inputTexture", name, "inputTexture");
				firstFilterName = name;
			}
			else
				pipelineLayout.connect(previousName, "outputTexture", name, "inputTexture");
			
			previousName = name;
		}

		// Last : 
		const std::string shuffleFilterName = "FilterShuffle";
		ShaderSource shader(generateLastShuffleCode(width, flags), "<GenerateFFT1DPipeline::generate()>");
		FilterLayout filterLayout(shuffleFilterName, format, shader);
		pipelineLayout.add(filterLayout,shuffleFilterName);
		pipelineLayout.connect(previousName, "outputTexture", shuffleFilterName, "inputTexture");

		// Connect to output :
		pipelineLayout.connectToOutput(shuffleFilterName, "outputTexture", "outputTexture");

		return pipelineLayout;
	}
	
	LAYOUT_LOADER_MODULE_APPLY_IMPLEMENTATION( GenerateFFT1DPipeline )
	{
		UNUSED_PARAMETER(body)
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(dynamicPaths)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sharedCodeList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(geometryList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(startLine)
		UNUSED_PARAMETER(bodyLine)
		UNUSED_PARAMETER(executionCode)		

		PIPELINE_MUST_NOT_EXIST( arguments[1] )
		CAST_ARGUMENT(0, int, width)

		// Read the flags : 
		int flags = 0;
		for(unsigned int k=2; k<arguments.size(); k++)
		{
			FFTModules::Flag f = FFTModules::getFlag(arguments[k]);
			flags = flags | static_cast<int>(f);
		}

		APPEND_NEW_PIPELINE(arguments[1], generate(width, flags))
	}

// GenerateFFT2DPipeline :
	GenerateFFT2DPipeline::GenerateFFT2DPipeline(void)
	 :	LayoutLoaderModule(	"GENERATE_FFT1D_PIPELINE", 
					"Generate the 1D FFT Pipeline transformation.\n"
					"Options : SHIFTED, INVERSED, COMPATIBILITY_MODE.\n"
					"Arguments : width, height, name [, option, ...].",
					2,
					6, //3 base + 3 arguments
					-1)
	{ }

	std::string GenerateFFT2DPipeline::generateRadix2Code(int width, int currentLevel, int flags)
	{
		return "";
	}

	std::string GenerateFFT2DPipeline::generateLastShuffleCode(int width, int flags)
	{
		return "";
	}

	PipelineLayout GenerateFFT2DPipeline::generate(int width, int height, int flags)
	{
		/*double 	test1 = std::log(width)/std::log(2),
			test2 = std::floor(test1);

		if(test1!=test2)
			throw Exception("Size must be a power of 2 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ClientScriptException);
		if(width<4)
			throw Exception("Size must be at least 4 (current size : " + toString(width) + ").", __FILE__, __LINE__, Exception::ClientScriptException);

		HdlTextureFormat format(width, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		HdlTextureFormat halfFormat(width/2, 1, GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
		PipelineLayout pipelineLayout("FFT1D" + toString(width) + "Pipeline");
		pipelineLayout.addInput("inputTexture");
		pipelineLayout.addOutput("outputTexture");

		std::string 	previousName = "",
				firstFilterName = "";

		for(int l=width; l>1; l/=2)
		{
			ShaderSource shader(generateRadix2Code(width, l, flags), "<GenerateFFT1DPipeline::generate()>");
			std::string name = "Filter"+ toString(l);
			FilterLayout filterLayout(name, halfFormat, shader);
			pipelineLayout.add(filterLayout,name);

			if(previousName=="")
			{
				pipelineLayout.connectToInput("inputTexture", name, "inputTexture");
				firstFilterName = name;
			}
			else
				pipelineLayout.connect(previousName, "outputTexture", name, "inputTexture");
			
			previousName = name;
		}

		// Last : 
		const std::string shuffleFilterName = "FilterShuffle";
		ShaderSource shader(generateLastShuffleCode(width, flags), "<GenerateFFT1DPipeline::generate()>");
		FilterLayout filterLayout(shuffleFilterName, format, shader);
		pipelineLayout.add(filterLayout,shuffleFilterName);
		pipelineLayout.connect(previousName, "outputTexture", shuffleFilterName, "inputTexture");

		// Connect to output :
		pipelineLayout.connectToOutput(shuffleFilterName, "outputTexture", "outputTexture");

		return pipelineLayout;*/
	}
	
	LAYOUT_LOADER_MODULE_APPLY_IMPLEMENTATION( GenerateFFT2DPipeline )
	{
		UNUSED_PARAMETER(body)
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(dynamicPaths)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sharedCodeList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(geometryList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(startLine)
		UNUSED_PARAMETER(bodyLine)
		UNUSED_PARAMETER(executionCode)		

		PIPELINE_MUST_NOT_EXIST( arguments[2] )
		CAST_ARGUMENT(0, int, width)
		CAST_ARGUMENT(1, int, height)

		// Read the flags : 
		int flags = 0;
		for(unsigned int k=3; k<arguments.size(); k++)
		{
			FFTModules::Flag f = FFTModules::getFlag(arguments[k]);
			flags = flags | static_cast<int>(f);
		}

		APPEND_NEW_PIPELINE(arguments[2], generate(width, height, flags))
	}

