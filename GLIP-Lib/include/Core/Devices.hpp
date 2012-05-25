/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Devices.hpp                                                                               */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Input and Output devices objects                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Devices.hpp
 * \brief   Input and Output devices objects
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_DEVICES__
#define __GLIP_DEVICES__

    // Includes
        #include "NamingLayout.hpp"
        #include "HdlTexture.hpp"

	namespace Glip
	{
		namespace CoreGL
		{
			class HdlTexture;
		}

		using namespace CoreGL;

		namespace CorePipeline
		{
			// Objects
			/**
			\class InputDevice
			\brief Input of data onto the GPU
			**/
			class InputDevice : public ObjectName
			{
				private :
					// Data
					HdlTexture* 	t;
					bool        	newImage;
					int         	imagesMissed;
				protected :
					// Tools
					InputDevice(const std::string& name);

					void 		setTextureLink(HdlTexture* tex);
					void        	declareNewImage(void);
				public :
					// Tools
					~InputDevice(void);
					bool        	isNewImage(void);
					int         	getMissedImagesCount(void);
					HdlTexture& 	texture(void);
			};

			/**
			\class OutputDevice
			\brief Output of data from the GPU
			**/
			class OutputDevice : public ObjectName
			{
				private :

				protected :
					// Tools
					OutputDevice(const std::string& name);

					/**
					\fn virtual void process(HdlTexture& t) = 0;
					\brief The user-defined processing stage.
					\param t The input texture.
					**/
					virtual void process(HdlTexture& t) = 0;
				public :
					// Tools
					OutputDevice& operator<<(HdlTexture& t);
			};
		}
	}

#endif // DEVICES_HPP_INCLUDED
