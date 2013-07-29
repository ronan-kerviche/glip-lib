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

	// Include :
	#include "Core/OglInclude.hpp"
	#include "Core/Pipeline.hpp"

namespace Glip
{
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	namespace Modules
	{
		// Structure
		/**
		\class FFT1D
		\brief Compute the 1D FFT for a gray level image (real or complex) in single precision.

		This module might return erroneous result given that the driver will perform blind optimization.
		Thus results might be affected by lower accuracy and being fetched to a close value.
		**/
		class FFT1D
		{
			public :
				///Flags describing the computation options.
				enum Flags
				{
					///Shift the final result if the transform is direct or expect the input to be shifted if it is the reciprocal transform.
					Shifted			= 1,
					///Perform the reciprocal transform, iFFT : FFT(X*)* / N.
					Inversed		= 2,
					///In the final result, the blue channel will hold the magnitude of the complex number.
					ComputeMagnitude	= 4,
					///Use zero padding for input texture which are smaller than the transform size.
					UseZeroPadding		= 8,
					///The computation will use of old gl_FragColor GLSL built-in variable.
					CompatibilityMode	= 16,
				};

			private :
				// Data :
				HdlTexture 	*bitReversal,
						*wpTexture;
				Pipeline 	*pipeline;
				Filter		*lnkFirstFilter;
				bool		performanceMonitoring;
				double		sumTime,
						sumSqTime;
				int		numProcesses;

				// Tools :
				unsigned short reverse(unsigned short n);
				void getWp(unsigned int p, float& c, float& s);
				std::string generateCode(int delta, int coeffp);
				std::string generateFinalCode(void);

			public :
				// Data :
						///The size of the data set.
				const int 	size;

						///Set to true if the operation performed is the inverse FFT.
				const bool 	inversed,
						///Set to true if the final result is shifted in case of a FFT, or the input is assumed to be shifted for an iFFT.
						shift,
						///Set to true if the magnitude is computed.
						compMagnitude,
						///Set to true if the transform accept lower textures in size and zero-pad them before computing the transform.
						useZeroPadding,
						///Set to true if The computation will use gl_FragColor.
						compatibilityMode;

				// Tools :
				FFT1D(int _size, int flags = 0);
				~FFT1D(void);

				void process(HdlTexture& input);
				HdlTexture& output(void);
				int getSize(bool askDriver = false);

				void	enablePerfsMonitoring(void);
				void	disablePerfsMonitoring(void);
				bool	isMonitoringPerfs(void);
				int	getNumProcesses(void);
				double	getMeanTime(void);
				double	getStdDevTime(void);
		};
	}
}

#endif
