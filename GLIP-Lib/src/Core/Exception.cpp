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

#include "Core/Exception.hpp"

	using namespace Glip;
 
	/**
	\fn Exception::Exception(const std::string& m, std::string f, unsigned int l)
	\brief Exception constructor.
	\param m Message.
	\param f File name (__FILE__).
	\param l Line number (__LINE__).
	**/
	Exception::Exception(const std::string& m, std::string f, unsigned int l)
	 :	msg(m), 
		filename(f), 
		line(l), 
		completeMsg(),
		showHeader(true),
		subExceptions()
	{
		if(filename!="")
		{
			size_t p = filename.find_last_of("/\\");
			filename = filename.substr(p+1);
		}

		updateCompleteMessage();
	}

	/**
	\fn Exception::Exception(const Exception& e)
	\brief Exception constructor.
	\param e Copy.
	**/
	Exception::Exception(const Exception& e)
	 :	msg(e.msg), 
		filename(e.filename), 
		line(e.line), 
		completeMsg(e.completeMsg),
		showHeader(e.showHeader),
		subExceptions(e.subExceptions) 
	{ }

	Exception::~Exception(void) throw()
	{
		subExceptions.clear();
	}

	void Exception::updateCompleteMessage(void)
	{
		completeMsg.clear();

		int nMessages = subExceptions.size() + 1;

		std::string h = header(showHeader);

		if(nMessages==1)
		{
			if(h!="")
				completeMsg += "[ " + h + " ] " + msg;
			else
				completeMsg += msg;
		} 
		else
		{
			// Create all headers : 
			std::vector<std::string> headers, messages;
			size_t maxLength = 0;

			if(!h.empty())
			{
				headers.push_back( std::string("[ 1 | " + h + " ") );
				maxLength = headers.back().size();
			}
			else
				headers.push_back( std::string("[ 1 ") );

			messages.push_back(msg);

			for(int i=subExceptions.size()-1; i>=0; i--)
			{
				headers.push_back("");
				std::string header = subExceptions[i].header(showHeader);

				if(!header.empty())
					headers.back() = "[ " + toString(subExceptions.size()-i+1) + " | " + header + " ";
				else
					headers.back() = "[ " + toString(subExceptions.size()-i+1) + " ";

				messages.push_back(subExceptions[i].msg);

				maxLength = std::max(maxLength, headers.back().size());
			}

			// Finally : padd and create
			const std::string blank(maxLength, ' ');
			std::string padded;

			for(int k=0; k<static_cast<int>(headers.size()); k++)
			{
				if(k>0)
					completeMsg += "\n";

				padded = blank;
				padded.replace(0, headers[k].size(), headers[k]);
				padded += "] ";
				completeMsg += padded + messages[k];
			}
		}
	}

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

	std::string Exception::header(bool showHeaderControl) const throw()
	{
		if(filename.empty() || !showHeaderControl)
			return std::string();
		else
		{
			if(line!=0)
				return filename + "; " + toString(line);
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
		subExceptions.push_back(Exception(e.what()));
		subExceptions.push_back(Exception("<std::exception>"));

		updateCompleteMessage();

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
		if(!e.subExceptions.empty())
			subExceptions.insert( subExceptions.end(), e.subExceptions.begin(), e.subExceptions.end() );

		subExceptions.push_back(e);
		subExceptions.back().subExceptions.clear();

		updateCompleteMessage();

		return *this;
	}

	/**
	\fn int Exception::numSubExceptions(void) const throw();
	\brief Get the number of sub-error embedded.
	\return The number of sub-error embedded.
	**/
	int Exception::numSubExceptions(void) const throw()
	{
		return subExceptions.size();
	}

	/**
	\fn const Exception& Exception::subException(int i)
	\brief Return the sub-error at index i.
	\param i The index of the sub-error.
	\return Return the sub-error corresponding at index i or raise an error if the index is invalid.
	**/
	const Exception& Exception::subException(int i)
	{
		if(i<0 || i>numSubExceptions())
			throw Exception("Exception::subException - Index out of bounds (" + toString(i) + ", range is [0;" + toString(numSubExceptions()) + "].", __FILE__, __LINE__);

		else
			return subExceptions[i];
	}

	/**
	\fn void Exception::hideHeader(bool enabled)
	\brief Hide (or show) the headers in messages provided by what().
	\param enabled true (default) to hide the headers or true to show them.
	**/
	void Exception::hideHeader(bool enabled)
	{
		showHeader = !enabled;
		updateCompleteMessage();
	}

