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
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"	

	namespace Glip
	{
		namespace CoreGL
		{
			/**
			\class HdlTextureFormatDescriptor
			\brief Object handle describing an OpenGL Texture mode.
			**/
			struct GLIP_API HdlTextureFormatDescriptor
			{
								/// Name of the mode (GL_RGB, GL_RGBA, etc.).
				const GLenum 			modeID;
								/// Index of the red component (or -1 if the channel does not exist).
				const char			indexRed,
								/// Index of the green component (or -1 if the channel does not exist).
								indexGreen,
								/// Index of the blue component (or -1 if the channel does not exist).
								indexBlue,
								/// Index of the alpha component (or -1 if the channel does not exist).
								indexAlpha,
								/// Index of the luminance component (or -1 if the channel does not exist).
								indexLuminance;
								/// True if it is a compressed mode.
				const bool			isCompressed,
								/// True if it is associated with a floating point depth.
								isFloating;
								/// The generic mode (e.g. GL_RG32F will have GL_RG).
				const GLenum			aliasMode,
								/// If the mode is uncompressed, this will be the corresponding compressed mode. Otherwise it will the corresponding uncompressed mode.
								correspondingModeForCompressing,
								/// The required depth for this mode or GL_NONE if none (e.g. GL_RGBA32F will have GL_FLOAT).
								forcedDepthID;
								/// Total pixel size, including all components, in bytes, rounded up.
				const unsigned int		pixelSizeInBytes,
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
				const GLenum			redType,
								/// Type of the green component (GL_NONE if no type is defined or the channel does not exist).
								greenType,
								/// Type of the blue component (GL_NONE if no type is defined or the channel does not exist).
								blueType,
								/// Type of the alpha component (GL_NONE if no type is defined or the channel does not exist).
								alphaType,
								/// Type of the luminance component (GL_NONE if no type is defined or the channel does not exist).
								luminanceType;			
				
				// Tools : 
				int numChannels(void) const;
				bool hasRedChannel(void) const;
				bool hasGreenChannel(void) const;
				bool hasBlueChannel(void) const;
				bool hasAlphaChannel(void) const;
				bool hasLuminanceChannel(void) const;
				bool hasChannel(GLenum channel) const;
				int getPixelSize(GLenum depth) const;
				unsigned int channelSizeInBits(GLenum channel) const;
				GLenum channelType(GLenum channel) const;
				int channelIndex(GLenum channel) const;
				GLenum channelAtIndex(int idx) const;
				std::vector<GLenum> getChannelsList(void) const;
				std::vector<int> getOffsetsList(void) const;
				GLenum getCompressedMode(void) const;
				GLenum getUncompressedMode(void) const;
			};

			/**
			\class HdlTextureFormatDescriptorsList
			\brief Singleton handling the static list of all available HdlTextureFormatDescriptor objects.
			**/
			class GLIP_API HdlTextureFormatDescriptorsList
			{
				private :
					// Data :
						static const HdlTextureFormatDescriptor textureFormatDescriptors[];

					// Tools :
						HdlTextureFormatDescriptorsList(void);

				public :
					// Data :
						static const int MaxPixelSizeInBytes;

					// Tools :
						static int getNumDescriptors(void);
						static const HdlTextureFormatDescriptor& get(int id);
						static const HdlTextureFormatDescriptor& get(const GLenum& modeID);
						static int getTypeDepth(GLenum depth);
			};	
		}
	}

#endif
