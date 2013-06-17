/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : VanillaParser.hpp                                                                         */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Small file parser.                                                                        */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    VanillaParser.hpp 
 * \brief   Small file parser.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

#ifndef __VANILLA_PARSER_INCLUDE__
#define __VANILLA_PARSER_INCLUDE__

	#include <string>
	#include <vector>

namespace Glip
{
	namespace Modules
	{
		namespace VanillaParserSpace
		{
			/*
				Script general specification : 
				
					// Comment
					KEYWORD:NAME(arg1, arg2, ..., argN)
					{
						BODY
					}
					/* Comment ...
					   On Multiple lines ...
					*/
			/* */

			// Layout
			struct Element
			{
				enum Field
				{
					Keyword,
					Name,
					Arguments,
					AfterArguments,
					Body,
					NumField,
					Unknown
				};


				std::string 			strKeyword,
								name,
								body;
				std::vector<std::string>	arguments;
				bool				noName,		// Missing ':'
								noArgument,	// Missing '(' to ')'
								noBody;		// Missing '{' to '}'
				int				startLine,
								bodyLine;

				Element(void);
				Element(const Element& cpy);
				const Element& operator=(const Element& cpy);
				void clear(void);
				bool empty(void) const;
			};

			// Parser Class : 
			class VanillaParser 
			{
				private : 
					bool compare(const std::string& code, int& k, const std::string token);
					void testAndSaveCurrentElement(Element::Field& current, const Element::Field& next, Element& el);
					void record(Element& el, const Element::Field& field, char c, int currentLine);
		
				public :
					std::vector<Element>		elements;

					VanillaParser(const std::string& code, int lineOffset=1);
					const VanillaParser& operator<<(VanillaParser& subParser); 
			};
		}
	}
}

#endif

