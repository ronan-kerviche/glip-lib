/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : DeviceMemoryManager.hpp                                                                   */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Device Memory Manager / Storage Manager for fast resource access.                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPCOMPUTE_DEVICEMEMORYMANAGER__
#define __GLIPCOMPUTE_DEVICEMEMORYMANAGER__

	#include <list>
	#include "FreeImagePlusInterface.hpp"

	class DeviceMemoryManager
	{
		private : 
			size_t 							maxMemory,
										currentMemory;
			std::map<std::string, Glip::CoreGL::HdlTexture*>	resources;
			std::list<Glip::CoreGL::HdlTexture*>			history;

			void remember(Glip::CoreGL::HdlTexture* ptr);
			void forget(size_t oblivionSize);
		public : 
			DeviceMemoryManager(const size_t& _maxMemory);
			~DeviceMemoryManager(void);

			Glip::CoreGL::HdlTexture* get(const std::string& filename);
	};

#endif

