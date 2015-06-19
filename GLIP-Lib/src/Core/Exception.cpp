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

#include <algorithm>
#include "Core/Exception.hpp"

	using namespace Glip;
 
	/**
	\fn Exception::Exception(const std::string& m, std::string f, int l, const Type& t)
	\brief Exception constructor.
	\param m Message.
	\param f File name (__FILE__).
	\param l Line number (__LINE__).
	\param t Type of the exception (see Exception::Type).
	**/
	Exception::Exception(const std::string& m, std::string f, int l, const Type& t)
	 :	type(t),
		message(m), 
		filename(f), 
		line(l), 
		subordinateException(false),
		showHeader(true)
	{
		if(type==CoreException || type==ModuleException || type==GLException)
			filename = getShortFilename();

		updateCompleteMessage();
	}

	/**
	\fn Exception::Exception(const Exception& e)
	\brief Exception constructor.
	\param e Copy.
	**/
	Exception::Exception(const Exception& e)
	 :	type(e.type),
		message(e.message), 
		filename(e.filename), 
		line(e.line), 
		completeMessage(e.completeMessage),
		subordinateException(e.subordinateException),
		showHeader(e.showHeader),
		subExceptions(e.subExceptions) 
	{ }

	Exception::~Exception(void) throw()
	{
		subExceptions.clear();
	}

	void Exception::cleanSubException(void)
	{
		completeMessage.clear();
		subordinateException = true;
		subExceptions.clear();
	}

	std::string Exception::header(bool showHeaderControl) const
	{
		if(filename.empty() || !showHeaderControl)
			return "";
		else
		{
			if(line>=0)
				return getShortFilename() + "; " + toString(line);
			else
				return getShortFilename();
		}
	}

	void Exception::updateCompleteMessage(void)
	{
		const int nMessages = subExceptions.size() + 1;
		const std::string mainHeader = header(showHeader);

		completeMessage.clear();

		if(nMessages==1)
		{
			if(!mainHeader.empty())
				completeMessage = "[ " + mainHeader + " ] " + message;
			else
				completeMessage = message;
		} 
		else
		{
			// Create all headers : 
			std::vector<std::string> 	headers, 
							messages;
			size_t maxLength = 0;

			if(!mainHeader.empty())
			{
				headers.push_back(std::string("[ 1 | " + mainHeader + " ") );
				maxLength = headers.back().size();
			}
			else
				headers.push_back( std::string("[ 1 ") );

			messages.push_back(message);

			for(int i=subExceptions.size()-1; i>=0; i--)
			{
				const std::string header = subExceptions[i].header(showHeader);

				if(!header.empty())
					headers.push_back("[ " + toString(subExceptions.size()-i+1) + " | " + header + " ");
				else
					headers.push_back("[ " + toString(subExceptions.size()-i+1) + " ");

				messages.push_back(subExceptions[i].message);

				maxLength = std::max(maxLength, headers.back().size());
			}

			// Finally : padd and create
			const std::string blank(maxLength, ' ');
			std::string padded;

			for(int k=0; k<static_cast<int>(headers.size()); k++)
			{
				if(k>0)
					completeMessage += "\n";

				padded = blank;
				padded.replace(0, headers[k].size(), headers[k]);
				padded += "] ";
				completeMessage += padded + messages[k];
			}
		}
	}

	/**
	\fn Exception::Type Exception::getType(void) const
	\brief Get the type of the exception.
	\return The type of this exception (see Exception::Type).
	**/
	Exception::Type Exception::getType(void) const
	{
		return type;
	}

	/**
	\fn const char* Exception::what(void) const throw()
	\brief Get the complete exception message.
	\return C string.
	**/
	const char* Exception::what(void) const throw()
	{
		return completeMessage.c_str();
	}

	/**
	\fn const std::string& Exception::getMessage(void) const throw()
	\brief Get the exception message.
	\return Reference to a std::string containing the message.
	**/
	const std::string& Exception::getMessage(void) const throw()
	{
		return message;
	}

	/**
	\fn const std::string& Exception::getFilename(void) const throw()
	\brief Get the filename the exception was generated from.
	\return Reference to a std::string containing the filename.
	**/
	const std::string& Exception::getFilename(void) const throw()
	{
		return filename;
	}

	/**
	\fn std::string Exception::getShortFilename(void) const
	\brief Get the last part of the filename (following the last separator).
	\return String containing the shortened filename.
	**/
	std::string Exception::getShortFilename(void) const
	{
		size_t p = filename.find_last_of("/\\");
		
		if(p==std::string::npos)
			return filename;
		else
			return filename.substr(p+1);
	}

	/**
	\fn int Exception::getLineNumber(void) const throw()
	\brief Get the line number the exception was generated from.
	\return The line number.
	**/
	int Exception::getLineNumber(void) const throw()
	{
		return line;
	}	

	/**
	\fn Exception& Exception::operator=(const std::exception& e)
	\brief Copy an exception.
	\param e Source exception.
	\return Reference to this exception.
	**/
	Exception& Exception::operator=(const std::exception& e)
	{
		message		= e.what();
		type		= UnspecifiedException;
		line		= 0;
		showHeader	= false;
		filename.clear();
		subExceptions.clear();
		updateCompleteMessage();

		return (*this);
	}

	/**
	\fn Exception& Exception::operator=(const Exception& e)
	\brief Copy an exception.
	\param e Source exception.
	\return Reference to this exception.
	**/
	Exception& Exception::operator=(const Exception& e)
	{
		message			= e.message;
		filename		= e.filename;
		line			= e.line;
		completeMessage		= e.completeMessage;
		subordinateException	= e.subordinateException;
		showHeader		= e.showHeader;
		subExceptions		= e.subExceptions;

		return (*this);
	}

	/**
	\fn Exception& Exception::operator<<(const std::exception& e)
	\brief Concatenate the mesages of two exceptions.
	\param e The original exception.
	\return This exception message followed by the original exception message.
	**/
	Exception& Exception::operator<<(const std::exception& e)
	{
		Exception ex(e.what());
		ex.cleanSubException();
		subExceptions.push_back(ex);

		updateCompleteMessage();

		return *this;
	}

	/**
	\fn Exception& Exception::operator<<(const Exception& e)
	\brief Concatenate the mesages of two exceptions.
	\param e The original Exception.
	\return This exception message followed by the original exception message.
	**/
	Exception& Exception::operator<<(const Exception& e)
	{
		if(!e.subExceptions.empty())
			subExceptions.insert(subExceptions.end(), e.subExceptions.begin(), e.subExceptions.end());
	
		Exception ex(e);
		ex.cleanSubException();
		subExceptions.push_back(ex);

		updateCompleteMessage();

		return *this;
	}

	/**
	\fn void Exception::append(const Exception& e)
	\brief Concatenate the messages of two exceptions. Note that this is a reversed version of operator<<.
	\param e A sub Exception.
	**/
	void Exception::append(const Exception& e)
	{
		Exception ex(e);
		ex.cleanSubException();
		subExceptions.push_back(ex);

		if(!e.subExceptions.empty())
		{
			for(unsigned int k=e.subExceptions.size()-1; k>0; k++)
				subExceptions.push_back(e.subExceptions[k]);
			subExceptions.push_back(e.subExceptions.front());
		}

		updateCompleteMessage();
	}

	/**
	\fn void Exception::prepend(const Exception& e)
	\brief Concatenate the messages of two exceptions. Use this version to serially concatenate multiple descriptions.
	\param e A sub Exception.
	**/
	void Exception::prepend(const Exception& e)
	{	
		if(!e.subExceptions.empty())
			subExceptions.insert(subExceptions.begin(), e.subExceptions.begin(), e.subExceptions.end());
		
		Exception ex(e);
		ex.cleanSubException();
		subExceptions.insert(subExceptions.begin(), ex);

		updateCompleteMessage();
	}

	/**
	\fn int Exception::getNumSubExceptions(void) const throw();
	\brief Get the number of sub-error embedded.
	\return The number of sub-error embedded.
	**/
	int Exception::getNumSubExceptions(void) const throw()
	{
		return subExceptions.size();
	}

	/**
	\fn const Exception& Exception::getSubException(int i)
	\brief Return the sub-error at index i.
	\param i The index of the sub-error.
	\return Return the sub-error corresponding at index i or raise an error if the index is invalid.
	**/
	const Exception& Exception::getSubException(int i)
	{
		if(i<0 || i>getNumSubExceptions())
			throw Exception("Exception::subException - Index out of bounds (" + toString(i) + ", range is [0;" + toString(getNumSubExceptions()) + "].", __FILE__, __LINE__);
		else
			return subExceptions[i];
	}

	/**
	\fn bool Exception::isSubException(void) const
	\brief Test if the exception is a subordinate.
	\return True if the exception is a subordinate.
	**/
	bool Exception::isSubException(void) const
	{
		return subordinateException;
	}

	/**
	\fn void Exception::hideHeader(bool enabled)
	\brief Hide (or show) the headers in messages provided by what().
	\param enabled true (default) to hide the headers or true to show them.
	**/
	void Exception::hideHeader(bool enabled)
	{
		if(enabled==showHeader)
		{
			showHeader = !enabled;
			updateCompleteMessage();
		}
	}

	/**
	\fn bool Exception::isHeaderHidden(void) const
	\brief Test if the header will be shown in the exception message.
	\return True if the header is hidden.
	**/
	bool Exception::isHeaderHidden(void) const
	{
		return !showHeader;
	}

