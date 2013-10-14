/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : LayoutLoaderModules.hpp                                                                   */
/*     Original Date : October 14th 2013                                                                         */
/*                                                                                                               */
/*     Description   : Layout Loader Modules.                                                                    */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    LayoutLoaderModules.hpp
 * \brief   Layout Loader Modules.
 * \author  R. KERVICHE
 * \date    October 14th 2013
**/

#ifndef __LAYOUT_LOADER_MODULES_INCLUDE__
#define __LAYOUT_LOADER_MODULES_INCLUDE__

	#include <vector>
	#include <map>
	#include "Core/HdlTexture.hpp"
	#include "Core/Geometry.hpp"
	#include "Core/ShaderSource.hpp"
	#include "Core/Filter.hpp"
	#include "Core/Pipeline.hpp"
	#include "Modules/VanillaParser.hpp"

namespace Glip
{
	using namespace CoreGL;
	using namespace CorePipeline;

	namespace Modules
	{
		// Prototype : 
		class LayoutLoader;

		// Classes : 
		class LayoutLoaderModule
		{
			private : 
				const std::string 	name,
							manual;
				const int		minNumArguments,
							maxNumArguments;
				char 			bodyPresence;

			protected :
				LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence);

			public :
				LayoutLoaderModule(const LayoutLoaderModule& m);

				virtual void apply(	const std::vector<std::string>& 		arguments, 
							const std::string&				body, 
							const std::string&				currentPath,
							std::vector<std::string>&			dynamicPaths,
							std::map<std::string, std::string>&		sharedCodeList,
							std::map<std::string, HdlTextureFormat>& 	formatList,
							std::map<std::string, ShaderSource>& 		sourceList,
							std::map<std::string, GeometryModel>&		geometryList,
							std::map<std::string, FilterLayout>& 		filterList,
							std::map<std::string, PipelineLayout> &		pipelineList,
							const std::vector<std::string>&			staticPaths,
							const std::map<std::string,HdlTextureFormat>&	requiredFormatList,
							const std::map<std::string,GeometryModel>&	requiredGeometryList,
							const std::map<std::string,PipelineLayout>&	requiredPipelineList) = 0;

				std::string getName(void) const;
				const int getMinNumArguments(void) const;
				const int getMaxNumArguments(void) const;
				const char bodyPresenceTest(void) const;
				std::string getManual(void) const;

				// Static tools : 
				static void addBasicModules(LayoutLoader& loader);
		};

		// Simple MACROS : 
			#define LAYOUT_LOADER_MODULE_DEFINITION( name )	class name : public LayoutLoaderModule \
									{ \
										public : \
											name (void); \
											\
											void apply(	const std::vector<std::string>& 		arguments,  \
													const std::string&				body, \
													const std::string&				currentPath, \
													std::vector<std::string>&			dynamicPaths, \
													std::map<std::string, std::string>&		sharedCodeList, \
													std::map<std::string, HdlTextureFormat>& 	formatList, \
													std::map<std::string, ShaderSource>& 		sourceList, \
													std::map<std::string, GeometryModel>&		geometryList, \
													std::map<std::string, FilterLayout>& 		filterList, \
													std::map<std::string, PipelineLayout> &		pipelineList, \
													const std::vector<std::string>&			staticPaths, \
													const std::map<std::string,HdlTextureFormat>&	requiredFormatList, \
													const std::map<std::string,GeometryModel>&	requiredGeometryList, \
													const std::map<std::string,PipelineLayout>&	requiredPipelineList); \
									};

			#define LAYOUT_LOADER_MODULE_APPLY( moduleName, minArgs, maxArgs, bodyPresence, moduleManual)		moduleName :: moduleName (void) : LayoutLoaderModule( #moduleName, moduleManual, minArgs, maxArgs, bodyPresence) { } \
															void 	moduleName :: apply(	const std::vector<std::string>& 		arguments,  \
																			const std::string&				body, \
																			const std::string&				currentPath, \
																			std::vector<std::string>&			dynamicPaths, \
																			std::map<std::string, std::string>&		sharedCodeList, \
																			std::map<std::string, HdlTextureFormat>& 	formatList, \
																			std::map<std::string, ShaderSource>& 		sourceList, \
																			std::map<std::string, GeometryModel>&		geometryList, \
																			std::map<std::string, FilterLayout>& 		filterList, \
																			std::map<std::string, PipelineLayout> &		pipelineList, \
																			const std::vector<std::string>&			staticPaths, \
																			const std::map<std::string,HdlTextureFormat>&	requiredFormatList, \
																			const std::map<std::string,GeometryModel>&	requiredGeometryList, \
																			const std::map<std::string,PipelineLayout>&	requiredPipelineList)

			#define __ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::iterator iteratorName = varName.find( elementName );
			#define __CONST_ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::const_iterator iteratorName = varName.find( elementName );
			#define __ELEMENT_MUST_BE_IN(iteratorName, varName, elementName)	if(iteratorName==varName.end()) throw Exception("Element \"" + elementName + "\" was not found in \"" + #varName + "\".", __FILE__, __LINE__);
			#define __ELEMENT_MUST_NOT_BE_IN(iteratorName, varName, elementName)	if(iteratorName!=varName.end()) throw Exception("Element \"" + elementName + "\" already exists in \"" + #varName + "\".", __FILE__, __LINE__);
			#define __APPEND_NEW_ELEMENT(type, varName, elementName, element)	varName.insert( std::pair<std::string, type>( elementName, element ) );

			#define ITERATOR_TO_SHAREDCODE( iteratorName, elementName )		__ITERATOR_FIND(std::string, sharedCodeList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_SHAREDCODE( iteratorName, elementName )	__CONST_ITERATOR_FIND(std::string, sharedCodeList, iteratorName, elementName)
			#define SHAREDCODE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Hstd::string, sharedCodeList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, sharedCodeList, elementName) }
			#define SHAREDCODE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(std::string, sharedCodeList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, sharedCodeList, elementName) }
			#define APPEND_NEW_SHAREDCODE(elementName, newFormat)			__APPEND_NEW_ELEMENT(std::string, sharedCodeList, elementName, newFormat)

			#define ITERATOR_TO_FORMAT( iteratorName, elementName )			__ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_FORMAT( iteratorName, elementName )		__CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName)
			#define FORMAT_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, formatList, elementName) }
			#define FORMAT_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, formatList, elementName) }
			#define APPEND_NEW_FORMAT(elementName, newFormat)			__APPEND_NEW_ELEMENT(HdlTextureFormat, formatList, elementName, newFormat)

			#define ITERATOR_TO_SHADERSOURE( iteratorName, elementName )		__ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_SHADERSOURE( iteratorName, elementName )	__CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName)
			#define SHADERSOURE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, sourceList, elementName) }
			#define SHADERSOURE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, sourceList, elementName) }
			#define APPEND_NEW_SHADERSOURE(elementName, newFormat)			__APPEND_NEW_ELEMENT(ShaderSource, sourceList, elementName, newFormat)
			
			#define ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName)
			#define GEOMETRY_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, geometryList, elementName) }
			#define GEOMETRY_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, geometryList, elementName) }
			#define APPEND_NEW_GEOMETRY(elementName, newFormat)			__APPEND_NEW_ELEMENT(GeometryModel, geometryList, elementName, newFormat)

			#define ITERATOR_TO_FILTER( iteratorName, elementName )			__ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_FILTER( iteratorName, elementName )		__CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName)
			#define FILTER_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, filterList, elementName) }
			#define FILTER_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, filterList, elementName) }
			#define APPEND_NEW_FILTER(elementName, newFormat)			__APPEND_NEW_ELEMENT(FilterLayout, filterList, elementName, newFormat)
			
			#define ITERATOR_TO_PIPELINE( iteratorName, elementName )		__ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName)
			#define CONST_ITERATOR_TO_PIPELINE( iteratorName, elementName )		__CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName)
			#define PIPELINE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, pipelineList, elementName) }
			#define PIPELINE_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, pipelineList, elementName) }
			#define APPEND_NEW_PIPELINE(elementName, newFormat)			__APPEND_NEW_ELEMENT(PipelineLayout, pipelineList, elementName, newFormat)
			
			#define CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	__CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName)
			#define REQUIREDFORMAT_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			#define REQUIREDFORMAT_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }

			#define CONST_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName, elementName )	__CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName)
			#define REQUIREDGEOMETRY_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			#define REQUIREDGEOMETRY_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }

			#define CONST_ITERATOR_TO_REQUIREDPIPELINE( iteratorName, elementName )	__CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName)
			#define REQUIREDPIPELINE_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			#define REQUIREDPIPELINE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }
			
			#define CAST_ARGUMENT( argID, type, varName ) 				type varName; \
												if(!from_string(arguments[ argID ], varName)) \
													throw Exception("Unable to cast argument " + to_string( argID ) + " \"" + arguments[argID] + "\" to " + #type + ".", __FILE__, __LINE__);

		// Basic Modules : 
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SCALE )
	}
}

#endif
