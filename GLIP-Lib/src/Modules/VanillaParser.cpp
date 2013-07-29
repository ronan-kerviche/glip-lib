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
	#include "VanillaParser.hpp"
	#include "Exception.hpp"

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
		strKeyword.clear();
		name.clear();
		body.clear();
		arguments.clear();
	}

	bool Element::empty(void) const
	{
		return strKeyword.empty() && name.empty() && body.empty() && arguments.empty();
	}

	std::string Element::getCode(void) const
	{
		std::string res;

		if(strKeyword.empty())
			throw Exception("Element::getCode - No keyword defined.", __FILE__, __LINE__);

		// Header :
		res += strKeyword;

		if(!noName || !name.empty())
			res += ":" + name;

		if(!noArgument || !arguments.empty())
		{
			bool putComa = false;
		
			res += "(";

			for(int k=0; k<arguments.size(); k++)
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
	VanillaParser::VanillaParser(const std::string& code, int lineOffset)
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
		int 			currentLine		= lineOffset,
					bracketLevel		= 0;
		Element::Field 		currentField 		= Element::Keyword;
		Element			el;

		for(int k=0; k<code.size(); k++)
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
					throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character ':' when parsing arguments.", __FILE__, __LINE__);

				testAndSaveCurrentElement(currentField, Element::Name, el);
				el.noName = false;
				after = false;
				before = true;
			}
			else if(code[k]=='{')
			{
				if(currentField==Element::Arguments)
					throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character '{' when parsing arguments.", __FILE__, __LINE__);

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
				throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character '}'.", __FILE__, __LINE__);
			}
			else if(code[k]=='(')
			{
				if(currentField==Element::Arguments)
					throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character '(' when parsing arguments.", __FILE__, __LINE__);

				testAndSaveCurrentElement(currentField, Element::Arguments, el);
				el.noArgument = false;
				after = false;
				before = true;
			}
			else if(code[k]==',')
			{
				if(currentField!=Element::Arguments)
					throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character ','.", __FILE__, __LINE__);
				else
					el.arguments.push_back("");

				after = false;
				before = true;
			}
			else if(code[k]==')')
			{
				if(currentField!=Element::Arguments)
					throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : unexpected character ')'.", __FILE__, __LINE__);
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
				throw Exception("VanillaParser::VanillaParser - From line " + to_string(currentLine) + " : missing delimiter ','.", __FILE__, __LINE__);
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
			throw Exception("VanillaParser::VanillaParser - Parsing error at the end of the input, missing '}'.", __FILE__, __LINE__);
			
		if(currentField==Element::Arguments)
			throw Exception("VanillaParser::VanillaParser - Parsing error at the end of the input, missing ')'.", __FILE__, __LINE__);

		// Force save the last element :
		if(!el.empty())
			elements.push_back(el);
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
				throw Exception("VanillaParser::record - Internal error : attempt to save field after parsing arguments.", __FILE__, __LINE__);
			case Element::Body : 
				el.body += c;
				break;
			case Element::Unknown : 
			default :
				throw Exception("VanillaParser::record - Internal error : unknown field.", __FILE__, __LINE__);
		}

		if(el.startLine==-1)
			el.startLine = currentLine;
	}
	
	const VanillaParser& VanillaParser::operator<<(VanillaParser& subParser)
	{
		elements.insert(elements.end(), subParser.elements.begin(), subParser.elements.end());
		subParser.elements.clear();

		return (*this);
	}

