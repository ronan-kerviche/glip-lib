/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : Modules.hpp                                                                              **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Modules for the library                                                                  **/
/**                                                                                                             **/
/*****************************************************************************************************************/

#ifndef __GLIP_LIB_MODULES_LIBRARY_INCLUDE__
#define __GLIP_LIB_MODULES_LIBRARY_INCLUDE__

	#if (defined(GLIP_LOAD_ALL_MODULES) or defined(GLIP_LOAD_TEXTURE_READER)) and !(defined(GLIP_DONT_LOAD_TEXTURE_READER) or defined(GLIP_DONT_LOAD_ANY_MODULE))
		#include "./TextureReader.hpp"
	#endif

	#if (defined(GLIP_LOAD_ALL_MODULES) or defined(GLIP_LOAD_CAMERA_HANDLE)) and !(defined(GLIP_DONT_LOAD_CAMERA_HANDLE) or defined(GLIP_DONT_LOAD_ANY_MODULE))
		#include "./HdlCamera.hpp"
	#endif

#endif
