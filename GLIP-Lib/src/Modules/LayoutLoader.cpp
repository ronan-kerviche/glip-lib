/*****************************************************************************************************************/
/**                                                                                                             **/
/**    GLIP-LIB                                                                                                 **/
/**    OpenGL Image Processing LIBrary                                                                          **/
/**                                                                                                             **/
/**    Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                     **/
/**    LICENSE       : GPLv3                                                                                    **/
/**    Website       : http://sourceforge.net/projects/glip-lib/                                                **/
/**                                                                                                             **/
/**    File          : LayoutLoader.cpp                                                                         **/
/**    Original Date : October 17th 2010                                                                        **/
/**                                                                                                             **/
/**    Description   : Layout Loader from files or strings                                                      **/
/**                                                                                                             **/
/*****************************************************************************************************************/

	// Includes :
	#include <sstream>
	#include "Exception.hpp"
	#include "LayoutLoader.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Static variables :
	const char* LayoutLoader::keywords[NumKeywords] =  {	"TEXTURE_FORMAT",
								"SHADER_SOURCE",
								"FILTER_LAYOUT",
								"PIPELINE_LAYOUT",
								"PIPELINE_MAIN",
								"INCLUDE_FILE",
								"FILTER_INSTANCE",
								"PIPELINE_INSTANCE",
								"CONNECTION",
								"INPUT_PORTS",
								"OUTPUT_PORTS",
								"THIS"
							};

	// Functions :
	LayoutLoader::LayoutLoader(void)
	{
		entryType.clear();
		entryName.clear();
		entryCode.clear();
		formatList.clear();
		sourceList.clear();
		filterList.clear();
		pipelineList.clear();
	}

	LayoutLoader::~LayoutLoader(void)
	{
		clean();
	}

	void LayoutLoader::clean(void)
	{
		entryType.clear();
		entryName.clear();
		entryCode.clear();

		for(std::map<std::string,HdlTextureFormat*>::iterator it = formatList.begin(); it!=formatList.end(); it++)
			delete (*it).second;
		formatList.clear();

		for(std::map<std::string,ShaderSource*>::iterator it = sourceList.begin(); it!=sourceList.end(); it++)
			delete (*it).second;
		sourceList.clear();

		for(std::map<std::string,FilterLayout*>::iterator it = filterList.begin(); it!=filterList.end(); it++)
			delete (*it).second;
		filterList.clear();

		for(std::map<std::string,PipelineLayout*>::iterator it = pipelineList.begin(); it!=pipelineList.end(); it++)
			delete (*it).second;
		pipelineList.clear();
	}

	LoaderKeyword LayoutLoader::getKeyword(const std::string& str)
	{
		for(int i=0; i<NumKeywords; i++)
			if(keywords[i]==str) return static_cast<LoaderKeyword>(i);

		throw Exception("LayoutLoader::getKeyword - Unknown keyword : <" + str + ">", __FILE__, __LINE__);
	}

	void LayoutLoader::removeCommentary(std::string& source, std::string start, std::string end)
	{
		size_t posC1 = source.find(start);

		while(posC1!=std::string::npos)
		{
			size_t next = source.find(end, posC1+start.size());

			if(next==std::string::npos)
				throw Exception("LayoutLoader::removeCommentary - Unterminated commentary", __FILE__, __LINE__);

			source.erase(source.begin()+posC1,source.begin()+next+end.size());

			posC1 = source.find(start);
		}
	}

	std::string LayoutLoader::getSource(const std::string& sourceName)
	{
		std::string source;

		// Find newline :
		size_t newline = sourceName.find('\n');

		if(newline==std::string::npos)
		{
			// Open File
			std::fstream file;
			file.open(sourceName.c_str());

			// Did it fail?
			if(!file.is_open())
				throw Exception("LayoutLoader::getSource - Can't open file for reading : " + sourceName, __FILE__, __LINE__);

			// Set starting position
			file.seekg(0, std::ios::beg);

			std::string line;
			while(std::getline(file,line))
			{
				source += line;
				source += "\n";
			}
		}
		else // This is the source :
			source = sourceName;

		// Remove all commentaries
		removeCommentary(source,"/*","*/");
		removeCommentary(source,"//","\n");

		return source;
	}

	std::vector<std::string> LayoutLoader::getArguments(const std::string& code)
	{
		std::vector<std::string> res;
		size_t	current_pos 	= code.find('(')+1,
			max_pos		= code.find(')');

		while(current_pos!=std::string::npos && current_pos<max_pos)
		{
			size_t next_pos = code.find(',',current_pos);
			if(next_pos==std::string::npos) next_pos = max_pos;
			res.push_back(code.substr(current_pos,next_pos-current_pos));
			std::cout << "    Arg : " << res.back() << std::endl;
			current_pos = next_pos+1;
		}

		return res;
	}

	std::string LayoutLoader::getBody(const std::string& code)
	{
		size_t	start_pos 	= code.find('{'),
			end_pos		= code.rfind('}');

		if(start_pos==std::string::npos || end_pos==std::string::npos)
			return "";

		return code.substr(start_pos+1, end_pos-start_pos-1);
	}

	void LayoutLoader::updateEntriesLists(const std::string& sourceName, bool slave)
	{
		std::string source = getSource(sourceName);
		std::string line;
		size_t 	current_pos = 0,
			pos_sc = 0,
			pos_ac = 0;

		std::cout << source << std::endl;

		pos_sc = source.find(';', current_pos);
		pos_ac = source.find('{', current_pos);
		while(pos_sc!=std::string::npos && current_pos!=std::string::npos)
		{
			line.clear();

			//std::cout << "npos    : " << std::string::npos << std::endl;
			//std::cout << "current : " << current_pos << " (" << (int)(current_pos==std::string::npos) << ')' <<  std::endl;
			//std::cout << "pos_sc  : " << pos_sc << " (" << (int)(pos_sc==std::string::npos) << ')' << std::endl;
			//std::cout << "pos_ac  : " << pos_ac << std::endl;

			if(pos_ac<pos_sc || (pos_sc==std::string::npos && pos_ac!=std::string::npos))
			{
				//size_t next_pos = source.find('}', pos_ac);
				size_t next_pos = pos_ac+1;
				int level = 0;
				for( ; source.begin() + next_pos<source.end(); next_pos++)
				{
					if(source[next_pos]=='}' && level==0) break;
					if(source[next_pos]=='{') level++;
					if(source[next_pos]=='}' && level>0) level--;
				}

				if(source.begin() + next_pos == source.end())
					throw Exception("LayoutLoader::updateEntriesLists - '}' missing", __FILE__, __LINE__);
				line = source.substr(current_pos, next_pos-current_pos+1);
				current_pos = next_pos + 1;
			}
			else
			{
				line = source.substr(current_pos, pos_sc-current_pos+1);
				current_pos = pos_sc + 1;
			}

			// Process line
			int working = 0;
			std::cout << "Line before : "<< line << std::endl;
			for(std::string::iterator it=line.begin() ; it<line.end(); it++)
			{
				if( *it=='{' ) working++;
				if( *it=='}' ) working--;
				if( (*it==' ' || *it=='\t' || *it=='\n' || *it=='\r') && working==0) it = line.erase(it) - 1;
			}

			// Show the line :
			std::cout << "Line after : " << line << std::endl;

			// Find the first element of the line :
			size_t nameDelim = line.find(':');
			size_t argDelim  = line.find('(');

			if(argDelim==std::string::npos)
				throw Exception("LayoutLoader::updateEntriesLists - Element without argument", __FILE__, __LINE__);

			if(nameDelim!=std::string::npos)
			{
				std::string type = line.substr(0,nameDelim);
				std::string name = line.substr(nameDelim+1, argDelim-nameDelim-1);
				std::string code = line.substr(argDelim);
				LoaderKeyword key = getKeyword(type);

				std::cout << "Type : " << type << std::endl;
				std::cout << "Name : " << name << std::endl;
				std::cout << "Code : " << code << std::endl;
				std::cout << "Key  : " << key << std::endl;

				if(slave && key==PIPELINE_MAIN)
					key = PIPELINE_LAYOUT;

				entryType.push_back(key);
				entryName.push_back(name);
				entryCode.push_back(code);
			}
			else
			{
				std::string type = line.substr(0,argDelim);
				std::string code = line.substr(argDelim);
				std::cout << "Type : " << type << std::endl;
				std::cout << "NO Name"<< std::endl;
				std::cout << "Code : " << code << std::endl;
				std::cout << "Key  : " << getKeyword(type) << std::endl;
				if(getKeyword(type)!=INCLUDE_FILE)
					throw Exception("LayoutLoader::updateEntriesLists - Missing name for element of type " + type, __FILE__, __LINE__);

				entryType.push_back(getKeyword(type));
				entryName.push_back("NONAME");
				entryCode.push_back(code);
			}

			// Find the next line :
			pos_sc = source.find(';', current_pos);
			pos_ac = source.find('{', current_pos);
		}
	}

	HdlTextureFormat* LayoutLoader::buildFormat(const std::string& code, const std::string& name)
	{
		std::vector<std::string> arg = getArguments(code);
		if(arg.size()<6)
			throw Exception("LayoutLoader::buildFormat - Too few arguments for format " + name, __FILE__, __LINE__);
		if(arg.size()>10)
			throw Exception("LayoutLoader::buildFormat - Too much arguments for format " + name, __FILE__, __LINE__);

		int w, h;
		GLenum c, d, mf, Mf;

		if( !from_string(arg[0],w) )
			throw Exception("LayoutLoader::buildFormat - enable to read integers for width for " + name + ", got : " + arg[0], __FILE__, __LINE__);
		if( !from_string(arg[1],h) )
			throw Exception("LayoutLoader::buildFormat - enable to read integers for height for " + name + ", got : " + arg[1], __FILE__, __LINE__);

		c = gl_from_string(arg[2]);
		d = gl_from_string(arg[3]);
		if( c==GL_FALSE )
			throw Exception("LayoutLoader::buildFormat - Unable to read flags for channel layout for " + name + ", got : " + arg[2], __FILE__, __LINE__);
		if( d==GL_FALSE )
			throw Exception("LayoutLoader::buildFormat - Unable to read flags for bit depth for " + name + ", got : " + arg[3], __FILE__, __LINE__);

		mf = gl_from_string(arg[4]);
		Mf = gl_from_string(arg[5]);
		if( mf==GL_FALSE )
			throw Exception("LayoutLoader::buildFormat - Unable to read flags for min filtering for " + name + ", got : " + arg[4], __FILE__, __LINE__);
		if( Mf==GL_FALSE )
			throw Exception("LayoutLoader::buildFormat - Unable to read flags for max filtering for " + name + ", got : " + arg[5], __FILE__, __LINE__);

		// Create result :
		HdlTextureFormat* result = new HdlTextureFormat(w,h,c,d,mf,Mf);

		// Improvements :
		if(arg.size()>=7)
		{
			GLenum sw = gl_from_string(arg[6]);

			if( sw==GL_FALSE )
				throw Exception("LayoutLoader::buildFormat - Unable to read SWrapping option for format " + name + ", got : " + arg[6], __FILE__, __LINE__);

			result->setSWrapping(sw);
		}

		if(arg.size()>=8)
		{
			GLenum tw = gl_from_string(arg[7]);

			if( tw==GL_FALSE )
				throw Exception("LayoutLoader::buildFormat - Unable to read TWrapping option for format " + name + ", got : " + arg[7], __FILE__, __LINE__);

			result->setSWrapping(tw);
		}

		if(arg.size()>=9)
		{
			int l;

			if( !from_string(arg[8],l) )
				throw Exception("LayoutLoader::buildFormat - Unable to read max mipmapping level for format " + name + ", got : " + arg[8], __FILE__, __LINE__);

			result->setMaxLevel(l);
		}

		if(arg.size()>=10)
		{
			int l;

			if( !from_string(arg[9],l) )
				throw Exception("LayoutLoader::buildFormat - Unable to read base mipmapping level for format " + name + ", got : " + arg[9], __FILE__, __LINE__);

			result->setBaseLevel(l);
		}

		return result;
	}

	ShaderSource* LayoutLoader::buildShaderSource(const std::string& code, const std::string& name)
	{
		std::vector<std::string> arg = getArguments(code);

		if(arg.size()==0)
			return new ShaderSource(getBody(code));
		else
			return new ShaderSource(arg[0]);
	}

	FilterLayout* LayoutLoader::buildFilter(const std::string& code, const std::string& name)
	{
		std::vector<std::string> arg = getArguments(code);

		if(arg.size()!=2)
			throw Exception("LayoutLoader::buildFilter - Wrong number of arguments for " + name, __FILE__, __LINE__);

		std::map<std::string, HdlTextureFormat*>::iterator it0 = formatList.find(arg[0]);
		if(it0==formatList.end())
			throw Exception("LayoutLoader::buildFilter - Unknown format : " + arg[0] + " for " + name, __FILE__, __LINE__);

		std::map<std::string, ShaderSource*>::iterator it1 = sourceList.find(arg[1]);
		if(it1==sourceList.end())
			throw Exception("LayoutLoader::buildFilter - Unknown shader source : " + arg[1] + " for " + name, __FILE__, __LINE__);

		return new FilterLayout(name, *it0->second, *it1->second);
	}

	PipelineLayout* LayoutLoader::buildPipeline(std::string code, const std::string& name)
	{
		int 	inputDescription = 0,
			outputDescription = 0;
		std::vector<LoaderKeyword> commandList;
		std::vector<std::string>   nameList;
		std::vector<std::string>   argumentList;

		// Process the code to remove all useless caracters :
		for(std::string::iterator it=code.begin() ; it<code.end(); it++)
			if(*it==' ' || *it=='\t' || *it=='\n' || *it=='\r') it = code.erase(it) - 1;

		// Create a list of commands :
		size_t 	starting_pos = code.find('{'),
			ending_pos   = code.rfind('}'),
			current_pos  = starting_pos+1;

		while(current_pos!=std::string::npos && current_pos<ending_pos)
		{
			size_t next_pos = code.find(';', current_pos);
			if(next_pos==std::string::npos) next_pos = ending_pos;
			std::string line = code.substr(current_pos, next_pos-current_pos);

			std::cout << "Line : " << line << std::endl;

			// Extract info from the line :
			size_t nameDelim = line.find(':');
			size_t argDelim  = line.find('(');

			if(argDelim==std::string::npos)
				throw Exception("LayoutLoader::buildPipeline - Instruction has no arguments : " + line, __FILE__, __LINE__);

			if(nameDelim!=std::string::npos)
			{
				std::string type = line.substr(0,nameDelim);
				std::string name = line.substr(nameDelim+1, argDelim-nameDelim-1);
				std::string code = line.substr(argDelim);
				LoaderKeyword key = getKeyword(type);

				if(key!=FILTER_INSTANCE && key!=PIPELINE_INSTANCE)
					throw Exception("LayoutLoader::buildPipeline - Error while using type " + type + " in a pipeline description", __FILE__, __LINE__);

				commandList.push_back(key);
				nameList.push_back(name);
				argumentList.push_back(code);
			}
			else
			{
				std::string type = line.substr(0,argDelim);
				std::string code = line.substr(argDelim);
				LoaderKeyword key = getKeyword(type);

				if(getKeyword(type)!=CONNECTION && getKeyword(type)!=INPUT_PORTS && getKeyword(type)!=OUTPUT_PORTS)
					throw Exception("LayoutLoader::buildPipeline - Missing name for element of type " + type, __FILE__, __LINE__);

				if(key==INPUT_PORTS)  inputDescription++;
				if(key==OUTPUT_PORTS) outputDescription++;

				commandList.push_back(key);
				nameList.push_back("NONAME");
				argumentList.push_back(code);
			}

			current_pos = next_pos+1;
		}

		// Check input and output description :
		if(inputDescription!=1 || outputDescription!=1)
			throw Exception("LayoutLoader::buildPipeline - Missing or redefined input or output port descriptions for pipeline " + name, __FILE__, __LINE__);

		// Find the Input ports
		std::vector<std::string> inputPorts;
		for(int i=0; i<commandList.size(); i++)
		{
			if(commandList[i]==INPUT_PORTS)
			{
				inputPorts = getArguments(argumentList[i]);
				break;
			}
		}

		// Find the Output ports
		std::vector<std::string> outputPorts;
		for(int i=0; i<commandList.size(); i++)
		{
			if(commandList[i]==OUTPUT_PORTS)
			{
				outputPorts = getArguments(argumentList[i]);
				break;
			}
		}

		// Create the resulting pipeline :
		PipelineLayout* result = new PipelineLayout(name);

		for(int i=0; i<inputPorts.size(); i++)
			result->addInput(inputPorts[i]);

		for(int i=0; i<outputPorts.size(); i++)
			result->addOutput(outputPorts[i]);

		// Load all the filters and other pipelines :
		for(int i=0; i<commandList.size(); i++)
		{
			std::vector<std::string> arg;
			std::map<std::string, FilterLayout*>::iterator itf;
			std::map<std::string, PipelineLayout*>::iterator itp;

			switch(commandList[i])
			{
				case FILTER_INSTANCE:
					arg = getArguments(argumentList[i]);
					if(arg.size()!=1)
						throw Exception("LayoutLoader::buildPipeline - Filter instance as a wrong number of arguments, for " + nameList[i] + " in " + name, __FILE__, __LINE__);
					// Check if the filter exists :
					itf = filterList.find(arg[0]);
					if(itf==filterList.end())
						throw Exception("LayoutLoader::buildPipeline - Undefined reference to filter : " + arg[0] + " for " + nameList[i] + " in " + name, __FILE__, __LINE__);
					// Add it to pipeline :
					result->add(*itf->second, nameList[i]);
					break;
				case PIPELINE_INSTANCE:
					arg = getArguments(argumentList[i]);
					if(arg.size()!=1)
						throw Exception("LayoutLoader::buildPipeline - Pipeline instance as a wrong number of arguments, for " + nameList[i] + " in " + name, __FILE__, __LINE__);
					// Check if the filter exists :
					itp = pipelineList.find(arg[0]);
					if(itp==pipelineList.end())
						throw Exception("LayoutLoader::buildPipeline - Undefined reference to pipeline : " + arg[0] + " for " + nameList[i] + " in " + name, __FILE__, __LINE__);
					// Add it to pipeline :
					result->add(*itp->second, nameList[i]);
					break;
				default:
					break;
			}
		}

		// Make the connections :
		try
		{
			for(int i=0; i<commandList.size(); i++)
			{
				if(commandList[i]==CONNECTION)
				{
					std::vector<std::string> arg = getArguments(argumentList[i]);
					if(arg.size()!=4)
						throw Exception("LayoutLoader::buildPipeline - Wrong number of arguments for a connection in " + name, __FILE__, __LINE__);

					if(arg[0]==keywords[THIS]) // connection to input
					{
						result->connectToInput(arg[1], arg[2], arg[3]);
					}
					else if(arg[2]==keywords[THIS]) // connection to output
					{
						result->connectToOutput(arg[0], arg[1], arg[3]);
					}
					else
						result->connect(arg[0], arg[1], arg[2], arg[3]);
				}
			}
		}
		catch(std::exception& e)
		{
			Exception m("LayoutLoader::buildPipeline - Caught an exception while creating connections in " + name, __FILE__, __LINE__);
			throw m+e;
		}

		return result;
	}

	PipelineLayout* LayoutLoader::operator()(const std::string& source)
	{
		// Doesn't check for cylclic inclusion!

		try
		{

			// doesn't include previously loaded items :
			clean();
			updateEntriesLists(source);

			// Find all the INCLUDE and expand them :
			for(int i=0; i<entryType.size(); i++)
			{
				if(entryType[i]==INCLUDE_FILE)
				{
					std::vector<std::string> arg = getArguments(entryCode[i]);
					if(arg.size()<1)
						throw Exception("LayoutLoader::operator() - Not enough argument for INCLUDE_FILE", __FILE__, __LINE__);
					if(arg.size()>1)
						throw Exception("LayoutLoader::operator() - Too much arguments for INCLUDE_FILE", __FILE__, __LINE__);
					std::cout << "Including file : " << arg[0] << std::endl;

					// Creqte new loader :
					LayoutLoader l;
					l.updateEntriesLists(arg[0], true);

					// append its lists to the EntryList :
					entryType.insert(entryType.end(), l.entryType.begin(), l.entryType.end());
					entryName.insert(entryName.end(), l.entryName.begin(), l.entryName.end());
					entryCode.insert(entryCode.end(), l.entryCode.begin(), l.entryCode.end());
				}
			}

			std::cout << "Num contents : " << entryType.size() << std::endl;

			// Remove any corresponding double (meaning that a file was loaded from two separate locations) :
			for(int i=0; i<entryType.size(); i++)
			{
				for(int j=i+1; j<entryType.size(); j++)
				{
					if(entryType[j]==entryType[i] && entryName[j]==entryName[i] && entryCode[j]==entryCode[i])
					{
						entryType.erase(entryType.begin()+j);
						entryName.erase(entryName.begin()+j);
						entryCode.erase(entryCode.begin()+j);
						j--;
					}
				}
			}

			// Check double name :
			for(int i=0; i<entryType.size(); i++)
			{
				for(int j=i+1; j<entryType.size(); j++)
				{
					if(entryType[j]==entryType[i] && entryName[j]==entryName[i])
						throw Exception("LayoutLoader::operator() - Found two different entities of same type and name : " + entryName[j], __FILE__, __LINE__);
				}
			}

			// List all the formats :
			for(int i=0; i<entryType.size(); i++)
			{
				if(entryType[i]==FORMAT_LAYOUT)
					formatList[entryName[i]] = buildFormat(entryCode[i],entryName[i]);
			}

			// List all shaders :
			for(int i=0; i<entryType.size(); i++)
			{
				if(entryType[i]==SHADER_SOURCE)
					sourceList[entryName[i]] = buildShaderSource(entryCode[i],entryName[i]);
			}

			// List all Filters :
			for(int i=0; i<entryType.size(); i++)
			{
				if(entryType[i]==FILTER_LAYOUT)
					filterList[entryName[i]] = buildFilter(entryCode[i],entryName[i]);
			}

			// List all pipelines :
			for(int i=0; i<entryType.size(); i++)
			{
				if(entryType[i]==PIPELINE_LAYOUT)
					pipelineList[entryName[i]] = buildPipeline(entryCode[i],entryName[i]);
			}

			// Find Main Pipeline :
			int main=-1;

			for(int i=0; i<entryType.size(); i++)
				if(entryType[i]==PIPELINE_MAIN)
				{
					main = i;
					break;
				}

			if(main==-1)
				throw Exception("LayoutLoader::operator() - No main pipeline defined!",__FILE__,__LINE__);

			PipelineLayout* result = buildPipeline(entryCode[main],entryName[main]);
			clean();

			return result;
		}
		catch(std::exception& e)
		{
			clean();
			Exception m("LayoutLoader::operator() - Caught an exception while building pipeline layout.", __FILE__, __LINE__);
			throw m+e;
		}
	}

