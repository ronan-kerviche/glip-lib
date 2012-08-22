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

	#include "OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{

			struct HdlTextureFormatDescriptor
			{
				const GLenum 		modeID;
				const unsigned char	numComponents;
				const bool		hasRedLayer,
							hasBlueLayer,
							hasGreenLayer,
							hasAlphaLayer,
							hasLuminanceLayer,
							isCompressed;
				const GLenum		aliasMode,
							correspondingModeForCompressing,  // The Compressed mode if this is not compressed or the uncompressed mode otherwise
							forcedDepthID;
				const unsigned char	pixelSizeInBytes,
							pixelSizeInBits,
							redDepthInBits,
							greenDepthInBits,
							blueDepthInBits,
							alphaDepthInBits,
							luminanceDepthInBits;
			};

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
