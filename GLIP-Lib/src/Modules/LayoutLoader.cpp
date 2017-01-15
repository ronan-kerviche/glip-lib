/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : LayoutLoader.cpp                                                                          */
/*     Original Date : December 22th 2011                                                                        */
/*                                                                                                               */
/*     Description   : Layout Loader from files or strings.                                                      */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    LayoutLoader.cpp
 * \brief   Layout Loader from files or strings.
 * \author  R. KERVICHE
 * \date    December 22th 2011
**/

	// Includes :
	#include <sstream>
	#include <algorithm>
	#include "Core/Exception.hpp"
	#include "Modules/LayoutLoader.hpp"
	#include "Modules/UniformsLoader.hpp"
	#include "devDebugTools.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

	// Static variable :
	const char* Glip::Modules::LayoutLoader::keywords[LL_NumKeywords] =  {	"TRUE",
										"FALSE",
										"TEXTURE_FORMAT",
										"SOURCE",
										"FILTER_LAYOUT",
										"PIPELINE_LAYOUT",
										"PIPELINE_MAIN",
										"INCLUDE",
										"FILTER_INSTANCE",
										"PIPELINE_INSTANCE",
										"CONNECTION",
										"INPUT_PORTS",
										"OUTPUT_PORTS",
										"THIS",
										"REQUIRED_FORMAT",
										"REQUIRED_SOURCE",
										"REQUIRED_GEOMETRY",
										"REQUIRED_PIPELINE",
										"INSERT",
										"GEOMETRY",
										"GRID_2D",
										"GRID_3D",
										"CUSTOM_MODEL",
										"STANDARD_QUAD",
										"QUAD",
										"VERTEX",
										"ELEMENT",
										"ADD_PATH",
										"CALL",
										"SAFE_CALL",
										"UNIQUE"
									};

// LayoutLoader
	/**
	\fn LayoutLoader::LayoutLoader(void)
	\brief LayoutLoader constructor.
	**/
	LayoutLoader::LayoutLoader(void)
	 : isSubLoader(false)
	{
		clearPaths();
	}

	LayoutLoader::LayoutLoader(const LayoutLoader& master)
	 : isSubLoader(true)
	{
		// Copy static data : 
		staticPaths		= master.staticPaths;
		uniqueList		= master.uniqueList;
		requiredFormatList 	= master.requiredFormatList;	
		requiredSourceList	= master.requiredSourceList;
		requiredGeometryList 	= master.requiredGeometryList;
		requiredPipelineList 	= master.requiredPipelineList;
		modules			= master.modules;

		// Copy this path to the inner version :
		dynamicPaths 		= master.dynamicPaths;

		// Copy dynamic data into the static ones too (note that elements having the same name as a required element will be overwritten) : 
		requiredFormatList.insert(	master.formatList.begin(), 	master.formatList.end());
		requiredSourceList.insert(	master.sourceList.begin(),	master.sourceList.end());
		requiredGeometryList.insert(	master.geometryList.begin(),	master.geometryList.end());
		requiredPipelineList.insert(	master.pipelineList.begin(),	master.pipelineList.end());
	}

	LayoutLoader::~LayoutLoader(void)
	{
		// Dynamic :
		clean();

		// And static :
		staticPaths.clear();
		requiredFormatList.clear();
		requiredSourceList.clear();
		requiredGeometryList.clear();
		requiredPipelineList.clear();

		// Delete all modules if this is a root loader : 
		if(!isSubLoader)
		{
			for(std::map<std::string, LayoutLoaderModule*>::iterator it=modules.begin(); it!=modules.end(); it++)
				delete it->second;
		}
		modules.clear();
	}

	void LayoutLoader::clean(void)
	{
		// Dynamic only :
		currentPath.clear();
		dynamicPaths.clear();
		associatedKeyword.clear();
		uniqueList.clear();
		formatList.clear();
		sourceList.clear();
		geometryList.clear();
		filterList.clear();
		pipelineList.clear();
	}	

	void LayoutLoader::classify(const std::vector<VanillaParserSpace::Element>& elements, std::vector<LayoutLoaderKeyword>& associatedKeywords) const
	{
		associatedKeywords.clear();
		for(std::vector<VanillaParserSpace::Element>::const_iterator it = elements.begin(); it!=elements.end(); it++)
			associatedKeywords.push_back( getKeyword( (*it).strKeyword ) );
	}

	std::pair<std::string, std::string> LayoutLoader::splitPath(const std::string& filename) const
	{
		const size_t section = filename.find_last_of("/");
		return std::pair<std::string, std::string>(filename.substr(0, section+1), filename.substr(section+1));
	}

	bool LayoutLoader::fileExists(const std::string& filename, std::string& source, const bool test) const
	{
		std::ifstream file;
		file.open(filename.c_str());
		if(file.is_open() && file.good() && !file.fail())
		{
			std::string 	buffer,	
					line;
			file.seekg(0, std::ios::beg);
			while(std::getline(file,line))
			{
				buffer += line;
				buffer += "\n";
			}

			file.close();

			if(test)
				return source!=buffer;
			else
			{
				source = buffer;
				return true;
			}
		}
		else
			return false;
	}

	void LayoutLoader::loadFile(const std::string& filename, std::string& content, std::string& usedPath) const
	{
		bool oneLoaded = false;
		const std::pair<std::string, std::string> splittedPath = splitPath(filename);
		std::string source;
		std::vector<std::string> possiblePaths;
		// Blank path :
		if(fileExists(filename, source, oneLoaded))
		{
			possiblePaths.push_back(splittedPath.first);
			oneLoaded = true;
		}	
		// From dynamic path (which already include static path and currentPath) :
		for(std::set<std::string>::const_iterator it=dynamicPaths.begin(); it!=dynamicPaths.end(); it++)
		{
			if(fileExists(*it + filename, source, oneLoaded))
			{
				possiblePaths.push_back(*it + splittedPath.first);
				oneLoaded = true;
			}
			// Relative :
			if(!it->empty() && (*it)[0]=='.' && fileExists(currentPath + (*it) + filename, source, oneLoaded))
			{
				possiblePaths.push_back(currentPath + *it + splittedPath.first);
			}
		}

		if(possiblePaths.empty())
		{
			if(dynamicPaths.empty())
				throw Exception("Unable to load file \"" + filename + "\" from the current location.", "", 1, Exception::ClientScriptException);
			else
			{			
				Exception ex("Unable to load file \"" + filename + "\" from the following locations : ", "", 1, Exception::ClientScriptException);
				for(std::set<std::string>::const_iterator it=dynamicPaths.begin(); it!=dynamicPaths.end(); it++)
				{
					if(it->empty())
						ex << Exception("-> [./]", "", 1, Exception::ClientScriptException);
					else
						ex << Exception("-> " + *it, "", 1, Exception::ClientScriptException);
				}		
				throw ex;
			}
		}
		else if(possiblePaths.size()>1)
		{
			Exception ex("Ambiguous link : file \"" + filename + "\" was found in multiple locations, with different content : ", "", 1, Exception::ClientScriptException);
			for(std::vector<std::string>::const_iterator it=possiblePaths.begin(); it!=possiblePaths.end(); it++)
				ex << Exception("-> " + *it, "", 1, Exception::ClientScriptException);
			throw ex;
		}
		else
		{
			usedPath = possiblePaths.front();
			content.clear();
			content = source;
		}
	}

	void LayoutLoader::preliminaryTests(const VanillaParserSpace::Element& e, char nameProperty, int minArguments, int maxArguments, char bodyProperty, const std::string& objectName)
	{
		// xxxProperty :
		//	1  : Must have.
		//	0  : Indifferent.
		//	-1 : Must not have.

		// Generate help :
		std::string nameDecorator;

		if(!e.name.empty())
			nameDecorator = ", with name \"" + e.name + "\",";

		// Tests :
		if((e.noName || e.name.empty()) && nameProperty>0)
			throw Exception(objectName + " does not have a name.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else if((!e.noName || !e.name.empty()) && nameProperty<0)
			throw Exception(objectName + nameDecorator + " should not have a name.", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(!e.name.empty() && (LayoutLoader::getKeyword(e.name)!=LL_UnknownKeyword || UniformsLoader::getKeyword(e.name)!=UL_UnknownKeyword))
			throw Exception(objectName + nameDecorator + " has an illegal name.", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(maxArguments==0 && !e.arguments.empty())
			throw Exception(objectName + nameDecorator + " should not have arguments, but it has " + toString(e.arguments.size()) + ".", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(minArguments>0 && e.arguments.empty())
			throw Exception(objectName + nameDecorator + " should have at least " + toString(minArguments) + " argument(s).", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(static_cast<int>(e.arguments.size())!=minArguments && minArguments==maxArguments && minArguments>=0)
			throw Exception(objectName + nameDecorator + " should have exactly "  + toString(minArguments) + " argument(s).", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(static_cast<int>(e.arguments.size())<minArguments && minArguments>=0)
			throw Exception(objectName + nameDecorator + " should have at least " + toString(minArguments) + " argument(s), but it has only " + toString(e.arguments.size()) + ".", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(static_cast<int>(e.arguments.size())>maxArguments && maxArguments>=0)
			throw Exception(objectName + nameDecorator + " should have at most " + toString(maxArguments) + " argument(s), but it has " + toString(e.arguments.size()) + ".", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(e.noBody && bodyProperty>0)
			throw Exception(objectName + nameDecorator + " does not have a body.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else if(!e.noBody && bodyProperty<0)
			throw Exception(objectName + nameDecorator + " should not have a body.", e.sourceName, e.startLine, Exception::ClientScriptException);
	}

	ShaderSource LayoutLoader::enhanceShaderSource(const std::string& str, const std::string& sourceName, int startLine)
	{
		const std::string keyword = keywords[KW_LL_INSERT],
				  glslPragma = "#pragma";
	
		std::vector<std::string> lines;
		std::vector<ShaderSource::LineInfo> infos;
		ShaderSource tmp(str, sourceName, startLine);
		
		for(int k=1; k<=tmp.getNumLines(); k++)
		{
			lines.push_back(tmp.getLine(k, false));
			infos.push_back(tmp.getLineInfo(k));
		}

		for(size_t k=0; k<lines.size(); )
		{
			// Test if the current line contain the keyword :
			const std::string& currentLine = lines[k];
			const ShaderSource::LineInfo& currentInfo = infos[k];

			if(currentLine.find(keyword)!=std::string::npos)
			{
				VanillaParser parser(currentLine, currentInfo.sourceName, currentInfo.lineNumber);
				
				if(parser.elements.empty()) // The INSERT might be commented out.
				{
					k++;
					continue;
				}
				else if(parser.elements.size()>2)
					throw Exception("Multiple elements found during parsing, instead of one " + keyword + ".", currentInfo.sourceName, currentInfo.lineNumber, Exception::ClientScriptException);
				else if(parser.elements.size()==2 && (parser.elements.front().strKeyword!=glslPragma || parser.elements.back().strKeyword!=keyword))
					throw Exception("Bad keyword found : \"" + parser.elements.front().strKeyword + " " + parser.elements.back().strKeyword + "\" instead of \"" + glslPragma + " " + keyword + "\".", currentInfo.sourceName, currentInfo.lineNumber, Exception::ClientScriptException);
				else if(parser.elements.size()==1 && parser.elements.front().strKeyword!=keyword)
					throw Exception("Bad keyword found : \"" + parser.elements.front().strKeyword + "\" instead of " + keyword + ".", currentInfo.sourceName, currentInfo.lineNumber, Exception::ClientScriptException);
				
				// Continue the tests :
				preliminaryTests(parser.elements.back(), -1, 1, 1, -1, "Insert");
				
				const std::string sourceName = parser.elements.back().arguments[0];

				// Get it from the list :
				std::map<std::string, ShaderSource>::iterator it = sourceList.find(sourceName); 	
				if(it==sourceList.end())
				{
					it = requiredSourceList.find(sourceName);
					if(it==requiredSourceList.end())
						throw Exception("Source object \"" + sourceName + "\" is not referenced.", currentInfo.sourceName, currentInfo.lineNumber, Exception::ClientScriptException);
				}			

				// Else, generate the lists :
				std::vector<std::string> 		sharedLines;
				std::vector<ShaderSource::LineInfo>	sharedInfos;

				for(int l=1; l<=it->second.getNumLines(); l++)
				{
					sharedLines.push_back(it->second.getLine(l, false));
					sharedInfos.push_back(it->second.getLineInfo(l));
				}

				// Remove and insert :
				lines.erase(lines.begin()+k);
				infos.erase(infos.begin()+k);
				lines.insert(lines.begin()+k, sharedLines.begin(), sharedLines.end());
				infos.insert(infos.begin()+k, sharedInfos.begin(), sharedInfos.end());

				// Do not change k and check if the inserted lines also require INSERT.
			}
			else
				k++;
		}

		// Generate the final piece :
		std::string content;
		std::map<int, ShaderSource::LineInfo> linesInfo;
		for(size_t k=0; k<lines.size(); k++)
		{
			content += lines[k] + '\n';
			linesInfo[k+1] = infos[k];
		}

		return ShaderSource(content, sourceName, startLine, linesInfo);
	}

	void LayoutLoader::append(LayoutLoader& subLoader)
	{
		// Test for doubles :
		#define TEST_FOR_DOUBLES( varName, typeName, type) \
			for(std::map<std::string, type >::iterator it = subLoader.varName.begin(); it!=subLoader.varName.end(); it++ ) \
			{ \
				if( varName.find( it->first ) != varName.end() ) \
					throw Exception("The " + std::string(typeName) +  " \"" + it->first + "\" already exists in current script.", __FILE__, __LINE__, Exception::ClientScriptException); \
			} \

			TEST_FOR_DOUBLES( formatList,		"Format", 		HdlTextureFormat)
			TEST_FOR_DOUBLES( sourceList,		"ShaderSource", 	ShaderSource)
			TEST_FOR_DOUBLES( geometryList,		"Geometry",		GeometryModelList)
			TEST_FOR_DOUBLES( filterList,		"FilterLayout", 	FilterLayout)
			TEST_FOR_DOUBLES( pipelineList,		"PipelineLayout",	PipelineLayout)

		#undef TEST_FOR_DOUBLES

		// Append :
		#define INSERTION( item ) item.insert( subLoader.item.begin(), subLoader.item.end() )

			INSERTION( formatList );
			INSERTION( sourceList );
			INSERTION( geometryList );
			INSERTION( filterList );
			INSERTION( pipelineList );

		#undef INSERTION

		// Others :
		dynamicPaths.insert(subLoader.dynamicPaths.begin(), subLoader.dynamicPaths.end());
		uniqueList.insert(subLoader.uniqueList.begin(), subLoader.uniqueList.end());	
	}

	void LayoutLoader::appendPath(const VanillaParserSpace::Element& e)
	{
		preliminaryTests(e, -1, 1, 1, -1, "AppendPath");
		std::string resultingPath = currentPath + e.arguments[0];
		if(e.arguments[0].empty())
			throw Exception("Path is empty.", e.sourceName, e.startLine, Exception::ClientScriptException);
		// Force delimiter :
		if(resultingPath[resultingPath.size()-1]!='/')
			resultingPath.push_back('/');
		// Test if it is already in the dynamic path list and if not, add it (via sdt::set) :
		dynamicPaths.insert(resultingPath);
	}

	void LayoutLoader::includeFile(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, -1, 1, 1, -1, "IncludeFile");

		if(e.arguments[0].find('\n')!=std::string::npos)
			throw Exception("Cannot include file \"" + e.arguments[0] + "\" because its name contains a newline character.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else if(e.arguments[0].find('\\')!=std::string::npos)
			throw Exception("Cannot include file \"" + e.arguments[0] + "\" because its name contains the illegal character '\\'.", e.sourceName, e.startLine, Exception::ClientScriptException);

		LayoutLoader subLoader(*this); // 'this' is the master pipeline in that case.

		// Load the file :
		std::string content;
		std::pair<std::string, std::string> splittedPath = splitPath(e.arguments[0]);
		loadFile(e.arguments[0], content, subLoader.currentPath);
		std::string filename = subLoader.currentPath + e.arguments[0];
		// Add the loading path :
		if(!subLoader.currentPath.empty())
			subLoader.dynamicPaths.insert(subLoader.currentPath);
		try
		{
			// Build all the elements :
			std::string dummyMainPipelineName;
			subLoader.process(content, dummyMainPipelineName, filename);
			// Append :
			append(subLoader);
		}
		catch(Exception& ex)
		{
			if(!subLoader.currentPath.empty())
			{
				Exception m("Exception caught while loading file \"" + splittedPath.second + "\" (path : \"" + subLoader.currentPath + "\") : ", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
			else
			{
				Exception m("Exception caught while loading file \"" + splittedPath.second + "\" : ", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}
	}

	bool LayoutLoader::checkUnique(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, -1, 1, 1, -1, "Unique");
		return uniqueList.insert(e.arguments[0]).second;
	}

	void LayoutLoader::buildRequiredFormat(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, 10, -1, "RequiredFormat");

		// Identify the target :
		std::map<std::string, HdlTextureFormat>::iterator it = requiredFormatList.find(e.arguments[0]);
		if(it==requiredFormatList.end())
		{
			// Try in the current format list also :
			it = formatList.find(e.arguments[0]);
			if(it==formatList.end())
				throw Exception("The required format \"" + e.arguments[0] + "\" was not found.", e.sourceName, e.startLine, Exception::ClientScriptException);
		}

		if(formatList.find(e.name)!=formatList.end())
			throw Exception("A Format Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Check for possible arguments, to modify the texture :
		// Get the data :
		int w, h, mipmap = 0;
		GLenum mode, depth, minFilter, magFilter, sWrap, tWrap;
		if(e.arguments.size()>1)
		{
			if(e.arguments[1]=="*")
				w = it->second.getWidth();
			else if(!fromString(e.arguments[1], w))
				throw Exception("Cannot read width for format \"" + e.name + "\". Token : \"" + e.arguments[0] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		}
		else
			w = it->second.getWidth();

		if(e.arguments.size()>2)
		{
			if(e.arguments[2]=="*")
				h = it->second.getHeight();
			else if(!fromString(e.arguments[2], h))
				throw Exception("Cannot read height for format \"" + e.name + "\". Token : \"" + e.arguments[0] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		}
		else
			h = it->second.getHeight();

		if(e.arguments.size()>3)
		{
			if(e.arguments[3]=="*")
				mode = it->second.getGLMode();
			else
				mode = getGLEnum(e.arguments[3]);
		}
		else
			mode = it->second.getGLMode();

		if(e.arguments.size()>4)
		{
			if(e.arguments[4]=="*")
				depth = it->second.getGLDepth();
			else
				depth = getGLEnum(e.arguments[4]);
		}
		else
			depth = it->second.getGLDepth();

		if(e.arguments.size()>5)
		{
			if(e.arguments[5]=="*")
				minFilter = it->second.getMinFilter();
			else
				minFilter = getGLEnum(e.arguments[5]);
		}
		else
			minFilter = it->second.getMinFilter();

		if(e.arguments.size()>6)
		{
			if(e.arguments[6]=="*")
				magFilter = it->second.getMagFilter();
			else
				magFilter = getGLEnum(e.arguments[6]);
		}
		else
			magFilter = it->second.getMagFilter();

		if(e.arguments.size()>7)
		{
			if(e.arguments[7]=="*")
				sWrap = it->second.getSWrapping();
			else
				sWrap = getGLEnum(e.arguments[7]);
		}
		else
			sWrap = it->second.getSWrapping();

		if(e.arguments.size()>8)
		{
			if(e.arguments[8]=="*")
				tWrap = it->second.getTWrapping();
			else
				tWrap = getGLEnum(e.arguments[8]);
		}
		else
			tWrap = it->second.getTWrapping();

		if(e.arguments.size()>9)
		{
			if(e.arguments[9]=="*")
				mipmap = it->second.getMaxLevel();
			else if(!fromString(e.arguments[9], mipmap))
				throw Exception("Cannot read mipmap for format \"" + e.name + "\". Token : \"" + e.arguments[8] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		}
		else
			mipmap = it->second.getMaxLevel();

		// Find possible errors :
		if(mode==GL_FALSE)
			throw Exception("Cannot read mode for format \"" + e.name + "\". Token : \"" + e.arguments[2] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(depth==GL_FALSE)
			throw Exception("Cannot read depth for format \"" + e.name + "\". Token : \"" + e.arguments[3] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(minFilter==GL_FALSE)
			throw Exception("Cannot read MinFilter for format \"" + e.name + "\". Token : \"" + e.arguments[4] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(magFilter==GL_FALSE)
			throw Exception("Cannot read MagFilter for format \"" + e.name + "\". Token : \"" + e.arguments[5] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(sWrap==GL_FALSE)
			throw Exception("Cannot read SWrapping for format \"" + e.name + "\". Token : \"" + e.arguments[6] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(sWrap==GL_FALSE)
			throw Exception("Cannot read TWrapping for format \"" + e.name + "\". Token : \"" + e.arguments[7] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(mipmap<0)
			throw Exception("Mipmap cannot be negative for format \"" + e.name + "\". Token : \"" + e.arguments[4] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Create and push (note the 0 base mipmap) :
		formatList.insert( std::pair<std::string, HdlTextureFormat>( e.name, HdlTextureFormat(w, h, mode, depth, minFilter, magFilter, sWrap, tWrap, 0, mipmap) ) );
	}

	void LayoutLoader::buildRequiredSource(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, 1, -1, "RequiredSource");

		// Identify the target :
		std::map<std::string, ShaderSource>::iterator it = requiredSourceList.find(e.arguments[0]);
		if(it==requiredSourceList.end())
		{
			// Try in the current source list also :
			it = sourceList.find(e.arguments[0]);

			if(it==sourceList.end())
				throw Exception("The required source \"" + e.arguments[0] + "\" was not found.", e.sourceName, e.startLine, Exception::ClientScriptException);
		}

		if(sourceList.find(e.name)!=sourceList.end())
			throw Exception("A ShaderSource Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else
			sourceList.insert( std::pair<std::string, ShaderSource>(e.name, it->second) );
	}

	void LayoutLoader::buildRequiredGeometry(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, 1, -1, "RequiredGeometry");

		// Identify the target :
		std::map<std::string, GeometryModelList>::iterator it = requiredGeometryList.find(e.arguments[0]);
		if(it==requiredGeometryList.end())
		{
			// Try in the current geometry list also :
			it = geometryList.find(e.arguments[0]);

			if(it==geometryList.end())
				throw Exception("The required geometry \"" + e.arguments[0] + "\" was not found.", e.sourceName, e.startLine, Exception::ClientScriptException);
		}

		if(geometryList.find(e.name)!=geometryList.end())
			throw Exception("A GeometryModel Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else
			geometryList.insert( std::pair<std::string, GeometryModelList>(e.name, it->second) );
	}

	void LayoutLoader::buildRequiredPipeline(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, 1, -1, "RequiredPipeline");

		// Identify the target :
		std::map<std::string, PipelineLayout>::iterator it = requiredPipelineList.find(e.arguments[0]);
		if(it==requiredPipelineList.end())
		{
			// Try in the current pipeline list also :
			it = pipelineList.find(e.arguments[0]);

			if(it==pipelineList.end())
				throw Exception("The required pipeline \"" + e.arguments[0] + "\" was not found.", e.sourceName, e.startLine, Exception::ClientScriptException);
		}

		if(pipelineList.find(e.name)!=pipelineList.end())
			throw Exception("A PipelineLayout Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);
		else
			pipelineList.insert( std::pair<std::string, PipelineLayout>(e.name, it->second) );
	}

	void LayoutLoader::moduleCall(const VanillaParserSpace::Element& e, std::string& mainPipelineName, const bool safe)
	{
		// Find the name of the module : 
		preliminaryTests(e, 1, -1, -1, 0, "ModuleCall");

		std::map<std::string, LayoutLoaderModule*>::iterator it = modules.find( e.name );
		if(it==modules.end())
		{
			if(!safe)
				throw Exception("The module \"" + e.name + "\" was not loaded.", e.sourceName, e.startLine, Exception::ClientScriptException);
			else
				return ;
		}

		try
		{
			LayoutLoaderModule& module = *(it->second);

			// Re - test call : 
			preliminaryTests(e, 1, module.getMinNumArguments(), module.getMaxNumArguments(), module.bodyPresenceTest(), "Module \"" + module.getName() + "\"");

			// Make the call : 
			std::string 	_mainPipelineName = mainPipelineName,
					subExecution,
					subExecutionSourceName;
			int		subExecutionStartLine = 1;
			module.apply(	e.arguments, 
					e.body,	
					currentPath, 
					dynamicPaths,
					formatList, 
					sourceList,
					geometryList,
					filterList,
					pipelineList,
					_mainPipelineName, 
					staticPaths,
					requiredFormatList,
					requiredSourceList,
					requiredGeometryList,
					requiredPipelineList,
					modules,
					e.sourceName,
					e.startLine,
					e.bodyLine,
					subExecution,
					subExecutionSourceName,
					subExecutionStartLine);

			// Test if the main pipeline name is now different
			if(_mainPipelineName!=mainPipelineName && !mainPipelineName.empty())
				throw Exception("A main pipeline (named \"" + mainPipelineName + "\") was already defined.", e.sourceName, e.startLine, Exception::ClientScriptException);
			else
				mainPipelineName = _mainPipelineName;

			if(!subExecution.empty())
			{
				// Create a sub-loader of this :
				LayoutLoader subLoader(*this);

				// Copy this path to the inner version :
				subLoader.dynamicPaths = dynamicPaths;
				try
				{
					// Build all the elements :
					std::string dummyMainPipelineName;
					subLoader.process(subExecution, dummyMainPipelineName, subExecutionSourceName, subExecutionStartLine);

					// Append :
					append(subLoader);
				}
				catch(Exception& ex)
				{
					Exception m("Exception caught while processing code generated by Module \"" + e.name + "\" : ", e.sourceName, e.startLine, Exception::ClientScriptException);
					m << ex; 
					m << Exception("Generated code : \n" + subExecution, e.sourceName, e.startLine, Exception::ClientScriptException);
					throw m;
				}
			}
		}
		catch(Exception& ex)
		{
			Exception m("From line " + toString(e.startLine) + " : The module \"" + e.name + "\" reported an error : ", e.sourceName, e.startLine, Exception::ClientScriptException);
			m << ex;
			throw m;	
		}
	}

	void LayoutLoader::buildFormat(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 4, 9, -1, "Format");

		// Get the data :
		int w, h, mipmap = 0;
		GLenum mode, depth, minFilter, magFilter, sWrap, tWrap;

		if(!fromString(e.arguments[0], w))
			throw Exception("Cannot read width for format \"" + e.name + "\". Token : \"" + e.arguments[0] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(!fromString(e.arguments[1], h))
			throw Exception("Cannot read height for format \"" + e.name + "\". Token : \"" + e.arguments[1] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		mode  = getGLEnum(e.arguments[2]);
		depth = getGLEnum(e.arguments[3]);

		if(e.arguments.size()>4)
			minFilter = getGLEnum(e.arguments[4]);
		else
			minFilter = GL_NEAREST;

		if(e.arguments.size()>5)
			magFilter = getGLEnum(e.arguments[5]);
		else
			magFilter = GL_NEAREST;

		if(e.arguments.size()>6)
			sWrap = getGLEnum(e.arguments[6]);
		else
			sWrap = GL_CLAMP_TO_EDGE;

		if(e.arguments.size()>7)
			tWrap = getGLEnum(e.arguments[7]);
		else
			tWrap = GL_CLAMP_TO_EDGE;

		if(e.arguments.size()>8)
		{
			if(!fromString(e.arguments[8], mipmap))
				throw Exception("Cannot read mipmap for format \"" + e.name + "\". Token : \"" + e.arguments[8] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		}
		else
			mipmap = 0;

		// Find possible errors :
		if(mode==GL_FALSE)
			throw Exception("Cannot read mode for format \"" + e.name + "\". Token : \"" + e.arguments[2] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(depth==GL_FALSE)
			throw Exception("Cannot read depth for format \"" + e.name + "\". Token : \"" + e.arguments[3] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(minFilter==GL_FALSE)
			throw Exception("Cannot read MinFilter for format \"" + e.name + "\". Token : \"" + e.arguments[4] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(magFilter==GL_FALSE)
			throw Exception("Cannot read MagFilter for format \"" + e.name + "\". Token : \"" + e.arguments[5] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(sWrap==GL_FALSE)
			throw Exception("Cannot read SWrapping for format \"" + e.name + "\". Token : \"" + e.arguments[6] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(sWrap==GL_FALSE)
			throw Exception("Cannot read TWrapping for format \"" + e.name + "\". Token : \"" + e.arguments[7] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(mipmap<0)
			throw Exception("Mipmap cannot be negative for format \"" + e.name + "\". Token : \"" + e.arguments[4] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(formatList.find(e.name)!=formatList.end())
			throw Exception("A Format Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Create and push (note the 0 base mipmap) :
		formatList.insert( std::pair<std::string, HdlTextureFormat>( e.name, HdlTextureFormat(w, h, mode, depth, minFilter, magFilter, sWrap, tWrap, 0, mipmap) ) );
	}

	void LayoutLoader::buildSource(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 0, 1, 0, "Source");

		// Complementary tests :
		if(e.arguments.size()>0 && !e.noBody)
			throw Exception("Source \"" + e.name + "\" cannot have both argument(s) and a body.", e.sourceName, e.startLine, Exception::ClientScriptException);
		if(e.arguments.empty() && e.noBody)
			throw Exception("Source \"" + e.name + "\" must have either an argument (filename) or a body (and not both).", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(formatList.find(e.name)!=formatList.end())
			throw Exception("A Source Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Load data :
		if(e.noBody)
		{
			std::string usedPath;

			try
			{
				std::string content;

				// Custom load :
				loadFile(e.arguments[0], content, usedPath);
				const std::string filename = usedPath + e.arguments[0];

				std::map<int, ShaderSource::LineInfo> extrasInfo;
				ShaderSource s = enhanceShaderSource(content, filename);

				sourceList.insert( std::pair<std::string, ShaderSource>( e.name, s) );
			}
			catch(Exception& ex)
			{
				Exception m("An exception was caught while building ShaderSource \"" + e.name + "\" (loading from file \"" + e.arguments[0] + "\", (possibly incomplete) path : \"" + usedPath + "\") : ", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}
		else
		{
			try
			{
				std::string content = e.body;
				std::map<int, ShaderSource::LineInfo> extrasInfo;
				ShaderSource s = enhanceShaderSource(content, e.sourceName, e.bodyLine);

				sourceList.insert( std::pair<std::string, ShaderSource>( e.name, s) );
			}
			catch(Exception& ex)
			{
				Exception m("An exception was caught while building ShaderSource \"" + e.name + "\" : ", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}
	}

	void LayoutLoader::buildGeometry(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, -1, 0, "Geometry");

		// Test for duplicata :
		if(geometryList.find(e.name)!=geometryList.end())
			throw Exception("A Geometry Object with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Find the first argument :
		if(e.arguments[0]==keywords[KW_LL_STANDARD_QUAD])
		{
			if(e.arguments.size()!=1)
				throw Exception("The model \"" + std::string(keywords[KW_LL_STANDARD_QUAD]) + "\" requires to have exactly 1 argument in geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			geometryList.insert( std::pair<std::string, GeometryModelList>( e.name, GeometryModelList(1, GeometryPrimitives::StandardQuad()) ) );
		}
		else if(e.arguments[0]==keywords[KW_LL_QUAD])
		{
			if(e.arguments.size()!=5)
				throw Exception("The model \"" + std::string(keywords[KW_LL_QUAD]) + "\" requires to have exactly 5 arguments (included) in geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
			
			float 	width = 0.0,
				height = 0.0, 
				xCenter = 0.0,
				yCenter = 0.0;
			
			if(!fromString(e.arguments[1], width) || width<=0.0)
				throw Exception("Cannot read width for quad geometry \"" + e.name + "\". Token : \"" + e.arguments[1] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			if(!fromString(e.arguments[2], height) || height<=0.0)
				throw Exception("Cannot read height for quad geometry \"" + e.name + "\". Token : \"" + e.arguments[2] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			if(!fromString(e.arguments[3], xCenter))
				throw Exception("Cannot read xCenter for quad geometry \"" + e.name + "\". Token : \"" + e.arguments[3] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);
		
			if(!fromString(e.arguments[4], yCenter))
				throw Exception("Cannot read yCenter for quad geometry \"" + e.name + "\". Token : \"" + e.arguments[4] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			geometryList.insert( std::pair<std::string, GeometryModelList>( e.name, GeometryModelList(1, GeometryPrimitives::Quad(width, height, xCenter, yCenter)) ) );
		}
		else if(e.arguments[0]==keywords[KW_LL_GRID_2D])
		{
			if(e.arguments.size()!=3)
				throw Exception("The model \"" + std::string(keywords[KW_LL_GRID_2D]) + "\" requires to have exactly 3 arguments (included) in geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			int 	w = 0,
				h = 0;

			if(!fromString(e.arguments[1], w) || w<=0)
				throw Exception("Cannot read width for 2D grid geometry \"" + e.name + "\". Token : \"" + e.arguments[1] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			if(!fromString(e.arguments[2], h) || h<=0)
				throw Exception("Cannot read height for 2D grid geometry \"" + e.name + "\". Token : \"" + e.arguments[2] + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			geometryList.insert( std::pair<std::string, GeometryModelList>( e.name, GeometryModelList(1, GeometryPrimitives::PointsGrid2D(w,h)) ) );
		}
		else if(e.arguments[0]==keywords[KW_LL_GRID_3D])
		{
			if(e.arguments.size()!=4)
				throw Exception("The model \"" + std::string(keywords[KW_LL_GRID_3D]) + "\" requires to have exactly 4 arguments (included) in geometry \"" + e.name + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			int w, h, z;

			if(!fromString(e.arguments[1], w))
				throw Exception("Cannot read width for 3D grid geometry \"" + e.name + "\". Token : \"" + e.arguments[1] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			if(!fromString(e.arguments[2], h))
				throw Exception("Cannot read height for 3D grid geometry \"" + e.name + "\". Token : \"" + e.arguments[2] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			if(!fromString(e.arguments[3], z))
				throw Exception("Cannot read height for 3D grid geometry \"" + e.name + "\". Token : \"" + e.arguments[3] + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			geometryList.insert( std::pair<std::string, GeometryModelList>( e.name, GeometryModelList(1, GeometryPrimitives::PointsGrid3D(w,h,z)) ) );
		}
		else if(e.arguments[0]==keywords[KW_LL_CUSTOM_MODEL])
		{
			if(e.arguments.size()!=3 && e.arguments.size()!=4)
				throw Exception("The model \"" + std::string(keywords[KW_LL_CUSTOM_MODEL]) + "\" requires to have either 2 or 3 arguments (included) in geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			if(e.body.empty() || e.noBody)
				throw Exception("The custom model \"" + e.name + "\" does not have a body.", e.sourceName, e.startLine, Exception::ClientScriptException);

			GeometryPrimitives::CustomModel* g = NULL;

			try
			{
				GLenum primitive;

				primitive = getGLEnum(e.arguments[1]);

				bool 	hasNormals = false,
					hasTexCoords = false;

				if(e.arguments.size()>2)
					hasNormals = (e.arguments[2]==keywords[KW_LL_TRUE]);
					
				if(e.arguments.size()>3)
					hasTexCoords = (e.arguments[3]==keywords[KW_LL_TRUE]);

				// Parse the text : 
				VanillaParser parser(e.body, e.sourceName, e.bodyLine);

				// Classify the new data :
				std::vector<LayoutLoaderKeyword> associatedKeywords;

				classify(parser.elements, associatedKeywords);

				// First pass is only for vertices : 
				int pArg = 0;
				int numExpectedArguments = 0;			
				for(unsigned int k=0; k<associatedKeywords.size(); k++)
				{
					// Test :
					if(associatedKeywords[k]==KW_LL_VERTEX)
					{
						if(g==NULL)
						{
							preliminaryTests(parser.elements[k], -1, 2, 5, -1, "Vertex (sub-parsing Geometry)");

							int dim = parser.elements[k].arguments.size();
							dim = (hasTexCoords ? dim-2 : dim);
							dim = (hasNormals ? dim/2 : dim);
							g = new GeometryPrimitives::CustomModel(dim, primitive, hasNormals, hasTexCoords);

							// Save this number of arguments, always expect the same : 
							numExpectedArguments = parser.elements[k].arguments.size();
						}
						else
							preliminaryTests(parser.elements[k], -1, numExpectedArguments, numExpectedArguments, -1, "Vertex (sub-parsing Geometry)");
	
						GLfloat x = 0.0f, 
							y = 0.0f, 
							z = 0.0f,
							nx = 0.0f, 
							ny = 0.0f, 
							nz = 0.0f, 
							u = 0.0f, 
							v = 0.0f;
						pArg = 0;

						if(!fromString(parser.elements[k].arguments[pArg], x))
							throw Exception("Cannot read X for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);

						pArg++;

						if(!fromString(parser.elements[k].arguments[pArg], y))
							throw Exception("Cannot read Y for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);

						pArg++;

						if(g->dim>=3)
						{
							if(!fromString(parser.elements[k].arguments[pArg], z))
								throw Exception("Cannot read Z for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							pArg++;
						}

						if(g->hasNormals)
						{
							if(!fromString(parser.elements[k].arguments[pArg], nx))
								throw Exception("Cannot read NX for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
							pArg++;

							if(!fromString(parser.elements[k].arguments[pArg], ny))
								throw Exception("Cannot read NY for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
							pArg++;
	
							if(g->dim>=3)
							{
								if(!fromString(parser.elements[k].arguments[pArg], nz))
									throw Exception("Cannot read NZ for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
								pArg++;
							}
						}

						if(g->hasTexCoords)
						{
							if(!fromString(parser.elements[k].arguments[pArg], u))
								throw Exception("Cannot read U for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
							pArg++;

							if(!fromString(parser.elements[k].arguments[pArg], v))
								throw Exception("Cannot read V for vertex. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
							pArg++;
						}
						
						if(g->dim==2)
							g->newVertex2D(x, y, nx, ny, u, v);
						else if(g->dim==3)
							g->newVertex3D(x, y, z, nx, ny, nz, u, v);
						else
							throw Exception("Internal error - unsupported number of dimensions.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
					}
					else if(associatedKeywords[k]!=KW_LL_ELEMENT)
					{
							if(associatedKeywords[k]<LL_NumKeywords)
								throw Exception("The keyword " + std::string(keywords[associatedKeywords[k]]) + " is not allowed in a Geometry definition (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							else
								throw Exception("Unknown keyword \"" + parser.elements[k].strKeyword + "\" in a Geometry definition (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
					}
				}

				if(g==NULL)
					throw Exception("The custom model \"" + e.name + "\" does not have any vertex declared.", e.sourceName, e.startLine, Exception::ClientScriptException);

				// Second pass for elements : 
				std::vector<GLuint> indices(g->numVerticesPerElement);
				for(unsigned int k=0; k<associatedKeywords.size(); k++)
				{
					if(associatedKeywords[k]==KW_LL_ELEMENT)
					{
						preliminaryTests(parser.elements[k], -1, g->numVerticesPerElement, g->numVerticesPerElement, -1, "Element (sub-parsing Geometry)");

						// Load all the indices : 
						for(int pArg=0; pArg<g->numVerticesPerElement; pArg++)
						{
							if(!fromString(parser.elements[k].arguments[pArg], indices[pArg]))
								throw Exception("Cannot read vertex index " + toString(pArg) + " for primitive element. Token : \"" + parser.elements[k].arguments[pArg] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						}

						// Add it : 
						g->newElement(indices);
					}
				}

				// Final test : 
				if(!g->testIndices())
					throw Exception("Data parsing failed.", e.sourceName, e.startLine, Exception::ClientScriptException);

				geometryList.insert(std::pair<std::string, GeometryModelList>(e.name, GeometryModelList(1, *g)));
				delete g;
			}
			catch(Exception& ex)
			{
				delete g;
				Exception m("Exception caught while building Geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}
		else // Link all the geometries in argument :
		{
			if(geometryList.find(e.name)!=geometryList.end())
				throw Exception("A Geometry with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

			if(e.arguments.size()<=1)
				throw Exception("The model list must have two or more arguments in geometry \"" + e.name + "\".", __FILE__, __LINE__, Exception::ClientScriptException);

			GeometryModelList geometry;
			for(std::vector<std::string>::const_iterator itArg=e.arguments.begin(); itArg!=e.arguments.end(); itArg++)
			{
				// Search for the corresponding geometry :
				std::map<std::string, GeometryModelList>::const_iterator itGeometry = geometryList.find(*itArg);
				if(itGeometry==geometryList.end())
				{
					itGeometry = requiredGeometryList.find(*itArg);
					if(itGeometry==requiredGeometryList.end())
						throw Exception("No geometry model with name \"" + (*itArg) + "\" was registered and can be use in Geometry \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
				}
				// Append :
				geometry.insert(geometry.end(), itGeometry->second.begin(), itGeometry->second.end());
			}
			geometryList.insert(std::pair<std::string, GeometryModelList>(e.name, geometry));
		}
	}

	void LayoutLoader::buildFilter(const VanillaParserSpace::Element& e)
	{
		// Preliminary tests :
		preliminaryTests(e, 1, 1, 2, 0, "FilterLayout");

		// Duplicate test :	
		if(filterList.find(e.name)!=filterList.end())
			throw Exception("A Filter Layout with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);
	
		// Find the format :
		std::map<std::string, HdlTextureFormat>::iterator format = formatList.find(e.arguments[0]);
		if(format==formatList.end())
			throw Exception("No Format with name \"" + e.arguments[0] + "\" was registered and can be use in Filter \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		// Find the shaders, the geometry, the settings : 
		std::map<GLenum, ShaderSource*> shaders;
		std::map<std::string, GeometryModelList>::iterator geometryIt = geometryList.end();
		
		bool clearingSet = false;
			bool enableClearing = false;
		bool blendingSet = false;
			GLenum	blendingSFactor = GL_NONE,
				blendingDFactor = GL_NONE,
				blendingEquation = GL_NONE;
		bool depthTestingSet = false;
			GLenum depthTestingFunction = GL_NONE;

		// If the fragment source name is provided in the arguments : 
		if(e.arguments.size()>1)
		{
			std::map<std::string, ShaderSource>::iterator fragmentSource = sourceList.find(e.arguments[1]);
			if(fragmentSource==sourceList.end())
				throw Exception("No ShaderSource with name \"" + e.arguments[1] + "\" was registered and can be use in Filter \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
			else
				shaders[GL_FRAGMENT_SHADER] = &fragmentSource->second;
		}

		// Scan the body for possible other shaders, geometry and settings :
		if(!e.noBody && !e.body.empty())
		{
			try
			{
				VanillaParser parser(e.body, e.sourceName, e.bodyLine);

				// Classify :
				std::map<GLenum, bool> setParametersTest;
				setParametersTest[GL_VERTEX_SHADER]		= false;
				setParametersTest[GL_FRAGMENT_SHADER]		= (shaders.find(GL_FRAGMENT_SHADER)!=shaders.end()); // The fragment shader is not already set.
				setParametersTest[GL_COMPUTE_SHADER]		= false;
				#ifdef GLIP_USE_GL
				setParametersTest[GL_TESS_CONTROL_SHADER]	= false;
				setParametersTest[GL_TESS_EVALUATION_SHADER]	= false;
				setParametersTest[GL_GEOMETRY_SHADER]		= false;
				#endif
				setParametersTest[GL_CLEAR]			= false;
				setParametersTest[GL_BLEND]			= false;
				setParametersTest[GL_DEPTH_TEST]		= false;
				setParametersTest[GL_RENDER]			= false;

				for(unsigned int k=0; k<parser.elements.size(); k++)
				{
					GLenum glId = getGLEnum(parser.elements[k].strKeyword);
					
					if(glId==GL_VERTEX_SHADER)
					{
						if(setParametersTest[GL_VERTEX_SHADER])
							throw Exception("The GL_VERTEX_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_VERTEX_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_VERTEX_SHADER] = &source->second;
				
						setParametersTest[GL_VERTEX_SHADER] = true;
					}
					else if(glId==GL_FRAGMENT_SHADER)
					{
						if(setParametersTest[GL_FRAGMENT_SHADER])
							throw Exception("The GL_FRAGMENT_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_FRAGMENT_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_FRAGMENT_SHADER] = &source->second;

						setParametersTest[GL_FRAGMENT_SHADER] = true;
					}
					else if(glId==GL_COMPUTE_SHADER)
					{
						if(setParametersTest[GL_COMPUTE_SHADER])
							throw Exception("The GL_COMPUTE_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_COMPUTE_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_COMPUTE_SHADER] = &source->second;

						setParametersTest[GL_COMPUTE_SHADER] = true;
					}
					#ifdef GLIP_USE_GL
					else if(glId==GL_TESS_CONTROL_SHADER)
					{
						if(setParametersTest[GL_TESS_CONTROL_SHADER])
							throw Exception("The GL_TESS_CONTROL_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_TESS_CONTROL_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_TESS_CONTROL_SHADER] = &source->second;

						setParametersTest[GL_TESS_CONTROL_SHADER] = true;
					}
					else if(glId==GL_TESS_EVALUATION_SHADER)
					{
						if(setParametersTest[GL_TESS_EVALUATION_SHADER])
							throw Exception("The GL_TESS_EVALUATION_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_TESS_EVALUATION_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_TESS_EVALUATION_SHADER] = &source->second;

						setParametersTest[GL_TESS_EVALUATION_SHADER] = true;
					}
					else if(glId==GL_GEOMETRY_SHADER)
					{
						if(setParametersTest[GL_GEOMETRY_SHADER])
							throw Exception("The GL_GEOMETRY_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
		
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, ShaderSource>::iterator source = sourceList.find(parser.elements[k].arguments[0]);
						if(source==sourceList.end())
							throw Exception("No ShaderSource with name \"" + parser.elements[k].arguments[0] + "\" was registered for use as GL_GEOMETRY_SHADER.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							shaders[GL_GEOMETRY_SHADER] = &source->second;

						setParametersTest[GL_GEOMETRY_SHADER] = true;
					}
					#endif
					else if(glId==GL_RENDER)
					{
						if(setParametersTest[GL_RENDER])
							throw Exception("The GL_GEOMETRY_SHADER parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);

						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						std::map<std::string, GeometryModelList>::iterator geometry = geometryList.find(parser.elements[k].arguments[0]);
						if(geometry==geometryList.end())
							throw Exception("No Geometry with name \"" + parser.elements[k].arguments[0] + "\" was registered.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							geometryIt = geometry;	
						setParametersTest[GL_RENDER] = true;
					}
					else if(glId==GL_CLEAR)
					{
						if(setParametersTest[GL_CLEAR])
							throw Exception("The GL_CLEAR parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException); 

						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);
						
						clearingSet = true;
						if(parser.elements[k].arguments[0]==keywords[KW_LL_TRUE])
							enableClearing = true;
						else if(parser.elements[k].arguments[0]==keywords[KW_LL_FALSE])
							enableClearing = false;
						else
							throw Exception("Unknown settings \"" + parser.elements[k].arguments[0] + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException); 

						setParametersTest[GL_CLEAR] = true;
					}
					else if(glId==GL_BLEND)
					{
						if(setParametersTest[GL_BLEND])
							throw Exception("The GL_BLEND parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);

						preliminaryTests(parser.elements[k], -1, 3, 3, -1, e.name);

						blendingSet	= true;
						blendingSFactor = getGLEnum(parser.elements[k].arguments[0]),
						blendingDFactor = getGLEnum(parser.elements[k].arguments[1]),
						blendingEquation= getGLEnum(parser.elements[k].arguments[2]);
						setParametersTest[GL_BLEND] = true;
					}
					else if(glId==GL_DEPTH_TEST)
					{
						if(setParametersTest[GL_DEPTH_TEST])
							throw Exception("The GL_DEPTH_TEST parameter was already set.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							
						preliminaryTests(parser.elements[k], -1, 1, 1, -1, e.name);

						depthTestingSet = true;
						depthTestingFunction = getGLEnum(parser.elements[k].arguments[0]);

						setParametersTest[GL_DEPTH_TEST] = true;
					}
					else
						throw Exception("The parameter \"" + parser.elements[k].strKeyword + "\" is invalid. ", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
				}
			}
			catch(Exception& ex)
			{
				Exception m("Exception caught while building FilterLayout \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}

		// Construct : 
		filterList.insert( std::pair<std::string, FilterLayout>( e.name, FilterLayout(e.name, format->second, shaders, (geometryIt==geometryList.end() ? GeometryModelList() : geometryIt->second)) ) );
		std::map<std::string, FilterLayout>::iterator filterLayout = filterList.find(e.name);

		// Apply the options, if needed : 
		if(clearingSet)
		{
			if(enableClearing)	filterLayout->second.enableClearing();
			else			filterLayout->second.disableClearing();
		}

		if(blendingSet)
			filterLayout->second.enableBlending(blendingSFactor, blendingDFactor, blendingEquation);

		if(depthTestingSet)
			filterLayout->second.enableDepthTesting(depthTestingFunction);
	}

	void LayoutLoader::buildPipeline(const VanillaParserSpace::Element& e)
	{
		// Main pipeline indirection :
		if(e.strKeyword==getKeyword(KW_LL_PIPELINE_MAIN))
		{
			// Inheritance :
			if(!e.noArgument)
			{
				preliminaryTests(e, 1, 1, 1, 0, "MainPipeline (indirection)");

				// Duplicate test :	
				if(pipelineList.find(e.name)!=pipelineList.end())
					throw Exception("A Pipeline Layout with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

				// Check : 
				std::map<std::string, PipelineLayout>::iterator it = pipelineList.find(e.arguments.front());
				if(it==pipelineList.end())
					throw Exception("No PipelineLayout with name \"" + e.arguments.front() + "\" was registered and can be use as the Main Pipeline \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
				
				PipelineLayout pipeline = it->second;

				// Check the body : 
				if(!e.noBody)
				{
					VanillaParser parser(e.body, e.sourceName, e.bodyLine);

					// Classify the new data :
					std::vector<LayoutLoaderKeyword> associatedKeywords;

					classify(parser.elements, associatedKeywords);

					int 	inputPorts = -1,
						outputPorts = -1;
					for(unsigned int k=0; k<associatedKeywords.size(); k++)
					{
						switch(associatedKeywords[k])
						{
							case KW_LL_INPUT_PORTS :
								if(inputPorts>=0)
									throw Exception("The InputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
								else
									inputPorts = k;
								break;
							case KW_LL_OUTPUT_PORTS :
								if(outputPorts>=0)
									throw Exception("The OutputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
								else
									outputPorts = k;
								break;
							default :
								if( associatedKeywords[k]<LL_NumKeywords )
									throw Exception("The keyword " + std::string(keywords[associatedKeywords[k]]) + " is not allowed in a Main Pipeline indirection  (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
								else
									throw Exception("Unknown keyword \"" + parser.elements[k].strKeyword + "\" in a Main Pipeline indirection (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
								break;
						}
					}

					// Tests inputs : 
					if(inputPorts>=0)
					{
						if(static_cast<int>(parser.elements[inputPorts].arguments.size())!=pipeline.getNumInputPort())
							throw Exception("The pipeline " + pipeline.getFullName() + " has " + toString(pipeline.getNumInputPort()) + " input ports while the declaration of " + e.name + " expects " + toString(parser.elements[inputPorts].arguments.size()) + " input ports.", parser.elements[inputPorts].sourceName, parser.elements[inputPorts].startLine, Exception::ClientScriptException);

						for(unsigned int k=0; k<parser.elements[inputPorts].arguments.size(); k++)
							pipeline.setInputPortName(k, parser.elements[inputPorts].arguments[k]);
					}
					else if(pipeline.getNumInputPort()>0) // Do not throw error if there is no inputs to consider ...
						throw Exception("The pipeline " + e.name + " inheriting from " + it->second.getFullName() + " does not redefine its input port(s).", e.sourceName, e.startLine, Exception::ClientScriptException);

					// Tests inputs : 
					if(outputPorts>=0)
					{
						if(static_cast<int>(parser.elements[outputPorts].arguments.size())!=pipeline.getNumOutputPort())
							throw Exception("The pipeline " + pipeline.getFullName() + " has " + toString(pipeline.getNumOutputPort()) + " output ports while the declaration of " + e.name + " expects " + toString(parser.elements[outputPorts].arguments.size()) + " output ports.", parser.elements[outputPorts].sourceName, parser.elements[outputPorts].startLine, Exception::ClientScriptException);

						for(unsigned int k=0; k<parser.elements[outputPorts].arguments.size(); k++)
							pipeline.setOutputPortName(k, parser.elements[outputPorts].arguments[k]);
					}
					else
						throw Exception("The pipeline " + e.name + " inheriting from " + it->second.getFullName() + " does not redefine its output port(s).", e.sourceName, e.startLine, Exception::ClientScriptException);
				}
				else
					throw Exception("The pipeline " + e.name + " inheriting from " + it->second.getFullName() + " does not redeclare its input and output port(s).", e.sourceName, e.startLine, Exception::ClientScriptException);

				// Save :
				pipelineList.insert( std::pair<std::string, PipelineLayout>(e.name, pipeline) );
				return ;
			}
			// else, proceed as usual...
		}

		try
		{
			// Preliminary tests :
			preliminaryTests(e, 1, 0, 0, 1, "PipelineLayout");

			// Duplicate test :	
			if(pipelineList.find(e.name)!=pipelineList.end())
				throw Exception("A Pipeline Layout with the name \"" + e.name + "\" was already registered.", e.sourceName, e.startLine, Exception::ClientScriptException);

			// Load the content of the body :
			VanillaParser parser(e.body, e.sourceName, e.bodyLine);

			// Classify the new data :
			std::vector<LayoutLoaderKeyword> associatedKeywords;

			classify(parser.elements, associatedKeywords);

			// Test for possible external elements :
			int 	inputPorts = -1,
				outputPorts = -1,
				components = 0;
			for(unsigned int k=0; k<associatedKeywords.size(); k++)
			{
				switch(associatedKeywords[k])
				{
					case KW_LL_INPUT_PORTS :
						if(inputPorts>=0)
							throw Exception("The InputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							inputPorts = k;
						break;
					case KW_LL_OUTPUT_PORTS :
						if(outputPorts>=0)
							throw Exception("The OutputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							outputPorts = k;
						break;
					case KW_LL_FILTER_INSTANCE :
					case KW_LL_PIPELINE_INSTANCE :
						components++;
						break;
					case KW_LL_CONNECTION :
						break; //OK
					default :
						if( associatedKeywords[k]<LL_NumKeywords )
							throw Exception("The keyword " + std::string(keywords[associatedKeywords[k]]) + " is not allowed in a PipelineLayout definition (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							throw Exception("Unknown keyword \"" + parser.elements[k].strKeyword + "\" in a PipelineLayout definition (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						break;
				}
			}

			if(outputPorts<0)
				throw Exception("The OutputPorts are not declared for the PipelineLayout \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

			if(components==0)
				throw Exception("The PipelineLayout \"" + e.name + "\" does not contain any sub-component (such as Filters or sub-Pipelines).", e.sourceName, e.startLine, Exception::ClientScriptException);

			// Create the object :
			PipelineLayout layout(e.name);

			// Add the inputs (if they exist) :
			if(inputPorts>=0)
			{
				preliminaryTests(parser.elements[inputPorts], -1, 1, 256, -1, "InputPorts");

				for(unsigned int k=0; k<parser.elements[inputPorts].arguments.size(); k++)
					layout.addInput(parser.elements[inputPorts].arguments[k]);
			}

			// Add the outputs :
			preliminaryTests(parser.elements[outputPorts], -1, 1, 256, -1, "OutputPorts");

			for(unsigned int k=0; k<parser.elements[outputPorts].arguments.size(); k++)
				layout.addOutput(parser.elements[outputPorts].arguments[k]);

			// Parse and add the Objects for the PipelineLayout :
			std::map<std::string, FilterLayout>::iterator	filter;
			std::map<std::string, PipelineLayout>::iterator	pipeline;

			for(unsigned int k=0; k<associatedKeywords.size(); k++)
			{
				switch(associatedKeywords[k])
				{
					case KW_LL_FILTER_INSTANCE :
						{
							preliminaryTests(parser.elements[k], 1, 0, 1, -1, "FilterInstance");

							std::string filterName;

							if(parser.elements[k].arguments.empty())
								filterName = parser.elements[k].name;
							else
								filterName = parser.elements[k].arguments[0];

							filter = filterList.find(filterName);

							if(filter==filterList.end())
								throw Exception("No FilterLayout with name \"" + filterName + "\" was registered and can be use in PipelineLayout \"" + e.name + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							else
								layout.add(filter->second, parser.elements[k].name);
						}
						break;

					case KW_LL_PIPELINE_INSTANCE :
						{
							preliminaryTests(parser.elements[k], 1, 0, 1, -1, "PipelineInstance");

							std::string pipelineName;

							if(parser.elements[k].arguments.empty())
								pipelineName = parser.elements[k].name;
							else
								pipelineName = parser.elements[k].arguments[0];

							pipeline = pipelineList.find(pipelineName);

							if(pipeline==pipelineList.end())
								throw Exception("No PipelineLayout with name \"" + pipelineName + "\" was registered and can be use in PipelineLayout \"" + e.name + "\".", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
							else
								layout.add(pipeline->second, parser.elements[k].name);
						}
						break;

					default :
						break;
				}
			}

			// Install the connections :
			bool makeAutoConnect = true;
			for(unsigned int k=0; k<associatedKeywords.size(); k++)
			{
				if(associatedKeywords[k]==KW_LL_CONNECTION)
				{
					makeAutoConnect = false;

					preliminaryTests(parser.elements[k], -1, 4, 4, -1, "Connection");

					// Test the nature of the connection :
					if(parser.elements[k].arguments[0]==keywords[KW_LL_THIS_PIPELINE] && parser.elements[k].arguments[1]==keywords[KW_LL_THIS_PIPELINE])
						throw Exception("Direct connections between input and output are not allowed.", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
					else if(parser.elements[k].arguments[0]==keywords[KW_LL_THIS_PIPELINE])
						layout.connectToInput(parser.elements[k].arguments[1], parser.elements[k].arguments[2], parser.elements[k].arguments[3]);
					else if(parser.elements[k].arguments[2]==keywords[KW_LL_THIS_PIPELINE])
						layout.connectToOutput(parser.elements[k].arguments[0], parser.elements[k].arguments[1], parser.elements[k].arguments[3]);
					else
						layout.connect(parser.elements[k].arguments[0], parser.elements[k].arguments[1], parser.elements[k].arguments[2], parser.elements[k].arguments[3]);
				}
			}

			// If no connection was installed, try to autoConnect :
			if(makeAutoConnect)
				layout.autoConnect();

			// Save :
			pipelineList.insert( std::pair<std::string, PipelineLayout>(e.name, layout) );
		}
		catch(Exception& ex)
		{
			Exception m("Exception caught while building PipelineLayout \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
			m << ex;
			throw m;
		}
	}

	void LayoutLoader::process(const std::string& code, std::string& mainPipelineName, const std::string& sourceName, int startLine)
	{
		try
		{
			// Parse :
			VanillaParser rootParser(code, sourceName, startLine);

			// Class the elements :
			classify(rootParser.elements, associatedKeyword);

			// Check if there is any Unique requirement : 
			std::vector<LayoutLoaderKeyword>::iterator uniqueIterator = std::find(associatedKeyword.begin(), associatedKeyword.end(), KW_LL_UNIQUE);
	
			if(uniqueIterator!=associatedKeyword.end())
			{
				// Check if their is a second unique (error) : 
				std::vector<LayoutLoaderKeyword>::iterator secondUniqueIterator = std::find(uniqueIterator+1, associatedKeyword.end(), KW_LL_UNIQUE);

				if(secondUniqueIterator!=associatedKeyword.end())
				{
					const VanillaParserSpace::Element& e = rootParser.elements[std::distance(associatedKeyword.begin(), secondUniqueIterator)];

					throw Exception("Illegal second " + std::string(keywords[KW_LL_UNIQUE]) + " unique identifier in file.", e.sourceName, e.startLine, Exception::ClientScriptException);
				}
				else 
				{
					const VanillaParserSpace::Element& e = rootParser.elements[std::distance(associatedKeyword.begin(), uniqueIterator)];

					if(!checkUnique(e))
						return ; // Do not load the code if an ID matches.
				}
			}

			// Process :
			for(unsigned int k=0; k<associatedKeyword.size(); k++)
			{
				switch(associatedKeyword[k])
				{
					case KW_LL_ADD_PATH :
						appendPath(rootParser.elements[k]);
						break;
					case KW_LL_INCLUDE :
						includeFile(rootParser.elements[k]);
						break;
					case KW_LL_UNIQUE :
						break; // Already processed, nothing to do here.
					case KW_LL_REQUIRED_FORMAT :
						buildRequiredFormat(rootParser.elements[k]);
						break;
					case KW_LL_REQUIRED_SOURCE :
						buildRequiredSource(rootParser.elements[k]);
						break;
					case KW_LL_REQUIRED_GEOMETRY :
						buildRequiredGeometry(rootParser.elements[k]);
						break;
					case KW_LL_REQUIRED_PIPELINE :
						buildRequiredPipeline(rootParser.elements[k]);
						break;
					case KW_LL_CALL :
						moduleCall(rootParser.elements[k], mainPipelineName);
						break;
					case KW_LL_SAFE_CALL :
						moduleCall(rootParser.elements[k], mainPipelineName, true);
						break;
					case KW_LL_FORMAT :
						buildFormat(rootParser.elements[k]);
						break;
					case KW_LL_SOURCE :
						buildSource(rootParser.elements[k]);
						break;
					case KW_LL_GEOMETRY :
						buildGeometry(rootParser.elements[k]);
						break;
					case KW_LL_FILTER_LAYOUT :
						buildFilter(rootParser.elements[k]);
						break;
					case KW_LL_PIPELINE_MAIN :
						if(!isSubLoader)
						{
							if(mainPipelineName.empty())
								mainPipelineName = rootParser.elements[k].name;
							else
								throw Exception("A main pipeline (named \"" + mainPipelineName + "\") was already defined.", rootParser.elements[k].sourceName, rootParser.elements[k].startLine, Exception::ClientScriptException);
						}
						// And ...
					case KW_LL_PIPELINE_LAYOUT :
						buildPipeline(rootParser.elements[k]);
						break;
					default :
						if(associatedKeyword[k]<LL_NumKeywords)
							throw Exception("The keyword " + std::string(keywords[associatedKeyword[k]]) + " is not allowed in a PipelineScript.", rootParser.elements[k].sourceName, rootParser.elements[k].startLine, Exception::ClientScriptException);
						else
							throw Exception("Unknown keyword : \"" + rootParser.elements[k].strKeyword + "\".", rootParser.elements[k].sourceName, rootParser.elements[k].startLine, Exception::ClientScriptException);
						break;
				}
			}

			// Check Errors :
			if(mainPipelineName.empty() && !isSubLoader)
				throw Exception("No main pipeline (\"" + std::string(keywords[KW_LL_PIPELINE_MAIN]) + "\") was defined in this code.", rootParser.getSourceName(), 1, Exception::ClientScriptException);
		}
		catch(Exception& ex)
		{
			Exception m("Exception caught while processing pipeline script : ", sourceName, startLine, Exception::ClientScriptException);
			m << ex;
			throw m;
		}
	}

	void LayoutLoader::listPipelinePorts(const VanillaParserSpace::Element& e, std::vector<std::string>& inputs, std::vector<std::string>& outputs)
	{
		bool mustHaveInputsAndOutputs = true;

		// Main pipeline indirection : 
		if(e.strKeyword==getKeyword(KW_LL_PIPELINE_MAIN) && !e.noArgument)
		{
			if(e.noBody)
				return ;
			else
				mustHaveInputsAndOutputs = false;
		}

		try
		{
			// Load the content of the body :
			VanillaParser parser(e.body, e.sourceName, e.bodyLine);

			// Classify the new data :
			std::vector<LayoutLoaderKeyword> associatedKeywords;

			classify(parser.elements, associatedKeywords);

			// Test for possible external elements :
			int 	inputPorts = -1,
				outputPorts = -1;
			for(unsigned int k=0; k<associatedKeywords.size(); k++)
			{
				switch(associatedKeywords[k])
				{
					case KW_LL_INPUT_PORTS :
						if(inputPorts>=0)
							throw Exception("The InputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							inputPorts = k;
						break;
					case KW_LL_OUTPUT_PORTS :
						if(outputPorts>=0)
							throw Exception("The OutputPorts have already been declared for this PipelineLayout (\"" + e.name + "\").", parser.elements[k].sourceName, parser.elements[k].startLine, Exception::ClientScriptException);
						else
							outputPorts = k;
						break;
					default : // Drop.
						break;
				}
			}

			// This code was testing the presence of input ports. It was removed to allow for input-less pipeline.
			/*if(inputPorts<0)
			{
				if(mustHaveInputsAndOutputs)
					throw Exception("From line " + toString(e.startLine) + " : The InputPorts are not declared for the PipelineLayout \"" + e.name + "\".", __FILE__, __LINE__);
			}			
			else*/

			if(inputPorts>=0)
				inputs  = parser.elements[inputPorts].arguments;

			if(outputPorts<0)
			{
				if(mustHaveInputsAndOutputs)
					throw Exception("The OutputPorts are not declared for the PipelineLayout \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
			}			
			else
				outputs = parser.elements[outputPorts].arguments;
		}
		catch(Exception& ex)
		{
			Exception m("Exception caught while building PipelineLayout \"" + e.name + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);
			m << ex;
			throw m;
		}
	}

	/**
	\fn const std::set<std::string>& LayoutLoader::paths(void) const
	\brief Access the list of paths.
	\return A read-only access to the list of paths.
	**/
	const std::set<std::string>& LayoutLoader::paths(void) const
	{
		return staticPaths;
	}

	/**
	\fn void LayoutLoader::clearPaths(void)
	\brief Clear the list of paths.
	**/
	void LayoutLoader::clearPaths(void)
	{
		staticPaths.clear();
	}

	/**
	\fn void LayoutLoader::addToPaths(std::string p)
	\brief Add a path.
	\param p The path (might be invalid, avoid input of non-canonical paths which might results in ambiguous links errors).
	**/
	void LayoutLoader::addToPaths(std::string p)
	{
		if(!p.empty() && p!="./" && p!=".")
		{
			// Force the folder delimiter : 
			if(p[p.size()-1]!='/')
				p.push_back('/');
			staticPaths.insert(p);
		}
	}

	/**
	\fn void LayoutLoader::addToPaths(const std::vector<std::string>& paths)
	\brief Add several paths.
	\param paths The list of paths to add (might be invalid, avoid input of non-canonical paths which might results in ambiguous links errors).
	**/
	void LayoutLoader::addToPaths(const std::vector<std::string>& paths)
	{
		for(std::vector<std::string>::const_iterator it = paths.begin(); it!=paths.end(); it++)
			addToPaths(*it);
	}

	/**
	\fn bool LayoutLoader::removeFromPaths(const std::string& p)
	\brief Remove an entity from the paths.
	\param p The path to remove.
	\return True if a path was removed; false otherwise.
	**/
	bool LayoutLoader::removeFromPaths(const std::string& p)
	{
		std::set<std::string>::iterator it = staticPaths.find(p);
		const bool test = (it!=staticPaths.find(p));
		if(test)
			staticPaths.erase(it);
		return test;
	}

	/**
	\fn AbstractPipelineLayout LayoutLoader::getPipelineLayout(const std::string& source, std::string sourceName, int startLine)
	\brief Loads a pipeline layout from a file (see the script language description for more information).
	\param source The source to load. It is considered as a filename if it doesn't contain '\\n'.
	\param sourceName Specify a particular source name (for instance, a filename, an url, etc.).
	\param startLine The number of the first line in the source (only informational).
	\return The newly loaded layout or raise an exception if any errors occur.
	**/
	AbstractPipelineLayout LayoutLoader::getPipelineLayout(const std::string& source, std::string sourceName, int startLine)
	{
		std::pair<std::string, std::string> splittedPath;
		if(source.empty())
			throw Exception("LayoutLoader::operator() - The source is empty.", __FILE__, __LINE__, Exception::ModuleException);

		clean();
		// Tell the modules : 
		for(std::map<std::string, LayoutLoaderModule*>::iterator it=modules.begin(); it!=modules.end(); it++)
			it->second->beginLoadLayout();

		bool 		isAFile = false;
		std::string 	content;

		// Copy the path :
		dynamicPaths = staticPaths;

		// Is it a filename or the content :
		if(source.find('\n')==std::string::npos)
		{
			splittedPath = splitPath(source);
			// Add to the path :
			currentPath = splittedPath.first;
			if(!currentPath.empty())
				dynamicPaths.insert(currentPath);
			loadFile(splittedPath.second, content, currentPath);
			if(sourceName.empty())
				sourceName = currentPath + splittedPath.second;
			isAFile = true;
		}
		else
			content = source;

		try
		{
			std::string mainPipelineName;
			process(content, mainPipelineName, sourceName, startLine);

			// Get the mainPipeline :
			std::map<std::string, PipelineLayout>::iterator it = pipelineList.find(mainPipelineName);

			if(it==pipelineList.end())
				throw Exception("Main pipeline \"" + mainPipelineName + "\" was not found.", sourceName, startLine, Exception::ClientScriptException);

			// Tell the modules : 
			for(std::map<std::string, LayoutLoaderModule*>::iterator it=modules.begin(); it!=modules.end(); it++)
				it->second->endLoadLayout();

			return AbstractPipelineLayout(it->second);
		}
		catch(Exception& e)
		{
			// Tell the modules :
			for(std::map<std::string, LayoutLoaderModule*>::iterator it=modules.begin(); it!=modules.end(); it++)
				it->second->endLoadLayout();

			if(isAFile && currentPath.empty())
			{
				Exception m("Exception caught while processing file \"" + splittedPath.second + "\" : ", sourceName, startLine, Exception::ClientScriptException);
				m << e;
				throw m;
			}
			else if(isAFile)
			{
				Exception m("Exception caught while processing file \"" + splittedPath.second + "\" (path : \"" + currentPath + "\") : ", sourceName, startLine, Exception::ClientScriptException);
				m << e;
				throw m;
			}
			else
			{
				Exception m("Exception caught while processing string : ", sourceName, startLine, Exception::ClientScriptException);
				m << e;
				throw m;
			}
		}
	}

	/**
	\fn Pipeline* LayoutLoader::getPipeline(const std::string& source, std::string pipelineName, std::string sourceName, int startLine)
	\brief Loads a pipeline from a file (see the script language description for more information).
	\param source The source to load. It is considered as a filename if it doesn't contain '\\n'.
	\param pipelineName The name of the unique instance created (or take the type name if left empty).
	\param sourceName Specify a particular source name (for instance, a filename, an url, etc.).
	\param startLine The number of the first line in the source (only informational).
	\return A pointer to the unique instance built on the newly loaded layout or raise an exception if any errors occur. You have the charge to delete the newly created object.
	**/
	Pipeline* LayoutLoader::getPipeline(const std::string& source, std::string pipelineName, std::string sourceName, int startLine)
	{
		AbstractPipelineLayout layout = getPipelineLayout(source, sourceName, startLine);

		if(pipelineName.empty())
			pipelineName = layout.getLayoutName();

		Pipeline* pipeline = new Pipeline(layout, pipelineName);

		return pipeline;
	}

	/**
	\fn void LayoutLoader::addRequiredElement(const std::string& name, const HdlAbstractTextureFormat& fmt, bool replace)
	\brief Add a HdlAbstractTextureFormat to do the possibly required elements, along with its name. 

	Will raise an exception if an elemient with the same name already exists and the replacement flag is set to false. All the following pipelines loaded and containing a call REQUIRED_FORMAT:someName(name); will use this format.

	\param name The name of the element.
	\param fmt The element to be associated.
	\param replace Enable replacement.
	**/
	void LayoutLoader::addRequiredElement(const std::string& name, const HdlAbstractTextureFormat& fmt, bool replace)
	{
		std::map<std::string, HdlTextureFormat>::iterator it = requiredFormatList.find(name);
		if(it!=requiredFormatList.end())
		{
			if(!replace)
				throw Exception("LayoutLoader::addRequiredElement - An element with the name " + name + " already exists in the HdlTexture formats database.", __FILE__, __LINE__, Exception::ModuleException);
			else
				requiredFormatList.erase(it);
		}	
		// Then :
		requiredFormatList.insert( std::pair<std::string, HdlTextureFormat>(name, fmt) );
	}

	/**
	\fn bool LayoutLoader::hasRequiredFormat(const std::string& name) const
	\brief Test if a required format was provided.
	\param name Name of the required format.
	\return True if the format is known.
	**/
	bool LayoutLoader::hasRequiredFormat(const std::string& name) const
	{
		return requiredFormatList.find(name)!=requiredFormatList.end();
	}

	/**
	\fn const HdlAbstractTextureFormat& LayoutLoader::getRequiredFormat(const std::string& name) const
	\brief Get the required format.
	\param name Name of the required format.
	\return A constant reference to the corresponding format or raise and exception if it does not exist.
	**/
	const HdlAbstractTextureFormat& LayoutLoader::getRequiredFormat(const std::string& name) const
	{
		std::map<std::string, HdlTextureFormat>::const_iterator it = requiredFormatList.find(name);	
		if(it==requiredFormatList.end())
			throw Exception("LayoutLoader::getRequiredFormat - Required format \"" + name + "\" does not exist.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn int LayoutLoader::clearRequiredFormat(const std::string& name)
	\brief Remove required formats of given name.
	\param name Name of the required format.
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredFormat(const std::string& name)
	{
		std::map<std::string, HdlTextureFormat>::iterator it = requiredFormatList.find(name);
		if(it!=requiredFormatList.end())
		{
			requiredFormatList.erase(it);
			return 1;
		}
		else
			return 0;
	}

	/**
	\fn int LayoutLoader::clearRequiredFormat(bool (*filter)(const std::string&))
	\brief Remove required formats with user-defined criterion.
	\param filter The filtering function (returning true to remove an element).
	\return The number of elements removed.
	**/	
	int LayoutLoader::clearRequiredFormat(bool (*filter)(const std::string&))
	{
		int count = 0;
		for(std::map<std::string, HdlTextureFormat>::iterator it=requiredFormatList.begin(); it!=requiredFormatList.end(); )
		{
			if(filter(it->first))
			{	
				std::map<std::string, HdlTextureFormat>::iterator delIt = it;
				it++;
				requiredFormatList.erase(delIt);
				count++;
			}
			else
				it++;
		}
		return count;
	}
	
	/**
	\fn void LayoutLoader::addRequiredElement(const std::string& name, const ShaderSource& src, bool replace)
	\brief Add a ShaderSource to do the possibly required elements, along with its name. 

	Will raise an exception if an element with the same name already exists and the replacement flag is set to false. All the following pipelines loaded and containing a call REQUIRED_SOURCE:someName(name); will use this format.

	\param name The name of the element.
	\param src The element to be associated.
	\param replace Enable replacement.
	**/
	void LayoutLoader::addRequiredElement(const std::string& name, const ShaderSource& src, bool replace)
	{
		std::map<std::string, ShaderSource>::iterator it = requiredSourceList.find(name);
		
		if(it!=requiredSourceList.end())
		{
			if(!replace)
				throw Exception("LayoutLoader::addRequiredElement - An element with the name \"" + name + "\" already exists in the ShaderSource formats database.", __FILE__, __LINE__, Exception::ModuleException);
			else
				requiredSourceList.erase(it);
		}
		// Then :
		requiredSourceList.insert( std::pair<std::string, ShaderSource>(name, src) );
	}

	/**
	\fn bool LayoutLoader::hasRequiredSource(const std::string& name) const
	\brief Test if a required source was provided.
	\param name Name of the required source.
	\return True if the source is known.
	**/
	bool LayoutLoader::hasRequiredSource(const std::string& name) const
	{
		return requiredSourceList.find(name)==requiredSourceList.end();
	}

	/**
	\fn const ShaderSource& LayoutLoader::getRequiredSource(const std::string& name) const
	\brief Get the required source.
	\param name Name of the required source.
	\return A constant reference to the corresponding source or raise and exception if it does not exist.
	**/
	const ShaderSource& LayoutLoader::getRequiredSource(const std::string& name) const
	{
		std::map<std::string, ShaderSource>::const_iterator it = requiredSourceList.find(name);

		if(it==requiredSourceList.end())
			throw Exception("LayoutLoader::getRequiredSource - Required source \"" + name + "\" does not exist.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}
	
	/**
	\fn int LayoutLoader::clearRequiredSource(const std::string& name)
	\brief Remove required source of given name.
	\param name Name of the required source.
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredSource(const std::string& name)
	{
		std::map<std::string, ShaderSource>::iterator it = requiredSourceList.find(name);
		if(it!=requiredSourceList.end())
		{
			requiredSourceList.erase(it);
			return 1;
		}
		else
			return 0;
	}

	/**
	\fn int LayoutLoader::clearRequiredSource(bool (*filter)(const std::string&))
	\brief Remove required sources with user-defined criterion.
	\param filter The filtering function (returning true to remove an element).
	\return The number of elements removed.
	**/	
	int LayoutLoader::clearRequiredSource(bool (*filter)(const std::string&))
	{
		int count = 0;
		for(std::map<std::string, ShaderSource>::iterator it=requiredSourceList.begin(); it!=requiredSourceList.end(); )
		{
			if(filter(it->first))
			{
				std::map<std::string, ShaderSource>::iterator delIt = it;
				it++;
				requiredSourceList.erase(delIt);
				count++;
			}
			else
				it++;
		}
		return count;
	}
	
	/**
	\fn void LayoutLoader::addRequiredElement(const std::string& name, const GeometryModelList& mdl, bool replace)
	\brief Add a GeometryModelList to do the possibly required elements, along with its name.

	Will raise an exception if an element with the same name already exists and the replacement flag is set to false. All the following pipelines loaded and containing a call REQUIRED_GEOMETRY:someName(name); will use this geometry model.

	\param name The name of the element.
	\param mdl The element to be associated.
	\param replace Enable replacement.
	**/
	void LayoutLoader::addRequiredElement(const std::string& name, const GeometryModelList& mdl, bool replace)
	{
		std::map<std::string, GeometryModelList>::iterator it = requiredGeometryList.find(name);
		if(it!=requiredGeometryList.end())
		{
			if(!replace)
				throw Exception("LayoutLoader::addRequiredElement - An element with the name \"" + name + "\" already exists in the GeometryModel database.", __FILE__, __LINE__, Exception::ModuleException);
			else
				requiredGeometryList.erase(it);
		}
		// Then :
		requiredGeometryList.insert( std::pair<std::string, GeometryModelList>(name, mdl) );
	}

	/**
	\fn void LayoutLoader::addRequiredElement(const std::string& name, const GeometryModel& mdl, bool replace)
	\brief Add a GeometryModel to do the possibly required elements, along with its name.

	Will raise an exception if an element with the same name already exists and the replacement flag is set to false. All the following pipelines loaded and containing a call REQUIRED_GEOMETRY:someName(name); will use this geometry model.
	
	\param name The name of the element.
	\param mdl The element to be associated.
	\param replace Enable replacement.
	**/
	void LayoutLoader::addRequiredElement(const std::string& name, const GeometryModel& mdl, bool replace)
	{
		addRequiredElement(name, std::list<GeometryModel>(1, mdl), replace);
	}

	/**
	\fn bool LayoutLoader::hasRequiredGeometry(const std::string& name) const
	\brief Test if a required geometry was provided.
	\param name Name of the required geometry.
	\return True if the geometry is known.
	**/
	bool LayoutLoader::hasRequiredGeometry(const std::string& name) const
	{
		return requiredGeometryList.find(name)!=requiredGeometryList.end();
	}

	/**
	\fn const GeometryModelList& LayoutLoader::getRequiredGeometry(const std::string& name) const
	\brief Get the required geometry list.
	\param name Name of the required geometry list.
	\return A constant reference to the corresponding geometry list or raise and exception if it does not exist.
	**/
	const LayoutLoader::GeometryModelList& LayoutLoader::getRequiredGeometry(const std::string& name) const
	{
		std::map<std::string, GeometryModelList>::const_iterator it = requiredGeometryList.find(name);
		if(it==requiredGeometryList.end())
			throw Exception("Layoutoader::getRequiredGeometry - Required geometry \"" + name + "\" does not exist.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn int LayoutLoader::clearRequiredGeometry(const std::string& name)
	\brief Remove required geometry of given name.
	\param name Name of the required format.
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredGeometry(const std::string& name)
	{
		std::map<std::string,GeometryModelList>::iterator it = requiredGeometryList.find(name);
		if(it!=requiredGeometryList.end())
		{
			requiredGeometryList.erase(it);
			return 1;
		}
		else
			return 0;
	}

	/**
	\fn int LayoutLoader::clearRequiredGeometry(bool (*filter)(const std::string&))
	\brief Remove required geometry with user-defined criterion.
	\param filter The filtering function (returning true to remove an element).
	\return The number of elements removed.
	**/	
	int LayoutLoader::clearRequiredGeometry(bool (*filter)(const std::string&))
	{
		int count = 0;
		for(std::map<std::string, GeometryModelList>::iterator it=requiredGeometryList.begin(); it!=requiredGeometryList.end(); )
		{
			if(filter(it->first))
			{
				std::map<std::string, GeometryModelList>::iterator delIt = it;
				it++;
				requiredGeometryList.erase(delIt);
				count++;
			}
			else
				it++;
		}
		return count;
	}

	/**
	\fn void LayoutLoader::addRequiredElement(const std::string& name, AbstractPipelineLayout& layout, bool replace)
	\brief Add a AbstractPipelineLayout to do the possibly required elements, along with its name. 

	Will raise an exception if an element with the same name already exists and the replacement flag is set to false. All the following pipelines loaded and containing a call REQUIRED_PIPELINE:someName(name); will use this pipeline layout.

	\param name The name of the element.
	\param layout The element to be associated.
	\param replace Enable replacement.
	**/
	void LayoutLoader::addRequiredElement(const std::string& name, AbstractPipelineLayout& layout, bool replace)
	{
		std::map<std::string, PipelineLayout>::iterator it = requiredPipelineList.find(name);
		if(it!=requiredPipelineList.end())
		{
			if(!replace)
				throw Exception("LayoutLoader::addRequiredElement - An element with the name \"" + name + "\" already exists in the pipeline layouts database.", __FILE__, __LINE__, Exception::ModuleException);
			else
				requiredPipelineList.erase(it);
		}
		// Then :
		requiredPipelineList.insert( std::pair<std::string, PipelineLayout>(name, layout) );
	}

	/**
	\fn bool LayoutLoader::hasRequiredPipeline(const std::string& name) const
	\brief Test if a required pipeline was provided.
	\param name Name of the required pipeline.
	\return True if the pipeline is known.
	**/
	bool LayoutLoader::hasRequiredPipeline(const std::string& name) const
	{
		return requiredPipelineList.find(name)!=requiredPipelineList.end();
	}

	/**
	\fn const AbstractPipelineLayout& LayoutLoader::getRequiredPipelineLayout(const std::string& name) const
	\brief Get the required pipeline.
	\param name Name of the required pipeline.
	\return A constant reference to the corresponding pipeline or raise and exception if not it is not found.
	**/
	const AbstractPipelineLayout& LayoutLoader::getRequiredPipelineLayout(const std::string& name) const
	{
		std::map<std::string, PipelineLayout>::const_iterator it = requiredPipelineList.find(name);
		if(it==requiredPipelineList.end())
			throw Exception("LayoutLoader::getRequiredipelineLayout - Required pipeline \"" + name + "\" does not exist.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn int LayoutLoader::clearRequiredPipelineLayout(const std::string& name)
	\brief Remove required pipeline layout of given name.
	\param name Name of the required format.
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredPipelineLayout(const std::string& name)
	{
		std::map<std::string, PipelineLayout>::iterator it = requiredPipelineList.find(name);
		if(it!=requiredPipelineList.end())
		{
			requiredPipelineList.erase(it);
			return 1;
		}
		else
			return 0;
	}

	/**
	\fn int LayoutLoader::clearRequiredPipelineLayout(bool (*filter)(const std::string&))
	\brief Remove required pipeline layout with user-defined criterion.
	\param filter The filtering function (returning true to remove an element).
	\return The number of elements removed.
	**/	
	int LayoutLoader::clearRequiredPipelineLayout(bool (*filter)(const std::string&))
	{
		int count = 0;
		for(std::map<std::string, PipelineLayout>::iterator it=requiredPipelineList.begin(); it!=requiredPipelineList.end(); )
		{
			if(filter(it->first))
			{
				std::map<std::string, PipelineLayout>::iterator delIt = it;
				it++;
				requiredPipelineList.erase(delIt);
				count++;
			}
			else
				it++;
		}
		return count;
	}

	/**
	\fn int LayoutLoader::clearRequiredElements(const std::string& name)
	\brief Remove the elements having the given name.
	\param name The name of the targeted element. If empty, will remove all elements.
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredElements(const std::string& name)
	{	
		int numElemErased = 0;
		numElemErased += clearRequiredFormat(name);
		numElemErased += clearRequiredSource(name);
		numElemErased += clearRequiredGeometry(name);
		numElemErased += clearRequiredPipelineLayout(name);
		return numElemErased;
	}

	/**
	\fn int LayoutLoader::clearRequiredElements(bool (*filter)(const std::string&))
	\brief Remove elements according to a user-defined criterion.
	\param filter Filtering functions (returns true to remove an element).
	\return The number of elements removed.
	**/
	int LayoutLoader::clearRequiredElements(bool (*filter)(const std::string&))
	{
		int numElemErased = 0;
		numElemErased += clearRequiredFormat(filter);
		numElemErased += clearRequiredSource(filter);
		numElemErased += clearRequiredGeometry(filter);
		numElemErased += clearRequiredPipelineLayout(filter);
		return numElemErased;
	} 

	/**
	\fn LayoutLoader::PipelineScriptElements LayoutLoader::listElements(const std::string& source, std::string sourceName, int startLine)
	\brief List the resources contained in a script.
	\param source The source to load. It is considered as a filename if it doesn't contain '\\n'.
	\param sourceName Specify a particular source name (for instance, a filename, an url, etc.).
	\param startLine The number of the first line in the source (only informational).
	\return A LayoutLoader::PipelineScriptElements object containing all informations.
	**/
	LayoutLoader::PipelineScriptElements LayoutLoader::listElements(const std::string& source, std::string sourceName, int startLine)
	{
		std::pair<std::string, std::string> splittedPath;
		PipelineScriptElements result;

		clean();

		if(source.empty())
			throw Exception("LayoutLoader::listElements - The source is empty.", __FILE__, __LINE__, Exception::ModuleException);

		bool isAFile = false;
		std::string content;

		// Copy the path :
		dynamicPaths = staticPaths;

		// Is it a filename or the content :
		if(source.find('\n')==std::string::npos)
		{
			splittedPath = splitPath(source);
			// Add to the path :
			currentPath = splittedPath.first;
			if(!currentPath.empty())
				dynamicPaths.insert(currentPath);
			loadFile(splittedPath.second, content, currentPath);
			if(sourceName.empty())
				sourceName = currentPath + splittedPath.second;
			isAFile = true;
		}
		else
			content = source;

		try
		{
			// Parse :
			VanillaParser rootParser(content, sourceName, startLine);

			// Class the elements :
			classify(rootParser.elements, associatedKeyword);

			// Process
			for(unsigned int k=0; k<associatedKeyword.size(); k++)
			{
				switch(associatedKeyword[k])
				{
					case KW_LL_ADD_PATH :
						preliminaryTests(rootParser.elements[k], -1, 1, 1, -1, "AppendPath");
						result.addedPaths.push_back( rootParser.elements[k].arguments[0] );
						break;
					case KW_LL_INCLUDE :
						preliminaryTests(rootParser.elements[k], -1, 1, 1, -1, "IncludeFile");
						result.includedFiles.push_back( rootParser.elements[k].arguments[0] );
						break;
					case KW_LL_UNIQUE:
						preliminaryTests(rootParser.elements[k], -1, 1, 1, -1, "Unique");
						result.unique = rootParser.elements[k].arguments[0];
						break;
					case KW_LL_REQUIRED_FORMAT :
						preliminaryTests(rootParser.elements[k], 1, 1, 10, -1, "RequiredFormat");
						result.requiredFormats.push_back( rootParser.elements[k].arguments[0] );
						result.formats.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_REQUIRED_SOURCE :
						preliminaryTests(rootParser.elements[k], 1, 1, 1, -1, "RequiredSource");
						result.requiredSources.push_back( rootParser.elements[k].arguments[0] );
						result.sources.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_REQUIRED_GEOMETRY :
						preliminaryTests(rootParser.elements[k], 1, 1, 1, -1, "RequiredGeometry"); 
						result.requiredGeometries.push_back( rootParser.elements[k].arguments[0] );
						result.geometries.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_REQUIRED_PIPELINE :
						preliminaryTests(rootParser.elements[k], 1, 1, 1, -1, "RequiredPipeline");
						result.requiredPipelines.push_back( rootParser.elements[k].arguments[0] );
						// WARNING, THE REQUIRED PIPELINE I/O ARE NOT LISTED BECAUSE WE CAN'T KNOW THEM AHEAD OF TIME.
						break;
					case KW_LL_CALL :
					case KW_LL_SAFE_CALL :
						preliminaryTests(rootParser.elements[k], 1, -1, -1, 0, "ModuleCall");
						result.modulesCalls.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_FORMAT :
						preliminaryTests(rootParser.elements[k], 1, 4, 9, -1, "Format"); 
						result.formats.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_SOURCE :
						preliminaryTests(rootParser.elements[k], 1, 0, 1, 0, "Source");
						result.sources.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_GEOMETRY :
						preliminaryTests(rootParser.elements[k], 1, 1, 4, 0, "Geometry");
						result.geometries.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_FILTER_LAYOUT :
						preliminaryTests(rootParser.elements[k], 1, 1, 2, 0, "FilterLayout");
						result.filtersLayout.push_back( rootParser.elements[k].name );
						break;
					case KW_LL_PIPELINE_MAIN :
						preliminaryTests(rootParser.elements[k], 1, 0, 1, 0, "MainPipelineLayout");
						result.mainPipeline = rootParser.elements[k].name;
						listPipelinePorts(rootParser.elements[k], result.mainPipelineInputs, result.mainPipelineOutputs);
						break;
					case KW_LL_PIPELINE_LAYOUT :
						preliminaryTests(rootParser.elements[k], 1, 0, 0, 1, "PipelineLayout");
						result.pipelines.push_back( rootParser.elements[k].name );
						result.pipelineInputs.push_back( std::vector<std::string>() );
						result.pipelineOutputs.push_back( std::vector<std::string>() );
						listPipelinePorts(rootParser.elements[k], result.pipelineInputs.back(), result.pipelineOutputs.back());	
						break;
					default :
						if(associatedKeyword[k]<LL_NumKeywords)
							throw Exception("The keyword " + std::string(keywords[associatedKeyword[k]]) + " is not allowed in a Pipeline file.", rootParser.elements[k].sourceName, rootParser.elements[k].startLine, Exception::ClientScriptException);
						else
							throw Exception("Unknown keyword : \"" + rootParser.elements[k].strKeyword + "\".", rootParser.elements[k].sourceName, rootParser.elements[k].startLine, Exception::ClientScriptException);
						break;
				}
			}
		}
		catch(Exception& ex)
		{
			if(isAFile && currentPath.empty())
			{
				Exception m("Exception caught while processing file \"" + splittedPath.second + "\" : ", sourceName, startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
			else if(isAFile)
			{
				Exception m("Exception caught while processing file \"" + splittedPath.second + "\" (path : \"" + currentPath + "\") : ", sourceName, startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
			else
			{
				Exception m("Exception caught while processing string : ", sourceName, startLine, Exception::ClientScriptException);
				m << ex;
				throw m;
			}
		}

		clean();

		return result;
	}

	/**
	\fn void LayoutLoader::addModule(const LayoutLoaderModule& m, bool replace)
	\brief Add a module which can be called from a script to generate dynamic data.
	\param module The module to add. See LayoutLoaderModule documentation for more information. The memory will be safely releaed by the destructor of LayoutLoader, the user shall not release it.
	\param replace Set to true if any module having similar name must be replaced. Raise an exception otherwise.

	If the replace flag is set, any existing module with this name will be deleted.
	**/
	void LayoutLoader::addModule(LayoutLoaderModule* m, bool replace)
	{
		std::map<std::string, LayoutLoaderModule*>::iterator it = modules.find(m->getName());
		if(it==modules.end())
			modules.insert( std::pair<std::string, LayoutLoaderModule*>( m->getName(), m ) );
		else if(it!=modules.end() && replace)
		{
			delete it->second;
			it->second = m;
		}
		else
			throw Exception("LayoutLoader::addModule - A module with the name \"" + m->getName() + " already exists.", __FILE__, __LINE__, Exception::ModuleException);
	}

	/**
	\fn bool LayoutLoader::hasModule(const LayoutLoaderModule* module) const
	\brief Test if a module is present.
	\param module The module to test.
	\return True if the module is present.
	**/
	bool LayoutLoader::hasModule(const LayoutLoaderModule* module) const
	{
		for(std::map<std::string, LayoutLoaderModule*>::const_iterator it=modules.begin(); it!=modules.end(); it++)
		{
			if(it->second==module)
				return true;
		}

		return false;
	}

	/**
	\fn bool LayoutLoader::hasModule(const std::string& name) const
	\brief Test if a module is present.
	\param name The name of the module to test.
	\return True if the module is present.
	**/
	bool LayoutLoader::hasModule(const std::string& name) const
	{
		return (modules.find(name)!=modules.end());
	}

	/**
	\fn std::vector<const LayoutLoaderModule*> LayoutLoader::listModules(void) const
	\brief List the modules used by this LayoutLoader object.
	\return A list of constant pointers to the LayoutLoaderModule objects used.
	**/
	std::vector<const LayoutLoaderModule*> LayoutLoader::listModules(void) const
	{
		std::vector<const LayoutLoaderModule*> modulesList;	
		for(std::map<std::string, LayoutLoaderModule*>::const_iterator it=modules.begin(); it!=modules.end(); it++)
			modulesList.push_back(it->second);
		return modulesList;
	}

	/**
	\fn std::vector< LayoutLoaderModule*> LayoutLoader::listModules(void)
	\brief List the modules used by this LayoutLoader object.
	\return A list of pointers to the LayoutLoaderModule objects used.
	**/
	std::vector<LayoutLoaderModule*> LayoutLoader::listModules(void)
	{
		std::vector<LayoutLoaderModule*> modulesList;	
		for(std::map<std::string, LayoutLoaderModule*>::const_iterator it=modules.begin(); it!=modules.end(); it++)
			modulesList.push_back(it->second);
		return modulesList;
	}

	/**
	\fn std::vector<std::string> LayoutLoader::listModuleNames(void) const
	\brief List all modules loaded for this LayoutLoader object.
	\return A list of the names of the loaded modules (possibly empty if no module was loaded).
	**/
	std::vector<std::string> LayoutLoader::listModuleNames(void) const
	{
		std::vector<std::string> modulesNamesList;
		for(std::map<std::string, LayoutLoaderModule*>::const_iterator it = modules.begin(); it!=modules.end(); it++)
			modulesNamesList.push_back( it->first );
		return modulesNamesList;
	}	
	
	/**
	\fn const LayoutLoaderModule& LayoutLoader::module(const std::string& name) const
	\brief Access a loaded module from its name.
	\param name The name of the module to access.
	\return A constant reference to the targeted module or raise an exception otherwise.
	**/
	const LayoutLoaderModule& LayoutLoader::module(const std::string& name) const
	{
		std::map<std::string, LayoutLoaderModule*>::const_iterator it = modules.find(name);
		if(it==modules.end())
			throw Exception("LayoutLoader::module - No module with name \"" + name + " is not loaded for LayoutLoader object.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return *it->second;
	}

	/**
	\fn LayoutLoaderModule& LayoutLoader::module(const std::string& name)
	\brief Access a loaded module from its name.
	\param name The name of the module to access.
	\return A constant reference to the targeted module or raise an exception otherwise.
	**/
	LayoutLoaderModule& LayoutLoader::module(const std::string& name)
	{
		std::map<std::string, LayoutLoaderModule*>::const_iterator it = modules.find(name);
		if(it==modules.end())
			throw Exception("LayoutLoader::module - No module with name \"" + name + " is not loaded for LayoutLoader object.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return *it->second;
	}

	/**
	\fn const LayoutLoaderModule* LayoutLoader::removeModule(const LayoutLoaderModule* module)
	\brief Remove a loaded module.
	\param module Pointer to the module to remove.
	\return The pointer of the removed module or NULL if not found. The user must take care of deallocating it.
	**/
	const LayoutLoaderModule* LayoutLoader::removeModule(const LayoutLoaderModule* module)
	{
		const LayoutLoaderModule* res = NULL;
		for(std::map<std::string, LayoutLoaderModule*>::iterator it=modules.begin(); it!=modules.end(); it++)
		{
			if(it->second==module)
			{
				modules.erase(it);
				res = module;
			}
		}
		return res;
	}

	/**
	\fn LayoutLoaderModule*LayoutLoader::removeModule(const std::string& name)
	\brief Remove a loaded module, or raise an Exception if any error occur.
	\param name The name of the module to remove.
	\return The pointer of the removed module. The user must take care of deallocating it.
	**/
	LayoutLoaderModule* LayoutLoader::removeModule(const std::string& name)
	{
		std::map<std::string, LayoutLoaderModule*>::iterator it = modules.find(name);
		if(it==modules.end())
			throw Exception("LayoutLoader::removeModule - No module with name \"" + name + " is used by this LayoutLoader object.", __FILE__, __LINE__, Exception::ModuleException);
		else
		{
			LayoutLoaderModule* res = it->second;
			modules.erase(it);
			return res;
		}
	}

	/**
	\fn LayoutLoaderKeyword LayoutLoader::getKeyword(const std::string& str)
	\brief Get the keyword symbol from a string.
	\param str Input string.
	\returns The corresponding keyword enum or LL_UnknownKeyword if none is found.
	**/
	LayoutLoaderKeyword LayoutLoader::getKeyword(const std::string& str)
	{
		for(int i=0; i<LL_NumKeywords; i++)
			if(keywords[i]==str) return static_cast<LayoutLoaderKeyword>(i);
		return LL_UnknownKeyword;
	}

	/**
	\fn const char* LayoutLoader::getKeyword(LayoutLoaderKeyword k)
	\brief Get the actual keyword string.
	\param k The index of the keyword.
	\return A const pointer to a C-style character string.
	**/
	const char* LayoutLoader::getKeyword(LayoutLoaderKeyword k)
	{
		if(k>=0 && k<LL_NumKeywords)
			return keywords[k];
		else if(k==LL_UnknownKeyword)
			return "<Unknown Keyword>";
		else
			throw Exception("LayoutLoader::getKeyword - Invalid keyword of index " + toString(k) + ".", __FILE__, __LINE__, Exception::ModuleException);
	}

// LayoutWriter 
	/*
	\fn LayoutWriter::LayoutWriter(void)
	\brief LayoutWriter constructor.
	*/
	/*LayoutWriter::LayoutWriter(void)
	{ }

	LayoutWriter::~LayoutWriter(void)
	{
		code.clear();
	}*/

	/* 
	\fn VanillaParserSpace::Element LayoutWriter::write(const HdlAbstractTextureFormat& format, const std::string& name)
	\brief Generate the code element for a particular format.
	\param format Format object.
	\param name Name of the object.
	\return A coded element containing all format data.
	*/
	/*VanillaParserSpace::Element LayoutWriter::write(const HdlAbstractTextureFormat& format, const std::string& name)
	{
		if(name.empty())
			throw Exception("LayoutWriter - Writing " + std::string(LayoutLoader::getKeyword( KW_LL_FORMAT )) + " : name cannot be empty.", __FILE__, __LINE__, Exception::ModuleException);
		if(format.getBaseLevel()!=0)
			throw Exception("LayoutWriter - Writing " + std::string(LayoutLoader::getKeyword( KW_LL_FORMAT )) + " : base level cannot be different than 0 (current : " + toString(format.getBaseLevel()) + ") .", __FILE__, __LINE__, Exception::ModuleException);

		VanillaParserSpace::Element e;

		e.strKeyword = LayoutLoader::getKeyword( KW_LL_FORMAT );
		e.name = name;
		e.noName = false;
		e.body.clear();
		e.noBody = true;

		e.noArgument = false;
		e.arguments.push_back( toString( format.getWidth() ) );
		e.arguments.push_back( toString( format.getHeight() ) );
		e.arguments.push_back( getGLEnumName( format.getGLMode() ) );
		e.arguments.push_back( getGLEnumName( format.getGLDepth() ) );
		e.arguments.push_back( getGLEnumName( format.getMinFilter() ) );
		e.arguments.push_back( getGLEnumName( format.getMagFilter() ) );
		e.arguments.push_back( getGLEnumName( format.getSWrapping() ) );
		e.arguments.push_back( getGLEnumName( format.getTWrapping() ) );
		e.arguments.push_back( toString( format.getMaxLevel() ) );

		return e;
	}*/
 
	/*
	\fn VanillaParserSpace::Element LayoutWriter::write(const ShaderSource& source, const std::string& name)
	\brief Generate the code element for a particular source.
	\param source Source object.
	\param name Name of the source.
	\return A coded element containing all source data.
	*/
	/*VanillaParserSpace::Element LayoutWriter::write(const ShaderSource& source, const std::string& name)
	{
		if(name.empty())
			throw Exception("LayoutWriter - Writing " + std::string(LayoutLoader::getKeyword( KW_LL_SOURCE )) + " : name cannot be empty.", __FILE__, __LINE__, Exception::ModuleException);

		VanillaParserSpace::Element e;

		e.strKeyword	= LayoutLoader::getKeyword( KW_LL_SOURCE );
		e.name		= name;
		e.body		= source.getSource();
		e.arguments.clear();
		e.noArgument	= true;

		return e;
	}*/

	/*
	\fn VanillaParserSpace::Element LayoutWriter::write(const GeometryModel& mdl, const std::string& name)
	\brief Generate the code element for a particular geometry.
	\param mdl Geometry model object.
	\param name Name of teh geometry.
	\return A coded element containing all geometry data.
	*/
	/*VanillaParserSpace::Element LayoutWriter::write(const GeometryModel& mdl, const std::string& name)
	{
		if(name.empty())
			throw Exception("LayoutWriter - Writing " + std::string(LayoutLoader::getKeyword( KW_LL_GEOMETRY )) + " : name cannot be empty.", __FILE__, __LINE__, Exception::ModuleException);

		VanillaParserSpace::Element e;

		e.strKeyword	= LayoutLoader::getKeyword( KW_LL_GEOMETRY );
		e.name		= name;

		if(mdl.type==GeometryModel::StandardQuad)	
		{
			e.arguments.push_back( LayoutLoader::getKeyword( KW_LL_STANDARD_QUAD ) );
			e.noBody	= true;
		}
		else if(mdl.type==GeometryModel::PointsGrid2D)
		{
			e.arguments.push_back( LayoutLoader::getKeyword( KW_LL_GRID_2D ) );

			// Find grid size :
			GLfloat x = 0.0, y = 0.0; 
			for(unsigned int k=0; k<mdl.getNumVertices(); k++)
			{
				x = std::max(x, mdl.x(k));
				y = std::max(y, mdl.y(k));
			}

			e.arguments.push_back( toString(x+1) );
			e.arguments.push_back( toString(y+1) );
		}	
		else if(mdl.type==GeometryModel::PointsGrid3D)
		{
			e.arguments.push_back( LayoutLoader::getKeyword( KW_LL_GRID_3D ) );

			// Find grid size : 
			GLfloat x = 0.0, y = 0.0, z = 0.0; 
			for(unsigned int k=0; k<mdl.getNumVertices(); k++)
			{
				x = std::max(x, mdl.x(k));
				y = std::max(y, mdl.y(k));
				z = std::max(z, mdl.z(k));
			}

			e.arguments.push_back( toString(x+1) );
			e.arguments.push_back( toString(y+1) );
			e.arguments.push_back( toString(z+1) );
		}
		else if(mdl.type==GeometryModel::CustomModel)
		{
			e.arguments.push_back( LayoutLoader::getKeyword( KW_LL_CUSTOM_MODEL ) );
			e.arguments.push_back( getGLEnumName(mdl.primitiveGL) );

			e.arguments.push_back( mdl.hasNormals ? LayoutLoader::getKeyword(KW_LL_TRUE) : LayoutLoader::getKeyword(KW_LL_FALSE) );
			e.arguments.push_back( mdl.hasTexCoords ? LayoutLoader::getKeyword(KW_LL_TRUE) : LayoutLoader::getKeyword(KW_LL_FALSE) );

			// Write model : 
			VanillaParserSpace::Element v;
			v.strKeyword	= LayoutLoader::getKeyword(KW_LL_VERTEX);
			v.noName 	= true;
			v.noBody 	= true;
			for(unsigned int k=0; k<mdl.getNumVertices(); k++)
			{
				v.arguments.clear();

				v.arguments.push_back(toString(mdl.x(k)));
				v.arguments.push_back(toString(mdl.y(k)));

				if(mdl.dim>2)
					v.arguments.push_back( toString(mdl.z(k)) );
				else if(mdl.dim!=2 && mdl.dim!=3)
					throw Exception("LayoutWriter::write - Geometry : internal error, unsupported number of dimensions.", __FILE__, __LINE__, Exception::ModuleException);

				if(mdl.hasNormals)
				{
					v.arguments.push_back(toString(mdl.nx(k)));
					v.arguments.push_back(toString(mdl.ny(k)));
					v.arguments.push_back(toString(mdl.nz(k)));
				}

				if(mdl.hasTexCoords)
				{
					v.arguments.push_back(toString(mdl.u(k)));
					v.arguments.push_back(toString(mdl.v(k)));
				}

				// Push : 
				e.body += v.getCode() + "\n";
			}

			VanillaParserSpace::Element p;
			p.strKeyword	= LayoutLoader::getKeyword(KW_LL_ELEMENT);
			p.noName 	= true;
			p.noBody 	= true;
			e.body += "\n";
			for(unsigned int k=0; k<mdl.getNumElements(); k++)
			{
				p.arguments.clear();

				if(mdl.numVerticesPerElement>=1) p.arguments.push_back( toString(mdl.a(k)) );
				if(mdl.numVerticesPerElement>=2) p.arguments.push_back( toString(mdl.b(k)) );
				if(mdl.numVerticesPerElement>=3) p.arguments.push_back( toString(mdl.c(k)) );
				if(mdl.numVerticesPerElement>=4) p.arguments.push_back( toString(mdl.d(k)) );

				// Push : 
				e.body += p.getCode() + "\n";
			}
		}

		return e;
	}

	VanillaParserSpace::Element LayoutWriter::write(const AbstractFilterLayout& fLayout)
	{
		const std::string 	formatName = fLayout.getFullName() + "_format",
					modelName  = fLayout.getFullName() + "_model";

		VanillaParserSpace::Element e;
		e.strKeyword	= LayoutLoader::getKeyword( KW_LL_FILTER_LAYOUT );
		e.name		= fLayout.getLayoutName();
		e.noName	= false;
		e.noBody	= false;
		e.noArgument	= false;
		e.arguments.push_back(formatName);

		// Write the format :
		VanillaParserSpace::Element e1 = LayoutWriter::write(fLayout, formatName);
		code += e1.getCode() + "\n\n";
		
		// Write all required shaders :
		for(unsigned int k=0; k<HandleOpenGL::numShaderTypes; k++)
		{
			const GLenum shaderType = HandleOpenGL::shaderTypes[k];
			const std::string shaderTypeName = getGLEnumName(shaderType);
			const ShaderSource* ptr = fLayout.getShaderSource(shaderType);
			if(ptr!=NULL)
			{
				const std::string name = fLayout.getFullName() + "_" + shaderTypeName;
				// Create the source code for the shader :
				VanillaParserSpace::Element es = LayoutWriter::write(*ptr, name);
				code += es.getCode();
				// Attach :
				VanillaParserSpace::Element el;
				el.strKeyword = shaderTypeName;
				el.noName = true;
				el.noBody = true;
				el.noArgument = false;
				el.arguments.push_back(name);
				e.body += "\t" + el.getCode() + "\n";
			}
		}

		if(!fLayout.isStandardGeometryModel())
		{
			VanillaParserSpace::Element es = LayoutWriter::write(fLayout.getGeometryModel(), modelName);
			code += es.getCode();
			VanillaParserSpace::Element el;
			el.strKeyword = getGLEnumName(GL_RENDER);
			el.noName = true;
			el.noBody = true;
			el.noArgument = false;
			el.arguments.push_back(modelName);
			e.body += "\t" + el.getCode() + "\n";
		}

		// Always copy the clear settings :
		{
			VanillaParserSpace::Element el;
			el.strKeyword = getGLEnumName(GL_CLEAR);
			el.noName = true;
			el.noBody = true;
			el.noArgument = false;
			el.arguments.push_back(fLayout.isClearingEnabled() ? LayoutLoader::getKeyword(KW_LL_TRUE) : LayoutLoader::getKeyword(KW_LL_FALSE) );
			e.body += "\t" + el.getCode() + "\n";
		}

		if(fLayout.isBlendingEnabled())		
		{
			VanillaParserSpace::Element el;
			el.strKeyword = getGLEnumName(GL_BLEND);
			el.noName = true;
			el.noBody = true;
			el.noArgument = false;
			el.arguments.push_back(getGLEnumName(fLayout.getSFactor()));
			el.arguments.push_back(getGLEnumName(fLayout.getDFactor()));
			el.arguments.push_back(getGLEnumName(fLayout.getBlendingEquation()));
			e.body += "\t" + el.getCode() + "\n";
		}

		if(fLayout.isDepthTestingEnabled())
		{
			VanillaParserSpace::Element el;
			el.strKeyword = getGLEnumName(GL_DEPTH_TEST);
			el.noName = true;
			el.noBody = true;
			el.noArgument = false;
			el.arguments.push_back(getGLEnumName(fLayout.getDepthTestingFunction()));
			e.body += "\t" + el.getCode() + "\n";
		}

		return e;
	}

	VanillaParserSpace::Element LayoutWriter::write(const AbstractPipelineLayout& pLayout, bool isMain)
	{
		std::string keyword;

		if(isMain)
			keyword = LayoutLoader::getKeyword( KW_LL_PIPELINE_MAIN );
		else
			keyword = LayoutLoader::getKeyword( KW_LL_PIPELINE_LAYOUT );

		VanillaParserSpace::Element e;

		e.strKeyword	= keyword;
		e.name		= pLayout.getLayoutName();
		e.arguments.clear();
		e.noArgument	= true;
		e.body.clear();

		// Declare all ports : 
		VanillaParserSpace::Element inPorts;
		inPorts.strKeyword = LayoutLoader::getKeyword( KW_LL_INPUT_PORTS );
		inPorts.name.clear();
		inPorts.noName = true;
		inPorts.body.clear();
		inPorts.noBody = true;
		
		for(int k=0; k<pLayout.getNumInputPort(); k++)
			inPorts.arguments.push_back( pLayout.getInputPortName(k) );

		e.body += inPorts.getCode() + "\n";

		VanillaParserSpace::Element outPorts;
		outPorts.strKeyword = LayoutLoader::getKeyword( KW_LL_OUTPUT_PORTS );
		outPorts.name.clear();
		outPorts.noName = true;
		outPorts.body.clear();
		outPorts.noBody = true;

		for(int k=0; k<pLayout.getNumOutputPort(); k++)
			outPorts.arguments.push_back( pLayout.getOutputPortName(k) );
		
		e.body += outPorts.getCode() + "\n";
		e.body += "\n";

		// Declare all sub-elements :
		for(int k=0; k<pLayout.getNumElements(); k++)
		{
			VanillaParserSpace::Element c, d;

			// Element to push in the body :
			d.name = pLayout.getElementName(k);
			

			switch(pLayout.getElementKind(k))
			{
				case AbstractPipelineLayout::FILTER :
					c 		= write( pLayout.filterLayout(k) );
					d.strKeyword	= LayoutLoader::getKeyword( KW_LL_FILTER_INSTANCE );
					break;
				case AbstractPipelineLayout::PIPELINE :
					c = write( pLayout.pipelineLayout(k) );
					d.strKeyword	= LayoutLoader::getKeyword( KW_LL_PIPELINE_INSTANCE );
					break;
				default :
					throw Exception("LayoutWriter::write - Internal error : unknown element type.", __FILE__, __LINE__, Exception::ModuleException);
			}

			// Element to push outside the body :
			code += c.getCode() + "\n\n";

			// Element to push inside the body :
			d.arguments.push_back( c.name );
			e.body += d.getCode() + "\n";
		}

		e.body += "\n";

		// Add Connections :  
		VanillaParserSpace::Element c;
		c.strKeyword	= LayoutLoader::getKeyword(KW_LL_CONNECTION);
		c.name.clear();
		c.noName = true;
		c.body.clear();
		c.noBody = true;

		for(int k=0; k<pLayout.getNumConnections(); k++)
		{
			AbstractPipelineLayout::Connection x = pLayout.getConnection(k);

			c.arguments.clear();

			if(x.idOut==AbstractPipelineLayout::THIS_PIPELINE)
			{
				c.arguments.push_back( LayoutLoader::getKeyword(KW_LL_THIS_PIPELINE) );
				c.arguments.push_back( pLayout.getInputPortName(x.portOut) );
			}
			else
			{
				c.arguments.push_back( pLayout.getElementName(x.idOut) );
				c.arguments.push_back( pLayout.componentLayout(x.idOut).getOutputPortName(x.portOut) );
			}
			
			if(x.idIn==AbstractPipelineLayout::THIS_PIPELINE)
			{
				c.arguments.push_back( LayoutLoader::getKeyword(KW_LL_THIS_PIPELINE) );
				c.arguments.push_back( pLayout.getOutputPortName(x.portIn) );
			}
			else
			{
				c.arguments.push_back( pLayout.getElementName(x.idIn) );
				c.arguments.push_back( pLayout.componentLayout(x.idIn).getInputPortName(x.portIn) );
			}

			e.body += c.getCode() + "\n";
		}

		return e;
	}*/

	/*
	\fn std::string LayoutWriter::getCode(const AbstractPipelineLayout& pipelineLayout)
	\brief Build the human-readable code for the given AbstractPipelineLayout object.
	\param pipelineLayout The pipeline layout to convert.
	\return A standard string containing the full pipeline layout description. 
	*/
	/*std::string LayoutWriter::getCode(const AbstractPipelineLayout& pipelineLayout)
	{
		code.clear();
		VanillaParserSpace::Element e = write(pipelineLayout, true);
		code += e.getCode() + "\n";
		return code;
	}*/

	/*
	\fn void LayoutWriter::writeToFile(const AbstractPipelineLayout& pipelineLayout, const std::string& filename)
	\brief Build the human-readable code for the given AbstractPipelineLayout object and write it to a file. WARNING : it will discard all previous content.
	\param pipelineLayout The pipeline layout to convert.
	\param filename The filename to write to (Warning : discard all previous content).
	*/
	/*void LayoutWriter::writeToFile(const AbstractPipelineLayout& pipelineLayout, const std::string& filename)
	{
		std::fstream file(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
		if(!file.is_open())
			throw Exception("LayoutWriter::writeToFile - Unable to write pipeline layout \"" + pipelineLayout.getLayoutName() + "\" to file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

		file << getCode(pipelineLayout);
		file.close();
	}*/

