/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FFT2D.cpp                                                                                 */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : 2D FFT for gray level input (real or complex).                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT2D.cpp
 * \brief   2D FFT for gray level input (real or complex).
 * \author  R. KERVICHE
 * \date    August 20th 2012
**/

	// Includes
	#include <cmath>
	#include "Exception.hpp"
	#include "FFT2D.hpp"
	#include "devDebugTools.hpp"
	#include "ShaderSource.hpp"
	#include "HdlShader.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	/**
	\fn FFT2D::FFT2D(int _w, int _h, int flags)
	\brief FFT2D constructor, will compute the 2D Fast Fourier Transformation on a 2D texture. In both the input and the output, the red and green channels hold the real and complex part respectively.
	\param _w The width of the transformation, must be a power of two. It will raise an exception otherwise.
	\param _h The height of the transformation, must be a power of two. It will raise an exception otherwise.
	\param flags The flags associated for this computation (combined with | operator), see FFT2D::Flags.
	**/
	FFT2D::FFT2D(int _w, int _h, int flags)
	 : width_bitReversal(NULL), width_wpTexture(NULL), height_bitReversal(NULL), height_wpTexture(NULL), pipeline(NULL), lnkFirstWidthFilter(NULL), w(_w), h(_h), inverse((flags & Inversed)>0), shift((flags & Shifted)>0), compMagnitude((flags & ComputeMagnitude)>0), useZeroPadding((flags & UseZeroPadding)>0), compatibilityMode((flags & CompatibilityMode)>0)
	{
		double 	test1 = log(w)/log(2),
			test2 = floor(test1),
			test3 = log(h)/log(2),
			test4 = floor(test3);

		if(test1!=test2 || test3!=test4)
			throw Exception("FFT2D::FFT2D - Width and Height must be a power of 2.", __FILE__, __LINE__);

		if(w<4 || h<4)
			throw Exception("FFT2D::FFT2D - Width and Height must be at least 4.", __FILE__, __LINE__);

		// Fill bit reversal :
			// Width :
				float* data1w = new float[w*2];
				for(unsigned short i=0; i<w; i++)
				{
					unsigned short r = reverse(i, true);
					data1w[2*i+0] = static_cast<float>(r)/static_cast<float>(w);
					data1w[2*i+1] = 0.0;
				}
				HdlTextureFormat widthBitReversalFormat(w, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
				width_bitReversal = new HdlTexture(widthBitReversalFormat);
				width_bitReversal->write(data1w);
				delete[] data1w;
			// Height :
				if(w==h)
					height_bitReversal = width_bitReversal;
				else
				{
					float* data1h = new float[h*2];
					for(unsigned short i=0; i<h; i++)
					{
						unsigned short r = reverse(i, false);
						data1h[2*i+0] = static_cast<float>(r)/static_cast<float>(h);
						data1h[2*i+1] = 0.0;
					}
					HdlTextureFormat heightBitReversalFormat(h, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
					height_bitReversal = new HdlTexture(heightBitReversalFormat);
					height_bitReversal->write(data1h);
					delete[] data1h;
				}

		// Fill wpTexture :
			// Width :
				float* data2w = new float[w/2*2];
				for(int i=0; i<w/2; i++)
				{
					float a,b;
					getWp(i,a,b,true);
					data2w[2*i+0] = a;
					data2w[2*i+1] = b;
				}
				HdlTextureFormat widthWpTextureFormat(w/2, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
				width_wpTexture = new HdlTexture(widthWpTextureFormat);
				width_wpTexture->write(data2w);
				delete[] data2w;
			// Height :
				if(w==h)
					height_wpTexture = width_wpTexture;
				else
				{
					float* data2h = new float[h/2*2];
					for(int i=0; i<h/2; i++)
					{
						float a,b;
						getWp(i,a,b,false);
						data2h[2*i+0] = a;
						data2h[2*i+1] = b;
					}
					HdlTextureFormat heightWpTextureFormat(h/2, 1, GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
					height_wpTexture = new HdlTexture(heightWpTextureFormat);
					height_wpTexture->write(data2h);
					delete[] data2h;
				}


		// Clean
			HdlTexture::unbind();

		// Write the pipeline :
		try
		{
			HdlTextureFormat widthfmt(w/2,h,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			HdlTextureFormat heightfmt(w,h/2,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);

			HdlTextureFormat *fmtout;

			if(compMagnitude)
				fmtout = new HdlTextureFormat(w,h,GL_RGBA32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);
			else
				fmtout = new HdlTextureFormat(w,h,GL_RG32F, GL_FLOAT, GL_NEAREST, GL_NEAREST);

			PipelineLayout playout("FFTPipeline");
			playout.addInput("input");
			playout.addInput("widthReversalTexture");
			playout.addInput("heightReversalTexture");
			playout.addInput("widthWpTexture");
			playout.addInput("heightWpTexture");
			playout.addOutput("output");

			// First : width
			int coeffp = w/2;
			std::string 	previousName = "",
					firstWidthFilterName = "";

			for(int i=1; i<=w/2; i*=2)
			{
				ShaderSource shader(generateCode(i,coeffp,true));
				std::string name = "Wfilter"+to_string(i);
				FilterLayout fl("WRadix2", widthfmt, shader);
				playout.add(fl,name);

				if(previousName=="")
				{
					playout.connectToInput("input", name, "inputTexture");
					playout.connectToInput("widthReversalTexture", name, "reversalTexture");
					firstWidthFilterName = name;
				}
				else
				{
					playout.connect(previousName, "output", name, "inputTexture");
					playout.connectToInput("widthWpTexture", name, "wpTexture");
				}

				previousName = name;

				coeffp = coeffp/2;
			}

			// Unpack :
			ShaderSource wshader(generateFinalCode(true));
			FilterLayout flw("WReOrder", *fmtout, wshader);
			playout.add(flw,"fWReOrder");
			playout.connect(previousName, "output", "fWReOrder", "inputTexture");

			// Second : height
			previousName = "";
			coeffp = h/2;

			for(int i=1; i<=h/2; i*=2)
			{
				ShaderSource shader(generateCode(i,coeffp,false));
				std::string name = "Hfilter"+to_string(i);
				FilterLayout fl("HRadix2", heightfmt, shader);
				playout.add(fl,name);

				if(previousName=="")
				{
					playout.connect("fWReOrder", "output", name, "inputTexture");
					playout.connectToInput("heightReversalTexture", name, "reversalTexture");
				}
				else
				{
					playout.connect(previousName, "output", name, "inputTexture");
					playout.connectToInput("heightWpTexture", name, "wpTexture");
				}

				previousName = name;

				coeffp = coeffp/2;
			}

			// Add last filter :
			ShaderSource hshader(generateFinalCode(false));
			FilterLayout flh("HReOrder", *fmtout, hshader);
			playout.add(flh,"fHReOrder");
			playout.connect(previousName, "output", "fHReOrder", "inputTexture");

			// Connect to output :
			playout.connectToOutput("fHReOrder", "output", "output");

			// Done :
			pipeline = new Pipeline(playout, "instFFT2D");
			lnkFirstWidthFilter = &((*pipeline)[firstWidthFilterName]);

			delete fmtout;
		}
		catch(Exception& e)
		{
			std::cout << "Caught an exception while writing the pipeline : " << std::endl;
			std::cout << e.what() << std::endl;
		}
	}

	FFT2D::~FFT2D(void)
	{
		if(width_bitReversal!=height_bitReversal)
			delete width_bitReversal;
		delete height_bitReversal;

		if(width_wpTexture!=height_wpTexture)
			delete width_wpTexture;
		delete height_wpTexture;

		delete pipeline;
	}

	unsigned short FFT2D::reverse(unsigned short n, bool forWidth)
	{
		unsigned short 	res = 0,
				j = 1;

		unsigned int 	size = 0;

		if(forWidth)	size = w;
		else		size = h;

		for(unsigned short i=size/2; i>0; i/=2)
		{
			res += j*((i&n)>0);
			j *= 2;
		}

		return res;
	}

	void FFT2D::getWp(unsigned int p, float& c, float& s, bool forWidth)
	{
		const double 	PI = 3.14159265359;
		double 		pd = static_cast<double>(p),
				nd = 0.0;

		if(forWidth)	nd = static_cast<double>(w);
		else		nd = static_cast<double>(h);

		double 		cd = cos(-2.0*PI*pd/nd),
				sd = sin(-2.0*PI*pd/nd);

		c = static_cast<float>(cd);
		s = static_cast<float>(sd);
	}

	std::string FFT2D::generateCode(int delta, int coeffp, bool forWidth)
	{
		// delta  : number of An coeffient (from 1 to size/2)
		// coeffp : coefficient in the wp transformation
		std::stringstream str;

		str << "#version 130\n";
		str << "\n";
		str << "uniform sampler2D inputTexture; \n";

		if(delta==1)
		{
			str << "uniform sampler2D reversalTexture; \n";

			if(useZeroPadding && forWidth)
				str << "uniform int xOffset, yOffset; \n";
			else
				str << "const int xOffset=0, yOffset=0; \n";
		}
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
			if(forWidth)
			{
				str << "    const int sz     = " << w << "; \n";
				str << "    const int hsz    = " << w/2 << "; \n";
				str << "    const int tsz    = " << h << "; \n";
				str << "    int globid       = int(gl_TexCoord[0].s*hsz); \n";
				str << "    int transverseId = int(gl_TexCoord[0].t*tsz); \n";
			}
			else
			{
				str << "    const int sz     = " << h << "; \n";
				str << "    const int hsz    = " << h/2 << "; \n";
				str << "    const int tsz    = " << w << "; \n";
				str << "    int globid       = int(gl_TexCoord[0].t*hsz); \n";
				str << "    int transverseId = int(gl_TexCoord[0].s*tsz); \n";
			}

			str << "    vec4 pA          = texelFetch(reversalTexture, ivec2(globid*2, 0), 0); \n";
			str << "    vec4 pB          = texelFetch(reversalTexture, ivec2(globid*2+1, 0), 0); \n";
			str << "    int ipA          = int(pA.s*sz); \n";
			str << "    int ipB          = int(pB.s*sz); \n";

			if(shift && inverse && forWidth) // only on the first process
			{
				str << "    if(ipA<sz/2)           ipA = ipA+sz/2; \n";
				str << "    else                   ipA = ipA-sz/2; \n";
				str << "    if(ipB<sz/2)           ipB = ipB+sz/2; \n";
				str << "    else                   ipB = ipB-sz/2; \n";
				str << "    if(transverseId<tsz/2) transverseId = transverseId+tsz/2; \n";
				str << "    else                   transverseId = transverseId-tsz/2; \n";
			}

			if(forWidth)
			{
				str << "    vec4 A           = texelFetch(inputTexture, ivec2(ipA-xOffset,transverseId-yOffset), 0); \n";
				str << "    vec4 B           = texelFetch(inputTexture, ivec2(ipB-xOffset,transverseId-yOffset), 0); \n";
			}
			else
			{
				str << "    vec4 A           = texelFetch(inputTexture, ivec2(transverseId,ipA), 0); \n";
				str << "    vec4 B           = texelFetch(inputTexture, ivec2(transverseId,ipB), 0); \n";
			}

			str << "    output.r  = A.r + B.r;           //real part of Xp \n";

			if(!inverse)
				str << "    output.g  = A.g + B.g;   //imag part of Xp \n";
			else
				str << "    output.g  = - A.g - B.g; //imag part of Xp \n";

			str << "    output.b  = A.r - B.r; //real part of Xp+n/2 \n";

			if(!inverse)
				str << "    output.a  = A.g - B.g; //imag part of Xp+n/2 \n";
			else
				str << "    output.a  = - A.g + B.g; //imag part of Xp+n/2 \n";
		}
		else
		{
			if(forWidth)
			{
				str << "    const int sz     = " << w << "; \n";
				str << "    const int hsz    = " << w/2 << "; \n";
				str << "    const int tsz    = " << h << "; \n";
			}
			else
			{
				str << "    const int sz     = " << h << "; \n";
				str << "    const int hsz    = " << h/2 << "; \n";
				str << "    const int tsz    = " << w << "; \n";
			}
			str << "    const int coeffp = " << coeffp << "; \n";
			str << "    const int delta  = " << delta << "; \n";

			// Find in which computing element and which block this is working :
			if(forWidth)
			{
				str << "    int globid       = int(gl_TexCoord[0].s*hsz); \n";
				str << "    int transverseId = int(gl_TexCoord[0].t*tsz); \n";
			}
			else
			{
				str << "    int globid       = int(gl_TexCoord[0].t*hsz); \n";
				str << "    int transverseId = int(gl_TexCoord[0].s*tsz); \n";
			}
			str << "    int blockid      = globid/delta; \n";
			str << "    int compid       = globid-blockid*delta; \n";

			// Compute the position of the elements :
			str << "    int mcompid      = compid; \n";
			str << "    if(compid>=delta/2) \n";
			str << "        mcompid      = compid - delta/2; \n";
			str << "    int ipA          = blockid*delta+mcompid; \n";
			str << "    int ipB          = ipA + delta/2; \n";

			// Get the elements :
			if(forWidth)
			{
				str << "    vec4 A           = texelFetch(inputTexture, ivec2(ipA,transverseId), 0); \n";
				str << "    vec4 B           = texelFetch(inputTexture, ivec2(ipB,transverseId), 0); \n";
			}
			else
			{
				str << "    vec4 A           = texelFetch(inputTexture, ivec2(transverseId,ipA), 0); \n";
				str << "    vec4 B           = texelFetch(inputTexture, ivec2(transverseId,ipB), 0); \n";
			}
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

	std::string FFT2D::generateFinalCode(bool forWidth)
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
			str << "    vec4 output; \n";

		if(forWidth)
		{
			str << "    const int sz             = " << w << "; \n";
			str << "    const int tsz            = " << h << "; \n";
			str << "    const int hsz            = " << w/2 << "; \n";
			str << "    int globid               = int(gl_TexCoord[0].s*sz); \n";
			str << "    int transversId          = int(gl_TexCoord[0].t*tsz); \n";
			str << "    int mglobid              = globid; \n";
			str << "    if(globid>=hsz) \n";
			str << "        mglobid              = globid - hsz; \n";
			str << "    vec4 X                   = texelFetch(inputTexture, ivec2(mglobid, transversId), 0); \n";
			str << "    if(globid<hsz) \n";
			str << "    { \n";
			str << "        output.r      = X.r; \n";
			str << "        output.g      = X.g; \n";
			str << "    } \n";
			str << "    else \n";
			str << "    { \n";
			str << "        output.r      = X.b; \n";
			str << "        output.g      = X.a; \n";
			str << "    } \n";

			if(inverse)
			{
				str << "    output.r          =  output.r/sz; \n";
				str << "    output.g          = -output.g/sz; \n";
			}
		}
		else
		{
			str << "    const int w              = " << w << "; \n";
			str << "    const int h              = " << h << "; \n";
			str << "    const int hh             = " << h/2 << "; \n";
			str << "    const int hw             = " << w/2 << "; \n";
			str << "    int wglobid              = int(gl_TexCoord[0].s*w); \n";
			str << "    int hglobid              = int(gl_TexCoord[0].t*h); \n";

			if(shift && !inverse)
			{
				str << "    if(wglobid<hw)    wglobid += hw; \n";
				str << "    else              wglobid -= hw; \n";
			}

			str << "    int mhglobid             = hglobid; \n";
			str << "    if(hglobid>=hh) \n";
			str << "        mhglobid             = hglobid - hh; \n";
			str << "    vec4 X                   = texelFetch(inputTexture, ivec2(wglobid, mhglobid), 0); \n";

			if(!shift || inverse)
			{
				str << "    if(hglobid<hh) \n";
				str << "    { \n";
				str << "        output.r      = X.r; \n";
				str << "        output.g      = X.g; \n";
				str << "    } \n";
				str << "    else \n";
				str << "    { \n";
				str << "        output.r      = X.b; \n";
				str << "        output.g      = X.a; \n";
				str << "    } \n";
			}
			else
			{
				str << "    if(hglobid<hh) \n";
				str << "    { \n";
				str << "        output.r      = X.b; \n";
				str << "        output.g      = X.a; \n";
				str << "    } \n";
				str << "    else \n";
				str << "    { \n";
				str << "        output.r      = X.r; \n";
				str << "        output.g      = X.g; \n";
				str << "    } \n";
			}

			if(inverse)
			{
				str << "        output.r      =  output.r/h; \n";
				str << "        output.g      = -output.g/h; \n";
			}

			if(compMagnitude)
				str << "        output.b      = sqrt(output.r*output.r+output.g*output.g); \n";
		}

		if(compatibilityMode)
			str << "    gl_FragColor = output; \n";

		str << "} \n";

		return str.str();
	}

	/**
	\fn void FFT2D::process(HdlTexture& input)
	\brief This function compute the FFT over the data input which format is specified above.
	\param input The input data for the FFT 2D.
	**/
	void FFT2D::process(HdlTexture& input)
	{
		if(!useZeroPadding && (input.getWidth()!=w || input.getHeight()!=h))
			throw Exception("FFT2D::process - Wrong texture format (Zero padding is disabled).", __FILE__, __LINE__);

		if(useZeroPadding && (input.getWidth()>w || input.getHeight()>h))
			throw Exception("FFT2D::process - Wrong texture format (Zero padding is enabled, input texture is too large).", __FILE__, __LINE__);

		if(useZeroPadding)
		{
			// Update the offsets :
			int 	xOffset = (w-input.getWidth())/2,
				yOffset = (h-input.getHeight())/2;

			lnkFirstWidthFilter->prgm().modifyVar("xOffset", HdlProgram::Var, xOffset);
			lnkFirstWidthFilter->prgm().modifyVar("yOffset", HdlProgram::Var, yOffset);

		}

		//#ifdef __DEVELOPMENT_VERBOSE__
			//static bool once = true;
			//if(once)
				pipeline->enablePerfsMonitoring();
		//#endif

		(*pipeline) << input << (*width_bitReversal) << (*height_bitReversal) << (*width_wpTexture) << (*height_wpTexture) << Pipeline::Process;

		//#ifdef __DEVELOPMENT_VERBOSE__
			//if(once)
			//{
				std::cout << "Total : " << pipeline->getTotalTiming() << " ms." << std::endl;

				for(int i=0; i<pipeline->getNumActions(); i++)
				{
					std::string name;
					float t = pipeline->getTiming(i, name);
					std::cout << "    " << name << "\t: " << t << " ms" << std::endl;
				}

				pipeline->disablePerfsMonitoring();
			//	once = false;
			//}
		//#endif
	}

	/**
	\fn HdlTexture& FFT2D::output(void)
	\brief Returns a reference to the result of the last computation.
	\return A reference to a HdlTexture object.
	**/
	HdlTexture& FFT2D::output(void)
	{
		if(pipeline!=NULL)
			return pipeline->out(0);
		else
			throw Exception("FFT2D::output - pipeline is NULL.", __FILE__, __LINE__);
	}

	/**
	\fn int FFT2D::getSize(bool askDriver)
	\brief Get the size in bytes of the elements on the GPU for this module.
	\param  askDriver If true, it will use HdlTexture::getSizeOnGPU() to determine the real size (might be slower).
	\return Size in bytes.
	**/
	int FFT2D::getSize(bool askDriver)
	{
		int size = 0;

		size += pipeline->getSize(askDriver);

		if(askDriver)
		{
			size += width_bitReversal->getSizeOnGPU();
			if(height_bitReversal!=width_bitReversal)
				size += height_bitReversal->getSizeOnGPU();
			size += width_wpTexture->getSizeOnGPU();
			if(height_wpTexture!=width_wpTexture)
				size += height_wpTexture->getSizeOnGPU();
		}
		else
		{
			size += width_bitReversal->getSize();
			if(height_bitReversal!=width_bitReversal)
				size += height_bitReversal->getSize();
			size += width_wpTexture->getSize();
			if(height_wpTexture!=width_wpTexture)
				size += height_wpTexture->getSize();
		}

		return size;
	}
