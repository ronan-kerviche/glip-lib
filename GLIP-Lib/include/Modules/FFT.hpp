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
			/// Flag describing the computation options.
			enum FFTFlag
			{
				/// Shift the final result if the transform is direct or expect the input to be shifted if it is the reciprocal transform.
				Shifted			= 0x00001,
				/// Perform the reciprocal transform, iFFT : FFT(X*)* / N.
				Inversed		= 0x00010,
				/// Use zero padding for input texture which are smaller than the transform size.
				UseZeroPadding		= 0x00100,
				/// The computation will use of old gl_FragColor GLSL built-in variable.
				CompatibilityMode	= 0x01000,
				/// Pipelines have no input (the user must provide a PRE-function).
				NoInput			= 0x10000
				// Update Flag getFlag(const std::string& str)
			};

			GLIP_API_FUNC Flag getFlag(const std::string& str);
		}

		/**
		\class GenerateFFT1DPipeline
		\brief Generate a 1D FFT PipelineLayout.
		\related FFTModules

		<b>Warning</b> : This module might return low accuracy or even erroneous result if the driver performs a blind optimization.

		The FFT is computed in single precision. The pipeline will have the input port <i>inputTexture</i> and the output port <i>outputTexture</i>. The input texture will have the real part in its red channel and the imaginary part in its green channel. 

		Here is an example of PRE and POST functions given when creating the pipeline via a LayoutLoader script. Note that you can provide the content of the blocks PRE and POST directly to the GenerateFFT1DPipeline::generate function.
		\code
		CALL:GENERATE_FFT1D_PIPELINE(512, FFTPipelineLayout)
		{
			PRE
			{
				uniform vec4 selection = vec4(1.0, 0.0, 0.0, 0.0);

				vec4 pre(in vec4 c, in int x) // c will contain the color of the texture, sampled at the right position, x is the position (normalized in the [0, 1] range).
				{
					c.r = dot(c, selection)/(selection.r + selection.g + selection.b); // Select which channel (or combination to use as the real data.
					c.gba = vec3(0.0, 0.0, 0.0); // Do not set any imaginary data (green channel). Both blue and alpha channels are omitted.
					return c;
				}
			}
			POST
			{
				vec4 post(in vec4 c, in int x) // c will contain the color of the texture, sampled at the right position, x is the position (normalized in the [0, 1] range).
				{
					c.g = c.r; // copy the real value of the FFT transform (red channel) to the imaginary value (green channel).
					return c;
				}
			}
		}
		\endcode
		**/
		class GLIP_API GenerateFFT1DPipeline : public LayoutLoaderModule
		{
			private :
				static ShaderSource generateRadix2Code(int width, int currentLevel, int flags, const ShaderSource& pre);
				static ShaderSource generateLastShuffleCode(int width, int flags, const ShaderSource& post);

			public :
				static const std::string 	inputPortName,
								outputPortName;

				GenerateFFT1DPipeline(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static PipelineLayout generate(int width, int flags = 0, const ShaderSource& pre=std::string(), const ShaderSource& post=std::string());
		};

		/**
		\class GenerateFFT2DPipeline
		\brief Generate a 2D FFT PipelineLayout.
		\related FFTModules

		<b>Warning</b> : This module might return low accuracy or even erroneous result if the driver performs a blind optimization.

		The FFT is computed in single precision. The pipeline will have the input port <i>inputTexture</i> and the output port <i>outputTexture</i>. The input texture will have the real part in its red channel and the imaginary part in its green channel. 

		Here is an example of PRE and POST functions given when creating the pipeline via a LayoutLoader script. Note that you can provide the content of the blocks PRE and POST directly to the GenerateFFT2DPipeline::generate function.
		\code
		CALL:GENERATE_FFT2D_PIPELINE(512, 512, FFTPipelineLayout)
		{
			PRE
			{
				uniform vec4 selection = vec4(1.0, 0.0, 0.0, 0.0);

				vec4 pre(in vec4 c, in vec2 x) // c will contain the color of the texture, sampled at the right position, x is the position vector (normalized in the [0, 1] range).
				{
					c.r = dot(c, selection)/(selection.r + selection.g + selection.b); // Select which channel (or combination to use as the real data.
					c.gba = vec3(0.0, 0.0, 0.0); // Do not set any imaginary data (green channel). Both blue and alpha channels are omitted.
					return c;
				}
			}
			POST
			{
				vec4 post(in vec4 c, in vec2 x) // c will contain the color of the texture, sampled at the right position, x is the position vector (normalized in the [0, 1] range).
				{
					c.g = c.r; // copy the real value of the FFT transform (red channel) to the imaginary value (green channel).
					return c;
				}
			}
		}
		\endcode
		**/
		class GLIP_API GenerateFFT2DPipeline : public LayoutLoaderModule
		{
			private :
				static ShaderSource generateRadix2Code(int width, int oppositeWidth, int currentLevel, int flags, bool horizontal, const ShaderSource& pre);
				static ShaderSource generateLastShuffleCode(int width, int oppositeWidth, int flags, bool horizontal, const ShaderSource& post);

			public :
				static const std::string 	inputPortName,
								outputPortName;

				GenerateFFT2DPipeline(void);

				LAYOUT_LOADER_MODULE_APPLY_SIGNATURE

				static PipelineLayout generate(int width, int height, int flags = 0, const ShaderSource& pre=std::string(), const ShaderSource& post=std::string());
		};
	}
}

#endif
