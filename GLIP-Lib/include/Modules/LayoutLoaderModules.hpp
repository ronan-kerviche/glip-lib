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
	#include "Core/LibTools.hpp"
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
/**
\class LayoutLoaderModule
\brief Module description for the LayoutLoader class.

You can write a Module MyModule for a LayoutLoader object, load it, and allow the user to use CALL:MyModule. It will have limited access to the data of the LayoutLoader, and thus be able to create, modify or destroy formats, source codes, shaders, geometries, filters, pipelines, etc. Each Module has a name, a minimum and maximum number of arguments and the information on the need of a body or not.

In order to ease the creation of modules, you will find Macros for accessing data given to the Module.

Several modules are already created but need to be manually added to any LayoutLoader you would like to use via : 
	\code
	LayoutLoader myLoader;
	LayoutLoaderModule::addBasicModules(myLoader);
	\endcode

These modules are : 
<CENTER>
Module       			| Description
------------------------------- | --------------------------
IF_SHAREDCODE_DEFINED		| Check if the SHAREDCODE was defined. Arguments : sharedCodeName.
IF_FORMAT_DEFINED		| Check if the FORMAT was defined. Arguments : formatName.
IF_SHADERSOURCE_DEFINED		| Check if the SHADERSOURCE was defined. Arguments : shaderSourceName.
IF_GEOMETRY_DEFINED		| Check if the GEOMETRY was defined. Arguments : geometryName.
IF_FILTERLAYOUT_DEFINED		| Check if the FILTERLAYOUT was defined. Arguments : filterLayoutName.
IF_PIPELINELAYOUT_DEFINED	| Check if the PIPELINELAYOUT was defined. Arguments : pipelineLayoutName.
IF_REQUIREDFORMAT_DEFINED	| Check if the REQUIREDFORMAT was defined. Arguments : requiredFormatName.
IF_REQUIREDGEOMETRY_DEFINED	| Check if the REQUIREDGEOMETRY was defined. Arguments : requiredGeometryName.
IF_REQUIREDPIPELINE_DEFINED	| Check if the REQUIREDPIPELINE was defined. Arguments : requiredPipelineName.
FORMAT_CHANGE_SIZE		| Change the size of a format, save as a new format. Arguments : nameOriginal, widthNew, heightNew, nameNew.
FORMAT_SCALE_SIZE		| Scale a format by a scalar (or two), save as a new format. Arguments : nameOriginal, scaleFactor, nameNew OR nameOriginal, scaleFactorX, scaleFactorY, nameNew.
FORMAT_CHANGE_CHANNELS		| Change the channels of a format, save as a new format. Arguments : nameOriginal, channelNew, nameNew.
FORMAT_CHANGE_DEPTH		| Change the depth of a format, save as a new format. Arguments : nameOriginal, depthNew, nameNew.
FORMAT_CHANGE_FILTERING		| Change the filtering of a format, save as a new format. Arguments : nameOriginal, minNew, magNew, nameNew.
FORMAT_CHANGE_WRAPPING		| Change the wrapping of a format, save as a new format. Arguments : nameOriginal, sNew, tNew, nameNew.
FORMAT_CHANGE_MIPMAP		| Change the mipmap level of a format, save as a new format. Arguments : nameOriginal, mNew, nameNew.
FORMAT_MINIMUM_WIDTH		| Find the format having the smallest width, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MAXIMUM_WIDTH		| Find the format having the largest width, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MINIMUM_HEIGHT		| Find the format having the smallest height, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MAXIMUM_HEIGHT		| Find the format having the largest height, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MINIMUM_PIXELS		| Find the format having the smallest number of pixels, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MAXIMUM_PIXELS		| Find the format having the largest number of pixels, save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MINIMUM_ELEMENTS		| Find the format having the smallest number of elements (pixels times channels count), save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_MAXIMUM_ELEMENTS		| Find the format having the largest number of elements (pixels times channels count), save as a new format. Arguments : nameFormat1, nameFormat2, [nameFormat3, ...,] nameNew.
FORMAT_SMALLER_POWER_OF_TWO	| Generate a new format clamped to the closest smaller power of 2. Arguments : nameFormat, nameNew [, strict].
FORMAT_LARGER_POWER_OF_TWO	| Generate a new format clamped to the closest larger power of 2. Arguments : nameFormat, nameNew [, strict].
FORMAT_SWITCH_DIMENSIONS	| Switch the width and height values, save as a new format, Arguments : nameFormat, nameNew.
IF_FORMAT_SETTING_MATCH		| Match if a format setting is equal to a value (integer). Arguments : nameFormat, nameSetting, value.
IF_FORMAT_SETTING_LARGERTHAN	| Match if a format setting is larger than a value (integer or GL keyword). Arguments : nameFormat, nameSetting, value.
GENERATE_SAME_SIZE_2D_GRID	| Create a 2D grid geometry of the same size as the format in argument. Arguments : nameFormat, nameNewGeometry, [normalized].
GENERATE_SAME_SIZE_3D_GRID	| Create a 3D grid geometry of the same size as the format in argument. Arguments : nameFormat, nameNewGeometry, [normalized].
CHAIN_PIPELINES			| Create a pipeline by connecting the pipelines passed in arguments, in line. Arguments : nameNewPipelineLayout, isStrict, namePipelineLayout1, namePipelineLayout2, ...
FORMAT_TO_CONSTANT		| Create a shared code object containt a "ivec2(width, height)" code from the texture format passed in argument. Argument : nameFormat, nameSharedCode.
SINGLE_FILTER_PIPELINE		| Create a pipeline with a single filter. Arguments : pipelineName, outputTextureFormat. Body : fragment shader source.
ABORT_ERROR			| Return a user defined error. Argument : error description.
GENERATE_FFT1D_PIPELINE		| Generate the 1D FFT Pipeline transformation. Options : SHIFTED, INVERSED, COMPATIBILITY_MODE. Arguments : width, name [, option, ...].
GENERATE_FFT2D_PIPELINE		| Generate the 2D FFT Pipeline transformation. Options : SHIFTED, INVERSED, COMPATIBILITY_MODE. Arguments : width, height, name [, option, ...].
</CENTER>

Example in a script file : 
	\code
	REQUIRED_FORMAT: fmt (inputFormat)

	// Create a new format by copying inputFormat and changing the filtering : 
	CALL: FORMAT_CHANGE_FILTERING (fmt, GL_LINEAR, GL_LINEAR, fmt1)

	// Create a new format by copying inputFormat and dividing by 2 its size : 
	CALL: FORMAT_SCALE_SIZE (fmt, 0.5, fmt2)
	\endcode

Example, creating a simple Module :
	\code 
		// Declare your module in some header : 
		LAYOUT_LOADER_MODULE_DEFINITION( MyAdditionModule )
		// (If your module ought to be more complex, you will have to go through a full description)

		// In the source file (must have exactly 3 arguments (min = max = 3), no body and show the manual if any errors occur) : 
		LAYOUT_LOADER_MODULE_APPLY( MyAdditionModule, 3, 3, -1, true, 	"Increase or decrease the size of a format, save as a new format.\n"
										"Arguments : nameOriginal, delta, nameNew.")
		{
			// Check that the target element exists : 
			FORMAT_MUST_EXIST( arguments[0] )
			// Check that the output name is free :
			FORMAT_MUST_NOT_EXIST( arguments.back() );

			// Get an iterator to the target :
			CONST_ITERATOR_TO_FORMAT( it, arguments[0] )

			// Build the new format : 
			HdlTextureFormat newFmt = it->second;

			// Read the delta argument : 
			CAST_ARGUMENT( 1, double, delta)

			// Change the size : 
			newFmt.setWidth( newFmt.getWidth() + delta );
			newFmt.setHeight( newFmt.getHeight() + delta );

			// Check the size : 
			if(newFmt.getWidth()<=0 || newFmt.getHeight()<=0)
				throw Exception("The new format is not valid (size : " + toString(newFmt.getWidth()) + "x" + toString(newFmt.getHeight()) + ").", __FILE__, __LINE__);

			// Append the new format under the right name :
			APPEND_NEW_FORMAT( arguments.back(), newFmt )
		}
	\endcode

	Use in a script : 
	\code 
		CALL: MyAdditionModule (someFormat, 128, newLargerFormat)
	\endcode
	
**/
		class GLIP_API LayoutLoaderModule
		{
			private : 
				const std::string 	name,
							manual;
				const int		minNumArguments,
							maxNumArguments;
				const char 		bodyPresence;
				bool			showManualOnError;

			protected :
				LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence, bool _showManualOnError = true);

			public :
				LayoutLoaderModule(const LayoutLoaderModule& m);
				virtual ~LayoutLoaderModule(void);

				/**
				\fn virtual void LayoutLoaderModule::apply(const std::vector<std::string>& arguments, const std::string& body, const std::string& currentPath, std::vector<std::string>& dynamicPaths, std::map<std::string, ShaderSource>& sharedCodeList, std::map<std::string, HdlTextureFormat>& formatList, std::map<std::string, ShaderSource>& sourceList, std::map<std::string, GeometryModel>& geometryList, std::map<std::string, FilterLayout>& filterList, std::map<std::string, PipelineLayout>& pipelineList, std::string& mainPipelineName, const std::vector<std::string>& staticPaths, const std::map<std::string,HdlTextureFormat>& requiredFormatList, const std::map<std::string,GeometryModel>& requiredGeometryList, const std::map<std::string,PipelineLayout>& requiredPipelineList, const std::string& sourceName, const int startLine, const int bodyLine, std::string& executionCode) = 0
				\brief Interface of the module : this function will be called on each corresponding token CALL for the LayoutLoader which has the module. 
				\param arguments 		The arguments of the called, their number has already been checked.
				\param body 			The body of the call (might be empty), its presence has already been checked.
				\param currentPath 		The currentPath in which the LayoutLoader is operating.
				\param dynamicPaths		The list of paths dynamically built (only for the current load operation).
				\param sharedCodeList		The list of Shared Code currently loaded.
								For easy access see #ITERATOR_TO_SHAREDCODE, #CONST_ITERATOR_TO_SHAREDCODE, #SHAREDCODE_MUST_EXIST, #SHAREDCODE_MUST_NOT_EXIST and #APPEND_NEW_SHAREDCODE.
				\param formatList		The list of formats currently loaded.
								For easy access see #ITERATOR_TO_FORMAT, #CONST_ITERATOR_TO_FORMAT, #FORMAT_MUST_EXIST, #FORMAT_MUST_NOT_EXIST and #APPEND_NEW_FORMAT.
				\param sourceList		The list of sources currently loaded.
								For easy access see #ITERATOR_TO_SHADERSOURCE, #CONST_ITERATOR_TO_SHADERSOURCE, #SHADERSOURCE_MUST_EXIST, #SHADERSOURCE_MUST_NOT_EXIST and #APPEND_NEW_SHADERSOURCE.
				\param geometryList		The list of geometries currently loaded.
								For easy access see #ITERATOR_TO_GEOMETRY, #CONST_ITERATOR_TO_GEOMETRY, #GEOMETRY_MUST_EXIST, #GEOMETRY_MUST_NOT_EXIST and #APPEND_NEW_GEOMETRY.
				\param filterList		The list of filters currently loaded.
								For easy access see #ITERATOR_TO_FILTER, #CONST_ITERATOR_TO_FILTER, #FILTER_MUST_EXIST, #FILTER_MUST_NOT_EXIST and #APPEND_NEW_FILTER.
				\param pipelineList		The list of pipelines currently loaded.
								For easy access see #ITERATOR_TO_PIPELINE, #CONST_ITERATOR_TO_PIPELINE, #PIPELINE_MUST_EXIST, #PIPELINE_MUST_NOT_EXIST and #APPEND_NEW_PIPELINE.
				\param mainPipelineName		The name of the current main pipeline, if already parsed.
				\param staticPaths		The list of static paths (known for all load operations).
				\param requiredFormatList	The list of static formats available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDFORMAT, #REQUIREDFORMAT_MUST_EXIST, #REQUIREDFORMAT_MUST_NOT_EXIST.
				\param requiredGeometryList	The list of static geometries available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDGEOMETRY, #REQUIREDGEOMETRY_MUST_EXIST, #REQUIREDGEOMETRY_MUST_NOT_EXIST.
				\param requiredPipelineList	The list of static pipelines available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDPIPELINE, #REQUIREDPIPELINE_MUST_EXIST, #REQUIREDPIPELINE_MUST_NOT_EXIST.
				\param sourceName		Name of the source from which the call was extracted.
				\param startLine		Line index of the module call.
				\param bodyLine			Line index of the body for this module call.
				\param executionCode		The code which has to be run at after the function complete (leave it empty if no code needs to be run, the syntax of the code expected is the same as scripts).
				**/
				virtual void apply(	const std::vector<std::string>& 		arguments, 
							const std::string&				body, 
							const std::string&				currentPath,
							std::vector<std::string>&			dynamicPaths,
							std::map<std::string, ShaderSource>&		sharedCodeList,
							std::map<std::string, HdlTextureFormat>& 	formatList,
							std::map<std::string, ShaderSource>& 		sourceList,
							std::map<std::string, GeometryModel>&		geometryList,
							std::map<std::string, FilterLayout>& 		filterList,
							std::map<std::string, PipelineLayout>&		pipelineList,
							std::string&					mainPipelineName, 
							const std::vector<std::string>&			staticPaths,
							const std::map<std::string,HdlTextureFormat>&	requiredFormatList,
							const std::map<std::string,GeometryModel>&	requiredGeometryList,
							const std::map<std::string,PipelineLayout>&	requiredPipelineList,
							const std::string& 				sourceName,
							const int 					startLine,
							const int 					bodyLine,
							std::string& 					executionCode) = 0;

				const std::string& getName(void) const;
				const int& getMinNumArguments(void) const;
				const int& getMaxNumArguments(void) const;
				const char& bodyPresenceTest(void) const;
				const bool& requiringToShowManualOnError(void) const;
				const std::string& getManual(void) const;

				// Events : 
				virtual void beginLoadLayout(void);
				virtual void endLoadLayout(void);

				// Static tools : 
				static void addBasicModules(LayoutLoader& loader);
				static bool getBoolean(const std::string& arg, const std::string& sourceName="", int line=1);
				static void getCases(const std::string& body, std::string& trueCase, std::string& falseCase, const std::string& sourceName="", int bodyLine=1);
				static std::vector<std::string> findFile(const std::string& filename, const std::vector<std::string>& dynamicPaths);
		};

		// Simple MACROS : 
			/** LAYOUT_LOADER_MODULE_DEFINITION( name )	Declaration of a module. **/
			#define LAYOUT_LOADER_MODULE_DEFINITION( name )	class name : public LayoutLoaderModule \
									{ \
										public : \
											name (void); \
											\
											void apply(	const std::vector<std::string>& 		arguments,  \
													const std::string&				body, \
													const std::string&				currentPath, \
													std::vector<std::string>&			dynamicPaths, \
													std::map<std::string, ShaderSource>&		sharedCodeList, \
													std::map<std::string, HdlTextureFormat>& 	formatList, \
													std::map<std::string, ShaderSource>& 		sourceList, \
													std::map<std::string, GeometryModel>&		geometryList, \
													std::map<std::string, FilterLayout>& 		filterList, \
													std::map<std::string, PipelineLayout>&		pipelineList, \
													std::string&					mainPipelineName, \
													const std::vector<std::string>&			staticPaths, \
													const std::map<std::string,HdlTextureFormat>&	requiredFormatList, \
													const std::map<std::string,GeometryModel>&	requiredGeometryList, \
													const std::map<std::string,PipelineLayout>&	requiredPipelineList, \
													const std::string& 				sourceName, \
													const int 					startLine, \
													const int 					bodyLine, \
													std::string& 					executionCode); \
									};
			
			/** LAYOUT_LOADER_MODULE_APPLY_SIGNATURE Get the signature of the apply function. **/
			#define LAYOUT_LOADER_MODULE_APPLY_SIGNATURE			void apply(const std::vector<std::string>& arguments,  \
											const std::string&					body, \
											const std::string&					currentPath, \
											std::vector<std::string>&				dynamicPaths, \
											std::map<std::string, ShaderSource>&			sharedCodeList, \
											std::map<std::string, HdlTextureFormat>& 		formatList, \
											std::map<std::string, ShaderSource>& 			sourceList, \
											std::map<std::string, GeometryModel>&			geometryList, \
											std::map<std::string, FilterLayout>& 			filterList, \
											std::map<std::string, PipelineLayout>&			pipelineList, \
											std::string&						mainPipelineName, \
											const std::vector<std::string>&				staticPaths, \
											const std::map<std::string,HdlTextureFormat>&		requiredFormatList, \
											const std::map<std::string,GeometryModel>&		requiredGeometryList, \
											const std::map<std::string,PipelineLayout>&		requiredPipelineList, \
											const std::string& 					sourceName, \
											const int 						startLine, \
											const int 						bodyLine, \
											std::string& 						executionCode);

			/** LAYOUT_LOADER_MODULE_APPLY_SIGNATURE( className ) Get the implementation signature of the apply function. **/
			#define LAYOUT_LOADER_MODULE_APPLY_IMPLEMENTATION( className )	void className :: apply(const std::vector<std::string>& arguments,  \
											const std::string&					body, \
											const std::string&					currentPath, \
											std::vector<std::string>&				dynamicPaths, \
											std::map<std::string, ShaderSource>&			sharedCodeList, \
											std::map<std::string, HdlTextureFormat>& 		formatList, \
											std::map<std::string, ShaderSource>& 			sourceList, \
											std::map<std::string, GeometryModel>&			geometryList, \
											std::map<std::string, FilterLayout>& 			filterList, \
											std::map<std::string, PipelineLayout>&			pipelineList, \
											std::string&						mainPipelineName, \
											const std::vector<std::string>&				staticPaths, \
											const std::map<std::string,HdlTextureFormat>&		requiredFormatList, \
											const std::map<std::string,GeometryModel>&		requiredGeometryList, \
											const std::map<std::string,PipelineLayout>&		requiredPipelineList, \
											const std::string& 					sourceName, \
											const int 						startLine, \
											const int 						bodyLine, \
											std::string& 						executionCode)

			/** LAYOUT_LOADER_MODULE_APPLY( moduleName, minArgs, maxArgs, bodyPresence, showManualOnError, moduleManual)			Source of a module. **/
			#define LAYOUT_LOADER_MODULE_APPLY( moduleName, minArgs, maxArgs, bodyPresence, showManualOnError, moduleManual)		moduleName :: moduleName (void) : LayoutLoaderModule( #moduleName, moduleManual, minArgs, maxArgs, bodyPresence) { } \
																		void 	moduleName :: apply(	const std::vector<std::string>& 		arguments,  \
																						const std::string&				body, \
																						const std::string&				currentPath, \
																						std::vector<std::string>&			dynamicPaths, \
																						std::map<std::string, ShaderSource>&		sharedCodeList, \
																						std::map<std::string, HdlTextureFormat>& 	formatList, \
																						std::map<std::string, ShaderSource>& 		sourceList, \
																						std::map<std::string, GeometryModel>&		geometryList, \
																						std::map<std::string, FilterLayout>& 		filterList, \
																						std::map<std::string, PipelineLayout>&		pipelineList, \
																						std::string&					mainPipelineName, \
																						const std::vector<std::string>&			staticPaths, \
																						const std::map<std::string,HdlTextureFormat>&	requiredFormatList, \
																						const std::map<std::string,GeometryModel>&	requiredGeometryList, \
																						const std::map<std::string,PipelineLayout>&	requiredPipelineList, \
																						const std::string& sourceName, \
																						const int startLine, \
																						const int bodyLine, \
																						std::string& executionCode)

			#define __ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::iterator iteratorName = varName.find( elementName );
			#define __CONST_ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::const_iterator iteratorName = varName.find( elementName );
			#define __ELEMENT_MUST_BE_IN(iteratorName, varName, elementName)	{ if(iteratorName==varName.end()) throw Exception("Element \"" + elementName + "\" was not found in \"" + #varName + "\".", sourceName, startLine, Exception::ClientScriptException); }
			#define __ELEMENT_MUST_NOT_BE_IN(iteratorName, varName, elementName)	{ if(iteratorName!=varName.end()) throw Exception("Element \"" + elementName + "\" already exists in \"" + #varName + "\".", sourceName, startLine, Exception::ClientScriptException); }
			#define __APPEND_NEW_ELEMENT(type, varName, elementName, element)	varName.insert( std::pair<std::string, type>( elementName, element ) );

			/** ITERATOR_TO_SHAREDCODE( iteratorName, elementName )			Get an iterator on the Shared Code named elementName. **/
			#define ITERATOR_TO_SHAREDCODE( iteratorName, elementName )		__ITERATOR_FIND(ShaderSource, sharedCodeList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_SHAREDCODE( iteratorName, elementName )		Get a constant iterator on the Shared Code named elementName. **/
			#define CONST_ITERATOR_TO_SHAREDCODE( iteratorName, elementName )	__CONST_ITERATOR_FIND(ShaderSource, sharedCodeList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_SHAREDCODE( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_SHAREDCODE( iteratorName )			( iteratorName != sharedCodeList.end() )
			/** SHAREDCODE_MUST_EXIST( elementName )				Check that the Shared Code named elementName must exist (raise an exception otherwise). **/
			#define SHAREDCODE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(ShaderSource, sharedCodeList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, sharedCodeList, elementName) }
			/** SHAREDCODE_MUST_NOT_EXIST( elementName )				Check that the Shared Code named elementName must not exist (raise an exception otherwise). **/
			#define SHAREDCODE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(ShaderSource, sharedCodeList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, sharedCodeList, elementName) }
			/** APPEND_NEW_SHAREDCODE(elementName, newElement)			Append the new element to the Shared Code list. **/
			#define APPEND_NEW_SHAREDCODE(elementName, newElement)			__APPEND_NEW_ELEMENT(ShaderSource, sharedCodeList, elementName, newElement)

			/** ITERATOR_TO_FORMAT( iteratorName, elementName )			Get an iterator on the Format named elementName. **/
			#define ITERATOR_TO_FORMAT( iteratorName, elementName )			__ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_FORMAT( iteratorName, elementName )		Get a constant iterator on the Format named elementName. **/
			#define CONST_ITERATOR_TO_FORMAT( iteratorName, elementName )		__CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_FORMAT( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_FORMAT( iteratorName )			( iteratorName != formatList.end() )
			/** FORMAT_MUST_EXIST( elementName )					Check that the Format named elementName must exist (raise an exception otherwise). **/
			#define FORMAT_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, formatList, elementName) }
			/** FORMAT_MUST_NOT_EXIST( elementName )				Check that the Format named elementName must not exist (raise an exception otherwise). **/
			#define FORMAT_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, formatList, elementName) }
			/** APPEND_NEW_FORMAT(elementName, newElement)				Append the new element to the Format list. **/
			#define APPEND_NEW_FORMAT(elementName, newElement)			__APPEND_NEW_ELEMENT(HdlTextureFormat, formatList, elementName, newElement)

			/** ITERATOR_TO_SHADERSOURCE( iteratorName, elementName )		Get an iterator on the Shader Source named elementName. **/
			#define ITERATOR_TO_SHADERSOURCE( iteratorName, elementName )		__ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_SHADERSOURCE( iteratorName, elementName )		Get a constant iterator on the Shader Source named elementName. **/
			#define CONST_ITERATOR_TO_SHADERSOURCE( iteratorName, elementName )	__CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_SHADERSOURCE( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_SHADERSOURCE( iteratorName )			( iteratorName != sourceList.end() )
			/** SHADERSOURCE_MUST_EXIST( elementName )				Check that the Shader Source named elementName must exist (raise an exception otherwise). **/
			#define SHADERSOURCE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, sourceList, elementName) }
			/** SHADERSOURCE_MUST_NOT_EXIST( elementName )				Check that the Shader Source named elementName must not exist (raise an exception otherwise). **/
			#define SHADERSOURCE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, sourceList, elementName) }
			/** APPEND_NEW_SHADERSOURCE(elementName, newElement)			Append the new element to the Shader Source list. **/
			#define APPEND_NEW_SHADERSOURCE(elementName, newElement)		__APPEND_NEW_ELEMENT(ShaderSource, sourceList, elementName, newElement)
			
			/** ITERATOR_TO_GEOMETRY( iteratorName, elementName )			Get an iterator on the Geometry named elementName. **/
			#define ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_GEOMETRY( iteratorName, elementName )		Get a constant iterator on the Geometry named elementName. **/
			#define CONST_ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_GEOMETRY( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_GEOMETRY( iteratorName )			( iteratorName != geometryList.end() )
			/** GEOMETRY_MUST_EXIST( elementName )					Check that the Geometry named elementName must exist (raise an exception otherwise). **/
			#define GEOMETRY_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, geometryList, elementName) }
			/** GEOMETRY_MUST_NOT_EXIST( elementName )				Check that the Geometry named elementName must not exist (raise an exception otherwise). **/
			#define GEOMETRY_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(GeometryModel, geometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, geometryList, elementName) }
			/** APPEND_NEW_GEOMETRY(elementName, newElement)			Append the new element to the Geometry list. **/
			#define APPEND_NEW_GEOMETRY(elementName, newElement)			__APPEND_NEW_ELEMENT(GeometryModel, geometryList, elementName, newElement)

			/** ITERATOR_TO_FILTER( iteratorName, elementName )			Get an iterator on the Filter named elementName. **/
			#define ITERATOR_TO_FILTER( iteratorName, elementName )			__ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_FILTER( iteratorName, elementName )		Get a constant iterator on the Filter named elementName. **/
			#define CONST_ITERATOR_TO_FILTER( iteratorName, elementName )		__CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_FILTER( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_FILTER( iteratorName )			( iteratorName != filterList.end() )
			/** FILTER_MUST_EXIST( elementName )					Check that the Filter named elementName must exist (raise an exception otherwise). **/
			#define FILTER_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, filterList, elementName) }
			/** FILTER_MUST_NOT_EXIST( elementName )				Check that the Filter named elementName must not exist (raise an exception otherwise). **/
			#define FILTER_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, filterList, elementName) }
			/** APPEND_NEW_FILTER(elementName, newElement)				Append the new element to the Filter list. **/
			#define APPEND_NEW_FILTER(elementName, newElement)			__APPEND_NEW_ELEMENT(FilterLayout, filterList, elementName, newElement)

			/** ITERATOR_TO_PIPELINE( iteratorName, elementName )			Get an iterator on the Pipeline named elementName. **/
			#define ITERATOR_TO_PIPELINE( iteratorName, elementName )		__ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_PIPELINE( iteratorName, elementName )		Get a constant iterator on the Pipeline named elementName. **/
			#define CONST_ITERATOR_TO_PIPELINE( iteratorName, elementName )		__CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_PIPELINE( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_PIPELINE( iteratorName )			( iteratorName != pipelineList.end() )
			/** PIPELINE_MUST_EXIST( elementName )					Check that the Pipeline named elementName must exist (raise an exception otherwise). **/
			#define PIPELINE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, pipelineList, elementName) }
			/** PIPELINE_MUST_NOT_EXIST( elementName )				Check that the Pipeline named elementName must not exist (raise an exception otherwise). **/
			#define PIPELINE_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, pipelineList, elementName) }
			/** APPEND_NEW_PIPELINE(elementName, newElement)			Append the new element to the Pipeline list. **/
			#define APPEND_NEW_PIPELINE(elementName, newElement)			__APPEND_NEW_ELEMENT(PipelineLayout, pipelineList, elementName, newElement)
			
			/** CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	Get a constant iterator on the Required Format named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	__CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDFORMAT( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDFORMAT( iteratorName )		( iteratorName != requiredFormatList.end() )
			/** REQUIREDFORMAT_MUST_EXIST( elementName )				Check that the Required Format named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDFORMAT_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			/** REQUIREDFORMAT_MUST_NOT_EXIST( elementName )			Check that the Required Format named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDFORMAT_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }

			/** CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	Get a constant iterator on the Required Geometry named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName, elementName )	__CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName )		( iteratorName != requiredGeometryList.end() )
			/** REQUIREDGEOMETRY_MUST_EXIST( elementName )				Check that the Required Geometry named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDGEOMETRY_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			/** REQUIREDGEOMETRY_MUST_NOT_EXIST( elementName )			Check that the Required Geometry named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDGEOMETRY_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(GeometryModel, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }

			/** CONST_ITERATOR_TO_REQUIREDPIPELINE( iteratorName, elementName )	Get a constant iterator on the Required Pipeline named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDPIPELINE( iteratorName, elementName )	__CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDPIPELINE( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDPIPELINE( iteratorName )		( iteratorName != requiredPipelineList.end() )
			/** REQUIREDPIPELINE_MUST_EXIST( elementName )				Check that the Required Pipeline named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDPIPELINE_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			/** REQUIREDPIPELINE_MUST_NOT_EXIST( elementName )			Check that the Required Pipeline named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDPIPELINE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }
			
			/** CAST_ARGUMENT( argID, type, varName ) 				Cast the argument arguments[argID] to some type (and create the variable varName). Raise an exception if the cast fails. **/
			#define CAST_ARGUMENT( argID, type, varName ) 				type varName; if(!fromString(arguments[ argID ], varName)) throw Exception("Unable to cast argument " + toString( argID ) + " (\"" + arguments[argID] + "\") to " + #type + ".", sourceName, startLine, Exception::ClientScriptException);

		// Basic Modules : 
			LAYOUT_LOADER_MODULE_DEFINITION( IF_SHAREDCODE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_SHADERSOURCE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_GEOMETRY_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FILTERLAYOUT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_PIPELINELAYOUT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDFORMAT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDGEOMETRY_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDPIPELINE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SCALE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_CHANNELS )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_DEPTH )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_FILTERING )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_WRAPPING )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_MIPMAP )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MINIMUM_WIDTH )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MAXIMUM_WIDTH )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MINIMUM_HEIGHT )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MAXIMUM_HEIGHT )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MINIMUM_PIXELS )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MAXIMUM_PIXELS )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MINIMUM_ELEMENTS )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_MAXIMUM_ELEMENTS )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SMALLER_POWER_OF_TWO )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_LARGER_POWER_OF_TWO )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SWITCH_DIMENSIONS )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_SETTING_MATCH )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_SETTING_LARGERTHAN )
			LAYOUT_LOADER_MODULE_DEFINITION( GENERATE_SAME_SIZE_2D_GRID )
			LAYOUT_LOADER_MODULE_DEFINITION( GENERATE_SAME_SIZE_3D_GRID )
			LAYOUT_LOADER_MODULE_DEFINITION( CHAIN_PIPELINES )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_TO_CONSTANT )
			LAYOUT_LOADER_MODULE_DEFINITION( SINGLE_FILTER_PIPELINE )
			LAYOUT_LOADER_MODULE_DEFINITION( ABORT_ERROR )
	}
}

#endif
