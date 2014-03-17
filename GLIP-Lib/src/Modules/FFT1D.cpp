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

	/**
	\fn FFT1D::FFT1D(int _size, int flags)
	\brief FFT1D constructor, will compute the 1D Fast Fourier Transformation on a 1D texture (will raise an exception otherwise). In both the input and the output, the red and green channels hold the real and complex part respectively.
	\param _size Size of the data (1D), must be a power of two. It will raise an exception otherwise.
	\param flags The flags associated for this computation (combined with | operator), see FFT1D::Flags.
	**/
	FFT1D::FFT1D(int _size, int flags)
	 : bitReversal(NULL), wpTexture(NULL), pipeline(NULL), size(_size), shift((flags & Shifted)>0), inversed((flags & Inversed)>0), compMagnitude((flags & ComputeMagnitude)>0), useZeroPadding((flags & UseZeroPadding)>0), compatibilityMode((flags & CompatibilityMode)>0), performanceMonitoring(false), sumTime(0.0), sumSqTime(0.0), numProcesses(0)
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
			std::string 	previousName = "",
					firstFilterName = "";

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
					firstFilterName = name;
				}
				else
				{
					playout.connect(previousName, "outputTexture", name, "inputTexture");
					playout.connectToInput("wpTexture", name, "wpTexture");
				}

				previousName = name;

				coeffp = coeffp/2;
			}

			// Add last filter :
			HdlTextureFormat *fmtout = NULL;
			if(compMagnitude)
				fmtout = new HdlTextureFormat(size,1,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			else
				fmtout = new HdlTextureFormat(size,1,GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);

			ShaderSource shader(generateFinalCode());
			FilterLayout fl("ReOrder", *fmtout, shader);
			playout.add(fl,"fReOrder");
			playout.connect(previousName, "outputTexture", "fReOrder", "inputTexture");

			// Connect to output :
			playout.connectToOutput("fReOrder", "outputTexture", "output");

			// Done :
			pipeline = new Pipeline(playout, "instFFT");

			//throw Exception("FFT1D is not available at the moment.", __FILE__, __LINE__);
			//lnkFirstFilter = &((*pipeline)[firstFilterName]);

			int id = pipeline->getElementID(firstFilterName);
			lnkFirstFilter = &((*pipeline)[id]);

			delete fmtout;
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
		str << "precision mediump float;\n";
		str << "\n";
		str << "uniform sampler2D inputTexture; \n";

		if(delta==1)
		{
			str << "uniform sampler2D reversalTexture; \n";

			if(useZeroPadding)
				str << "uniform int offset; \n";
			else
				str << "const int offset=0; \n";
		}
		else
			str << "uniform sampler2D wpTexture; \n";

		if(!compatibilityMode)
			str << "out vec4 outputTexture; \n";

		str << "\n";
		str << "void main() \n";
		str << "{ \n";

		if(compatibilityMode)
			str << "    vec4 outputTexture = vec4(0.0,0.0,0.0,0.0); \n";

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

			// ADD
			if(shift && inversed)
			{
				str << "    if(ipA<hsz) ipA = ipA+hsz; \n";
				str << "    else        ipA = ipA-hsz; \n";
				str << "    if(ipB<hsz) ipB = ipB+hsz; \n";
				str << "    else        ipB = ipB-hsz; \n";
			}
			// END ADD

			str << "    vec4 A           = texelFetch(inputTexture, ivec2(ipA-offset,0), 0); \n";
			str << "    vec4 B           = texelFetch(inputTexture, ivec2(ipB-offset,0), 0); \n";
			str << "    outputTexture.r  = A.r + B.r;           //real part of Xp \n";

			if(!inversed)
				str << "    outputTexture.g  = A.g + B.g;   //imag part of Xp \n";
			else
				str << "    outputTexture.g  = - A.g - B.g; //imag part of Xp \n";

			str << "    outputTexture.b  = A.r - B.r;           //real part of Xp+n/2 \n";

			if(!inversed)
				str << "    outputTexture.a  = A.g - B.g;   //imag part of Xp+n/2 \n";
			else
				str << "    outputTexture.a  = - A.g + B.g; //imag part of Xp+n/2 \n";
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
			str << "    outputTexture.r  = A.r + wp.r*B.r - wp.g*B.g; //real part of Xp \n";
			str << "    outputTexture.g  = A.g + wp.r*B.g + wp.g*B.r; //imag part of Xp \n";
			str << "    outputTexture.b  = A.r - wp.r*B.r + wp.g*B.g; //real part of Xp+n/2 \n";
			str << "    outputTexture.a  = A.g - wp.r*B.g - wp.g*B.r; //imag part of Xp+n/2 \n";
		}

		if(compatibilityMode)
			str << "    gl_FragColor = outputTexture; \n";

		str << "} \n";

		return str.str();
	}

	std::string FFT1D::generateFinalCode(void)
	{
		std::stringstream str;

		str << "#version 130\n";
		str << "precision mediump float;\n";
		str << "\n";
		str << "uniform sampler2D inputTexture; \n";

		if(!compatibilityMode)
			str << "out vec4 outputTexture; \n";

		str << "\n";
		str << "void main() \n";
		str << "{ \n";

		if(compatibilityMode)
			str << "    vec4 outputTexture = vec4(0.0,0.0,0.0,0.0); \n";

		str << "    const int sz             = " << size << "; \n";
		str << "    const int hsz            = " << size/2 << "; \n";
		str << "    int globid               = int(gl_TexCoord[0].s*sz); \n";
		str << "    int mglobid              = globid; \n";
		str << "    if(globid>=hsz) \n";
		str << "        mglobid              = globid - hsz; \n";
		str << "    vec4 X                   = texelFetch(inputTexture, ivec2(mglobid,0), 0); \n";
		str << "    if(globid<hsz) \n";
		str << "    { \n";


		if(shift && !inversed)
		{
			str << "        outputTexture.r      = X.b; \n";
			str << "        outputTexture.g      = X.a; \n";
			str << "    } \n";
			str << "    else \n";
			str << "    { \n";
			str << "        outputTexture.r      = X.r; \n";
			str << "        outputTexture.g      = X.g; \n";
		}
		else
		{
			str << "        outputTexture.r      = X.r; \n";
			str << "        outputTexture.g      = X.g; \n";
			str << "    } \n";
			str << "    else \n";
			str << "    { \n";
			str << "        outputTexture.r      = X.b; \n";
			str << "        outputTexture.g      = X.a; \n";
		}
		str << "    } \n";

		if(inversed)
		{
			str << "    outputTexture.r =  outputTexture.r/sz; \n";
			str << "    outputTexture.g = -outputTexture.g/sz; \n";
		}

		if(compMagnitude)
			str << "    outputTexture.b      = sqrt(outputTexture.r*outputTexture.r+outputTexture.g*outputTexture.g); \n";

		if(compatibilityMode)
			str << "    gl_FragColor = outputTexture; \n";

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
			throw Exception("FFT1D::process - Internal error : pipeline is NULL.", __FILE__, __LINE__);

		if(!useZeroPadding && (input.getWidth()!=size || input.getHeight()!=1))
			throw Exception("FFT1D::process - Wrong texture format (Zero padding is disabled).", __FILE__, __LINE__);

		if(useZeroPadding && (input.getWidth()>size || input.getHeight()!=1))
			throw Exception("FFT1D::process - Wrong texture format (Zero padding is enabled, input texture is too large).", __FILE__, __LINE__);

		if(useZeroPadding)
		{
			// Update the offsets :
			int 	offset = (size-input.getWidth())/2;

			lnkFirstFilter->program().modifyVar("offset", GL_INT, offset);
		}

		(*pipeline) << input << (*bitReversal) << (*wpTexture) << Pipeline::Process;

		if(performanceMonitoring)
		{
			double ts 	= pipeline->getTotalTiming();
			sumTime 	+= ts;
			sumSqTime	+= ts*ts;
			numProcesses++;
		}
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

	/**
	\fn int FFT1D::getSize(bool askDriver)
	\brief Get the size in bytes of the elements on the GPU for this module.
	\param  askDriver If true, it will use HdlTexture::getSizeOnGPU() to determine the real size (might be slower).
	\return Size in bytes.
	**/
	int FFT1D::getSize(bool askDriver)
	{
		int size = 0;

		size += pipeline->getSize(askDriver);

		if(askDriver)
		{
			size += bitReversal->getSizeOnGPU();
			size += wpTexture->getSizeOnGPU();
		}
		else
		{
			size += bitReversal->getSize();
			size += wpTexture->getSize();
		}

		return size;
	}

	/**
	\fn void FFT1D::enablePerfsMonitoring(void)
	\brief Start performance monitoring for this instance.

	If a monitoring session is already running, it will reset to zero.
	**/
	void	FFT1D::enablePerfsMonitoring(void)
	{
		if(!performanceMonitoring)
		{
			pipeline->enablePerfsMonitoring();
			performanceMonitoring = true;
		}

		// Reset :
		sumTime	= 0.0;
		sumSqTime = 0.0;
		numProcesses = 0;
	}

	/**
	\fn void FFT1D::disablePerfsMonitoring(void)
	\brief Stop performance monitoring for this instance.

	This function will not erase performance monitoring results of the previous session.
	**/
	void	FFT1D::disablePerfsMonitoring(void)
	{
		if(performanceMonitoring)
		{
			pipeline->disablePerfsMonitoring();
			performanceMonitoring = false;
		}
	}

	/**
	\fn bool FFT1D::isMonitoringPerfs(void)
	\brief Test if this instance is currently in a performance monitoring session.
	\return True if in such session.
	**/
	bool	FFT1D::isMonitoringPerfs(void)
	{
		return performanceMonitoring;
	}

	/**
	\fn int	FFT1D::getNumProcesses(void)
	\brief Get the number of process stages done in current monitoring session.
	\return The number of process stages done.
	**/
	int	FFT1D::getNumProcesses(void)
	{
		return numProcesses;
	}

	/**
	\fn double FFT1D::getMeanTime(void)
	\brief Get the mean time for one process given the statistics on all processes done during this monitoring session.
	\return Time in milliseconds.
	**/
	double	FFT1D::getMeanTime(void)
	{
		if(numProcesses==0)
			return 0.0;
		else
			return sumTime/numProcesses;
	}

	/**
	\fn double FFT1D::getStdDevTime(void)
	\brief Get the standard deviation on time for one process given the statistics on all processes done during this monitoring session.
	\return Time in milliseconds.
	**/
	double	FFT1D::getStdDevTime(void)
	{
		if(numProcesses==0)
			return 0.0;
		else
		{
			double m = getMeanTime();
			return sqrt(sumSqTime/numProcesses - m*m);
		}
	}
