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
	#include "Exception.hpp"
	#include "FFT1D.hpp"
	#include "devDebugTools.hpp"
	#include "ShaderSource.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	/**
	\fn FFT1D::FFT1D(int _size, int flags)
	\brief FFT1D constructor, will compute the 1D Fast Fourier Transformation on a 1D texture (will raise an exception otherwise). In both the input and the output, the red and green channels hold the real and complex part respectively.
	\param _size Size of the data (1D), must be a power of two. It will raise an exception otherwise.
	\param flags The flags associated for this computation (combined with | operator), see FFT1D::Flags.
	**/
	FFT1D::FFT1D(int _size, int flags)
	 : bitReversal(NULL), wpTexture(NULL), pipeline(NULL), size(_size), shift((flags & Shifted)>0), inversed((flags & Inversed)>0), compMagnitude((flags & ComputeMagnitude)>0), compatibilityMode((flags & CompatibilityMode)>0)
	{
		double 	test1 = log(size)/log(2),
			test2 = floor(test1);

		if(test1!=test2)
			throw Exception("FFT1D::FFT1D - Size must be a power of 2.", __FILE__, __LINE__);

		if(size<4)
			throw Exception("FFT1D::FFT1D - Size must be at least 4.", __FILE__, __LINE__);

		// Fill bit reversal :
			float* data1 = new float[size*2];

			for(unsigned short i=0; i<size; i++)
			{
				unsigned short r = reverse(i);

				data1[2*i+0] = static_cast<float>(r)/static_cast<float>(size);
				data1[2*i+1] = 0.0;
			}
			HdlTextureFormat bitReversalFormat(size, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			bitReversal = new HdlTexture(bitReversalFormat);
			bitReversal->write(data1);
			delete[] data1;

		// Fill wpTexture :
			float* data2 = new float[size/2*2];
			for(int i=0; i<size/2; i++)
			{
				float a,b;
				getWp(i,a,b);
				data2[2*i+0] = a;
				data2[2*i+1] = b;
			}
			HdlTextureFormat wpTextureFormat(size/2, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			wpTexture = new HdlTexture(wpTextureFormat);
			wpTexture->write(data2);
			delete[] data2;

		// Clean
			HdlTexture::unbind();

		// Write the pipeline :
		try
		{
			HdlTextureFormat fmt(size/2,1,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			PipelineLayout playout("FFTPipeline");
			playout.addInput("input");
			playout.addInput("reversalTexture");
			playout.addInput("wpTexture");
			playout.addOutput("output");

			int coeffp = size/2;
			std::string previousName = "";

			for(int i=1; i<=size/2; i*=2)
			{
				ShaderSource shader(generateCode(i,coeffp));
				std::string name = "filter"+to_string(i);
				FilterLayout fl("Radix2", fmt, shader);
				playout.add(fl,name);

				if(previousName=="")
				{
					playout.connectToInput("input", name, "inputTexture");
					playout.connectToInput("reversalTexture", name, "reversalTexture");
				}
				else
				{
					playout.connect(previousName, "output", name, "inputTexture");
					playout.connectToInput("wpTexture", name, "wpTexture");
				}

				previousName = name;

				coeffp = coeffp/2;
			}

			// Add last filter :
			HdlTextureFormat fmtout(size,1,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			ShaderSource shader(generateFinalCode());
			FilterLayout fl("ReOrder", fmtout, shader);
			playout.add(fl,"fReOrder");
			playout.connect(previousName, "output", "fReOrder", "inputTexture");

			// Connect to output :
			playout.connectToOutput("fReOrder", "output", "output");

			// Done :
			pipeline = new Pipeline(playout, "instFFT");
		}
		catch(Exception& e)
		{
			std::cout << "Caught an exception while writing the pipeline : " << std::endl;
			std::cout << e.what() << std::endl;
		}
	}

	FFT1D::~FFT1D(void)
	{
		delete bitReversal;
		delete wpTexture;
		delete pipeline;
	}

	unsigned short FFT1D::reverse(unsigned short n)
	{
		unsigned short 	res = 0,
				j = 1;

		for(unsigned short i=size/2; i>0; i/=2)
		{
			res += j*((i&n)>0);
			j *= 2;
		}

		return res;
	}

	void FFT1D::getWp(unsigned int p, float& c, float& s)
	{
		const double PI = 3.14159265359;
		double 	pd = static_cast<double>(p),
			nd = static_cast<double>(size),
			cd = cos(-2.0*PI*pd/nd),
			sd = sin(-2.0*PI*pd/nd);
		c = static_cast<float>(cd);
		s = static_cast<float>(sd);
	}

	std::string FFT1D::generateCode(int delta, int coeffp)
	{
		// delta  : number of An coeffient (from 1 to size/2)
		// coeffp : coefficient in the wp transformation
		std::stringstream str;

		str << "#version 130\n";
		str << "\n";
		str << "uniform sampler2D inputTexture; \n";

		if(delta==1)
			str << "uniform sampler2D reversalTexture; \n";
		else
			str << "uniform sampler2D wpTexture; \n";

		if(!compatibilityMode)
			str << "out vec4 output; \n";

		str << "\n";
		str << "void main() \n";
		str << "{ \n";

		if(compatibilityMode)
			str << "    vec4 output = vec4(0.0,0.0,0.0,0.0); \n";

		if(delta==1)
		{
			str << "    const int sz     = " << size << "; \n";
			str << "    const int hsz    = " << size/2 << "; \n";
			str << "    const float pRev = " << 1.0f/static_cast<float>(size) << "; \n";
			str << "    int globid       = int(gl_TexCoord[0].s*hsz); \n";
			str << "    vec4 pA          = texelFetch(reversalTexture, ivec2(globid*2, 0), 0); \n";
			str << "    vec4 pB          = texelFetch(reversalTexture, ivec2(globid*2+1, 0), 0); \n";
			str << "    int ipA          = int(pA.s*sz); \n";
			str << "    int ipB          = int(pB.s*sz); \n";
			str << "    vec4 A           = texelFetch(inputTexture, ivec2(ipA,0), 0); \n";
			str << "    vec4 B           = texelFetch(inputTexture, ivec2(ipB,0), 0); \n";
			str << "    output.r  = A.r + B.r;           //real part of Xp \n";

			if(!inversed)
				str << "    output.g  = A.g + B.g;   //imag part of Xp \n";
			else
				str << "    output.g  = - A.g - B.g; //imag part of Xp \n";

			str << "    output.b  = A.r - B.r;           //real part of Xp+n/2 \n";

			if(!inversed)
				str << "    output.a  = A.g - B.g;   //imag part of Xp+n/2 \n";
			else
				str << "    output.a  = - A.g + B.g; //imag part of Xp+n/2 \n";
		}
		else
		{
			str << "    const int sz     = " << size << "; \n";
			str << "    const int hsz    = " << size/2 << "; \n";
			str << "    const int coeffp = " << coeffp << "; \n";
			str << "    const int delta  = " << delta << "; \n";

			// Find in which computing element and which block this is working :
			str << "    int globid       = int(gl_TexCoord[0].s*hsz); \n";
			str << "    int blockid      = globid/delta; \n";
			str << "    int compid       = globid-blockid*delta; \n";

			// Compute the position of the elements :
			str << "    int mcompid      = compid; \n";
			str << "    if(compid>=delta/2) \n";
			str << "        mcompid      = compid - delta/2; \n";
			str << "    int ipA          = blockid*delta+mcompid; \n";
			str << "    int ipB          = ipA + delta/2; \n";

			// Get the elements :
			str << "    vec4 A           = texelFetch(inputTexture, ivec2(ipA,0), 0); \n";
			str << "    vec4 B           = texelFetch(inputTexture, ivec2(ipB,0), 0); \n";
			str << "    if(mcompid!=compid) \n";
			str << "    { \n";
			str << "        A.r          = A.b; \n";
			str << "        A.g          = A.a; \n";
			str << "        B.r          = B.b; \n";
			str << "        B.g          = B.a; \n";
			str << "    } \n";

			// Get Wp
			str << "    int ipWp         = compid*coeffp; \n";
			str << "    vec4 wp          = texelFetch(wpTexture, ivec2(ipWp,0), 0); \n";

			// Compute :
			str << "    output.r  = A.r + wp.r*B.r - wp.g*B.g; //real part of Xp \n";
			str << "    output.g  = A.g + wp.r*B.g + wp.g*B.r; //imag part of Xp \n";
			str << "    output.b  = A.r - wp.r*B.r + wp.g*B.g; //real part of Xp+n/2 \n";
			str << "    output.a  = A.g - wp.r*B.g - wp.g*B.r; //imag part of Xp+n/2 \n";
		}

		if(compatibilityMode)
			str << "    gl_FragColor = output; \n";

		str << "} \n";

		return str.str();
	}

	std::string FFT1D::generateFinalCode(void)
	{
		std::stringstream str;

		str << "#version 130\n";
		str << "\n";
		str << "uniform sampler2D inputTexture; \n";

		if(!compatibilityMode)
			str << "out vec4 output; \n";

		str << "\n";
		str << "void main() \n";
		str << "{ \n";

		if(compatibilityMode)
			str << "    vec4 output = vec4(0.0,0.0,0.0,0.0); \n";

		str << "    const int sz             = " << size << "; \n";
		str << "    const int hsz            = " << size/2 << "; \n";
		str << "    int globid               = int(gl_TexCoord[0].s*sz); \n";
		str << "    int mglobid              = globid; \n";
		str << "    if(globid>=hsz) \n";
		str << "        mglobid              = globid - hsz; \n";
		str << "    vec4 X                   = texelFetch(inputTexture, ivec2(mglobid,0), 0); \n";
		str << "    if(globid<hsz) \n";
		str << "    { \n";

		if(!shift)
		{
			str << "        output.r      = X.r; \n";
			str << "        output.g      = X.g; \n";
			str << "    } \n";
			str << "    else \n";
			str << "    { \n";
			str << "        output.r      = X.b; \n";
			str << "        output.g      = X.a; \n";
		}
		else
		{
			str << "        output.r      = X.b; \n";
			str << "        output.g      = X.a; \n";
			str << "    } \n";
			str << "    else \n";
			str << "    { \n";
			str << "        output.r      = X.r; \n";
			str << "        output.g      = X.g; \n";
		}
		str << "    } \n";

		if(inversed)
		{
			str << "    output.r =  output.r/sz; \n";
			str << "    output.g = -output.g/sz; \n";
		}

		if(compMagnitude)
			str << "    output.b      = sqrt(output.r*output.r+output.g*output.g); \n";

		if(compatibilityMode)
			str << "    gl_FragColor = output; \n";

		str << "} \n";

		return str.str();
	}

	/**
	\fn void FFT1D::process(HdlTexture& input)
	\brief This function compute the FFT over the data input which format is specified above.
	\param input The input data for the FFT 1D.
	**/
	void FFT1D::process(HdlTexture& input)
	{
		if(pipeline==NULL)
			throw Exception("FFT1D::process - pipeline is NULL.", __FILE__, __LINE__);

		if(input.getWidth()!=size || input.getHeight()!=1)
			throw Exception("FFT1D::process - Wrong texture format.", __FILE__, __LINE__);

		(*pipeline) << input << (*bitReversal) << (*wpTexture) << Pipeline::Process;
	}

	/**
	\fn HdlTexture& FFT1D::output(void)
	\brief Returns a reference to the result of the last computation.
	\return A reference to a HdlTexture object.
	**/
	HdlTexture& FFT1D::output(void)
	{
		if(pipeline!=NULL)
			return pipeline->out(0);
		else
			throw Exception("FFT1D::output - pipeline is NULL.", __FILE__, __LINE__);
	}
