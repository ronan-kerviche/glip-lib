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
		\fn LayoutLoaderModule::LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence, bool _showManualOnError)
		\brief LayoutLoaderModule constructor. For simple modules you can just use the macro LAYOUT_LOADER_MODULE_APPLY.
		\param _name Name of the module.
		\param _manual Manual of the module.
		\param _minNumArguments Minimum number of arguments of the module.
		\param _maxNumArguments Maximum number of arguments of the module (-1 for no limitation).
		\param _bodyPresence Requirement on the body (-1 for no body, 0 for indifferent, 1 for needed).
		\param _showManualOnError Requiring the LayoutLoader to show the manual if any errors occur.
		**/
		LayoutLoaderModule::LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence, bool _showManualOnError)
		 : 	name(_name), 
			manual(_manual), 
			minNumArguments(_minNumArguments), 
			maxNumArguments(_maxNumArguments), 
			bodyPresence(_bodyPresence),
			showManualOnError(_showManualOnError)
		{ }

		LayoutLoaderModule::LayoutLoaderModule(const LayoutLoaderModule& m)
		 : 	name(m.name), 
			manual(m.manual), 
			minNumArguments(m.minNumArguments), 
			maxNumArguments(m.maxNumArguments), 
			bodyPresence(m.bodyPresence), 
			showManualOnError(m.showManualOnError)
		{ }

		LayoutLoaderModule::~LayoutLoaderModule(void)
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
		\fn const int& LayoutLoaderModule::getMaxNumArguments(void) const
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
		\fn const bool& LayoutLoaderModule::requiringToShowManualOnError(void) const
		\brief Test if the module requires the LayoutLoader object to show this LayoutLoaderModule manual if any errors occur during LayoutLoaderModule::apply.
		\return A boolean corresponding to the requirement.
		**/
		const bool& LayoutLoaderModule::requiringToShowManualOnError(void) const
		{
			return showManualOnError;
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
			loader.addModule( new IF_SHAREDCODE_DEFINED );
			loader.addModule( new IF_FORMAT_DEFINED );
			loader.addModule( new IF_SHADERSOURCE_DEFINED );
			loader.addModule( new IF_GEOMETRY_DEFINED );
			loader.addModule( new IF_FILTERLAYOUT_DEFINED );
			loader.addModule( new IF_PIPELINELAYOUT_DEFINED );
			loader.addModule( new IF_REQUIREDFORMAT_DEFINED );
			loader.addModule( new IF_REQUIREDGEOMETRY_DEFINED );
			loader.addModule( new IF_REQUIREDPIPELINE_DEFINED );
			loader.addModule( new FORMAT_CHANGE_SIZE );
			loader.addModule( new FORMAT_SCALE_SIZE );
			loader.addModule( new FORMAT_CHANGE_CHANNELS );
			loader.addModule( new FORMAT_CHANGE_DEPTH );
			loader.addModule( new FORMAT_CHANGE_FILTERING );
			loader.addModule( new FORMAT_CHANGE_WRAPPING );
			loader.addModule( new FORMAT_CHANGE_MIPMAP );
			loader.addModule( new FORMAT_MINIMUM_WIDTH );
			loader.addModule( new FORMAT_MAXIMUM_WIDTH );
			loader.addModule( new FORMAT_MINIMUM_HEIGHT );
			loader.addModule( new FORMAT_MAXIMUM_HEIGHT );
			loader.addModule( new FORMAT_MINIMUM_PIXELS );
			loader.addModule( new FORMAT_MAXIMUM_PIXELS );
			loader.addModule( new FORMAT_MINIMUM_ELEMENTS );
			loader.addModule( new FORMAT_MAXIMUM_ELEMENTS );
			loader.addModule( new IF_FORMAT_SETTING_MATCH );
			loader.addModule( new IF_FORMAT_SETTING_LARGERTHAN );
			loader.addModule( new GENERATE_SAME_SIZE_2D_GRID );
			loader.addModule( new GENERATE_SAME_SIZE_3D_GRID );
			loader.addModule( new CHAIN_PIPELINES );
			loader.addModule( new FORMAT_TO_CONSTANT );
			loader.addModule( new ABORT_ERROR );
		}

		/**
		\fn void LayoutLoaderModule::getCases(const std::string& body, std::string& trueCase, std::string& falseCase, const std::string& sourceName, int bodyLine)
		\brief Get true and false cases out of a body.
	
		In a if-statement you can write : 
		IF(tatement)
		{
			TRUE
			{
				//...
			}
			FALSE
			{
				//...
			}
		}

		\param body Body to extract the data from.
		\param trueCase Body of the true statement (not modified is none is found).
		\param falseCase Body of the false statement (not modified is none is found).
		\param sourceName Name of the source.
		\param bodyLine Line counter start index.
		**/
		void LayoutLoaderModule::getCases(const std::string& body, std::string& trueCase, std::string& falseCase, const std::string& sourceName, int bodyLine)
		{
			VanillaParser parser(body, sourceName, bodyLine);
			bool 	trueCaseAlreadySet = false,
				falseCaseAlreadySet = false;

			for(std::vector<Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
			{
				if(it->strKeyword==LayoutLoader::getKeyword(KW_LL_TRUE) && !it->noBody)
				{
					if(trueCaseAlreadySet)
						throw Exception("True case already set.", it->sourceName, it->startLine, Exception::ClientScriptException);
					
					trueCase = it->getCleanBody();
					trueCaseAlreadySet = true;
				}
				else if(it->strKeyword==LayoutLoader::getKeyword(KW_LL_FALSE)  && !it->noBody)
				{
					if(falseCaseAlreadySet)
						throw Exception("False case already set.", it->sourceName, it->startLine, Exception::ClientScriptException);

					falseCase = it->getCleanBody();
					falseCaseAlreadySet = true;
				}				
				else
					throw Exception("Unknown case keyword \"" + it->strKeyword + "\". Expected " + LayoutLoader::getKeyword(KW_LL_TRUE) + " or " + LayoutLoader::getKeyword(KW_LL_FALSE) + ".", it->sourceName, it->startLine, Exception::ClientScriptException);
			}
		}

	// Simple modules : 
			LAYOUT_LOADER_MODULE_APPLY( IF_SHAREDCODE_DEFINED, 1, 1, 1, true, 	"Check if the SHAREDCODE was defined.\n"
												"Arguments : sharedCodeName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_SHAREDCODE(it, arguments[0])

				if(it!=sharedCodeList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_FORMAT_DEFINED, 1, 1, 1, true, 		"Check if the FORMAT was defined.\n"
												"Arguments : formatName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_FORMAT(it, arguments[0])

				if(it!=formatList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_SHADERSOURCE_DEFINED, 1, 1, 1, true, 	"Check if the SHADERSOURCE was defined.\n"
												"Arguments : shaderSourceName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_SHADERSOURCE(it, arguments[0])

				if(it!=sourceList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_GEOMETRY_DEFINED, 1, 1, 1, true, 	"Check if the GEOMETRY was defined.\n"
												"Arguments : geometryName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_GEOMETRY(it, arguments[0])

				if(it!=geometryList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_FILTERLAYOUT_DEFINED, 1, 1, 1, true, 	"Check if the FILTERLAYOUT was defined.\n"
												"Arguments : filterLayoutName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_FILTER(it, arguments[0])

				if(it!=filterList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_PIPELINELAYOUT_DEFINED, 1, 1, 1, true, 	"Check if the PIPELINELAYOUT was defined.\n"
												"Arguments : pipelineLayoutName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_PIPELINE(it, arguments[0])

				if(it!=pipelineList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_REQUIREDFORMAT_DEFINED, 1, 1, 1, true, 	"Check if the REQUIREDFORMAT was defined.\n"
												"Arguments : requiredFormatName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_REQUIREDFORMAT(it, arguments[0])

				if(it!=requiredFormatList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_REQUIREDGEOMETRY_DEFINED, 1, 1, 1, true, "Check if the REQUIREDGEOMETRY was defined.\n"
												"Arguments : requiredGeometryName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_REQUIREDGEOMETRY(it, arguments[0])

				if(it!=requiredGeometryList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_REQUIREDPIPELINE_DEFINED, 1, 1, 1, true, "Check if the REQUIREDPIPELINE was defined.\n"
												"Arguments : requiredPipelineName.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(startLine)
				UNUSED_PARAMETER(executionCode)

				std::string 	trueCase, 
						falseCase;
				getCases(body, trueCase, falseCase, sourceName, bodyLine);
				
				CONST_ITERATOR_TO_REQUIREDPIPELINE(it, arguments[0])

				if(it!=requiredPipelineList.end())
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_SIZE, 4, 4, -1, true,	"Change the size of a format, save as a new format.\n"
											"Arguments : nameOriginal, widthNew, heightNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

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
					throw Exception("The new format is not valid (size : " + toString(newFmt.getWidth()) + "x" + toString(newFmt.getHeight()) + ").", sourceName, startLine, Exception::ClientScriptException);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_SCALE_SIZE, 3, 4, -1, true,	"Scale a format by a scalar (or two), save as a new format.\n"
											"Will prevent to reach a 0x0 texture by ensuring that the size is\n"
											"at least 1 pixel in each dimension\n"
											"Arguments : nameOriginal, scaleFactor, nameNew.\n"
											"            nameOriginal, scaleFactorX, scaleFactorY, nameNew.")
		 	{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;

				if(arguments.size()==3)
				{
					CAST_ARGUMENT( 1, double, s) 	
					if(s<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (s = " + toString(s) + ").", sourceName, startLine, Exception::ClientScriptException);
		
					newFmt.setWidth( std::max(newFmt.getWidth() * s, 1.0) );
					newFmt.setHeight( std::max(newFmt.getHeight() * s, 1.0) );
				}
				else
				{
					CAST_ARGUMENT( 1, double, sx)
					CAST_ARGUMENT( 2, double, sy)
					if(sx<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (sx = " + toString(sx) + ").", sourceName, startLine, Exception::ClientScriptException);
					if(sy<=0.0)
						throw Exception("The scale cannot be negative or equal to zero (sy = " + toString(sy) + ").", sourceName, startLine, Exception::ClientScriptException);
		
					newFmt.setWidth( std::max(newFmt.getWidth() * sx, 1.0) );
					newFmt.setHeight( std::max(newFmt.getHeight() * sy, 1.0) );
				}

				// Test : 
				if(newFmt.getWidth()<=0 || newFmt.getHeight()<=0)
					throw Exception("The new format is not valid (size : " + toString(newFmt.getWidth()) + "x" + toString(newFmt.getHeight()) + ").", sourceName, startLine, Exception::ClientScriptException);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_CHANNELS, 3, 3, -1, true,	"Change the channels of a format, save as a new format.\n"
												"Arguments : nameOriginal, channelNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setGLMode( glFromString(arguments[1]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_DEPTH, 3, 3, -1, true,	"Change the depth of a format, save as a new format.\n"
												"Arguments : nameOriginal, depthNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setGLDepth( glFromString(arguments[1]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_FILTERING, 4, 4, -1, true,	"Change the filtering of a format, save as a new format.\n"
												"Arguments : nameOriginal, minNew, magNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setMinFilter( glFromString(arguments[1]) );
				newFmt.setMagFilter( glFromString(arguments[2]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_WRAPPING, 4, 4, -1, true,	"Change the wrapping of a format, save as a new format.\n"
												"Arguments : nameOriginal, sNew, tNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)
	
				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				newFmt.setSWrapping( glFromString(arguments[1]) );
				newFmt.setTWrapping( glFromString(arguments[2]) );

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_CHANGE_MIPMAP, 4, 4, -1, true,	"Change the mipmap level of a format, save as a new format.\n"
												"Arguments : nameOriginal, mNew, nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				FORMAT_MUST_NOT_EXIST( arguments.back() );

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Build the new format : 
				HdlTextureFormat newFmt = it->second;
				CAST_ARGUMENT( 1, int, m)
				newFmt.setMaxLevel(m);

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MINIMUM_WIDTH, 3, -1, -1, true,	"Find the format having the smallest width, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					widthBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getWidth() <= widthBest || k==0 )
					{
						kBest 		= k;
						widthBest 	= it->second.getWidth();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MAXIMUM_WIDTH, 3, -1, -1, true,	"Find the format having the largest width, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					widthBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getWidth() >= widthBest || k==0 )
					{
						kBest 		= k;
						widthBest 	= it->second.getWidth();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MINIMUM_HEIGHT, 3, -1, -1, true,	"Find the format having the smallest height, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					heightBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getHeight() <= heightBest || k==0 )
					{
						kBest 		= k;
						heightBest 	= it->second.getHeight();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MAXIMUM_HEIGHT, 3, -1, -1, true,	"Find the format having the largest height, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					heightBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getHeight() >= heightBest || k==0 )
					{
						kBest 		= k;
						heightBest 	= it->second.getHeight();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MINIMUM_PIXELS, 3, -1, -1, true,	"Find the format having the smallest number of pixels, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					sizeBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getNumPixels() <= sizeBest || k==0 )
					{
						kBest 		= k;
						sizeBest 	= it->second.getNumPixels();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MAXIMUM_PIXELS, 3, -1, -1, true,	"Find the format having the largest number of pixels, save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				int 	kBest 		= 0,
					sizeBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getNumPixels() >= sizeBest || k==0 )
					{
						kBest 		= k;
						sizeBest 	= it->second.getNumPixels();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MINIMUM_ELEMENTS, 3, -1, -1, true,	"Find the format having the smallest number of elements (pixels times channels count), save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)				

				int 	kBest 		= 0,
					sizeBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getNumElements() <= sizeBest || k==0 )
					{
						kBest 		= k;
						sizeBest 	= it->second.getNumElements();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_MAXIMUM_ELEMENTS, 3, -1, -1, true,	"Find the format having the largest number of elements (pixels times channels count), save as a new format.\n"
												"Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)				

				int 	kBest 		= 0,
					sizeBest	= 0;

				for(unsigned int k=0; k<arguments.size()-1; k++)
				{
					FORMAT_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_FORMAT( it, arguments[k] )

					if( it->second.getNumElements() >= sizeBest || k==0 )
					{
						kBest 		= k;
						sizeBest 	= it->second.getNumElements();
					}
				}

				FORMAT_MUST_NOT_EXIST( arguments.back() );

				// New format : 
				CONST_ITERATOR_TO_FORMAT( it, arguments[kBest] )
				HdlTextureFormat newFmt = it->second;

				APPEND_NEW_FORMAT( arguments.back(), newFmt )
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_FORMAT_SETTING_MATCH, 3, 3, 1, true, 	"Match if a format setting is equal to a value (integer or GL keyword).\n"
												"Arguments : nameFormat, nameSetting, value.\n"
												"            nameSettings : the settings name can be found in the documentation of HdlAbstractTextureFormat::getSetting).")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)

				FORMAT_MUST_EXIST( arguments[0] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Get the value : 
				GLenum setting = glFromString( arguments[1] );

				// Special cast : 
				unsigned int value;

				if(!fromString(arguments[2], value))
					value = glFromString( arguments[2] );

				// Get the cases : 
				std::string 	trueCase, 
						falseCase;

				getCases(body, trueCase, falseCase, sourceName, bodyLine);

				if(it->second.getSetting(setting)==value)
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( IF_FORMAT_SETTING_LARGERTHAN, 3, 3, 1, true, 	"Match if a format setting is larger than a value (integer or GL keyword).\n"
													"Arguments : nameFormat, nameSetting, value.\n"
													"            nameSettings : the settings name can be found in the documentation of HdlAbstractTextureFormat::getSetting).")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)

				FORMAT_MUST_EXIST( arguments[0] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				// Get the value : 
				GLenum setting = glFromString( arguments[1] );

				// Special cast : 
				unsigned int value;

				if(!fromString(arguments[2], value))
					value = glFromString( arguments[2] );

				// Get the cases : 
				std::string 	trueCase, 
						falseCase;

				getCases(body, trueCase, falseCase, sourceName, bodyLine);

				if(it->second.getSetting(setting)>value)
					executionCode = trueCase;
				else
					executionCode = falseCase;
			}

			LAYOUT_LOADER_MODULE_APPLY( GENERATE_SAME_SIZE_2D_GRID, 2, 4, -1, true,	"Create a 2D grid geometry of the same size as the format in argument.\n"
												"Arguments : nameFormat, nameNewGeometry [, normalized].\n"
												"            normalized : if 'true' (case sensitive) the geometry will be in the range [0, 1].")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				GEOMETRY_MUST_NOT_EXIST( arguments[1] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				bool normalized = false;

				if(arguments.size()>=3)
					normalized = (arguments[2]=="true");

				// Build the new geometry : 
				APPEND_NEW_GEOMETRY( arguments.back(), GeometryPrimitives::PointsGrid2D(it->second.getWidth(), it->second.getHeight(), normalized) )
			}

			LAYOUT_LOADER_MODULE_APPLY( GENERATE_SAME_SIZE_3D_GRID, 2, 3, -1, true,	"Create a 3D grid geometry of the same size as the format in argument.\n"
												"Arguments : nameFormat, nameNewGeometry [, normalized].\n"
												"            normalized : if 'true' (case sensitive) the geometry will be in the range [0, 1].")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				GEOMETRY_MUST_NOT_EXIST( arguments[1] )

				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				bool normalized = false;

				if(arguments.size()>=3)
					normalized = (arguments[2]=="true");

				// Build the new geometry : 
				APPEND_NEW_GEOMETRY( arguments.back(), GeometryPrimitives::PointsGrid3D(it->second.getWidth(), it->second.getHeight(), it->second.getNumChannels(), normalized) )
			}

			LAYOUT_LOADER_MODULE_APPLY( CHAIN_PIPELINES, 4, -1, -1, true, 	"Create a pipeline by connecting the pipelines passed in arguments, in line.\n"
											"Arguments : nameNewPipelineLayout, isStrict, namePipelineLayout1, namePipelineLayout2, ...\n"
											"            isStrict if 'true' (case sensitive) the pipelines connection are enforced strictly (if outputs of\n"
											"                     the first pipeline are not equal to the number of input of the second pipeline,\n"
											"                     then the module will report an error.\n")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				const unsigned int startPipelines = 2;
				std::string requiredElements;
				VanillaParserSpace::Element 	result;

				PIPELINE_MUST_NOT_EXIST( arguments[0] );

				// Get the arguments : 
				const bool 	isStrict	= (arguments[1]=="true");

				result.strKeyword 	= LayoutLoader::getKeyword(KW_LL_PIPELINE_LAYOUT);
				result.name		= arguments[0];
				result.noName		= false;
				result.noArgument	= true;
				result.noBody		= false;
				
				result.body.clear();

				// Get all the pipelines : 
				std::string lastInstance;
				std::vector<std::string> outputPortNames;
				for(unsigned int k=startPipelines; k<arguments.size(); k++)
				{
					// Access the pipeline : 
					PIPELINE_MUST_EXIST( arguments[k] );

					CONST_ITERATOR_TO_PIPELINE(itCurrentPipeline, arguments[k]);

					// Add the requirement : 
					VanillaParserSpace::Element requirement;
				
					requirement.strKeyword	= LayoutLoader::getKeyword(KW_LL_REQUIRED_PIPELINE);
					requirement.name	= arguments[k] + "_required" + toString(k-startPipelines);
					requirement.noName	= false;
					requirement.arguments.push_back(arguments[k]);
					requirement.noArgument	= false;

					requiredElements += requirement.getCode() + "\n";

					// Add the instance : 
					VanillaParserSpace::Element instance;
				
					instance.strKeyword 	= LayoutLoader::getKeyword(KW_LL_PIPELINE_INSTANCE);
					instance.name		= arguments[k] + "_instance" + toString(k-startPipelines);
					instance.noName		= false;
					instance.arguments.push_back(requirement.name);
					instance.noArgument	= false;

					result.body += instance.getCode() + "\n";

					if(k==startPipelines)
					{
						// Append the inputs : 
						VanillaParserSpace::Element inputs;

						inputs.strKeyword = LayoutLoader::getKeyword(KW_LL_INPUT_PORTS);

						for(int i=0; i<itCurrentPipeline->second.getNumInputPort(); i++)
							inputs.arguments.push_back(itCurrentPipeline->second.getInputPortName(i));
						inputs.noArgument = false;

						result.body += inputs.getCode() + "\n";

						// Make the connections : 
						for(int i=0; i<itCurrentPipeline->second.getNumInputPort(); i++)
						{
							VanillaParserSpace::Element connection;

							connection.strKeyword = LayoutLoader::getKeyword(KW_LL_CONNECTION);
							connection.noArgument = false;
							connection.arguments.push_back( LayoutLoader::getKeyword(KW_LL_THIS_PIPELINE) );
							connection.arguments.push_back( itCurrentPipeline->second.getInputPortName(i) );
							connection.arguments.push_back( instance.name );
							connection.arguments.push_back( itCurrentPipeline->second.getInputPortName(i) );

							result.body += connection.getCode() + "\n";
						}
					}
					else
					{
						// Standard check : 
						if(itCurrentPipeline->second.getNumInputPort()>static_cast<int>(outputPortNames.size()))
							throw Exception("The pipeline " + instance.name + " has " + toString(itCurrentPipeline->second.getNumInputPort()) + " input ports while the previous element in the chain (" + lastInstance + ") has only " + toString(outputPortNames.size()) + " output ports.", sourceName, startLine, Exception::ClientScriptException);

						// Check the previous number of output : 
						if(isStrict && itCurrentPipeline->second.getNumInputPort()!=static_cast<int>(outputPortNames.size()))
							throw Exception("The pipeline " + instance.name + " has " + toString(itCurrentPipeline->second.getNumInputPort()) + " input ports while the previous element in the chain (" + lastInstance + ") has " + toString(outputPortNames.size()) + " output ports and the connections are specified as STRICT.", sourceName, startLine, Exception::ClientScriptException);

						// Make the connections : 
						for(int k=0; k<itCurrentPipeline->second.getNumInputPort(); k++)
						{
							VanillaParserSpace::Element connection;

							connection.strKeyword = LayoutLoader::getKeyword(KW_LL_CONNECTION);
							connection.noArgument = false;
							connection.arguments.push_back( lastInstance );
							connection.arguments.push_back( outputPortNames[k] );
							connection.arguments.push_back( instance.name );
							connection.arguments.push_back( itCurrentPipeline->second.getInputPortName(k) );

							result.body += connection.getCode() + "\n";
						}
					}

					// If last : 
					if(k==arguments.size()-1)
					{
						// Outputs : 
						VanillaParserSpace::Element outputs;

						outputs.strKeyword = LayoutLoader::getKeyword(KW_LL_OUTPUT_PORTS);

						for(int o=0; o<itCurrentPipeline->second.getNumOutputPort(); o++)
							outputs.arguments.push_back(itCurrentPipeline->second.getOutputPortName(o));
						outputs.noArgument = false;

						result.body += outputs.getCode() + "\n";

						// Make the connections : 
						for(int o=0; o<itCurrentPipeline->second.getNumOutputPort(); o++)
						{
							VanillaParserSpace::Element connection;

							connection.strKeyword = LayoutLoader::getKeyword(KW_LL_CONNECTION);
							connection.noArgument = false;
							connection.arguments.push_back( instance.name );
							connection.arguments.push_back( itCurrentPipeline->second.getOutputPortName(o) );
							connection.arguments.push_back( LayoutLoader::getKeyword(KW_LL_THIS_PIPELINE) );
							connection.arguments.push_back( itCurrentPipeline->second.getOutputPortName(o) );
							
							result.body += connection.getCode() + "\n";
						}
					}
					else
					{
						lastInstance = instance.name;

						// Update the outputs list :
						outputPortNames.clear();
						for(int o=0; o<itCurrentPipeline->second.getNumOutputPort(); o++)
							outputPortNames.push_back(itCurrentPipeline->second.getOutputPortName(o));

					}
				}

				// Return : 
				executionCode = requiredElements + "\n" + result.getCode();
			}

			LAYOUT_LOADER_MODULE_APPLY( FORMAT_TO_CONSTANT, 2, 2, -1, true,	"Create a SHARED_SOURCE containing a const ivec2 declaration describing the size of the texture passed in argument.\n"
											"For instance, can be used in a shader with : INSERT(name)\n"
											"Arguments   : textureFormat, sharedCodeName\n")
			{
				UNUSED_PARAMETER(body)
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				FORMAT_MUST_EXIST( arguments[0] )
				SHAREDCODE_MUST_NOT_EXIST( arguments[1] )
	
				CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

				std::string str = "const ivec2 " + arguments[1] + " = ivec2(" + toString(it->second.getWidth()) + ", " + toString(it->second.getHeight()) + ");\n";
				// The newline is mandatory here, to avoid ShaderSource mistaking this for a filename.

				APPEND_NEW_SHAREDCODE(arguments[1], ShaderSource(str, sourceName, startLine));
			}

			LAYOUT_LOADER_MODULE_APPLY( ABORT_ERROR, 1, 1, 0, false,	"Return a user defined error.\n"
											"Argument    : error description.\n"
											"Body (opt.) : more complete description of the error.")
			{
				UNUSED_PARAMETER(currentPath)
				UNUSED_PARAMETER(dynamicPaths)
				UNUSED_PARAMETER(sharedCodeList)
				UNUSED_PARAMETER(formatList)
				UNUSED_PARAMETER(sourceList)
				UNUSED_PARAMETER(geometryList)
				UNUSED_PARAMETER(filterList)
				UNUSED_PARAMETER(pipelineList)
				UNUSED_PARAMETER(staticPaths)
				UNUSED_PARAMETER(requiredFormatList)
				UNUSED_PARAMETER(requiredGeometryList)
				UNUSED_PARAMETER(requiredPipelineList)
				UNUSED_PARAMETER(bodyLine)
				UNUSED_PARAMETER(executionCode)

				Exception m("Error : " + arguments.front(), sourceName, startLine, Exception::ClientScriptException);

				if(body.empty())
					throw m;
				else
				{
					Exception e(body, sourceName, startLine, Exception::ClientScriptException);
					m << e;
					throw m;
				}
			}

