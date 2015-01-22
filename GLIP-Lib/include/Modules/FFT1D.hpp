/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FFT1D.hpp                                                                                 */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : 1D FFT for gray level input (real or complex).                                            */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT1D.hpp
 * \brief   1D FFT for gray level input (real or complex).
 * \author  R. KERVICHE
 * \date    August 20th 2012
**/

#ifndef __FFT1D_INCLUDE__
#define __FFT1D_INCLUDE__

	// Includes
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"
	#include "Modules/LayoutLoaderModules.hpp"

namespace Glip
{
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		namespace FFTModules
		{
			///Flag describing the computation options.
			enum Flag
			{
				///Shift the final result if the transform is direct or expect the input to be shifted if it is the reciprocal transform.
				Shifted			= 0x0001,
				///Perform the reciprocal transform, iFFT : FFT(X*)* / N.
				Inversed		= 0x0010,
				///Use zero padding for input texture which are smaller than the transform size.
				UseZeroPadding		= 0x0100,
				///The computation will use of old gl_FragColor GLSL built-in variable.
				CompatibilityMode	= 0x1000
				// Update static Flag getFlag(const std::string& str)
			};

			Flag getFlag(const std::string& str);
		}

		/**
		\class FFT1D
		\brief Compute the 1D FFT for a gray level image (real or complex) in single precision.

		<b>Warning</b> : This module might return erroneous result given that the driver will perform blind optimization.
		Thus results might be of lower accuracy.
		**/
		class GLIP_API GenerateFFT1DPipeline : public LayoutLoaderModule
		{
			public :
				

			private :
				static std::string generateRadix2Code(int width, int currentLevel, int flags);
				static std::string generateLastShuffleCode(int width, int flags);

			public :
				GenerateFFT1DPipeline(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static PipelineLayout generate(int width, int flags = 0);
		};

		/**
		\class FFT2D
		\brief Compute the 2D FFT for a gray level image (real or complex) in single precision.

		<b>Warning</b> : This module might return erroneous result given that the driver will perform blind optimization.
		Thus results might be of lower accuracy.
		**/
		class GLIP_API GenerateFFT2DPipeline : public LayoutLoaderModule
		{
			public :
				

			private :
				static std::string generateRadix2Code(int width, int currentLevel, int flags);
				static std::string generateLastShuffleCode(int width, int flags);

			public :
				GenerateFFT2DPipeline(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static PipelineLayout generate(int width, int height, int flags = 0);
		};
	}
}

#endif
