/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : FFT.hpp                                                                                   */
/*     Original Date : August 20th 2012                                                                          */
/*                                                                                                               */
/*     Description   : FFT pipeline generators for gray level input (real and complex).                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    FFT.hpp
 * \brief   FFT pipeline generators for gray level input (real and complex).
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
				// Update Flag getFlag(const std::string& str)
			};

			GLIP_API_FUNC Flag getFlag(const std::string& str);
		}

		/**
		\class GenerateFFT1DPipeline
		\brief Generate a 1D FFT PipelineLayout.

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
		\class GenerateFFT2DPipeline
		\brief Generate a 2D FFT PipelineLayout.

		<b>Warning</b> : This module might return erroneous result given that the driver will perform blind optimization.
		Thus results might be of lower accuracy.
		**/
		class GLIP_API GenerateFFT2DPipeline : public LayoutLoaderModule
		{
			public :
				

			private :
				static std::string generateRadix2Code(int width, int oppositeWidth, int currentLevel, int flags, bool horizontal);
				static std::string generateLastShuffleCode(int width, int flags, bool horizontal);

			public :
				GenerateFFT2DPipeline(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static PipelineLayout generate(int width, int height, int flags = 0);
		};
	}
}

#endif
