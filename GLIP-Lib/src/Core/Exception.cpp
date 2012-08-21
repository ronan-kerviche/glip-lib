/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
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
		if(filename!="")
		{
			size_t p = filename.rfind('/');
			filename = filename.substr(p+1);
		}

		//std::cout << "Building exception on : " << std::endl;
		//std::cout << m << std::endl;
	}

	/**
	\fn Exception::Exception(const Exception& e)
	\brief Exception constructor.
	\param e Copy.
	**/
	Exception::Exception(const Exception& e)
	 : msg(e.msg), filename(e.filename), line(e.line), subErrors(e.subErrors)
	{ }

	Exception::~Exception(void) throw()
	{
		subErrors.clear();
	}

	/**
	\fn const char* Exception::what(void) const throw()
	\brief Get the complete exception message.
	\return C string.
	**/
	const char* Exception::what(void) const throw()
	{
		std::string completeMsg;

		int nMessages = subErrors.size() + 1;

		std::string h = header();

		if(nMessages==1)
		{
			if(h!="")
				completeMsg += "[ " + h + " ] " + msg;
			else
				completeMsg += msg;
		}
		else
		{
			if(h!="")
				completeMsg += "[ 1 | " + h + " ] " + msg + "\n";
			else
				completeMsg += "[ 1 ] " + msg + "\n";

			for(int i=subErrors.size()-1; i>=0; i--)
			{
				std::string he = subErrors[i].header();
				if(he!="")
					completeMsg += "[ " + to_string(subErrors.size()-i+1) + " | " + he + " ] " + subErrors[i].msg + "\n";
				else
					completeMsg += "[ " + to_string(subErrors.size()-i+1) + " ] " + subErrors[i].msg + "\n";
			}
		}

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

	std::string Exception::header(void) const throw()
	{
		if(filename=="")
			return "";
		else
		{
			if(line!=0)
				return filename + "; " + to_string(line);
			else
				return filename;
		}
	}

	/**
	\fn const Exception& Exception::operator+(const std::exception& e)
	\brief Add the mesages of two exceptions.
	\param e The original exception.
	\return This exception message followed by the original exception message.
	**/
	const Exception& Exception::operator+(const std::exception& e)
	{
		subErrors.push_back(Exception(e.what()));
		//std::cout << "std::exception!" << std::endl;
		subErrors.push_back(Exception("<std::exception>"));

		return *this;
	}

	/**
	\fn const Exception& Exception::operator+(const Exception& e)
	\brief Add the mesages of two exceptions.
	\param e The original Exception.
	\return This exception message followed by the original exception message.
	**/
	const Exception& Exception::operator+(const Exception& e)
	{
		//std::cout << "Adding : (has " << e.subErrors.size() << " subs)" << std::endl;
		//std::cout << e.what() << std::endl;

		if(!e.subErrors.empty());
			subErrors.insert( subErrors.end(), e.subErrors.begin(), e.subErrors.end() );

		subErrors.push_back(e);
		subErrors.back().subErrors.clear();

		//std::cout << "TEST : " << std::endl;
		//std::cout << "---------------------------------------------" << std::endl;
		//std::cout << what();
		//std::cout << "---------------------------------------------" << std::endl;
		//std::cout << std::endl << std::endl << std::endl;

		return *this;
	}

	/**
	\fn int Exception::numSubError(void) const throw();
	\brief Get the number of sub-error embedded.
	\return The number of sub-error embedded.
	**/
	int Exception::numSubError(void) const throw()
	{
		return subErrors.size();
	}

	/**
	\fn const Exception& Exception::subError(int i)
	\brief Return the sub-error at index i.
	\param i The index of the sub-error.
	\return Return the sub-error corresponding at index i or raise an error if the index is invalid.
	**/
	const Exception& Exception::subError(int i)
	{
		if(i<0 || i>numSubError())
			throw Exception("Exception::subError - Index out of bounds (" + to_string(i) + " out of [0;" + to_string(numSubError()) + "].", __FILE__, __LINE__);

		else
			return subErrors[i];
	}
