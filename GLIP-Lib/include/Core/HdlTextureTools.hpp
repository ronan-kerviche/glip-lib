/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTextureTools.hpp                                                                       */
/*     Original Date : August 18th 2012                                                                          */
/*                                                                                                               */
/*     Description   : OpenGL Texture Handle and descriptors                                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlTextureTools.hpp
 * \brief   OpenGL Texture Handle Tools
 * \author  R. KERVICHE
 * \date    August 18th 2012
**/

#ifndef __HDLTEXTURETOOLS_INCLUDE__
#define __HDLTEXTURETOOLS_INCLUDE__

	#include "Core/OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			/**
			\class HdlTextureFormatDescriptor
			\brief Object handle describing an OpenGL Texture mode.
			**/
			struct HdlTextureFormatDescriptor
			{
							/// Name of the mode (GL_RGB, GL_RGBA, etc.).
				const GLenum 		modeID;
							/// Number of components.
				const unsigned char	numComponents;
							/// True if it has a red layer.
				const bool		hasRedLayer,
							/// True if it has a green layer.
							hasBlueLayer,
							/// True if it has a blue layer.
							hasGreenLayer,
							/// True if it has a alpha layer.
							hasAlphaLayer,
							/// True if it has a luminance or intensity layer.
							hasLuminanceLayer,
							/// True if it is a compressed mode.
							isCompressed,
							/// True if it is associated with a floatting point depth.
							isFloatting;
							/// The generic mode (e.g. GL_RG32F will have GL_RG).
				const GLenum		aliasMode,
							/// If the mode is uncompressed, this will be the correspondin compressed mode. Otherwise it will the corresponding uncompressed mode.
							correspondingModeForCompressing,
							/// The required depth for this mode or GL_NONE if none (e.g. GL_RGBA32F will have GL_FLOAT).
							forcedDepthID;
							/// Total pixel size, including all components, in bytes, rounded up.
				const unsigned char	pixelSizeInBytes,
							/// Total exact pixel size, including all components, in bits.
							pixelSizeInBits,
							/// Size of the red component for a single pixel, in bits.
							redDepthInBits,
							/// Size of the blue component for a single pixel, in bits.
							greenDepthInBits,
							/// Size of the green component for a single pixel, in bits.
							blueDepthInBits,
							/// Size of the alpha component for a single pixel, in bits.
							alphaDepthInBits,
							/// Size of the luminance, or intensity, component for a single pixel, in bits.
							luminanceDepthInBits;
			};

			/**
			\class HdlTextureFormatDescriptorsList
			\brief Singleton handling the static list of all available HdlTextureFormatDescriptor objects.
			**/
			class HdlTextureFormatDescriptorsList
			{
				private :
					// Data :
						static const HdlTextureFormatDescriptor textureFormatDescriptors[];

					// Tools :
						HdlTextureFormatDescriptorsList(void);

				public :
					// Tools :
						static int getNumDescriptors(void);
						static const HdlTextureFormatDescriptor& get(int id);
						static const HdlTextureFormatDescriptor& get(const GLenum& modeID);
						static int getTypeDepth(GLenum depth);
			};

		}
	}

#endif
