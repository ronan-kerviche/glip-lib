/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
			public :
				/// Exception type.
				enum Type
				{
					/// Unspecified exception (default if not specified).
					UnspecifiedException,
					/// From a core library component.
					CoreException,
					/// From a Module.
					ModuleException,
					/// From a glCall, or similar.
					GLException,
					/// From a failed shader compilation, with code provided by the client.
					ClientShaderException,
					/// From a failed script evaluaton, with code provided by the client.
					ClientScriptException,
					/// Client type exception.
					ClientException
				};

			private :
				// Data :
				Type			type;
				std::string  		message,
							filename;
				int			line;
				std::string		completeMessage;
				bool			subordinateException,
							showHeader;
				std::vector<Exception>	subExceptions;	

				// Tools :
				void cleanSubException(void);
				std::string header(bool showHeaderControl=true) const;
				void updateCompleteMessage(void);

			public :
				// Tools :
				Exception(const std::string& m, std::string f="", int l=-1, const Type& t=UnspecifiedException);
				Exception(const Exception& e);
				virtual ~Exception(void) throw();

				Type getType(void) const;
				const char* what(void) const throw();
				const std::string& getMessage(void) const throw();
				const std::string& getFilename(void) const throw();
				std::string getShortFilename(void) const;
				int getLineNumber(void) const throw();

				Exception& operator=(const std::exception& e);
				Exception& operator=(const Exception& e);
				Exception& operator<<(const std::exception& e);
				Exception& operator<<(const Exception& e);

				void append(const Exception& e);
				void prepend(const Exception& e);
				int getNumSubExceptions(void) const throw();
				const Exception& getSubException(int i);
				bool isSubException(void) const;
				void hideHeader(bool enabled=true);
				bool isHeaderHidden(void) const;
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
			return static_cast<bool>(iss >> Dest);
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
