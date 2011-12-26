/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : LayoutLoader.hpp                                                                         **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Layout Loader from files or strings                                                      **/
/**                                                                                                             **/
/*****************************************************************************************************************/


#ifndef __LAYOUT_LOADER_INCLUDE__
#define __LAYOUT_LOADER_INCLUDE__

	#include <map>
	#include "../Core/HdlTexture.hpp"
	#include "../Core/ShaderSource.hpp"
	#include "../Core/Filter.hpp"
	#include "../Core/Pipeline.hpp"

namespace Glip
{
	using namespace CoreGL;
	using namespace CorePipeline;

	namespace Modules
	{
		enum LoaderKeyword
		{
			FORMAT_LAYOUT,
			SHADER_SOURCE,
			FILTER_LAYOUT,
			PIPELINE_LAYOUT,
			PIPELINE_MAIN,
			INCLUDE_FILE,
			FILTER_INSTANCE,
			PIPELINE_INSTANCE,
			CONNECTION,
			INPUT_PORTS,
			OUTPUT_PORTS,
			THIS,
			NumKeywords
		};

		class LayoutLoader
		{
			private :
				static const char* 				keywords[NumKeywords];
				std::vector<LoaderKeyword> 			entryType;
				std::vector<std::string> 			entryName;
				std::vector<std::string>			entryCode;
				std::map<std::string,HdlTextureFormat*> 	formatList;
				std::map<std::string,ShaderSource*> 		sourceList;
				std::map<std::string,FilterLayout*> 		filterList;
				std::map<std::string,PipelineLayout*> 		pipelineList;

				LoaderKeyword 			getKeyword(const std::string& str);
				void				removeCommentary(std::string& source, std::string start, std::string end);
				std::string 			getSource(const std::string& sourceName);
				std::vector<std::string> 	getArguments(const std::string& code);
				std::string 			getBody(const std::string& code);
				void				updateEntriesLists(const std::string& sourceName, bool slave=false);
				HdlTextureFormat*		buildFormat(const std::string& code, const std::string& name);
				ShaderSource*			buildShaderSource(const std::string& code, const std::string& name);
				FilterLayout* 			buildFilter(const std::string& code, const std::string& name);
				PipelineLayout*			buildPipeline(std::string code, const std::string& name);
				void 				clean(void);
			public :
				LayoutLoader(void);
				~LayoutLoader(void);
				PipelineLayout* operator()(const std::string& source); //can be a file or directly the source
		};
	}
}

#endif // LAYOUTLOADER_HPP_INCLUDED
