/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : VanillaParser.cpp                                                                         */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Small file parser.                                                                        */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    VanillaParser.cpp 
 * \brief   Small file parser.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

	// Includes : 
	#include <limits>
	#include <cmath>
	#include <algorithm>
	#include "Modules/VanillaParser.hpp"
	#include "Core/Exception.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

// LayoutLoaderParser::Element
	Element::Element(void)
	{
		clear();
	}

	Element::Element(const Element& cpy)
	{
		(*this) = cpy;
	}

	const Element& Element::operator=(const Element& cpy)
	{
		sourceName	= cpy.sourceName;
		strKeyword	= cpy.strKeyword;
		name		= cpy.name;
		body		= cpy.body;
		arguments	= cpy.arguments;
		noName		= cpy.noName;
		noArgument	= cpy.noArgument;
		noBody		= cpy.noBody;
		startLine	= cpy.startLine;
		bodyLine	= cpy.bodyLine;

		return (*this);
	}

	void Element::clear(void)
	{
		noName		= true;
		noArgument	= true;
		noBody		= true;
		startLine	= -1;
		bodyLine	= -1;
		sourceName.clear();
		strKeyword.clear();
		name.clear();
		body.clear();
		arguments.clear();
	}

	bool Element::empty(void) const
	{
		return strKeyword.empty() && name.empty() && body.empty() && arguments.empty();
	}

	std::string Element::getCleanBody(void) const
	{	
		// Align the elements with their tabs.
		const std::string 	spacers = " \t\r\n\f\v";//,
					//spaces	= " \t";
		const char 		newLine	= '\n',
					//space	= ' ',
					tab	= '\t';

		size_t 	lineStart 	= 0,
			lineEnd		= body.find(newLine, lineStart),
			minNumTabs	= std::numeric_limits<size_t>::max(),
			firstLine	= std::string::npos,			// not specified yet.
			lastLine	= std::string::npos;			// not specified yet.
			 
		// Test : 
		while(lineStart!=std::string::npos)
		{
			// Is the line empty of usable characters?
			size_t firstChar = body.find_first_not_of(spacers, lineStart);

			if( firstChar < lineEnd )
			{
				// Count the number of spaces (or equivalent) :
				bool 	isSpaceChar 	= true;
				size_t 	numSpaces 	= 0,
					pos 		= lineStart;

				while(isSpaceChar)
				{
					if( body[pos]==tab )
						numSpaces += 1;
					else
						isSpaceChar = false;

					pos++;

					if(pos>=body.size())
						isSpaceChar = false;
				}

				// Get the minimum : 
				minNumTabs = std::min(minNumTabs, numSpaces);
			}

			// Next line : 
			if(lineEnd!=std::string::npos)
			{
				lineStart 	= lineEnd + 1;
				lineEnd		= body.find(newLine, lineStart);
			}
			else
				lineStart	= std::string::npos;
		}

		// Remove : 
		std::string	cleanBody = body;

		lineStart 	= 0;
		lineEnd		= cleanBody.find(newLine, lineStart);
		
		while(lineStart!=std::string::npos)
		{
			// Is the line empty of usable characters?
			size_t firstChar = cleanBody.find_first_not_of(spacers, lineStart);

			if( firstChar < lineEnd )
			{
				// Remove the minimum number of spaces or equivalent : 
				size_t 		endRemoval 	= lineStart;
				long long	count		= minNumTabs;

				while(count>0)
				{
					if(cleanBody[endRemoval]==tab)
						count-=1;
					else 
						throw Exception("Element::getCleanBody - Out of range (internal error).", __FILE__, __LINE__, Exception::ModuleException);

					endRemoval++;
				}

				// Remove : 
				cleanBody.erase(lineStart, endRemoval - lineStart);

				// Lines : 
				if(firstLine==std::string::npos)
					firstLine = lineStart;

				lastLine = lineStart; // Save this line as the last line.

				// Find new End : 
				lineEnd	= cleanBody.find(newLine, lineStart);
			}

			// Next line : 
			if(lineEnd!=std::string::npos)
			{
				lineStart 	= lineEnd + 1;
				lineEnd		= cleanBody.find(newLine, lineStart);
			}
			else
				lineStart	= std::string::npos;
		}

		// No content : 
		if(firstLine==std::string::npos)
			return "";

		// Remove front and back lines : 
		size_t lastCharacter = cleanBody.find(newLine, lastLine);

		if(lastCharacter==std::string::npos)
			return cleanBody.substr(firstLine);
		else
			return cleanBody.substr(firstLine, lastCharacter-firstLine);
	}

	std::string Element::getCode(void) const
	{
		std::string res;

		if(strKeyword.empty())
			throw Exception("Element::getCode - No keyword defined.", __FILE__, __LINE__, Exception::ModuleException);

		// Header :
		res += strKeyword;

		if(!noName || !name.empty())
			res += ":" + name;

		if(!noArgument || !arguments.empty())
		{
			bool putComa = false;
		
			res += "(";

			for(unsigned int k=0; k<arguments.size(); k++)
			{
				if(putComa)
					res += ", ";
				
				res += arguments[k];

				putComa = true;
			}

			res +=")";
		}

		// Body and indent
		if(!noBody || !body.empty())
		{
			res += "\n{\n";

			std::string tmpBody = body;

			size_t ptr = tmpBody.find('\n');

			while(ptr!=std::string::npos)
			{
				tmpBody.insert( ptr+1, "\t");
				ptr = tmpBody.find('\n', ptr+1);
			}

			res += "\t" + tmpBody;

			res += "\n}";
		}

		return res;
	}
	
// LayoutLoaderParser::VanillaParser
	VanillaParser::VanillaParser(const std::string& code, const std::string& _sourceName, int startLine)
	 :	sourceName(_sourceName)
	{
		const std::string	spacers 		= " \t\r\n\f\v",
				  	cmtMultiStart		= "/*",
				  	cmtMultiEnd		= "*/",
				  	cmtMonoStart  		= "//",
				  	cmtMonoEnd    		= "\n";

		bool			multiLineComment	= false,
					monoLineComment		= false,
					before			= true,
					after			= false;
		int 			currentLine		= startLine,
					bracketLevel		= 0;
		Element::Field 		currentField 		= Element::Keyword;
		Element			el;

		for(int k=0; k<static_cast<int>(code.size()); k++)
		{
			const bool isSpacer = (spacers.find(code[k])!=std::string::npos);

			if(code[k]=='\n')
				currentLine++;

			if(bracketLevel>0)
			{
				// Inside some brackets?
				if(code[k]=='{')
					bracketLevel++;
				else if(code[k]=='}')
					bracketLevel--;
				
				if(currentField==Element::Body && bracketLevel>0)
					el.body += code[k];

				if(bracketLevel==0) //out
				{
					testAndSaveCurrentElement(currentField, Element::Keyword, el);
					after = false;
					before = true;
				}
			}
			else if(multiLineComment)
			{
				// This is the end of the multiline comment :  
				if( compare(code, k, cmtMultiEnd) )
					multiLineComment = false;
			}
			else if(monoLineComment)
			{
				// This is the end of the monoline comment : 
				if( compare(code, k, cmtMonoEnd) )
					monoLineComment = false;
			}
			else if( compare(code, k, cmtMultiStart) )
			{
				// Start multi line comment : 
				multiLineComment = true;
			}
			else if( compare(code, k, cmtMonoStart) )
			{
				// Start mono line comment : 
				monoLineComment = true;
			}
			else if(code[k]==':')
			{
				if(currentField==Element::Arguments)
					throw Exception("Unexpected character ':' when parsing arguments.", getSourceName(), currentLine, Exception::ClientScriptException);

				testAndSaveCurrentElement(currentField, Element::Name, el);
				el.noName = false;
				after = false;
				before = true;
			}
			else if(code[k]=='{')
			{
				if(currentField==Element::Arguments)
					throw Exception("Unexpected character '{' when parsing arguments.", getSourceName(), currentLine, Exception::ClientScriptException);

				testAndSaveCurrentElement(currentField, Element::Body, el);
				el.noBody = false;
				after = false;
				before = true;
				bracketLevel = 1;
				if(el.startLine<0)
					el.startLine = currentLine;
				if(el.bodyLine<0)
					el.bodyLine = currentLine;
			}
			else if(code[k]=='}')
			{
				throw Exception("Unexpected character '}'.", getSourceName(), currentLine, Exception::ClientScriptException);
			}
			else if(code[k]=='(')
			{
				if(currentField==Element::Arguments)
					throw Exception("Unexpected character '(' when parsing arguments.", getSourceName(), currentLine, Exception::ClientScriptException);

				testAndSaveCurrentElement(currentField, Element::Arguments, el);
				el.noArgument = false;
				after = false;
				before = true;
			}
			else if(code[k]==',')
			{
				if(currentField!=Element::Arguments)
					throw Exception("Unexpected character ','.", getSourceName(), currentLine, Exception::ClientScriptException);
				else
					el.arguments.push_back("");

				after = false;
				before = true;
			}
			else if(code[k]==')')
			{
				if(currentField!=Element::Arguments)
					throw Exception("Unexpected character ')'.", getSourceName(), currentLine, Exception::ClientScriptException);
				else
					currentField = Element::AfterArguments;

				after = false;
				before = true;
			}
			else if(!isSpacer && !after)
			{
				if(currentField==Element::AfterArguments)
					testAndSaveCurrentElement(currentField, Element::Keyword, el);

				record(el, currentField, code[k], currentLine);
				before = false;
			}
			else if(isSpacer && !after && !before)
			{
				after = true;
			}
			else if(!isSpacer && after && currentField==Element::Arguments)
			{
				throw Exception("Missing delimiter ','.", getSourceName(), currentLine, Exception::ClientScriptException);
			}
			else if(!isSpacer && after)
			{
				// Will necessarily save!
				testAndSaveCurrentElement(currentField, Element::Keyword, el);
				record(el, currentField, code[k], currentLine);
				after = false;
			}
		}

		// Test for possible end of input : 
		if(bracketLevel>0)
			throw Exception("Parsing error at the end of the input, missing '}'.", getSourceName(), currentLine, Exception::ClientScriptException);
			
		if(currentField==Element::Arguments)
			throw Exception("Parsing error at the end of the input, missing ')'.", getSourceName(), currentLine, Exception::ClientScriptException);

		// Force save the last element :
		if(!el.empty())
		{
			el.sourceName = getSourceName();
			elements.push_back(el);
		}
	}

	bool VanillaParser::compare(const std::string& code, int& k, const std::string token)
	{
		if(code.substr(k,token.size())==token)
		{
			k += token.size() - 1;
			return true;
		}
		else 
			return false;
	}

	void VanillaParser::testAndSaveCurrentElement(Element::Field& current, const Element::Field& next, Element& el)
	{
		if(next<=current && !el.empty())
		{
			el.sourceName = getSourceName();
			elements.push_back(el);
			el.clear();
		}

		current = next;
	}

	void VanillaParser::record(Element& el, const Element::Field& field, char c, int currentLine)
	{
		switch(field)
		{
			case Element::Keyword : 
				el.strKeyword += c;
				break;
			case Element::Name : 
				el.name += c;
				break;
			case Element::Arguments :
				if(el.arguments.empty())
					el.arguments.push_back("");
				el.arguments.back() += c;
				break;
			case Element::AfterArguments :
				throw Exception("VanillaParser::record - Internal error : attempt to save field after parsing arguments.", __FILE__, __LINE__, Exception::ModuleException);
			case Element::Body : 
				el.body += c;
				break;
			case Element::Unknown : 
			default :
				throw Exception("VanillaParser::record - Internal error : unknown field.", __FILE__, __LINE__, Exception::ModuleException);
		}

		if(el.startLine==-1)
			el.startLine = currentLine;
	}

	const std::string& VanillaParser::getSourceName(void) const
	{
		return sourceName;
	}
	
	VanillaParser& VanillaParser::operator<<(const VanillaParser& subParser)
	{
		elements.insert(elements.end(), subParser.elements.begin(), subParser.elements.end());

		return (*this);
	}

