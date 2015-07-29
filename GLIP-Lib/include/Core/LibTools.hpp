/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : OglInclude.hpp                                                                            */
/*     Original Date : December 2nd 2013                                                                         */
/*                                                                                                               */
/*     Description   : Specific defines for the API structure.                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    LibTools.hpp
 * \brief   Specific defines for the API structure.
 * \author  R. KERVICHE
 * \date    December 2nd 2013
**/

#ifndef __LIBTOOLS_INCLUDE__
#define __LIBTOOLS_INCLUDE__

	#ifdef _WIN32
		#define	GLIP_API 	__declspec(dllexport)
		#define	GLIP_API_FUNC 	__declspec(dllexport) extern
	#else
		#define GLIP_API 	
		#define	GLIP_API_FUNC 	extern
	#endif

	#define UNUSED_PARAMETER(x) (void)(x);

#endif

