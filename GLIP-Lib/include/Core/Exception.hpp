/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Exception.hpp                                                                             */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Exception class                                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Exception.hpp
 * \brief   Exception class
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_EXCEPTION__
#define __GLIP_EXCEPTION__

	// Includes
	#include <iostream>
	#include <sstream>
	#include <vector>
	#include "Core/LibTools.hpp"

	namespace Glip
	{
		// Object
		/**
		\class Exception
		\brief Exception class, derived from std::exception.
		**/
		class GLIP_API Exception : public std::exception
		{
			private :
				// Data :
				std::string  msg;
				std::string  filename;
				unsigned int line;
				std::string completeMsg;
				std::vector<Exception>	subErrors;
				bool showHeader;

				// Tools :
				std::string header(bool showHeaderControl=true) const throw();
				void updateCompleteMessage(void);

			public :
				// Tools :
				Exception(const std::string& m, std::string f="", unsigned int l=0);
				Exception(const Exception& e);
				virtual ~Exception(void) throw();

				const char* what(void) const throw();
				const char* message(void) const throw();
				const char* file(void) const throw();
				unsigned int lineNumber(void) const throw();

				const Exception& operator+(const std::exception& e);
				const Exception& operator+(const Exception& e);

				int numSubError(void) const throw();
				const Exception& subError(int i);
				void hideHeader(bool t=true);
		};

		// Tools
		/**
		\fn template<typename TYPE> bool fromString(const std::string & Str, TYPE& Dest)
		\brief Conversion, from string to data.
		\param Str Input string.
		\param Dest Output.
		\return true if conversion was successful.
		**/
		template<typename TYPE>
		bool fromString(const std::string & Str, TYPE& Dest)
		{
			std::istringstream iss( Str );
			return iss >> Dest != 0;
		}

		/**
		\fn template<typename TYPE> std::string toString(const TYPE& Value )
		\brief Conversion, from data to string.
		\param Value Input data.
		\return Standard string.
		**/
		template<typename TYPE>
		std::string toString(const TYPE& Value )
		{
			std::ostringstream oss;
			oss << Value;
			return oss.str();
		}
	}

#endif
