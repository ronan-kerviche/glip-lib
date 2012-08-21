/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FFT2D.hpp                                                                                 */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : 2D FFT for gray level input (real or complex).                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT2D.hpp
 * \brief   2D FFT for gray level input (real or complex).
 * \author  R. KERVICHE
 * \date    August 20th 2012
**/

#ifndef __FFT2D_INCLUDE__
#define __FFT2D_INCLUDE__

	// Include :
	#include "../Core/OglInclude.hpp"
	#include "../Core/Pipeline.hpp"

namespace Glip
{
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		// Structure
		/**
		\class FFT2D
		\brief Compute the 2D FFT for a gray level image (real or complex). THIS MODULE HASN'T BEEN FULLY TESTED YET.
		**/
		class FFT2D
		{
			public :
				enum Flags
				{
					Shifted			= 1,
					Inversed		= 2,
					ComputeMagnitude	= 4
				};

				/**
				\enum FFT1D::Flags
				Flags describing the computation options.

				\var FFT1D::Flags FFT1D::Shifted
				Shift the final result.
				\var FFT1D::Flags FFT1D::Inversed
				Perform iFFT : FFT(X*)* / N.
				\var FFT1D::Flags FFT1D::ComputeMagnitude
				In the final result, the blue channel will hold the magnitude of the complex number.
				**/

			private :
				// Data :
				HdlTexture 	*width_bitReversal,
						*width_wpTexture,
						*height_bitReversal,
						*height_wpTexture;
				Pipeline 	*pipeline;

				// Tools :
				unsigned short reverse(unsigned short n, bool forWidth);
				void getWp(unsigned int p, float& c, float& s, bool forWidth);
				std::string generateCode(int delta, int coeffp, bool forWidth);
				std::string generateFinalCode(bool forWidth);

			public :
				// Data :
						///The width of the data set.
				const int 	w,
						///The height of the data set.
						h;

						///Set to true if the operation performed is the inverse FFT.
				const bool 	inversed,
						///Set to true if the final result is shifted in case of a FFT, or the input is assumed to be shifted for an iFFT.
						shift,
						///Set to true if the magnitude is computed.
						compMagnitude;

				// Tools :
				FFT2D(int _w, int _h, int flags = 0);
				~FFT2D(void);

				void process(HdlTexture& input);
				HdlTexture& output(void);
		};
	}
}

#endif
