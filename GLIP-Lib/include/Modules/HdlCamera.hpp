/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : HdlCamera.hpp                                                                            **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Camera handle                                                                            **/
/**                                                                                                             **/
/*****************************************************************************************************************/

#ifndef __HANDLE_CAMERA_INCLUDE__
#define __HANDLE_CAMERA_INCLUDE__

	// Include
		#if defined(linux) || defined(__linux) || defined(__linux__)
			#include "./Cameras/Cameras_Unicap2.hpp"
			typedef UnicapCamera HdlCamera;	
		#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
			#error "Not Supported for the moment!"
		#else
			#error "No camera library chosen! Please, compile library using the following define : GLIP_DONT_LOAD_CAMERA_HANDLE"
		#endif

#endif

