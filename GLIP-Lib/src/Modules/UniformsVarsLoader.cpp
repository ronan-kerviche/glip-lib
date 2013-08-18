/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : UniformsVarsLoader.cpp                                                                     */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Uniforms variables save/load.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    UniformsVarsLoader.cpp
 * \brief   Uniforms variables save/load.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

	// Includes :
	#include <cstring>
	#include "Core/Exception.hpp"
	#include "Modules/UniformsVarsLoader.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

	// Static variables :
	const char* Glip::Modules::keywordsUniformsVarsLoader[UL_NumKeywords] =  {	"PIPELINE",
											"FILTER",
											"GL_FLOAT",
											"GL_FLOAT_VEC2",
											"GL_FLOAT_VEC3",
											"GL_FLOAT_VEC4",
											"GL_DOUBLE",
											"GL_DOUBLE_VEC2",
											"GL_DOUBLE_VEC3",
											"GL_DOUBLE_VEC4",
											"GL_INT",
											"GL_INT_VEC2",
											"GL_INT_VEC3",
											"GL_INT_VEC4",
											"GL_UNSIGNED_INT",
											"GL_UNSIGNED_INT_VEC2",
											"GL_UNSIGNED_INT_VEC3",
											"GL_UNSIGNED_INT_VEC4",
											"GL_BOOL",
											"GL_BOOL_VEC2",
											"GL_BOOL_VEC3",
											"GL_BOOL_VEC4",
											"GL_FLOAT_MAT2",
											"GL_FLOAT_MAT3",
											"GL_FLOAT_MAT4"
										};

// Nodes :
	UniformsVarsLoader::Ressource::Ressource(void)
	 : type(GL_NONE), data(NULL)
	{ }

	UniformsVarsLoader::Ressource::Ressource(const Ressource& cpy)
	 : name(cpy.name), type(cpy.type), data(NULL)
	{
		if(cpy.data!=NULL)
		{
			#define COPY_ELEMENT( glType, cType, narg ) \
				if(type== glType ) \
				{ \
					cType* tmp = new cType [ narg ]; \
					std::memcpy(tmp, cpy.data, narg * sizeof( cType ) ); \
					data = reinterpret_cast<void*>( tmp ); \
				}

				COPY_ELEMENT( GL_FLOAT, 		float, 		1 )
			else	COPY_ELEMENT( GL_FLOAT_VEC2, 		float,		2 )
			else	COPY_ELEMENT( GL_FLOAT_VEC3, 		float, 		3 )
			else	COPY_ELEMENT( GL_FLOAT_VEC4,		float, 		4 )
			else	COPY_ELEMENT( GL_DOUBLE,		double,		1 )
			else	COPY_ELEMENT( GL_DOUBLE_VEC2,		double,		2 )
			else	COPY_ELEMENT( GL_DOUBLE_VEC3,		double,		3 )
			else	COPY_ELEMENT( GL_DOUBLE_VEC4,		double,		4 )
			else	COPY_ELEMENT( GL_INT,			int,		1 )
			else	COPY_ELEMENT( GL_INT_VEC2,		int,		2 )
			else	COPY_ELEMENT( GL_INT_VEC3,		int,		3 )
			else	COPY_ELEMENT( GL_INT_VEC4,		int,		4 )
			else	COPY_ELEMENT( GL_UNSIGNED_INT,		unsigned int,	1 )
			else	COPY_ELEMENT( GL_UNSIGNED_INT_VEC2,	unsigned int,	2 )
			else	COPY_ELEMENT( GL_UNSIGNED_INT_VEC3,	unsigned int,	3 )
			else	COPY_ELEMENT( GL_UNSIGNED_INT_VEC4,	unsigned int,	4 )
			else	COPY_ELEMENT( GL_BOOL,			int,		1 )
			else	COPY_ELEMENT( GL_BOOL_VEC2,		int,		2 )
			else	COPY_ELEMENT( GL_BOOL_VEC3,		int,		3 )
			else	COPY_ELEMENT( GL_BOOL_VEC4,		int,		4 )
			else	COPY_ELEMENT( GL_FLOAT_MAT2,		float,		4 )
			else	COPY_ELEMENT( GL_FLOAT_MAT3,		float,		9 )
			else	COPY_ELEMENT( GL_FLOAT_MAT4,		float,		16 )
			else
				throw Exception("Ressource::Ressource - Unknown type \"" + glParamName(type) + "\" for element \"" + name + "\".", __FILE__, __LINE__);

			#undef COPY_ELEMENT
		}
	}

	UniformsVarsLoader::Ressource::~Ressource(void)
	{
		if(data!=NULL)
		{
			#define DELETE_DATA( glType, cType ) \
				if( type == glType ) \
				{ \
					cType* tmp = reinterpret_cast< cType* >( data ); \
					data = NULL; \
					delete[] tmp; \
				}

				DELETE_DATA( GL_FLOAT, 			float)
			else	DELETE_DATA( GL_FLOAT_VEC2, 		float)
			else	DELETE_DATA( GL_FLOAT_VEC3, 		float)
			else	DELETE_DATA( GL_FLOAT_VEC4,		float)
			else	DELETE_DATA( GL_DOUBLE,			double)
			else	DELETE_DATA( GL_DOUBLE_VEC2,		double)
			else	DELETE_DATA( GL_DOUBLE_VEC3,		double)
			else	DELETE_DATA( GL_DOUBLE_VEC4,		double)
			else	DELETE_DATA( GL_INT,			int)
			else	DELETE_DATA( GL_INT_VEC2,		int)
			else	DELETE_DATA( GL_INT_VEC3,		int)
			else	DELETE_DATA( GL_INT_VEC4,		int)
			else	DELETE_DATA( GL_UNSIGNED_INT,		unsigned int)
			else	DELETE_DATA( GL_UNSIGNED_INT_VEC2,	unsigned int)
			else	DELETE_DATA( GL_UNSIGNED_INT_VEC3,	unsigned int)
			else	DELETE_DATA( GL_UNSIGNED_INT_VEC4,	unsigned int)
			else	DELETE_DATA( GL_BOOL,			int)
			else	DELETE_DATA( GL_BOOL_VEC2,		int)
			else	DELETE_DATA( GL_BOOL_VEC3,		int)
			else	DELETE_DATA( GL_BOOL_VEC4,		int)
			else	DELETE_DATA( GL_FLOAT_MAT2,		float)
			else	DELETE_DATA( GL_FLOAT_MAT3,		float)
			else	DELETE_DATA( GL_FLOAT_MAT4,		float)
			else
				throw Exception("Ressource::~Ressource - Unknown type " + glParamName(type) + ".", __FILE__, __LINE__);

			#undef DELETE_DATA
		}
	}

	void UniformsVarsLoader::Ressource::build(const VanillaParserSpace::Element& e)
	{
		if(e.noName || e.name.empty())
			throw Exception("From line " + to_string(e.startLine) + " : The element of type \"" + e.strKeyword + "\" should have a name.", __FILE__, __LINE__);

		if(!e.noBody)
			throw Exception("From line " + to_string(e.startLine) + " : Element \"" + e.name + "\" should not have a body.", __FILE__, __LINE__);

		// Set the name :
		name = e.name;

		#define READ_ELEMENT( glType, cType, narg ) \
			if(e.strKeyword==keywordsUniformsVarsLoader[KW_UL_##glType]) \
			{ \
				type = glType; \
				cType* tmp = new cType [ narg ]; \
				\
				if( e.arguments.size()!=narg ) \
					throw Exception("From line " + to_string(e.startLine) + " : Element \"" + e.name + "\" of type \"" + e.strKeyword + "\" has " + to_string(e.arguments.size()) + " while it should have exactly " + to_string(narg) + ".", __FILE__, __LINE__); \
				\
				for(int k=0; k<narg; k++) \
				{ \
					if( !from_string( e.arguments[k], tmp[k] ) ) \
						throw Exception("From line " + to_string(e.startLine) + " : Unable to read parameter " + to_string(k) + " in element \"" + e.name + "\" of type \"" + e.strKeyword + "\" (token : \"" + e.arguments[k] + "\").", __FILE__, __LINE__); \
				} \
				\
				data = reinterpret_cast<void*>(tmp); \
			}

		// Load the data :
			READ_ELEMENT( GL_FLOAT, 		float, 		1 )
		else	READ_ELEMENT( GL_FLOAT_VEC2, 		float,		2 )
		else	READ_ELEMENT( GL_FLOAT_VEC3, 		float, 		3 )
		else	READ_ELEMENT( GL_FLOAT_VEC4,		float, 		4 )
		else	READ_ELEMENT( GL_DOUBLE,		double,		1 )
		else	READ_ELEMENT( GL_DOUBLE_VEC2,		double,		2 )
		else	READ_ELEMENT( GL_DOUBLE_VEC3,		double,		3 )
		else	READ_ELEMENT( GL_DOUBLE_VEC4,		double,		4 )
		else	READ_ELEMENT( GL_INT,			int,		1 )
		else	READ_ELEMENT( GL_INT_VEC2,		int,		2 )
		else	READ_ELEMENT( GL_INT_VEC3,		int,		3 )
		else	READ_ELEMENT( GL_INT_VEC4,		int,		4 )
		else	READ_ELEMENT( GL_UNSIGNED_INT,		unsigned int,	1 )
		else	READ_ELEMENT( GL_UNSIGNED_INT_VEC2,	unsigned int,	2 )
		else	READ_ELEMENT( GL_UNSIGNED_INT_VEC3,	unsigned int,	3 )
		else	READ_ELEMENT( GL_UNSIGNED_INT_VEC4,	unsigned int,	4 )
		else	READ_ELEMENT( GL_BOOL,			int,		1 )
		else	READ_ELEMENT( GL_BOOL_VEC2,		int,		2 )
		else	READ_ELEMENT( GL_BOOL_VEC3,		int,		3 )
		else	READ_ELEMENT( GL_BOOL_VEC4,		int,		4 )
		else	READ_ELEMENT( GL_FLOAT_MAT2,		float,		4 )
		else	READ_ELEMENT( GL_FLOAT_MAT3,		float,		9 )
		else	READ_ELEMENT( GL_FLOAT_MAT4,		float,		16 )
		else
			throw Exception("From line " + to_string(e.startLine) + " : Unknown type \"" + e.strKeyword + "\" for element \"" + e.name + "\".", __FILE__, __LINE__);

		#undef READ_ELEMENT
	}

	void UniformsVarsLoader::Ressource::build(const std::string& varName, GLenum t, HdlProgram& prgm)
	{
		name = varName;

		#define ALLOC_ELEMENT( glType, cType, narg ) \
			if( t==glType ) \
			{ \
				type = glType; \
				cType* tmp = new cType [ narg ]; \
				\
				prgm.getVar(varName, tmp); \
				\
				data = reinterpret_cast<void*>(tmp); \
			}

			ALLOC_ELEMENT( GL_FLOAT, 		float, 		1 )
		else	ALLOC_ELEMENT( GL_FLOAT_VEC2, 		float,		2 )
		else	ALLOC_ELEMENT( GL_FLOAT_VEC3, 		float, 		3 )
		else	ALLOC_ELEMENT( GL_FLOAT_VEC4,		float, 		4 )
		/*else	ALLOC_ELEMENT( GL_DOUBLE,		double,		1 )
		else	ALLOC_ELEMENT( GL_DOUBLE_VEC2,		double,		2 )
		else	ALLOC_ELEMENT( GL_DOUBLE_VEC3,		double,		3 )
		else	ALLOC_ELEMENT( GL_DOUBLE_VEC4,		double,		4 )*/
		else	ALLOC_ELEMENT( GL_INT,			int,		1 )
		else	ALLOC_ELEMENT( GL_INT_VEC2,		int,		2 )
		else	ALLOC_ELEMENT( GL_INT_VEC3,		int,		3 )
		else	ALLOC_ELEMENT( GL_INT_VEC4,		int,		4 )
		else	ALLOC_ELEMENT( GL_UNSIGNED_INT,		unsigned int,	1 )
		else	ALLOC_ELEMENT( GL_UNSIGNED_INT_VEC2,	unsigned int,	2 )
		else	ALLOC_ELEMENT( GL_UNSIGNED_INT_VEC3,	unsigned int,	3 )
		else	ALLOC_ELEMENT( GL_UNSIGNED_INT_VEC4,	unsigned int,	4 )
		else	ALLOC_ELEMENT( GL_BOOL,			int,		1 )
		else	ALLOC_ELEMENT( GL_BOOL_VEC2,		int,		2 )
		else	ALLOC_ELEMENT( GL_BOOL_VEC3,		int,		3 )
		else	ALLOC_ELEMENT( GL_BOOL_VEC4,		int,		4 )
		else	ALLOC_ELEMENT( GL_FLOAT_MAT2,		float,		4 )
		else	ALLOC_ELEMENT( GL_FLOAT_MAT3,		float,		9 )
		else	ALLOC_ELEMENT( GL_FLOAT_MAT4,		float,		16 )
		else
			throw Exception("Ressource::build - Unknown type \"" + glParamName(t) + "\" for element \"" + varName + "\".", __FILE__, __LINE__);

		#undef ALLOC_ELEMENT
	}

	void UniformsVarsLoader::Ressource::apply(Filter& filter)
	{
		#define APPLY( glType, cType ) \
			if( type == glType ) \
				filter.prgm().modifyVar(name, glType, reinterpret_cast< cType* >(data));

			APPLY( GL_FLOAT, 			float)
		else	APPLY( GL_FLOAT_VEC2, 			float)
		else	APPLY( GL_FLOAT_VEC3, 			float)
		else	APPLY( GL_FLOAT_VEC4,			float)
		/*else	APPLY( GL_DOUBLE,			double)
		else	APPLY( GL_DOUBLE_VEC2,			double)
		else	APPLY( GL_DOUBLE_VEC3,			double)
		else	APPLY( GL_DOUBLE_VEC4,			double)*/
		else	APPLY( GL_INT,				int)
		else	APPLY( GL_INT_VEC2,			int)
		else	APPLY( GL_INT_VEC3,			int)
		else	APPLY( GL_INT_VEC4,			int)
		else	APPLY( GL_UNSIGNED_INT,			unsigned int)
		else	APPLY( GL_UNSIGNED_INT_VEC2,		unsigned int)
		else	APPLY( GL_UNSIGNED_INT_VEC3,		unsigned int)
		else	APPLY( GL_UNSIGNED_INT_VEC4,		unsigned int)
		else	APPLY( GL_BOOL,				int)
		else	APPLY( GL_BOOL_VEC2,			int)
		else	APPLY( GL_BOOL_VEC3,			int)
		else	APPLY( GL_BOOL_VEC4,			int)
		else	APPLY( GL_FLOAT_MAT2,			float)
		else	APPLY( GL_FLOAT_MAT3,			float)
		else	APPLY( GL_FLOAT_MAT4,			float)
		else
			throw Exception("Ressource::apply - Unknown type " + glParamName(type) + ".", __FILE__, __LINE__);

		#undef APPLY
	}

	VanillaParserSpace::Element UniformsVarsLoader::Ressource::getCode(void) const
	{
		#define GET_CODE( glType, cType, narg ) \
			if( type == glType ) \
			{ \
				VanillaParserSpace::Element e; \
				e.strKeyword 	= glParamName(type); \
				e.name		= name; \
				e.noName	= false; \
				e.noArgument	= false; \
				e.noBody	= true; \
				e.body.clear(); \
				\
				cType* tmp = reinterpret_cast< cType* >( data ); \
				\
				for(int k=0; k< narg ; k++) \
					e.arguments.push_back( to_string(tmp[k]) ); \
				\
				return e; \
			}

			GET_CODE( GL_FLOAT, 		float, 		1 )
		else	GET_CODE( GL_FLOAT_VEC2, 	float,		2 )
		else	GET_CODE( GL_FLOAT_VEC3, 	float, 		3 )
		else	GET_CODE( GL_FLOAT_VEC4,	float, 		4 )
		/*else	GET_CODE( GL_DOUBLE,		double,		1 )
		else	GET_CODE( GL_DOUBLE_VEC2,	double,		2 )
		else	GET_CODE( GL_DOUBLE_VEC3,	double,		3 )
		else	GET_CODE( GL_DOUBLE_VEC4,	double,		4 )*/
		else	GET_CODE( GL_INT,		int,		1 )
		else	GET_CODE( GL_INT_VEC2,		int,		2 )
		else	GET_CODE( GL_INT_VEC3,		int,		3 )
		else	GET_CODE( GL_INT_VEC4,		int,		4 )
		else	GET_CODE( GL_UNSIGNED_INT,	unsigned int,	1 )
		else	GET_CODE( GL_UNSIGNED_INT_VEC2,	unsigned int,	2 )
		else	GET_CODE( GL_UNSIGNED_INT_VEC3,	unsigned int,	3 )
		else	GET_CODE( GL_UNSIGNED_INT_VEC4,	unsigned int,	4 )
		else	GET_CODE( GL_BOOL,		int,		1 )
		else	GET_CODE( GL_BOOL_VEC2,		int,		2 )
		else	GET_CODE( GL_BOOL_VEC3,		int,		3 )
		else	GET_CODE( GL_BOOL_VEC4,		int,		4 )
		else	GET_CODE( GL_FLOAT_MAT2,	float,		4 )
		else	GET_CODE( GL_FLOAT_MAT3,	float,		9 )
		else	GET_CODE( GL_FLOAT_MAT4,	float,		16 )
		else
			throw Exception("Ressource::getCode - Unknown type \"" + glParamName(type) + "\" for element \"" + name + "\".", __FILE__, __LINE__);

		#undef GET_CODE
	}

	int UniformsVarsLoader::RessourceNode::apply(Pipeline& pipeline, __ReadOnly_PipelineLayout& current)
	{
		int c = 0;

		for(int k=0; k<subNodes.size(); k++)
		{
			if(!current.doesElementExist(subNodes[k].name))
				throw Exception("Missing element \"" + subNodes[k].name + "\" in " + current.getFullName() + ".", __FILE__, __LINE__);

			int id = current.getElementIndex(subNodes[k].name);

			if( current.getElementKind( id ) == __ReadOnly_PipelineLayout::PIPELINE )
				c += subNodes[k].apply( pipeline, current.pipelineLayout(id) );
			else if( current.getElementKind( id ) == __ReadOnly_PipelineLayout::FILTER )
			{
				if( !subNodes[k].subNodes.empty() )
					throw Exception("Element \"" + subNodes[k].name + "\" is a pipeline in the uniforms data and a filter in the Pipeline " + pipeline.getFullName() + ".", __FILE__, __LINE__);

				int gid = current.getElementID( id );
				Filter& filter = pipeline[gid];
				std::vector<Ressource>& ressources = subNodes[k].ressources;

				for(int l=0; l<ressources.size(); l++)
				{
					ressources[l].apply(filter);
				}

				c += ressources.size();
			}
			else
				throw Exception("Unknown component kind for element \"" + subNodes[k].name + "\".", __FILE__, __LINE__);
		}

		return c;
	}

	int UniformsVarsLoader::RessourceNode::getNumVariables(void) const
	{
		int c = 0;

		for(int k=0; k<subNodes.size(); k++)
			c += subNodes[k].getNumVariables();

		c += ressources.size();

		return c;
	}

// UniformsVarsLoader
	/**
	\fn UniformsVarsLoader::UniformsVarsLoader(void)
	\brief UniformsVarsLoader constructor.
	**/
	UniformsVarsLoader::UniformsVarsLoader(void)
	{ }

	UniformsVarsLoader::~UniformsVarsLoader(void)
	{
		// Clear :
		ressources.clear();
	}

	/**
	\fn void UniformsVarsLoader::load(std::string source, bool replace)
	\brief Loads a set of uniforms variables for one, or multiple, pipelines.
	\param source Either a filename or directly the source string (in this case it must contain at least one newline character '\\n').
	\param replace If set to true, and in the case that a pipeline with a similar name already exists, then the set of values is overwritten. Otherwise it will raise an Exception.
	**/
	void UniformsVarsLoader::load(std::string source, bool replace)
	{
		std::string filename;

		// Open the file :
		if(source.find('\n')==std::string::npos)
		{
			filename = source;

			// Load the file :
			std::fstream file;
			file.open(filename.c_str());

			// Did it fail?
			if(!file.is_open())
				throw Exception("UniformsVarsLoader::load - Cannot load file : \"" + filename + "\".", __FILE__, __LINE__);


			source.clear();

			// Set starting position
			file.seekg(0, std::ios::beg);

			std::string line;
			while(std::getline(file,line))
			{
				source += line;
				source += "\n";
			}

			file.close();
		}
		// else : "already loaded"

		// Do the first parse operation :
		try
		{
			VanillaParser parser(source);

			for(int k=0; k<parser.elements.size(); k++)
			{
				if(parser.elements[k].strKeyword!=keywordsUniformsVarsLoader[KW_UL_PIPELINE])
					throw Exception("From line " + to_string(parser.elements[k].startLine) + " : The element must be of type \"" + keywordsUniformsVarsLoader[KW_UL_PIPELINE] + "\" (current : \"" + parser.elements[k].strKeyword + "\").", __FILE__, __LINE__);

				if( parser.elements[k].noName || parser.elements[k].name.empty() )
					throw Exception("From line " + to_string(parser.elements[k].startLine) + " : The element must have a (non-empty) name.", __FILE__, __LINE__);

				if( parser.elements[k].noBody )
					throw Exception("From line " + to_string(parser.elements[k].startLine) + " : The element \"" + parser.elements[k].name + "\" must have a (non-empty) body.", __FILE__, __LINE__);

				RessourceNode* ptr = NULL;

				// Try to find a duplicate :
				for(int l=0; l<ressources.size(); l++)
				{
					if(ressources[l].name==parser.elements[k].name)
					{
						if(replace)
						{
							ressources[l].subNodes.clear();
							ressources[l].ressources.clear();
							ptr = &ressources[l];
						}
						else
							throw Exception("From line " + to_string(parser.elements[k].startLine) + " : An element with the name \"" + parser.elements[k].name + "\" has already been registered (replace=false).", __FILE__, __LINE__);
					}
				}

				if(ptr==NULL)
				{
					ressources.push_back( RessourceNode() );
					ptr = &ressources.back();
				}

				ptr->name = parser.elements[k].name;
				processNode(parser.elements[k].body, *ptr);
			}
		}
		catch(Exception& e)
		{
			if(!filename.empty())
			{
				Exception m("UniformsVarsLoader::UniformsVarsLoader - Unable to load uniforms data from file \"" + filename + "\" : ", __FILE__, __LINE__);
				throw m + e;
			}
			else
			{
				Exception m("UniformsVarsLoader::UniformsVarsLoader - Unable to load uniforms data : ", __FILE__, __LINE__);
				throw m + e;
			}
		}
	}

	/**
	\fn void UniformsVarsLoader::load(Pipeline& pipeline, bool replace)
	\brief Load the set of uniforms variables from a pipeline.
	\param pipeline The pipeline to extract the data from.
	\param replace If set to true, and in the case that a pipeline with a similar name already exists, then the set of values is overwritten. Otherwise it will raise an Exception.
	**/
	void UniformsVarsLoader::load(Pipeline& pipeline, bool replace)
	{
		// Try to find a duplicate :
		RessourceNode* ptr = NULL;

		// Try to find a duplicate :
		for(int l=0; l<ressources.size(); l++)
		{
			if(ressources[l].name==pipeline.getName())
			{
				if(replace)
				{
					ressources[l].subNodes.clear();
					ressources[l].ressources.clear();
					ptr = &ressources[l];
					break ;
				}
				else
					throw Exception("An element with the name \"" + pipeline.getName() + "\" has already been registered (replace=false).", __FILE__, __LINE__);
			}
		}

		if(ptr==NULL)
		{
			ressources.push_back( RessourceNode() );
			ptr = &ressources.back();
		}

		ptr->name = pipeline.getName();

		// Load the structure :
		processNode(pipeline, pipeline, *ptr);
	}

	void UniformsVarsLoader::processNode(std::string body, RessourceNode& root)
	{
		VanillaParser parser(body);

		for(int k=0; k<parser.elements.size(); k++)
		{
			if(parser.elements[k].strKeyword==keywordsUniformsVarsLoader[KW_UL_PIPELINE] || parser.elements[k].strKeyword==keywordsUniformsVarsLoader[KW_UL_FILTER])
			{
				if( parser.elements[k].noName || parser.elements[k].name.empty() )
					throw Exception("From line " + to_string(parser.elements[k].startLine) + " : The element must have a (non-empty) name.", __FILE__, __LINE__);

				if( parser.elements[k].noBody )
					throw Exception("From line " + to_string(parser.elements[k].startLine) + " : The element \"" + parser.elements[k].name + "\" must have a (non-empty) body.", __FILE__, __LINE__);

				root.subNodes.push_back( RessourceNode() );
				root.subNodes.back().name = parser.elements[k].name;

				processNode(parser.elements[k].body, root.subNodes.back());
			}
			else // It must be a ressource :
			{
				root.ressources.push_back( Ressource() );
				root.ressources.back().build( parser.elements[k] );
			}
		}

		// Separate filters from pipelines :
		if( !root.ressources.empty() && !root.subNodes.empty())
			throw Exception("Found sub-elements AND ressources in node \"" + root.name + "\".", __FILE__, __LINE__);
	}

	void UniformsVarsLoader::processNode(Pipeline& pipeline, __ReadOnly_PipelineLayout& current, RessourceNode& root)
	{
		for(int k=0; k<current.getNumElements(); k++)
		{
			if(current.getElementKind(k)==__ReadOnly_PipelineLayout::PIPELINE)
			{
				// Add node :
				root.subNodes.push_back( RessourceNode() );
				root.subNodes.back().name = current.getElementName(k);

				processNode(pipeline, current.pipelineLayout(k), root.subNodes.back());
			}
			else if(current.getElementKind(k)==__ReadOnly_PipelineLayout::FILTER)
			{
				// Add node :
				root.subNodes.push_back( RessourceNode() );
				root.subNodes.back().name = current.getElementName(k);

				RessourceNode& r = root.subNodes.back();

				// Get variable list :
				int gid = current.getElementID(k);
				Filter& filter = pipeline[gid];

				std::vector<std::string> 	vars = filter.prgm().getUniformVarsNames();
				std::vector<GLenum> 		type = filter.prgm().getUniformVarsTypes();

				for(int l=0; l<vars.size(); l++)
				{
					if( filter.prgm().isValid(vars[l]) )
					{
						r.ressources.push_back( Ressource() );
						r.ressources.back().build( vars[l], type[l], filter.prgm() );
					}
				}
			}
			else
				throw Exception("UniformsVarsLoader::processNode - Unknown element kind.", __FILE__, __LINE__);
		}
	}

	VanillaParserSpace::Element UniformsVarsLoader::getNodeCode(const RessourceNode& node, const bool isRoot) const
	{
		VanillaParserSpace::Element e;

		if(node.subNodes.empty() && !node.ressources.empty())
		{
			// Filter :
			e.strKeyword 	= keywordsUniformsVarsLoader[KW_UL_FILTER];
			e.name		= node.name;
			e.noName	= false;
			e.arguments.clear();
			e.noArgument	= true;
			e.noBody	= false;

			for(int k=0; k<node.ressources.size(); k++)
			{
				VanillaParserSpace::Element es = node.ressources[k].getCode();
				e.body += es.getCode();

				if(k<node.ressources.size()-1)
					e.body += "\n";
			}
		}
		else if(!node.subNodes.empty() || isRoot)
		{
			// Pipeline :
			e.strKeyword 	= keywordsUniformsVarsLoader[KW_UL_PIPELINE];
			e.name		= node.name;
			e.noName	= false;
			e.arguments.clear();
			e.noArgument	= true;
			e.noBody	= false;

			for(int k=0; k<node.subNodes.size(); k++)
			{
				VanillaParserSpace::Element es = getNodeCode(node.subNodes[k]);

				// Don't show empty elements :
				if(!es.body.empty())
				{
					e.body += es.getCode();

					if(k<node.subNodes.size()-1)
						e.body += "\n\n";
				}
			}
		}

		return e;
	}

	/**
	\fn void UniformsVarsLoader::clear(void)
	\brief Clear the full set of data.
	**/
	void UniformsVarsLoader::clear(void)
	{
		ressources.clear();
	}

	/**
	\fn void UniformsVarsLoader::clear(const std::string& name)
	\brief Clear the pipeline with the corresponding name.
	\param name The name of the pipeline to be erased.
	**/
	void UniformsVarsLoader::clear(const std::string& name)
	{
		// Find element :
		for(int k=0; k<ressources.size(); k++)
		{
			if(ressources[k].name==name)
				ressources.erase( ressources.begin() + k );
		}
	}

	/**
	\fn bool UniformsVarsLoader::hasPipeline(const std::string& name) const
	\brief Test if a pipeline has values attached in this UniformsVarsLoader object (knowing its name).
	\param name The name of the pipeline to test (see Component::getName()).
	\return True if a pipeline with the correct name has values loaded in this UniformsVarsLoader object.
	**/
	bool UniformsVarsLoader::hasPipeline(const std::string& name) const
	{
		for(int k=0; k<ressources.size(); k++)
		{
			if(ressources[k].name==name)
				return true;
		}

		return false;
	}

	/**
	\fn std::vector<std::string> UniformsVarsLoader::getPipelinesTypeNames(void) const
	\brief Get the typenames of all the pipelines which have data loaded.
	\return A vector of strings, each one is a known pipeline typename (see __ReadOnly_ComponentLayout::getTypeName()).
	**/
	std::vector<std::string> UniformsVarsLoader::getPipelinesTypeNames(void) const
	{
		std::vector<std::string> typeNames;

		for(int k=0; k<ressources.size(); k++)
			typeNames.push_back( ressources[k].name );

		return typeNames;
	}

	/**
	\fn bool UniformsVarsLoader::empty(void) const
	\brief Test if this object has some data loaded.
	\return True if there is data loaded.
	**/
	bool UniformsVarsLoader::empty(void) const
	{
		return ressources.empty();
	}

	/**
	\fn int UniformsVarsLoader::getNumVariables(void) const
	\brief Get the total number of variables (scalar, vector, matrices, etc.) available in this object.
	\return The total number of uniforms variables currently stored in this object.
	**/
	int UniformsVarsLoader::getNumVariables(void) const
	{
		int c = 0;

		for(int k=0; k<ressources.size(); k++)
			c += ressources[k].getNumVariables();
		
		return c;
	}

	/**
	\fn int UniformsVarsLoader::getNumVariables(const std::string& name) const
	\brief Get the number of variables (scalar, vector, matrices, etc.) available for the pipeline of the given name.
	\param name The name of the target pipeline (see Component::getName()).
	\return The number of uniforms variables currently stored in this object for the pipeline of the corresponding name.
	**/
	int UniformsVarsLoader::getNumVariables(const std::string& name) const
	{
		int c = 0;	

		for(int k=0; k<ressources.size(); k++)
		{
			if(ressources[k].name==name)
				c += ressources[k].getNumVariables();
		}

		return c;
	}

	/**
	\fn int UniformsVarsLoader::applyTo(Pipeline& pipeline)
	\brief Copy the possibly loaded set of uniforms variables to a pipeline (for the corresponding name of the Pipeline instance).
	\param pipeline The pipeline to which the data has to be copied (if relevant).
	\return The total number of uniforms variables copied.
	**/
	int UniformsVarsLoader::applyTo(Pipeline& pipeline)
	{
		int c = 0;

		try
		{
			// Scan the current ressources to find a corresponding name :
			for(int k=0; k<ressources.size(); k++)
			{
				if(ressources[k].name==pipeline.getName())
					c += ressources[k].apply(pipeline, pipeline);
			}

			HdlProgram::stopProgram();
		}
		catch(Exception& e)
		{
			Exception m("UniformsVarsLoader::applyTo - Exception caught while applying variables to pipeline " + pipeline.getFullName() + " : ", __FILE__, __LINE__);
			throw m + e;
		}

		return c;
	}

	/**
	\fn std::string UniformsVarsLoader::getCode(void) const
	\brief Return the corresponding code for the current set of variables, for one, or multiple, pipelines. The format is human-readable.
	\return A standard string containing the structured data.
	**/
	std::string UniformsVarsLoader::getCode(void) const
	{
		std::string res;

		for(int k=0; k<ressources.size(); k++)
		{
			VanillaParserSpace::Element e = getNodeCode(ressources[k], true);
			res += e.getCode() + "\n";
		}

		return res;
	}

	/**
	\fn std::string UniformsVarsLoader::getCode(const std::string& name) const
	\brief Return the corresponding code of the corresponding pipeline. The format is human-readable.
	\param name The name of the pipeline to target.
	\return A standard string containing the structured data. Raise an Exception if no pipeline is found.
	**/
	std::string UniformsVarsLoader::getCode(const std::string& name) const
	{
		for(int k=0; k<ressources.size(); k++)
		{
			if(ressources[k].name==name)
			{
				VanillaParserSpace::Element e = getNodeCode(ressources[k], true);
				return e.getCode();
			}
		}

		throw Exception("UniformsVarsLoader::getCode - Unable to find element \"" + name + "\".", __FILE__, __LINE__);
	}

	/**
	\fn void UniformsVarsLoader::writeToFile(const std::string& filename) const
	\brief Write the corresponding code for the current set of variables, for one, or multiple, pipelines to a file. The format is human-readable. Note : in order to append, you must load the original first.
	\param filename The filename of the file to write to.
	**/
	void UniformsVarsLoader::writeToFile(const std::string& filename) const
	{
		std::fstream file;

		file.open(filename.c_str(), std::ios_base::out);

		if(!file.is_open())
				throw Exception("UniformsVarsLoader::writeToFile - Cannot load file : \"" + filename + "\".", __FILE__, __LINE__);

		file << getCode();

		file.close();
	}

