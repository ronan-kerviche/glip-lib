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
		/**
		\fn LayoutLoaderModule::LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence)
		\brief LayoutLoaderModule constructor. For simple modules you can just use the macro LAYOUT_LOADER_MODULE_APPLY.
		\param _name Name of the module.
		\param _manual Manual of the module.
		\param _minNumArguments Minimum number of arguments of the module.
		\param _maxNumArguments Maximum number of arguments of the module (-1 for no limitation).
		\param _bodyPresence Requirement on the body (-1 for no body, 0 for indifferent, 1 for needed).
		**/
		LayoutLoaderModule::LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence)
		 : name(_name), manual(_manual), minNumArguments(_minNumArguments), maxNumArguments(_maxNumArguments), bodyPresence(_bodyPresence)
		{ }

		LayoutLoaderModule::LayoutLoaderModule(const LayoutLoaderModule& m)
		 : name(m.name), manual(m.manual), minNumArguments(m.minNumArguments), maxNumArguments(m.maxNumArguments), bodyPresence(m.bodyPresence)
		{ }

		/**
		\fn const std::string& LayoutLoaderModule::getName(void) const
		\brief Get the name of the module.
		\return A standard string containing the name of the module.
		**/
		const std::string& LayoutLoaderModule::getName(void) const
		{
			return name;
		}

		/**
		\fn const int& LayoutLoaderModule::getMinNumArguments(void) const
		\brief Get the minimum number of arguments of the module.
		\return An integer being the  minimum number of arguments of the module.
		**/
		const int& LayoutLoaderModule::getMinNumArguments(void) const
		{
			return minNumArguments;
		}

		/**
		\fn const int& LayoutLoaderModule::getMinNumArguments(void) const
		\brief Get the maximum number of arguments of the module.
		\return An integer being the  maximum number of arguments of the module.
		**/
		const int& LayoutLoaderModule::getMaxNumArguments(void) const
		{
			return maxNumArguments;
		}

		/**
		\fn const char& LayoutLoaderModule::bodyPresenceTest(void) const
		\brief Get the requirement on the body.
		\return A byte containing the requirement as -1 for no body, 0 for indifferent and 1 for needed.
		**/
		const char& LayoutLoaderModule::bodyPresenceTest(void) const
		{
			return bodyPresence;
		}

		/**
		\fn const std::string& LayoutLoaderModule::getManual(void) const
		\brief Get the manual of the module.
		\return A standard string containing the manual of the module.
		**/
		const std::string& LayoutLoaderModule::getManual(void) const
		{
			return manual;
		}

		/**
		\fn void LayoutLoaderModule::addBasicModules(LayoutLoader& loader)
		\brief Add the basic modules to a LayoutLoader.
		\param loader A LayoutLoader object.
		**/
		void LayoutLoaderModule::addBasicModules(LayoutLoader& loader)
		{
			loader.addModule( new FORMAT_CHANGE_SIZE );
			loader.addModule( new FORMAT_SCALE_SIZE );
			loader.addModule( new FORMAT_CHANGE_CHANNELS );
			loader.addModule( new FORMAT_CHANGE_DEPTH );
			loader.addModule( new FORMAT_CHANGE_FILTERING );
			loader.addModule( new FORMAT_CHANGE_WRAPPING );
			loader.addModule( new FORMAT_CHANGE_MIPMAP );
			loader.addModule( new GENERATE_SAME_SIZE_2D_GRID );
			loader.addModule( new GENERATE_SAME_SIZE_3D_GRID );
		}

	// Simple modules : 
			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_SIZE, 4, 4, -1, 	"Change the size of a format, save as a new format.\n"
											"Arguments : nameOriginal, widthNew, heightNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;

				CAST_ARGUMENT( 1, double, w)
				CAST_ARGUMENT( 2, double, h)

				newFmt.setWidth(w);
				newFmt.setHeight(h);

				if(newFmt.getWidth()<=0 || newFmt.getHeight()<=0)
					throw Exception("The new format is not valid (size : " + to_string(newFmt.getWidth()) + "x" + to_string(newFmt.getHeight()) + ").", __FILE__, __LINE__);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_SCALE_SIZE, 3, 4, -1,	"Scale a format by a scalar (or two), save as a new format.\n"
											"Arguments : nameOriginal, scaleFactor, nameNew.\n"
											"            nameOriginal, scaleFactorX, scaleFactorY, nameNew.")
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

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_CHANNELS, 3, 3, -1, 	"Change the channels of a format, save as a new format.\n"
											"Arguments : nameOriginal, channelNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setGLMode( glFromString(arguments[1]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_DEPTH, 3, 3, -1, 	"Change the depth of a format, save as a new format.\n"
											"Arguments : nameOriginal, depthNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setGLDepth( glFromString(arguments[1]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_FILTERING, 4, 4, -1, 	"Change the filtering of a format, save as a new format.\n"
											"Arguments : nameOriginal, minNew, magNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setMinFilter( glFromString(arguments[1]) );
				newFmt.setMagFilter( glFromString(arguments[2]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_WRAPPING, 4, 4, -1, 	"Change the wrapping of a format, save as a new format.\n"
											"Arguments : nameOriginal, sNew, tNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setSWrapping( glFromString(arguments[1]) );
				newFmt.setTWrapping( glFromString(arguments[2]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_MIPMAP, 4, 4, -1, 	"Change the mipmap level of a format, save as a new format.\n"
											"Arguments : nameOriginal, mNew, nameNew.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				CAST_ARGUMENT( 1, int, m)
				newFmt.setMaxLevel(m);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( GENERATE_SAME_SIZE_2D_GRID, 2, 2, -1, 	"Create a 2D grid geometry of the same size as the format in argument.\n"
												"Arguments : nameFormat, nameNewGeometry.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				GEOMETRY_MUST_NOT_EXIST( arguments[1] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new geometry : 
				APPEND_NEW_GEOMETRY( arguments.back(), GeometryPrimitives::PointsGrid2D(it->second.getWidth(), it->second.getHeight()) )
			}

			LAYOUT_LOADER_MODULE_APPLY( GENERATE_SAME_SIZE_3D_GRID, 2, 2, -1, 	"Create a 3D grid geometry of the same size as the format in argument.\n"
												"Arguments : nameFormat, nameNewGeometry.")
			{
				FORMAT_MUST_EXIST( arguments[0] )
				GEOMETRY_MUST_NOT_EXIST( arguments[1] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new geometry : 
				APPEND_NEW_GEOMETRY( arguments.back(), GeometryPrimitives::PointsGrid3D(it->second.getWidth(), it->second.getHeight(), it->second.getNumChannels()) )
			}

