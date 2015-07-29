/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
				#define HdlTextureFormatDescriptor_MaxNumChannels (4)
				static const int maxNumChannels;
				static const int maxPixelSizeInBits;
				static const int maxPixelSize;

				/// Name of the mode (GL_RGB, GL_RGBA, etc.).
				const GLenum	mode,
				/// Name of the alias mode (simplified mode, GL_RGB, GL_RGBA, etc.).
						aliasMode,
				/// Name of the corresponding uncompressed mode (the same as mode if not a compressed format).
						uncompressedMode,
				/// Name of the corresponding compressed mode (the same as mode if already a compressed format).
						compressedMode;
				/// Number of channels.
				const int	numChannels;
				/// Channels content (GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_LUMINANCE, etc.) or GL_NONE if empty.
				const GLenum	channels[HdlTextureFormatDescriptor_MaxNumChannels];
				/// Size of the channels in bits or 0 if empty.
				const int	channelsSizeInBits[HdlTextureFormatDescriptor_MaxNumChannels];
				/// Depth associated with the channel or GL_NONE if no type is enforced or if channel is empty.
				const GLenum	channelsDepth[HdlTextureFormatDescriptor_MaxNumChannels];
				/// If the format is in floating point.
				const bool	isFloatingPoint,
				/// If the format is compressed.
						isCompressed,
				/// If the format is supported.
						isSupported;

				bool isDepthValid(GLenum depth) const;
				bool hasChannel(GLenum channel) const;
				int getChannelIndex(GLenum channel) const;
				int getChannelOffsetInBits(int channelIndex, GLenum depth) const;
				int getChannelOffset(int channelIndex, GLenum depth) const;
				int getChannelSizeInBits(int channelIndex, GLenum depth) const;
				int getChannelSize(int channelIndex, GLenum depth) const;
				int getPixelSizeInBits(GLenum depth) const;
				int getPixelSize(GLenum depth) const;
				
				static int getTypeSizeInBits(GLenum depth);		
				static int getTypeSize(GLenum depth);
				static void getShuffle(const HdlTextureFormatDescriptor& dst, const HdlTextureFormatDescriptor& src, char* shuffleIndex, const int length);
				static int getBitShuffle(const HdlTextureFormatDescriptor& dst, const GLenum& dstDepth, const HdlTextureFormatDescriptor& src, const GLenum& srcDepth, char* shuffleBitIndex, const int length, bool* isBlack=NULL);
				static void applyBitShuffle(char* dst, const char* src, const char* shuffleBitIndex, const int length);
			};

			/**
			\class HdlTextureFormatDescriptorsList
			\brief Singleton handling the static list of all available HdlTextureFormatDescriptor objects.
			**/
			class GLIP_API HdlTextureFormatDescriptorsList
			{
				private :
					static const HdlTextureFormatDescriptor textureFormatDescriptors[];

					HdlTextureFormatDescriptorsList(void);
				public :
					static int getNumDescriptors(void);
					static const HdlTextureFormatDescriptor& get(int id);
					static const HdlTextureFormatDescriptor& get(const GLenum& modeID);
			};	
		}
	}

#endif
