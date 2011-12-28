/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Exception.cpp                                                                             */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Exception class                                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Exception.cpp
 * \brief   Exception class
 * \author  R. KERVICHE
 * \version 0.6
 * \date    August 15th 2011
**/

#include "Exception.hpp"

	using namespace Glip;

	/**
	\fn Exception::Exception(const std::string& m, std::string f, unsigned int l)
	\brief Exception constructor.
	\param m Message.
	\param f File name (__FILE__).
	\param l Line number (__LINE__).
	**/
	Exception::Exception(const std::string& m, std::string f, unsigned int l)
	 : msg(m), filename(f), line(l)
	{
		completeMsg = '[';

		if(filename!="")
		{
			completeMsg += filename;
			completeMsg += ", ";
		}


		if(line!=0)
		{
			completeMsg += to_string(line);
		}

		completeMsg += "]\t";
		completeMsg += msg;
	}

	/**
	\fn Exception::Exception(const Exception& e)
	\brief Exception constructor.
	\param e Copy.
	**/
	Exception::Exception(const Exception& e)
	 : msg(e.msg), filename(e.filename), line(e.line), completeMsg(e.completeMsg)
	{ }

	Exception::~Exception(void) throw()
	{ }

	/**
	\fn const char* Exception::what(void) const throw()
	\brief Get the complete exception message.
	\return C string.
	**/
	const char* Exception::what(void) const throw()
	{
		return completeMsg.c_str();
	}

	/**
	\fn const char* Exception::message(void) const throw()
	\brief Get the exception message.
	\return C string.
	**/
	const char* Exception::message(void) const throw()
	{
		return msg.c_str();
	}

	/**
	\fn const char* Exception::file(void) const throw()
	\brief Get the filename the exception was generated from.
	\return C string.
	**/
	const char* Exception::file(void) const throw()
	{
		return filename.c_str();
	}

	/**
	\fn unsigned int Exception::lineNumber(void) const throw()
	\brief Get the line number the exception was generated from.
	\return The line number.
	**/
	unsigned int Exception::lineNumber(void) const throw()
	{
		return line;
	}

	/**
	\fn const Exception& Exception::operator+(const std::exception& e)
	\brief Add the mesages of two exceptions.
	\param e The original exception.
	\return This exception message followed by the original exception message.
	**/
	const Exception& Exception::operator+(const std::exception& e)
	{
		std::string str(e.what());

		size_t beg = 0;

		while((beg = str.find('\n', beg))!=std::string::npos)
		{
			beg++;
			str.insert(beg, 1, '\t'); // insert one TAB after the END-OF-LINE
		}

		msg += "\n\t";
		msg += str;

		completeMsg += "\n\t";
		completeMsg += str;

		return *this;
	}
