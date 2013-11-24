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

	#include <vector>
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
							/// Number of channels.
				const unsigned char	numChannels;
							/// True if it has a red channel.
				const bool		hasRedChannel,
							/// True if it has a green channel.
							hasGreenChannel,
							/// True if it has a blue channel.
							hasBlueChannel,
							/// True if it has a alpha channel.
							hasAlphaChannel,
							/// True if it has a luminance or intensity channel.
							hasLuminanceChannel,
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
							/// Size of the green component for a single pixel, in bits.
							greenDepthInBits,
							/// Size of the blue component for a single pixel, in bits.
							blueDepthInBits,
							/// Size of the alpha component for a single pixel, in bits.
							alphaDepthInBits,
							/// Size of the luminance, or intensity, component for a single pixel, in bits.
							luminanceDepthInBits;
							/// Type of the red component (GL_NONE if no type is defined or the channel does not exist).
				const GLenum		redType,
							/// Type of the green component (GL_NONE if no type is defined or the channel does not exist).
							greenType,
							/// Type of the blue component (GL_NONE if no type is defined or the channel does not exist).
							blueType,
							/// Type of the alpha component (GL_NONE if no type is defined or the channel does not exist).
							alphaType,
							/// Type of the luminance component (GL_NONE if no type is defined or the channel does not exist).
							luminanceType;
							/// Index of the red component (or -1 if the channel does not exist).
				const char		indexRed,
							/// Index of the green component (or -1 if the channel does not exist).
							indexGreen,
							/// Index of the blue component (or -1 if the channel does not exist).
							indexBlue,
							/// Index of the alpha component (or -1 if the channel does not exist).
							indexAlpha,
							/// Index of the luminance component (or -1 if the channel does not exist).
							indexLuminance;

				// Tools : 
				bool hasChannel(GLenum channel) const;
				unsigned char channelSizeInBits(GLenum channel) const;
				GLenum channelType(GLenum channel) const;
				char channelIndex(GLenum channel) const;
				std::vector<GLenum> getChannelsList(void) const;
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
