/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : LayoutLoaderModules.cpp                                                                   */
/*     Original Date : October 14th 2013                                                                         */
/*                                                                                                               */
/*     Description   : Layout Loader Modules.                                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    LayoutLoaderModules.cpp
 * \brief   Layout Loader Modules.
 * \author  R. KERVICHE
 * \date    October 14th 2013
**/

	// Includes : 
	#include "Modules/LayoutLoaderModules.hpp"
	#include "Core/Exception.hpp"
	#include "Modules/LayoutLoader.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

	// Modules tools : 
		LayoutLoaderModule::LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence)
		 : name(_name), manual(_manual), minNumArguments(_minNumArguments), maxNumArguments(_maxNumArguments), bodyPresence(_bodyPresence)
		{ }

		LayoutLoaderModule::LayoutLoaderModule(const LayoutLoaderModule& m)
		 : name(m.name), manual(m.manual), minNumArguments(m.minNumArguments), maxNumArguments(m.maxNumArguments), bodyPresence(m.bodyPresence)
		{ }

		std::string LayoutLoaderModule::getName(void) const
		{
			return name;
		}

		const int LayoutLoaderModule::getMinNumArguments(void) const
		{
			return minNumArguments;
		}

		const int LayoutLoaderModule::getMaxNumArguments(void) const
		{
			return maxNumArguments;
		}

		const char LayoutLoaderModule::bodyPresenceTest(void) const
		{
			return bodyPresence;
		}

		std::string LayoutLoaderModule::getManual(void) const
		{
			return manual;
		}

		void LayoutLoaderModule::addBasicModules(LayoutLoader& loader)
		{
			loader.addModule( new FORMAT_SCALE );
		}

	// Simple modules : 
		// FORMAT_SCALE
			LAYOUT_LOADER_MODULE_APPLY( FORMAT_SCALE, 3, 4, -1,	"Scale a format by a scalar (or two), save as a new format.\n"
										"Arguments : nameOriginal, scaleFactor, nameNew\n"
										"            nameOriginal, scaleFactorX, scaleFactorY, nameNew\n")
		 	{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;

				if(arguments.size()==3)
				{
					CAST_ARGUMENT( 1, double, s) 	
					if(s<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (s = " + to_string(s) + ").", __FILE__, __LINE__);
		
					newFmt.setWidth( newFmt.getWidth() * s );
					newFmt.setHeight( newFmt.getHeight() * s );
				}
				else
				{
					CAST_ARGUMENT( 1, double, sx)
					CAST_ARGUMENT( 2, double, sy)
					if(sx<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (sx = " + to_string(sx) + ").", __FILE__, __LINE__);
					if(sy<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (sy = " + to_string(sy) + ").", __FILE__, __LINE__);
		
					newFmt.setWidth( newFmt.getWidth() * sx );
					newFmt.setHeight( newFmt.getHeight() * sy );
				}

				// Test : 
				if(newFmt.getWidth()<=0 || newFmt.getHeight()<=0)
					throw Exception("The new format is not valid (size : " + to_string(newFmt.getWidth()) + "x" + to_string(newFmt.getHeight()) + ").", __FILE__, __LINE__);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

