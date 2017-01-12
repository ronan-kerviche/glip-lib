/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
		// (If your module was to be more complex, you will have to go through a full description)

		// In the source file (must have exactly 3 arguments (min = max = 3), no body and show the manual if any errors occur) : 
		LAYOUT_LOADER_MODULE_APPLY( MyAdditionModule, 3, 3, -1, true, 	"DESCRIPTION{Increase or decrease the size of a format, save as a new format.}"
										 ARGUMENT:nameOriginal{Name of the original format.}"
										 ARGUMENT:delta{Integer, change in width/height.}"
										 ARGUMENT:nameNew{Name of the new format.}")
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
			CAST_ARGUMENT(1, int, delta)

			// Change the size : 
			newFmt.setWidth( newFmt.getWidth() + delta );
			newFmt.setHeight( newFmt.getHeight() + delta );

			// Check the size : 
			if(newFmt.getWidth()<=0 || newFmt.getHeight()<=0) // The following exception will retain the line and source name information :
				throw Exception("The new format is not valid (size : " + toString(newFmt.getWidth()) + "x" + toString(newFmt.getHeight()) + ").", sourceName, startLine, Exception::ClientScriptException);

			// Append the new format under the right name :
			APPEND_NEW_FORMAT( arguments.back(), newFmt )
		}
	\endcode

	Use it in a script : 
	\code 
		CALL: MyAdditionModule(someFormat, 128, newLargerFormat)
	\endcode

## Modules List
### IF_MODULE_AVAILABLE
<blockquote>
<b>CALL</b>:IF_MODULE_AVAILABLE(moduleName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the MODULE is available and can be used.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>moduleName</i></td> <td>Name of the module to be used.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_FORMAT_DEFINED
<blockquote>
<b>CALL</b>:IF_FORMAT_DEFINED(formatName [, includeRequiredFormats])<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the FORMAT was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>formatName</i></td> <td>Name of the format to test.</td></tr>
<tr class="glipDescrRow"><td><i>includeRequiredFormats</i></td> <td>Optional, include the required formats. Either TRUE (default) or FALSE.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_SOURCE_DEFINED
<blockquote>
<b>CALL</b>:IF_SOURCE_DEFINED(sourceName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the SOURCE was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>sourceName</i></td> <td>Name of the source to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_GEOMETRY_DEFINED
<blockquote>
<b>CALL</b>:IF_GEOMETRY_DEFINED(geometryName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the GEOMETRY was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>geometryName</i></td> <td>Name of the geometry to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_FILTERLAYOUT_DEFINED
<blockquote>
<b>CALL</b>:IF_FILTERLAYOUT_DEFINED(filterLayoutName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the FILTER_LAYOUT was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>filterLayoutName</i></td> <td>Name of the filter layout to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_PIPELINELAYOUT_DEFINED
<blockquote>
<b>CALL</b>:IF_PIPELINELAYOUT_DEFINED(pipelineLayoutName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the PIPELINE_LAYOUT was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>pipelineLayoutName</i></td> <td>Name of the pipeline layout to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_REQUIREDFORMAT_DEFINED
<blockquote>
<b>CALL</b>:IF_REQUIREDFORMAT_DEFINED(requiredFormatName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the REQUIRED_FORMAT was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>requiredFormatName</i></td> <td>Name of the required format to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_REQUIREDGEOMETRY_DEFINED
<blockquote>
<b>CALL</b>:IF_REQUIREDGEOMETRY_DEFINED(requiredGeometryName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the REQUIRED_GEOMETRY was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>requiredGeometryName</i></td> <td>Name of the required geometry to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### IF_REQUIREDPIPELINE_DEFINED
<blockquote>
<b>CALL</b>:IF_REQUIREDPIPELINE_DEFINED(requiredPipelineName)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Check if the REQUIRED_PIPELINE was defined.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>requiredPipelineName</i></td> <td>Name of the required pipeline to test.</td></tr>
</table>

<b>Body</b> : Cases corresponding to the test : TRUE{...} FALSE{...}.

### FORMAT_CHANGE_SIZE
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_SIZE(nameNew, nameOriginal, widthNew, heightNew)<br>
</blockquote>

Change the size of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format</td></tr>
<tr class="glipDescrRow"><td><i>widthNew</i></td> <td>New width.</td></tr>
<tr class="glipDescrRow"><td><i>heightNew</i></td> <td>New height.</td></tr>
</table>


### FORMAT_INCREASE_SIZE
<blockquote>
<b>CALL</b>:FORMAT_INCREASE_SIZE(nameNew, nameOriginal, increaseDelta)<br>
</blockquote>

Increase a format size by a scalar (or two), save as a new format. Will prevent to reach a 0x0 texture by ensuring that the size is at least 1 pixel in each dimension.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>increaseDelta</i></td> <td>Increase to be applied, can be splitted into X and Y. It can also be the name of an existing format.</td></tr>
</table>


### FORMAT_DECREASE_SIZE
<blockquote>
<b>CALL</b>:FORMAT_DECREASE_SIZE(nameNew, nameOriginal, increaseDelta)<br>
</blockquote>

Decrease a format size by a scalar (or two), save as a new format. Will prevent to reach a 0x0 texture by ensuring that the size is at least 1 pixel in each dimension.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>increaseDelta</i></td> <td>Decrease delta to be applied, can be splitted into X and Y. It can also be the name of an existing format.</td></tr>
</table>


### FORMAT_SCALE_SIZE
<blockquote>
<b>CALL</b>:FORMAT_SCALE_SIZE(nameNew, nameOriginal, scaleFactor)<br>
</blockquote>

Scale a format by a scalar (or two), save as a new format. Will prevent to reach a 0x0 texture by ensuring that the size is at least 1 pixel in each dimension.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>scaleFactor</i></td> <td>Scaling to be applied, can be splitted into X and Y. It can also be the name of an existing format.</td></tr>
</table>


### FORMAT_INVSCALE_SIZE
<blockquote>
<b>CALL</b>:FORMAT_INVSCALE_SIZE(nameNew, nameOriginal, scaleFactor)<br>
</blockquote>

Scale a format by the inverse of a scalar (or two), save as a new format. Will prevent to reach a 0x0 texture by ensuring that the size is at least 1 pixel in each dimension. The results is rounded to the floor.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>scaleFactor</i></td> <td>Inverse scaling to be applied, can be splitted into X and Y. It can also be the name of an existing format.</td></tr>
</table>


### FORMAT_CHANGE_CHANNELS
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_CHANNELS(nameNew, nameOriginal, channelNew)<br>
</blockquote>

Change the channels of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>channelNew</i></td> <td>New channel mode.</td></tr>
</table>


### FORMAT_CHANGE_DEPTH
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_DEPTH(nameNew, nameOriginal, depthNew)<br>
</blockquote>

Change the depth of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>depthNew</i></td> <td>New depth.</td></tr>
</table>


### FORMAT_CHANGE_FILTERING
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_FILTERING(nameNew, nameOriginal, minNew, magNew)<br>
</blockquote>

Change the filtering of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>minNew</i></td> <td>New minification filter.</td></tr>
<tr class="glipDescrRow"><td><i>magNew</i></td> <td>New magnification filter.</td></tr>
</table>


### FORMAT_CHANGE_WRAPPING
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_WRAPPING(nameNew, nameOriginal, sNew, tNew)<br>
</blockquote>

Change the wrapping of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>sNew</i></td> <td>New S wrapping parameter.</td></tr>
<tr class="glipDescrRow"><td><i>tNew</i></td> <td>New T wrapping parameter.</td></tr>
</table>


### FORMAT_CHANGE_MIPMAP
<blockquote>
<b>CALL</b>:FORMAT_CHANGE_MIPMAP(nameNew, nameOriginal, mNew)<br>
</blockquote>

Change the mipmap level of a format, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameOriginal</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>mNew</i></td> <td>New maximum mipmap parameter.</td></tr>
</table>


### FORMAT_MINIMUM_WIDTH
<blockquote>
<b>CALL</b>:FORMAT_MINIMUM_WIDTH(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the smallest width, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MAXIMUM_WIDTH
<blockquote>
<b>CALL</b>:FORMAT_MAXIMUM_WIDTH(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the largest width, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the first format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MINIMUM_HEIGHT
<blockquote>
<b>CALL</b>:FORMAT_MINIMUM_HEIGHT(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the smallest height, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MAXIMUM_HEIGHT
<blockquote>
<b>CALL</b>:FORMAT_MAXIMUM_HEIGHT(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the largest height, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MINIMUM_PIXELS
<blockquote>
<b>CALL</b>:FORMAT_MINIMUM_PIXELS(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the smallest number of pixels, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MAXIMUM_PIXELS
<blockquote>
<b>CALL</b>:FORMAT_MAXIMUM_PIXELS(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the largest number of pixels, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MINIMUM_ELEMENTS
<blockquote>
<b>CALL</b>:FORMAT_MINIMUM_ELEMENTS(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the smallest number of elements (pixels times channels count), save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_MAXIMUM_ELEMENTS
<blockquote>
<b>CALL</b>:FORMAT_MAXIMUM_ELEMENTS(nameNew, nameFormat1, nameFormat2 [, nameFormat3...])<br>
</blockquote>

Find the format having the largest number of elements (pixels times channels count), save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat1</i></td> <td>Name of the fist format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat2</i></td> <td>Name of the second format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat3...</i></td> <td>Other formats.</td></tr>
</table>


### FORMAT_SMALLER_POWER_OF_TWO
<blockquote>
<b>CALL</b>:FORMAT_SMALLER_POWER_OF_TWO(nameNew, nameFormat [, strict])<br>
</blockquote>

Generate a new format clamped to the closest smaller power of 2.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>strict</i></td> <td>Either TRUE or FALSE (default).</td></tr>
</table>


### FORMAT_LARGER_POWER_OF_TWO
<blockquote>
<b>CALL</b>:FORMAT_LARGER_POWER_OF_TWO(nameNew, nameFormat [, strict])<br>
</blockquote>

Generate a new format clamped to the closest larger power of 2.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>strict</i></td> <td>Either TRUE or FALSE (default).</td></tr>
</table>


### FORMAT_SWAP_DIMENSIONS
<blockquote>
<b>CALL</b>:FORMAT_SWAP_DIMENSIONS(nameNew, nameFormat)<br>
</blockquote>

Swap the width and height values, save as a new format.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNew</i></td> <td>Name of the new format.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the original format.</td></tr>
</table>


### IF_FORMAT_SETTING_MATCH
<blockquote>
<b>CALL</b>:IF_FORMAT_SETTING_MATCH(nameFormat, nameSetting, value)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Match if a format setting is equal to a value (unsigned integer or GL keyword).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the targeted format.</td></tr>
<tr class="glipDescrRow"><td><i>nameSetting</i></td> <td>Name of the setting. See the documentation of HdlAbstractTextureFormat.</td></tr>
<tr class="glipDescrRow"><td><i>value</i></td> <td>The unsigned integer value or GLenum name to test against.</td></tr>
</table>

<b>Body</b> : Contains any or all of the blocks TRUE{...} and FALSE{...}. The right block will be exectuted following this test.

### IF_FORMAT_SETTING_LARGERTHAN
<blockquote>
<b>CALL</b>:IF_FORMAT_SETTING_LARGERTHAN(nameFormat, nameSetting, value)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Match if a format setting is larger than a value (unsigned integer or GL keyword).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the targeted format.</td></tr>
<tr class="glipDescrRow"><td><i>nameSetting</i></td> <td>Name of the setting. See the documentation of HdlAbstractTextureFormat.</td></tr>
<tr class="glipDescrRow"><td><i>value</i></td> <td>The unsigned integer value or GLenum name to test against.</td></tr>
</table>

<b>Body</b> : Contains any or all of the blocks TRUE{...} and FALSE{...}. The right block will be exectuted following this test.

### OVERRIDE_REQUIRED_FORMAT
<blockquote>
<b>CALL</b>:OVERRIDE_REQUIRED_FORMAT(name, overrideFormat, defaultFormat [, remainingArguments])<br>
</blockquote>

Override the selection of a required format if another format with higher priority is found.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>name</i></td> <td>Name of the format to be created.</td></tr>
<tr class="glipDescrRow"><td><i>overrideFormat</i></td> <td>Name of the format which will be used first, if it exists.</td></tr>
<tr class="glipDescrRow"><td><i>defaultFormat</i></td> <td>Name of the default format to use</td></tr>
<tr class="glipDescrRow"><td><i>remainingArguments</i></td> <td>See REQUIRED_FORMAT arguments.</td></tr>
</table>


### GENERATE_SAME_SIZE_2D_GRID
<blockquote>
<b>CALL</b>:GENERATE_SAME_SIZE_2D_GRID(nameNewGeometry, nameFormat [, normalized])<br>
</blockquote>

Create a 2D grid geometry of the same size as the format in argument (width and height).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNewGeometry</i></td> <td>Name of the new geometry.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>normalized</i></td> <td>Either TRUE or FALSE. If enabled, the vertices coordinates will be in the range [0,1].</td></tr>
</table>


### GENERATE_SAME_SIZE_3D_GRID
<blockquote>
<b>CALL</b>:GENERATE_SAME_SIZE_3D_GRID(nameNewGeometry, nameFormat [, normalized])<br>
</blockquote>

Create a 3D grid geometry of the same size as the format in argument (width, height and number of channels).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNewGeometry</i></td> <td>Name of the new geometry.</td></tr>
<tr class="glipDescrRow"><td><i>nameFormat</i></td> <td>Name of the original format.</td></tr>
<tr class="glipDescrRow"><td><i>normalized</i></td> <td>Either TRUE or FALSE. If enabled, the vertices coordinates will be in the range [0,1].</td></tr>
</table>


### CHAIN_PIPELINES
<blockquote>
<b>CALL</b>:CHAIN_PIPELINES(nameNewPipelineLayout, isStrict, namePipelineLayout1, namePipelineLayout2 [, namePipelineLayout3...])<br>
</blockquote>

Create a pipeline by connecting the pipelines passed in arguments.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>nameNewPipelineLayout</i></td> <td>Name of the new pipeline.</td></tr>
<tr class="glipDescrRow"><td><i>isStrict</i></td> <td>Either TRUE or FALSE. If enabled, the pipelines connection are enforced strictly (if outputs of the first pipeline are not equal to the number of input of the second pipeline, then the module will report an error.</td></tr>
<tr class="glipDescrRow"><td><i>namePipelineLayout1</i></td> <td>Name of the first pipeline in the chain.</td></tr>
<tr class="glipDescrRow"><td><i>namePipelineLayout2</i></td> <td>Name of the second pipeline in the chain.</td></tr>
<tr class="glipDescrRow"><td><i>namePipelineLayout3...</i></td> <td>Other pipelines.</td></tr>
</table>


### FORMAT_TO_CONSTANT
<blockquote>
<b>CALL</b>:FORMAT_TO_CONSTANT(formatName [, sourceName])<br>
</blockquote>

Create a SOURCE containing a const ivec2 declaration describing the size of the texture passed in argument. For instance, can be used in a shader with : INSERT(name).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>formatName</i></td> <td>Name of the texture format to be used.</td></tr>
<tr class="glipDescrRow"><td><i>sourceName</i></td> <td>Name of the source to be created. If not set, the name of the source will be the same as the name of the format.</td></tr>
</table>


### SINGLE_FILTER_PIPELINE
<blockquote>
<b>CALL</b>:SINGLE_FILTER_PIPELINE(pipelineName, outputTextureFormat)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Create a pipeline with a single filter.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>pipelineName</i></td> <td>Name of the new pipeline.</td></tr>
<tr class="glipDescrRow"><td><i>outputTextureFormat</i></td> <td>Name of the format to render to.</td></tr>
</table>

<b>Body</b> : Source of the fragment shader to be implemented.

### IF_GLSL_VERSION_MATCH
<blockquote>
<b>CALL</b>:IF_GLSL_VERSION_MATCH(version)<br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}<br>
</blockquote>

Test the GLSL available available during compilation.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>version</i></td> <td>The GLSL version name to be tested : 1.30, 3.30, etc.</td></tr>
</table>

<b>Body</b> : Contains any or all of the blocks TRUE{...} and FALSE{...}. The right block will be exectuted following this test.

### ABORT_ERROR
<blockquote>
<b>CALL</b>:ABORT_ERROR(error)<br>
[{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}]<br>
</blockquote>

Abort the Layout loading operation with a user defined error.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>error</i></td> <td>Error description.</td></tr>
</table>

<b>Body</b> : Optional, More complete description of the error.

### GENERATE_FFT1D_PIPELINE
<blockquote>
<b>CALL</b>:GENERATE_FFT1D_PIPELINE(width, name [, options...])<br>
[{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}]<br>
</blockquote>

Generate the 1D FFT Pipeline transformation.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>width</i></td> <td>Width, can be either a numeral or the name of an existing format.</td></tr>
<tr class="glipDescrRow"><td><i>name</i></td> <td>Name of the new pipeline.</td></tr>
<tr class="glipDescrRow"><td><i>options...</i></td> <td>Options to be used by the FFT process : SHIFTED, INVERSED, ZERO_PADDING, COMPATIBILITY_MODE, NO_INPUT.</td></tr>
</table>

<b>Body</b> : PRE{...} block contains a filtering function to be applied before the FFT. It must define a function vec4 pre(in vec4 colorFromTexture, in float x). POST{...} block contains a filtering function to be applied after the FFT. It must implement a function vec4 post(in vec4 colorAfterFFT, in float x). Both of these block can declare their own uniform variables.

### GENERATE_FFT2D_PIPELINE
<blockquote>
<b>CALL</b>:GENERATE_FFT2D_PIPELINE(width, height [, name, options...])<br>
[{<br>
&nbsp;&nbsp;&nbsp;&nbsp;<i>body</i><br>
}]<br>
</blockquote>

Generate the 2D FFT Pipeline transformation.

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>width</i></td> <td>Width, can be either a numeral or the name of an existing format.</td></tr>
<tr class="glipDescrRow"><td><i>height</i></td> <td>Height, can be either a numeral or the name of an existing format.</td></tr>
<tr class="glipDescrRow"><td><i>name</i></td> <td>Name of the new pipeline.</td></tr>
<tr class="glipDescrRow"><td><i>options...</i></td> <td>Options to be used by the FFT process : SHIFTED, INVERSED, ZERO_PADDING, COMPATIBILITY_MODE, NO_INPUT.</td></tr>
</table>

<b>Body</b> : PRE{...} block contains a filtering function to be applied before the FFT. It must define a function vec4 pre(in vec4 colorFromTexture, in vec2 x). POST{...} block contains a filtering function to be applied after the FFT. It must implement a function vec4 post(in vec4 colorAfterFFT, in vec2 x). Both of these block can declare their own uniform variables.

### LOAD_OBJ_GEOMETRY
<blockquote>
<b>CALL</b>:LOAD_OBJ_GEOMETRY(filename, geometryName [, strict])<br>
</blockquote>

Load a geometry from a Wavefront file (OBJ).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>filename</i></td> <td>Name of the file to load.</td></tr>
<tr class="glipDescrRow"><td><i>geometryName</i></td> <td>Name of the new geometry.</td></tr>
<tr class="glipDescrRow"><td><i>strict</i></td> <td>Either TRUE or FALSE. If enabled, the loader will abort upon inding an unknown tag.</td></tr>
</table>


### LOAD_STL_GEOMETRY
<blockquote>
<b>CALL</b>:LOAD_STL_GEOMETRY(filename, geometryName)<br>
</blockquote>

Load a geometry from a StereoLithography file (STL).

<table class="glipDescrTable">
<tr class="glipDescrHeaderRow"><th class="glipDescrHeaderFirstColumn">Argument</th><th>Description</th></tr>
<tr class="glipDescrRow"><td><i>filename</i></td> <td>Name of the file to load</td></tr>
<tr class="glipDescrRow"><td><i>geometryName</i></td> <td>Name of the new geometry.</td></tr>
</table>

**/
		class GLIP_API LayoutLoaderModule
		{
			private : 
				const std::string 					name;
				std::string						description,
											bodyDescription;
				std::vector<std::pair<std::string,std::string> >	argumentsDescriptions;
				const int						minNumArguments,
											maxNumArguments;
				const char 						bodyPresence;

				void initManual(const std::string& _manual);

			protected :
				LayoutLoaderModule( const std::string& _name, const std::string& _manual, const int& _minNumArguments, const int& _maxNumArguments, const char& _bodyPresence);

			public :
				LayoutLoaderModule(const LayoutLoaderModule& m);
				virtual ~LayoutLoaderModule(void);

				/**
				\fn virtual void LayoutLoaderModule::apply(const std::vector<std::string>& arguments, const std::string& body, const std::string& currentPath, std::vector<std::string>& dynamicPaths, std::map<std::string, HdlTextureFormat>& formatList, std::map<std::string, ShaderSource>& sourceList, std::map<std::string, GeometryModel>& geometryList, std::map<std::string, FilterLayout>& filterList, std::map<std::string, PipelineLayout>& pipelineList, std::string& mainPipelineName, const std::vector<std::string>& staticPaths, const std::map<std::string,HdlTextureFormat>& requiredFormatList, const std::map<std::string,GeometryModel>& requiredGeometryList, const std::map<std::string,PipelineLayout>& requiredPipelineList, const std::map<std::string, LayoutLoaderModule*>& moduleList, const std::string& sourceName, const int startLine, const int bodyLine, std::string& executionSource, std::string& executionSourceName, int& executionStartLine) = 0
				\brief Interface of the module : this function will be called on each corresponding token CALL for the LayoutLoader which has the module. 
				\param arguments 		The arguments of the called, their number has already been checked.
				\param body 			The body of the call (might be empty), its presence has already been checked.
				\param currentPath 		The currentPath in which the LayoutLoader is operating.
				\param dynamicPaths		The list of paths dynamically built (only for the current load operation).
				\param formatList		The list of formats currently loaded.
								For easy access see #ITERATOR_TO_FORMAT, #CONST_ITERATOR_TO_FORMAT, #FORMAT_MUST_EXIST, #FORMAT_MUST_NOT_EXIST and #APPEND_NEW_FORMAT.
				\param sourceList		The list of sources currently loaded.
								For easy access see #ITERATOR_TO_SOURCE, #CONST_ITERATOR_TO_SOURCE, #SOURCE_MUST_EXIST, #SOURCE_MUST_NOT_EXIST and #APPEND_NEW_SOURCE.
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
				\param requiredSourceList	The list of static sources available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDSOURCE, #REQUIREDSOURCE_MUST_EXIST, #REQUIREDSOURCE_MUST_NOT_EXIST.
				\param requiredGeometryList	The list of static geometries available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDGEOMETRY, #REQUIREDGEOMETRY_MUST_EXIST, #REQUIREDGEOMETRY_MUST_NOT_EXIST.
				\param requiredPipelineList	The list of static pipelines available.
								For easy access see #CONST_ITERATOR_TO_REQUIREDPIPELINE, #REQUIREDPIPELINE_MUST_EXIST, #REQUIREDPIPELINE_MUST_NOT_EXIST.
				\param moduleList		The list of modules available.
								For easy access see #CONST_ITERATOR_TO_MODULE, #MODULE_MUST_EXIST, #MODULE_MUST_NOT_EXIST.
				\param sourceName		Name of the source from which the call was extracted.
				\param startLine		Line index of the module call.
				\param bodyLine			Line index of the body for this module call.
				\param executionSource		The code which has to be run at after the function complete (leave it empty if no code needs to be run, the syntax of the code expected is the same as scripts).
				\param executionSourceName	The name of the source for the post-execution.
				\param executionStartLine	The first line number of the source for the post-execution.
				**/
				virtual void apply(	const std::vector<std::string>& 			arguments, 
							const std::string&					body, 
							const std::string&					currentPath,
							std::vector<std::string>&				dynamicPaths,
							std::map<std::string, HdlTextureFormat>& 		formatList,
							std::map<std::string, ShaderSource>& 			sourceList,
							std::map<std::string, std::list<GeometryModel> >&	geometryList,
							std::map<std::string, FilterLayout>& 			filterList,
							std::map<std::string, PipelineLayout>&			pipelineList,
							std::string&						mainPipelineName, 
							const std::vector<std::string>&				staticPaths,
							const std::map<std::string, HdlTextureFormat>&		requiredFormatList,
							const std::map<std::string, ShaderSource>&		requiredSourceList,
							const std::map<std::string, std::list<GeometryModel> >&	requiredGeometryList,
							const std::map<std::string, PipelineLayout>&		requiredPipelineList,
							const std::map<std::string, LayoutLoaderModule*>&	moduleList,
							const std::string& 					sourceName,
							const int 						startLine,
							const int 						bodyLine,
							std::string& 						executionSource,
							std::string&						executionSourceName,
							int&							executionStartLine) = 0;

				const std::string& getName(void) const;
				const int& getMinNumArguments(void) const;
				const int& getMaxNumArguments(void) const;
				const char& bodyPresenceTest(void) const;
				const std::string& getDescription(void) const;
				const std::string& getBodyDescription(void) const;
				const std::vector<std::pair<std::string,std::string> >& getArgumentsDescriptions(void) const;
				std::string getManual(void) const;

				// Events : 
				virtual void beginLoadLayout(void);
				virtual void endLoadLayout(void);

				// Static tools : 
				static void addBasicModules(LayoutLoader& loader);
				static std::vector<LayoutLoaderModule*> getBasicModulesList(void);
				static bool getBoolean(const std::string& arg, const std::string& sourceName="", int line=1);
				static void getCases(const std::string& body, std::string& trueCase, int& trueCaseStartLine, std::string& falseCase, int& falseCaseStartLine, const std::string& sourceName="", int bodyLine=1);
				static std::vector<std::string> findFile(const std::string& filename, const std::vector<std::string>& dynamicPaths);
		};

		// Simple MACROS : 
			/** LAYOUT_LOADER_ARGUMENTS_LIST List of the arguments of LayoutLaoderModule::apply. It does not include the parenthesis and it is safe to use from any namespace. **/
			#define LAYOUT_LOADER_ARGUMENTS_LIST \
					const std::vector<std::string>& 						arguments,  \
					const std::string&								body, \
					const std::string&								currentPath, \
					std::vector<std::string>&							dynamicPaths, \
					std::map<std::string, Glip::CoreGL::HdlTextureFormat>& 				formatList, \
					std::map<std::string, Glip::CoreGL::ShaderSource>& 				sourceList, \
					std::map<std::string, std::list<Glip::CorePipeline::GeometryModel> >&		geometryList, \
					std::map<std::string, Glip::CorePipeline::FilterLayout>& 			filterList, \
					std::map<std::string, Glip::CorePipeline::PipelineLayout>&			pipelineList, \
					std::string&									mainPipelineName, \
					const std::vector<std::string>&							staticPaths, \
					const std::map<std::string, Glip::CoreGL::HdlTextureFormat>&			requiredFormatList, \
					const std::map<std::string, Glip::CoreGL::ShaderSource>&			requiredSourceList, \
					const std::map<std::string, std::list<Glip::CorePipeline::GeometryModel> >&	requiredGeometryList, \
					const std::map<std::string, Glip::CorePipeline::PipelineLayout>&		requiredPipelineList, \
					const std::map<std::string, Glip::Modules::LayoutLoaderModule*>&		moduleList, \
					const std::string& 								sourceName, \
					const int 									startLine, \
					const int 									bodyLine, \
					std::string& 									executionSource, \
					std::string&									executionSourceName, \
					int&										executionStartLine

			/** LAYOUT_LOADER_MODULE_DEFINITION( name )	Declaration of a module. **/
			#define LAYOUT_LOADER_MODULE_DEFINITION( name )	class name : public Glip::Modules::LayoutLoaderModule \
									{ \
										public : \
											name (void); \
											\
											void apply(LAYOUT_LOADER_ARGUMENTS_LIST); \
									};

			/** LAYOUT_LOADER_MODULE_APPLY( moduleName, minArgs, maxArgs, bodyPresence, moduleManual)			Source of a module. **/
			#define LAYOUT_LOADER_MODULE_APPLY( moduleName, minArgs, maxArgs, bodyPresence, moduleManual)	\
											moduleName :: moduleName (void) : Glip::Modules::LayoutLoaderModule( #moduleName, moduleManual, minArgs, maxArgs, bodyPresence) { } \
											void 	moduleName :: apply(LAYOUT_LOADER_ARGUMENTS_LIST)


			#define __ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::iterator iteratorName = varName.find( elementName );
			#define __CONST_ITERATOR_FIND(type, varName, iteratorName, elementName)	std::map<std::string, type >::const_iterator iteratorName = varName.find( elementName );
			#define __ELEMENT_MUST_BE_IN(iteratorName, varName, elementName)	{ if(iteratorName==varName.end()) throw Exception("Element \"" + elementName + "\" was not found in \"" + #varName + "\".", sourceName, startLine, Exception::ClientScriptException); }
			#define __ELEMENT_MUST_NOT_BE_IN(iteratorName, varName, elementName)	{ if(iteratorName!=varName.end()) throw Exception("Element \"" + elementName + "\" already exists in \"" + #varName + "\".", sourceName, startLine, Exception::ClientScriptException); }
			#define __APPEND_NEW_ELEMENT(type, varName, elementName, element)	varName.insert( std::pair<std::string, type>( elementName, element ) );

			/** ITERATOR_TO_FORMAT( iteratorName, elementName )			Get an iterator on the Format named elementName. **/
			#define ITERATOR_TO_FORMAT( iteratorName, elementName )			__ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, formatList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_FORMAT( iteratorName, elementName )		Get a constant iterator on the Format named elementName. **/
			#define CONST_ITERATOR_TO_FORMAT( iteratorName, elementName )		__CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, formatList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_FORMAT( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_FORMAT( iteratorName )			( iteratorName != formatList.end() )
			/** FORMAT_MUST_EXIST( elementName )					Check that the Format named elementName must exist (raise an exception otherwise). **/
			#define FORMAT_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, formatList, elementName) }
			/** FORMAT_MUST_NOT_EXIST( elementName )				Check that the Format named elementName must not exist (raise an exception otherwise). **/
			#define FORMAT_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, formatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, formatList, elementName) }
			/** APPEND_NEW_FORMAT(elementName, newElement)				Append the new element to the Format list. **/
			#define APPEND_NEW_FORMAT(elementName, newElement)			__APPEND_NEW_ELEMENT(Glip::CoreGL::HdlTextureFormat, formatList, elementName, newElement)

			/** ITERATOR_TO_SOURCE( iteratorName, elementName )			Get an iterator on the Source named elementName. **/
			#define ITERATOR_TO_SOURCE( iteratorName, elementName )			__ITERATOR_FIND(Glip::CoreGL::ShaderSource, sourceList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_SOURCE( iteratorName, elementName )		Get a constant iterator on the Source named elementName. **/
			#define CONST_ITERATOR_TO_SOURCE( iteratorName, elementName )		__CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, sourceList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_SOURCE( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_SOURCE( iteratorName )			( iteratorName != sourceList.end() )
			/** SOURCE_MUST_EXIST( elementName )					Check that the Shader Source named elementName must exist (raise an exception otherwise). **/
			#define SOURCE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, sourceList, elementName) }
			/** SOURCE_MUST_NOT_EXIST( elementName )				Check that the Shader Source named elementName must not exist (raise an exception otherwise). **/
			#define SOURCE_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, sourceList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, sourceList, elementName) }
			/** APPEND_NEW_SOURCE(elementName, newElement)				Append the new element to the Shader Source list. **/
			#define APPEND_NEW_SOURCE(elementName, newElement)			__APPEND_NEW_ELEMENT(Glip::CoreGL::ShaderSource, sourceList, elementName, newElement)

			/** ITERATOR_TO_GEOMETRY( iteratorName, elementName )			Get an iterator on the Geometry named elementName. **/
			#define ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, geometryList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_GEOMETRY( iteratorName, elementName )		Get a constant iterator on the Geometry named elementName. **/
			#define CONST_ITERATOR_TO_GEOMETRY( iteratorName, elementName )		__CONST_ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, geometryList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_GEOMETRY( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_GEOMETRY( iteratorName )			( iteratorName != geometryList.end() )
			/** GEOMETRY_MUST_EXIST( elementName )					Check that the Geometry named elementName must exist (raise an exception otherwise). **/
			#define GEOMETRY_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(std::list<Glip::CorePipelin::GeometryModel>, geometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, geometryList, elementName) }
			/** GEOMETRY_MUST_NOT_EXIST( elementName )				Check that the Geometry named elementName must not exist (raise an exception otherwise). **/
			#define GEOMETRY_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, geometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, geometryList, elementName) }
			/** APPEND_NEW_GEOMETRY(elementName, newElement)			Append the new element to the Geometry list. **/
			#define APPEND_NEW_GEOMETRY(elementName, newElement)			__APPEND_NEW_ELEMENT(std::list<Glip::CorePipeline::GeometryModel>, geometryList, elementName, newElement)

			/** ITERATOR_TO_FILTER( iteratorName, elementName )			Get an iterator on the Filter named elementName. **/
			#define ITERATOR_TO_FILTER( iteratorName, elementName )			__ITERATOR_FIND(Glip::CorePipeline::FilterLayout, filterList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_FILTER( iteratorName, elementName )		Get a constant iterator on the Filter named elementName. **/
			#define CONST_ITERATOR_TO_FILTER( iteratorName, elementName )		__CONST_ITERATOR_FIND(Glip::CorePipeline::FilterLayout, filterList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_FILTER( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_FILTER( iteratorName )			( iteratorName != filterList.end() )
			/** FILTER_MUST_EXIST( elementName )					Check that the Filter named elementName must exist (raise an exception otherwise). **/
			#define FILTER_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CorePipeline::FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, filterList, elementName) }
			/** FILTER_MUST_NOT_EXIST( elementName )				Check that the Filter named elementName must not exist (raise an exception otherwise). **/
			#define FILTER_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CorePipeline::FilterLayout, filterList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, filterList, elementName) }
			/** APPEND_NEW_FILTER(elementName, newElement)				Append the new element to the Filter list. **/
			#define APPEND_NEW_FILTER(elementName, newElement)			__APPEND_NEW_ELEMENT(Glip::CorePipeline::FilterLayout, filterList, elementName, newElement)

			/** ITERATOR_TO_PIPELINE( iteratorName, elementName )			Get an iterator on the Pipeline named elementName. **/
			#define ITERATOR_TO_PIPELINE( iteratorName, elementName )		__ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, pipelineList, iteratorName, elementName)
			/** CONST_ITERATOR_TO_PIPELINE( iteratorName, elementName )		Get a constant iterator on the Pipeline named elementName. **/
			#define CONST_ITERATOR_TO_PIPELINE( iteratorName, elementName )		__CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, pipelineList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_PIPELINE( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_PIPELINE( iteratorName )			( iteratorName != pipelineList.end() )
			/** PIPELINE_MUST_EXIST( elementName )					Check that the Pipeline named elementName must exist (raise an exception otherwise). **/
			#define PIPELINE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, pipelineList, elementName) }
			/** PIPELINE_MUST_NOT_EXIST( elementName )				Check that the Pipeline named elementName must not exist (raise an exception otherwise). **/
			#define PIPELINE_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, pipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, pipelineList, elementName) }
			/** APPEND_NEW_PIPELINE(elementName, newElement)			Append the new element to the Pipeline list. **/
			#define APPEND_NEW_PIPELINE(elementName, newElement)			__APPEND_NEW_ELEMENT(Glip::CorePipeline::PipelineLayout, pipelineList, elementName, newElement)
			
			/** CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	Get a constant iterator on the Required Format named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDFORMAT( iteratorName, elementName )	__CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, requiredFormatList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDFORMAT( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDFORMAT( iteratorName )		( iteratorName != requiredFormatList.end() )
			/** REQUIREDFORMAT_MUST_EXIST( elementName )				Check that the Required Format named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDFORMAT_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredFormatList, elementName) }
			/** REQUIREDFORMAT_MUST_NOT_EXIST( elementName )			Check that the Required Format named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDFORMAT_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CoreGL::HdlTextureFormat, requiredFormatList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredFormatList, elementName) }

			/** CONST_ITERATOR_TO_REQUIREDSOURCE( iteratorName, elementName )	Get a constant iterator on the Required Format named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDSOURCE( iteratorName, elementName )	__CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, requiredSourceList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDSOURCE( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDSOURCE( iteratorName )		( iteratorName != requiredSourceList.end() )
			/** REQUIREDSOURCE_MUST_EXIST( elementName )				Check that the Required Format named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDSOURCE_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, requiredSourceList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredSourceList, elementName) }
			/** REQUIREDSOURCE_MUST_NOT_EXIST( elementName )			Check that the Required Format named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDSOURCE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CoreGL::ShaderSource, requiredSourceList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredSourceList, elementName) }

			/** CONST_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName, elementName )	Get a constant iterator on the Required Geometry named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName, elementName )	__CONST_ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, requiredGeometryList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDGEOMETRY( iteratorName )		( iteratorName != requiredGeometryList.end() )
			/** REQUIREDGEOMETRY_MUST_EXIST( elementName )				Check that the Required Geometry named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDGEOMETRY_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requiredGeometryList, elementName) }
			/** REQUIREDGEOMETRY_MUST_NOT_EXIST( elementName )			Check that the Required Geometry named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDGEOMETRY_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(std::list<Glip::CorePipeline::GeometryModel>, requiredGeometryList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredGeometryList, elementName) }

			/** CONST_ITERATOR_TO_REQUIREDPIPELINE( iteratorName, elementName )	Get a constant iterator on the Required Pipeline named elementName. **/
			#define CONST_ITERATOR_TO_REQUIREDPIPELINE( iteratorName, elementName )	__CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, requiredPipelineList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_REQUIREDPIPELINE( iteratorName )			Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_REQUIREDPIPELINE( iteratorName )		( iteratorName != requiredPipelineList.end() )
			/** REQUIREDPIPELINE_MUST_EXIST( elementName )				Check that the Required Pipeline named elementName must exist (raise an exception otherwise). **/
			#define REQUIREDPIPELINE_MUST_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, requirePipelineList, elementName) }
			/** REQUIREDPIPELINE_MUST_NOT_EXIST( elementName )			Check that the Required Pipeline named elementName must not exist (raise an exception otherwise). **/
			#define REQUIREDPIPELINE_MUST_NOT_EXIST( elementName )			{ __CONST_ITERATOR_FIND(Glip::CorePipeline::PipelineLayout, requiredPipelineList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, requiredPipelineList, elementName) }
		
			/** CONST_ITERATOR_TO_MODULE( iteratorName, elementName )		Get a constant iterator on the LayoutLoaderModule named elementName. **/
			#define CONST_ITERATOR_TO_MODULE( iteratorName, elementName )		__CONST_ITERATOR_FIND(Glip::Modules::LayoutLoaderModule*, moduleList, iteratorName, elementName)
			/** VALID_ITERATOR_TO_MODULE( iteratorName )				Test if the iterator is valid (returns true or false).**/
			#define VALID_ITERATOR_TO_MODULE( iteratorName )			( iteratorName != moduleList.end() )
			/** MODULE_MUST_EXIST( elementName )					Check that the LayoutLoaderModule named elementName must exist (raise an exception otherwise). **/
			#define MODULE_MUST_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::Modules::LayoutLoaderModule*, moduleList, iteratorName, elementName) __ELEMENT_MUST_BE_IN(iteratorName, moduleList, elementName) }
			/** MODULE_MUST_NOT_EXIST( elementName )				Check that the LayoutLoaderModule named elementName must not exist (raise an exception otherwise). **/
			#define MODULE_MUST_NOT_EXIST( elementName )				{ __CONST_ITERATOR_FIND(Glip::Modules::LayoutLoaderModule*, moduleList, iteratorName, elementName) __ELEMENT_MUST_NOT_BE_IN(iteratorName, moduleList, elementName) }

	
			/** CAST_ARGUMENT( argID, type, varName ) 				Cast the argument arguments[argID] to some type (and create the variable varName). Raise an exception if the cast fails. **/
			#define CAST_ARGUMENT( argID, type, varName ) 				type varName; if(!fromString(arguments[ argID ], varName)) throw Exception("Unable to cast argument " + toString( argID ) + " (\"" + arguments[argID] + "\") to " + #type + ".", sourceName, startLine, Exception::ClientScriptException);

		// Basic Modules : 
			LAYOUT_LOADER_MODULE_DEFINITION( IF_MODULE_AVAILABLE )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_SOURCE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_GEOMETRY_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FILTERLAYOUT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_PIPELINELAYOUT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDFORMAT_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDSOURCE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDGEOMETRY_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_REQUIREDPIPELINE_DEFINED )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_CHANGE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_INCREASE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_DECREASE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SCALE_SIZE )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_INVSCALE_SIZE )
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
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_SWAP_DIMENSIONS )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_SETTING_MATCH )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_FORMAT_SETTING_LARGERTHAN)	
			LAYOUT_LOADER_MODULE_DEFINITION( OVERRIDE_REQUIRED_FORMAT )
			LAYOUT_LOADER_MODULE_DEFINITION( GENERATE_SAME_SIZE_2D_GRID )
			LAYOUT_LOADER_MODULE_DEFINITION( GENERATE_SAME_SIZE_3D_GRID )
			LAYOUT_LOADER_MODULE_DEFINITION( CHAIN_PIPELINES )
			LAYOUT_LOADER_MODULE_DEFINITION( FORMAT_TO_CONSTANT )
			LAYOUT_LOADER_MODULE_DEFINITION( SINGLE_FILTER_PIPELINE )
			LAYOUT_LOADER_MODULE_DEFINITION( IF_GLSL_VERSION_MATCH )
			LAYOUT_LOADER_MODULE_DEFINITION( ABORT_ERROR )
	}
}

#endif

