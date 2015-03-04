/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : UniformsLoader.cpp                                                                        */
/*     Original Date : June 8th 2013                                                                             */
/*                                                                                                               */
/*     Description   : Uniforms variables save/load.                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    UniformsLoader.cpp
 * \brief   Uniforms variables save/load.
 * \author  R. KERVICHE
 * \date    June 8th 2013
**/

	// Includes :
	#include <cstring>
	#include "Core/Exception.hpp"
	#include "Modules/UniformsLoader.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;
	using namespace Glip::Modules::VanillaParserSpace;

	// Static variables :
	const char* Glip::Modules::UniformsLoader::keywords[UL_NumKeywords] =  {	"PIPELINE",
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

// UniformsLoader::Resource : 
	UniformsLoader::Resource::Resource(void)
	 : 	data(NULL),
		modified(false)
	{ }

	UniformsLoader::Resource::Resource(const VanillaParserSpace::Element& e)
	 : 	data(NULL),
		modified(false)
	{
		if(e.noName || e.name.empty())
			throw Exception("From line " + toString(e.startLine) + " : The element of type \"" + e.strKeyword + "\" should have a name.", __FILE__, __LINE__, Exception::ClientScriptException);

		if(!e.noBody)
			throw Exception("From line " + toString(e.startLine) + " : Element \"" + e.name + "\" should not have a body.", __FILE__, __LINE__, Exception::ClientScriptException);

		// Set the name :
		name = e.name;

		// Create the data : 
		data = HdlDynamicData::build(getGLEnum(e.strKeyword));

		// Test : 
		if(data->getNumElements()!=static_cast<int>(e.arguments.size()))
		{
			const int expectedArgs = data->getNumElements();
			delete data;
			data = NULL;
			throw Exception("From line " + toString(e.startLine) + " : Element \"" + e.name + "\" of type \"" + e.strKeyword + "\" has " + toString(e.arguments.size()) + " arguments while it should have exactly " + toString(expectedArgs) + ".", __FILE__, __LINE__, Exception::ClientScriptException);
		}

		// Fill : 
		for(int j=0; j<data->getNumColumns(); j++)
		{	
			for(int i=0; i<data->getNumRows(); i++)
			{
				const int k = data->getIndex(i, j);
				double 	val = 0.0;
		
				if( !fromString( e.arguments[k], val ) )
					throw Exception("From line " + toString(e.startLine) + " : Unable to read parameter " + toString(k) + " in element \"" + e.name + "\" of type \"" + e.strKeyword + "\" (token : \"" + e.arguments[k] + "\").", __FILE__, __LINE__, Exception::ClientScriptException);
				else
					data->set( val, i, j);
			}
		}
	}

	UniformsLoader::Resource::Resource(const std::string& _name, const HdlDynamicData& _data)
	 : 	name(_name),
		data(NULL),
		modified(false)
	{
		data = HdlDynamicData::copy(_data);
	}

	UniformsLoader::Resource::Resource(const Resource& cpy)
	 : 	name(cpy.name),
		data(NULL),
		modified(cpy.modified)
	{
		if(cpy.data!=NULL)
			data = HdlDynamicData::copy(*cpy.data);
	}

	UniformsLoader::Resource::~Resource(void)
	{
		delete data;
		data = NULL;
	}
	
	UniformsLoader::Resource& UniformsLoader::Resource::operator=(const Resource& cpy)
	{
		name = cpy.name;
		modified = cpy.modified;
		delete data;
		data = NULL;

		if(cpy.data!=NULL)
			data = HdlDynamicData::copy(*cpy.data);
		
		return (*this);	
	}

	/**
	\fn const std::string& UniformsLoader::Resource::getName(void) const
	\brief Get the name of the resource.
	\return Constant reference to the standard string containing the name of the Resource.
	**/
	const std::string& UniformsLoader::Resource::getName(void) const
	{
		return name;
	}

	/**
	\fn const HdlDynamicData& UniformsLoader::Resource::object(void) const
	\brief Get the object of the resource (access to the variable itself).
	\return Constant reference to the variable.
	**/
	const HdlDynamicData& UniformsLoader::Resource::object(void) const
	{
		if(data==NULL)
			throw Exception("UniformsLoader::Resource::object - Data object not available.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return (*data);
	}

	/**
	\fn HdlDynamicData& UniformsLoader::Resource::object(void)
	\brief Get the object of the resource (access to the variable itself).
	\return Reference to the variable.
	**/
	HdlDynamicData& UniformsLoader::Resource::object(void)
	{
		if(data==NULL)
			throw Exception("UniformsLoader::Resource::object - Data object not available.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return (*data);
	}

	int UniformsLoader::Resource::applyTo(Filter& filter, bool forceWrite) const
	{
		if(data==NULL)
			throw Exception("UniformsLoader::Resource::applyTo - Data object not available.", __FILE__, __LINE__, Exception::ModuleException);
		else if(modified || forceWrite)
		{
			filter.program().setVar(name, *data);
			return 1;
		}
		else
			return 0;
	}

	VanillaParserSpace::Element UniformsLoader::Resource::getCodeElement(void) const
	{
		if(data==NULL)
			throw Exception("UniformsLoader::Resource::getCodeElement - Data object not available.", __FILE__, __LINE__, Exception::ModuleException);

		VanillaParserSpace::Element e;

		e.strKeyword 	= getGLEnumName(data->getGLType());
		e.name		= name;
		e.noName	= false;
		
		e.arguments.assign(data->getNumElements(), "");

		for(int j=0; j<data->getNumColumns(); j++)
		{	
			for(int i=0; i<data->getNumRows(); i++)
			{
				const int k = data->getIndex(i, j);
	
				e.arguments[k] = toString( data->get(i, j) );
			}
		}

		e.noArgument	= false;
		e.noBody	= true;
		e.body.clear();

		return e;
	}

// UniformsLoader::Node : 
	UniformsLoader::Node::Node(void)
	{ }

	UniformsLoader::Node::Node(const VanillaParserSpace::Element& e)
	{
		if(e.noName || e.name.empty())
			throw Exception("The element of type \"" + e.strKeyword + "\" should have a name.", e.sourceName, e.startLine, Exception::ClientScriptException);

		if(e.strKeyword!=keywords[KW_UL_FILTER] && e.strKeyword!=keywords[KW_UL_PIPELINE])
			throw Exception("Unknown element type : \"" + e.strKeyword + "\".", e.sourceName, e.startLine, Exception::ClientScriptException);

		name = e.name;

		const bool supposedToBeAFilter = (e.strKeyword==keywords[KW_UL_FILTER]);
					
		if(!e.noBody && !e.body.empty())
		{
			VanillaParser parser(e.body, e.sourceName, e.bodyLine);

			if(supposedToBeAFilter)
			{
				// Load all elements as resources : 
				for(std::vector<VanillaParserSpace::Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
				{
					Resource tmp(*it);
					resources[tmp.getName()] = tmp;
				}
			}
			else
			{
				// Load all elements as pipelines or filters : 
				for(std::vector<VanillaParserSpace::Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
				{
					Node tmp(*it);
			
					if(!tmp.empty())
						subNodes[tmp.getName()] = tmp;
				}	
			}
		}
		// else, stay empty.
	}

	UniformsLoader::Node::Node(const std::string& _name, Pipeline& pipeline, const AbstractPipelineLayout& current)
	 : 	name(_name)
	{
		for(int k=0; k<current.getNumElements(); k++)
		{
			switch(current.getElementKind(k))
			{
				case AbstractPipelineLayout::FILTER :
					subNodes[current.getElementName(k)] = Node(current.getElementName(k), pipeline, current, current.filterLayout(k));
					break;
				case AbstractPipelineLayout::PIPELINE :
					subNodes[current.getElementName(k)] = Node(current.getElementName(k), pipeline, current.pipelineLayout(k));
					break;
				default :
					throw Exception("UniformsLoader::Node::Node - Unknown element type (internal error).", __FILE__, __LINE__, Exception::ModuleException);
			}
		}
	}

	UniformsLoader::Node::Node(const std::string& _name, Pipeline& pipeline, const AbstractPipelineLayout& parent, const AbstractFilterLayout& current)
	 : 	name(_name)
	{
		UNUSED_PARAMETER(current)

		// Get access to the filter : 
		const int gid = parent.getElementID(name);
		Filter& filter = pipeline[gid];

		// Get the program : 
		HdlProgram& prgm = filter.program();

		// Get the variables : 
		const std::vector<std::string>& 	uniformsNames = prgm.getUniformsNames();
 		const std::vector<GLenum>& 		uniformsTypes = prgm.getUniformsTypes();

		for(unsigned int k=0; k<uniformsNames.size(); k++)
		{
			if(prgm.isUniformVariableValid(uniformsNames[k]))
			{
				HdlDynamicData* data = HdlDynamicData::build(uniformsTypes[k]);

				prgm.getVar( uniformsNames[k], *data);

				resources[ uniformsNames[k] ] = Resource(uniformsNames[k], *data);

				delete data;
			}
		}
	}

	UniformsLoader::Node::Node(const Node& cpy)
	 : 	name(cpy.name),
		subNodes(cpy.subNodes),
		resources(cpy.resources)
	{ }

	UniformsLoader::Node::~Node(void)
	{
		subNodes.clear();
		resources.clear();
	}

	UniformsLoader::Node& UniformsLoader::Node::operator=(const Node& cpy)
	{
		name 		= cpy.name;
		subNodes	= cpy.subNodes;
		resources	= cpy.resources;

		return (*this);
	}

	/**
	\fn void UniformsLoader::Node::softCopy(const Node& cpy, bool appendNewNodes, bool appendNewResources, bool removeAbsentNodes, bool removeAbsentResources)
	\brief Perform a soft copy from another node (copy the existing data, with possible filters).

	\param cpy The node to be copied (the source).
	\param appendNewNodes If true, nodes existing in the source but not in this will be added to this.
	\param appendNewResources If true, resources existing in the source but not in this will be added to this.
	\param removeAbsentNodes If true, remove the nodes existing in this but not in the source.
	\param removeAbsentResources If true, remove the resources existing in this but not in the source.
	**/
	void UniformsLoader::Node::softCopy(const Node& cpy, bool appendNewNodes, bool appendNewResources, bool removeAbsentNodes, bool removeAbsentResources)
	{
		// Do not copy the name.

		if(removeAbsentNodes)
		{
			for(NodeIterator it=nodeBegin(); it!=nodeEnd(); )
			{
				NodeIterator itLag = it;
				it++;

				if(!cpy.subNodeExists(itLag->second.getName()))
					subNodes.erase(itLag);
			}
		}

		if(removeAbsentResources)
		{
			for(ResourceIterator it=resourceBegin(); it!=resourceEnd(); )
			{
				ResourceIterator itLag = it;
				it++;

				if(!cpy.resourceExists(itLag->second.getName()))
					resources.erase(itLag);
			}
		}

		// Scan the sub-nodes of cpy :
		for(NodeConstIterator it=cpy.nodeBegin(); it!=cpy.nodeEnd(); it++)
		{
			NodeIterator itSub = findNode(it->first);

			if(itSub!=nodeEnd())
				itSub->second.softCopy(it->second, appendNewNodes, appendNewResources);
			else if(appendNewNodes)
				subNodes[it->first] = it->second;	
		}

		// Scan the resources of cpy :
		for(ResourceConstIterator it=cpy.resourceBegin(); it!=cpy.resourceEnd(); it++)
		{
			ResourceIterator itRes = findResource(it->first);

			if(itRes!=resourceEnd())
				itRes->second = it->second; // Copy the values.
			else if(appendNewResources)
				resources[it->first] = it->second;
		}
	}

	/**
	\fn void UniformsLoader::Node::softCopy(const Node& cpy, LoadingFilter loadingFilter)
	\brief Perform a soft copy from another node (copy the existing data, with possible filters).

	The nodes existing in this but not in the source will not be modified.

	\param cpy The node to be copied (the source).
	\param loadingFilter Select a specific method of loading.
	**/
	void UniformsLoader::Node::softCopy(const Node& cpy, LoadingFilter loadingFilter)
	{
		// Scan the sub-nodes of cpy :
		for(NodeConstIterator it=cpy.nodeBegin(); it!=cpy.nodeEnd(); it++)
		{
			NodeIterator itSub = findNode(it->first);

			if(itSub==nodeEnd())
			{
				if(loadingFilter==LoadAll || loadingFilter==NewOnly || loadingFilter==NewOnlySilent)
					subNodes[it->first] = it->second;
				else if(loadingFilter==ReplaceOnly)
					throw Exception("In " + getName() + " : The node with the name \"" + it->first + "\" is not currently registered (loadingFilter=ReplaceOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==ReplaceOnlySilent, nothing.
			}
			else
			{
				if(loadingFilter==LoadAll || loadingFilter==ReplaceOnly || loadingFilter==ReplaceOnlySilent)
					itSub->second.softCopy(it->second, loadingFilter);
				else if(loadingFilter==NewOnly)
					throw Exception("In " + getName() + " : The node with the name \"" + it->first + "\" is already registered (loadingFilter=NewOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==NewOnlySilent, nothing.
			}
		}

		// Scan the resources of cpy :
		for(ResourceConstIterator it=cpy.resourceBegin(); it!=cpy.resourceEnd(); it++)
		{
			ResourceIterator itRes = findResource(it->first);

			if(itRes==resourceEnd())
			{
				if(loadingFilter==LoadAll || loadingFilter==NewOnly || loadingFilter==NewOnlySilent)
					resources[it->first] = it->second;
				else if(loadingFilter==ReplaceOnly)
					throw Exception("In " + getName() + " : The resource with the name \"" + it->first + "\" is not currently registered (loadingFilter=ReplaceOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==ReplaceOnlySilent, nothing.
			}
			else
			{
				if(loadingFilter==LoadAll || loadingFilter==ReplaceOnly || loadingFilter==ReplaceOnlySilent)
					itRes->second = it->second;
				else if(loadingFilter==NewOnly)
					throw Exception("In " + getName() + " : The resource with the name \"" + it->first + "\" is already registered (loadingFilter=NewOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==NewOnlySilent, nothing.
			}
		}
	}

	/**
	\fn const std::string& UniformsLoader::Node::getName(void) const
	\brief Get the name of the node.
	\return Constant reference to the standard string containing the name of the Node.
	**/
	const std::string& UniformsLoader::Node::getName(void) const
	{
		return name;
	}

	/**
	\fn bool UniformsLoader::Node::isFilter(void) const
	\brief Get the name of the resource.
	\return True if the node is empty (no sub-node or no resource).
	**/
	bool UniformsLoader::Node::isFilter(void) const
	{
		return subNodes.empty() && !resources.empty();
	}

	/**
	\fn bool UniformsLoader::Node::isPipeline(void) const
	\brief Test if a Node is a pipeline (either the node is empty or it has sub-nodes only).
	\return True if the Node corresponds to a Pipeline structure.
	**/
	bool UniformsLoader::Node::isPipeline(void) const
	{
		return !subNodes.empty() && resources.empty();
	}

	/**
	\fn bool UniformsLoader::Node::empty(void) const
	\brief Test if a Node is a filter (either the node is empty or it has resources only).
	\return True if the Node corresponds to a Filter structure.
	**/
	bool UniformsLoader::Node::empty(void) const
	{
		return subNodes.empty() && resources.empty();
	}

	/**
	\fn void UniformsLoader::Node::clear(void)
	\brief Removes all resources and sub-nodes from this node.
	**/
	void UniformsLoader::Node::clear(void)
	{
		subNodes.clear();
		resources.clear();
	}

	/**
	\fn bool UniformsLoader::Node::hasModifications(void) const
	\brief Test if this branch contains modified variables.
	\return True, if variable contained declared modification (see UniformsLoader::Resource::modified).
	**/
	bool UniformsLoader::Node::hasModifications(void) const
	{
		if(empty())
			return false;

		bool test = false;

		for(ResourceConstIterator it=resourceBegin(); it!=resourceEnd(); it++)
			test = test || it->second.modified;
		
		for(NodeConstIterator it=nodeBegin(); it!=nodeEnd(); it++)
			test = test || it->second.hasModifications();

		return test;
	}

	/**
	\fn void UniformsLoader::Node::clearModifiedFlags(bool value)
	\brief Go through all sub-nodes and change any Resource::modified flag.
	\param value The new value to set for the flags (true, will set all the subsequent modified flags to true).
	**/
	void UniformsLoader::Node::clearModifiedFlags(bool value)
	{
		for(ResourceIterator it=resourceBegin(); it!=resourceEnd(); it++)
			it->second.modified = value;
		
		for(NodeIterator it=nodeBegin(); it!=nodeEnd(); it++)
			it->second.clearModifiedFlags(value);
	}

	/**
	\fn int UniformsLoader::Node::getNumSubNodes(void) const
	\brief Get the number of sub-nodes.
	\return The number of sub-nodes.
	**/
	int UniformsLoader::Node::getNumSubNodes(void) const
	{
		return subNodes.size();
	}

	/**
	\fn std::vector<std::string> UniformsLoader::Node::getSubNodesNamesList(void) const
	\brief Get a list of the name of all sub-nodes.
	\return A list of all sub-nodes names.
	**/
	std::vector<std::string> UniformsLoader::Node::getSubNodesNamesList(void) const
	{
		std::vector<std::string> namesList;

		for(std::map<std::string, Node>::const_iterator it=subNodes.begin(); it!=subNodes.end(); it++)
			namesList.push_back(it->first);

		return namesList;
	}
	
	/**
	\fn bool UniformsLoader::Node::subNodeExists(const std::string& nodeName) const
	\brief Test if a sub-node exist.
	\param nodeName The name of the targeted node.
	\return True if the node exists, false otherwise.
	**/
	bool UniformsLoader::Node::subNodeExists(const std::string& nodeName) const
	{
		return subNodes.find(nodeName)!=subNodes.end();
	}

	/**
	\fn const UniformsLoader::Node& UniformsLoader::Node::subNode(const std::string& nodeName) const
	\brief Access a sub-node by its name.
	\param nodeName The name of the targeted node.
	\return A (constant) reference to the targeted node or raise an exception is any error occurs.
	**/
	const UniformsLoader::Node& UniformsLoader::Node::subNode(const std::string& nodeName) const
	{
		std::map<std::string, Node>::const_iterator it = subNodes.find(nodeName);

		if(it==subNodes.end())
			throw Exception("UniformsLoader::Node::subNode - No sub-node named \"" + nodeName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn UniformsLoader::Node& UniformsLoader::Node::subNode(const std::string& nodeName)
	\brief Access a sub-node by its name.
	\param nodeName The name of the targeted node.
	\return A reference to the targeted node or raise an exception is any error occurs.
	**/
	UniformsLoader::Node& UniformsLoader::Node::subNode(const std::string& nodeName)
	{
		std::map<std::string, Node>::iterator it = subNodes.find(nodeName);

		if(it==subNodes.end())
			throw Exception("UniformsLoader::Node::subNode - No sub-node named \"" + nodeName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/*void UniformsLoader::Node::addNode(const Node& node)
	{
		subNodes[node.getName()] = node;
	}*/

	/**
	\fn void UniformsLoader::Node::eraseNode(const std::string& nodeName)
	\brief Remove a node by its name or raise an exception if any error occurs.
	\param nodeName The name of the targeted node.
	**/
	void UniformsLoader::Node::eraseNode(const std::string& nodeName)
	{
		std::map<std::string, Node>::iterator it = subNodes.find(nodeName);

		if(it==subNodes.end())
			throw Exception("UniformsLoader::Node::eraseNode - No sub-node named \"" + nodeName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			subNodes.erase(it);
	}

	/**
	\fn UniformsLoader::NodeConstIterator UniformsLoader::Node::nodeBegin(void) const
	\brief Get the 'begin' iterator on the sub-nodes list.
	\return A UniformsLoader::NodeConstIterator on 'begin'. 
	**/
	UniformsLoader::NodeConstIterator UniformsLoader::Node::nodeBegin(void) const
	{
		return subNodes.begin();
	}

	/**
	\fn UniformsLoader::NodeConstIterator UniformsLoader::Node::nodeEnd(void) const
	\brief Get the 'end' iterator on the sub-nodes list.
	\return A UniformsLoader::NodeConstIterator on 'end'. 
	**/
	UniformsLoader::NodeConstIterator UniformsLoader::Node::nodeEnd(void) const
	{
		return subNodes.end();
	}

	/**
	\fn UniformsLoader::NodeConstIterator UniformsLoader::Node::findNode(const std::string& nodeName) const
	\brief Find node.
	\param nodeName Name of the targeted node.
	\return A constant iterator to the node or UniformsLoader::Node::nodeEnd() if no match is found.
	**/	
	UniformsLoader::NodeConstIterator UniformsLoader::Node::findNode(const std::string& nodeName) const
	{
		return subNodes.find(nodeName);
	}

	/**
	\fn UniformsLoader::NodeIterator UniformsLoader::Node::nodeBegin(void)
	\brief Get the 'begin' iterator on the sub-nodes list.
	\return A UniformsLoader::NodeIterator on 'begin'. 
	**/
	UniformsLoader::NodeIterator UniformsLoader::Node::nodeBegin(void)
	{
		return subNodes.begin();
	}

	/**
	\fn UniformsLoader::NodeIterator UniformsLoader::Node::nodeEnd(void)
	\brief Get the 'end' iterator on the sub-nodes list.
	\return A UniformsLoader::NodeIterator on 'end'. 
	**/
	UniformsLoader::NodeIterator UniformsLoader::Node::nodeEnd(void)
	{
		return subNodes.end();
	}

	/**
	\fn UniformsLoader::NodeIterator UniformsLoader::Node::findNode(const std::string& nodeName)
	\brief Find node.
	\param nodeName Name of the targeted node.
	\return An iterator to the node or UniformsLoader::Node::nodeEnd() if no match is found.
	**/
	UniformsLoader::NodeIterator UniformsLoader::Node::findNode(const std::string& nodeName)
	{
		return subNodes.find(nodeName);
	}

	/**
	\fn int UniformsLoader::Node::getNumResources(void) const
	\brief Get the number of resources.
	\return The number of resources.
	**/
	int UniformsLoader::Node::getNumResources(void) const
	{
		return resources.size();
	}

	/**
	\fn std::vector<std::string> UniformsLoader::Node::getResourcesNamesList(void) const
	\brief Get a list of the name of all resources.
	\return A list of all resources names.
	**/
	std::vector<std::string> UniformsLoader::Node::getResourcesNamesList(void) const
	{
		std::vector<std::string> namesList;

		for(std::map<std::string, Resource>::const_iterator it=resources.begin(); it!=resources.end(); it++)
			namesList.push_back(it->first);

		return namesList;
	}

	/**
	\fn bool UniformsLoader::Node::resourceExists(const std::string& resourceName) const
	\brief Test if a resource exist.
	\param resourceName The name of the targeted resource.
	\return True if the resource exists, false otherwise.
	**/
	bool UniformsLoader::Node::resourceExists(const std::string& resourceName) const
	{
		return resources.find(resourceName)!=resources.end();
	}

	/**
	\fn const UniformsLoader::Resource& UniformsLoader::Node::resource(const std::string& resourceName) const
	\brief Access a resource by its name.
	\param resourceName The name of the targeted resource.
	\return A (constant) reference to the targeted resource or raise an exception is any error occurs.
	**/
	const UniformsLoader::Resource& UniformsLoader::Node::resource(const std::string& resourceName) const
	{
		std::map<std::string, Resource>::const_iterator it = resources.find(resourceName);

		if(it==resources.end())
			throw Exception("UniformsLoader::Node::resource - No resource named \"" + resourceName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn UniformsLoader::Resource& UniformsLoader::Node::resource(const std::string& resourceName)
	\brief Access a resource by its name.
	\param resourceName The name of the targeted resource.
	\return A reference to the targeted resource or raise an exception is any error occurs.
	**/
	UniformsLoader::Resource& UniformsLoader::Node::resource(const std::string& resourceName)
	{
		std::map<std::string, Resource>::iterator it = resources.find(resourceName);

		if(it==resources.end())
			throw Exception("UniformsLoader::Node::resource - No resource named \"" + resourceName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/*void UniformsLoader::Node::addResource(const Resource& resource)
	{
		resources[resource.getName()] = resource;
	}*/

	/**
	\fn void UniformsLoader::Node::eraseResource(const std::string& resourceName)
	\brief Remove a resource by its name or raise an exception if any error occurs.
	\param resourceName The name of the targeted resource.
	**/
	void UniformsLoader::Node::eraseResource(const std::string& resourceName)
	{
		std::map<std::string, Resource>::iterator it = resources.find(resourceName);

		if(it==resources.end())
			throw Exception("UniformsLoader::Node::eraseNode - No resource named \"" + resourceName + "\" is registered.", __FILE__, __LINE__, Exception::ModuleException);
		else
			resources.erase(it);
	}

	/**
	\fn UniformsLoader::ResourceConstIterator UniformsLoader::Node::resourceBegin(void) const
	\brief Get the 'begin' iterator on the resources list.
	\return A UniformsLoader::ResourceConstIterator on 'begin'. 
	**/
	UniformsLoader::ResourceConstIterator UniformsLoader::Node::resourceBegin(void) const
	{
		return resources.begin();
	}

	/**
	\fn UniformsLoader::ResourceConstIterator UniformsLoader::Node::resourceEnd(void) const
	\brief Get the 'end' iterator on the resources list.
	\return A UniformsLoader::ResourceConstIterator on 'end'. 
	**/
	UniformsLoader::ResourceConstIterator UniformsLoader::Node::resourceEnd(void) const
	{
		return resources.end();
	}

	/**
	\fn UniformsLoader::ResourceConstIterator UniformsLoader::Node::findResource(const std::string& resourceName) const
	\brief Find resource.
	\param resourceName Name of the targeted resource.
	\return A constant iterator to the resource or UniformsLoader::Node::resourceEnd() if no match is found.
	**/
	UniformsLoader::ResourceConstIterator UniformsLoader::Node::findResource(const std::string& resourceName) const
	{
		return resources.find(resourceName);
	}

	/**
	\fn UniformsLoader::ResourceIterator UniformsLoader::Node::resourceBegin(void)
	\brief Get the 'begin' iterator on the resources list.
	\return A UniformsLoader::ResourceIterator on 'begin'. 
	**/
	UniformsLoader::ResourceIterator UniformsLoader::Node::resourceBegin(void)
	{
		return resources.begin();
	}

	/**
	\fn UniformsLoader::ResourceIterator UniformsLoader::Node::resourceEnd(void)
	\brief Get the 'end' iterator on the resources list.
	\return A UniformsLoader::ResourceIterator on 'end'. 
	**/
	UniformsLoader::ResourceIterator UniformsLoader::Node::resourceEnd(void)
	{
		return resources.end();
	}

	/**
	\fn UniformsLoader::ResourceIterator UniformsLoader::Node::findResource(const std::string& resourceName)
	\brief Find resource.
	\param resourceName Name of the targeted resource.
	\return An iterator to the resource or UniformsLoader::Node::resourceEnd() if no match is found.
	**/
	UniformsLoader::ResourceIterator UniformsLoader::Node::findResource(const std::string& resourceName)
	{
		return resources.find(resourceName);
	}

	int UniformsLoader::Node::applyTo(Pipeline& pipeline, const AbstractPipelineLayout& current, bool forceWrite, bool silent) const
	{
		int c = 0;

		for(std::map<std::string, UniformsLoader::Node>::const_iterator it=subNodes.begin(); it!=subNodes.end(); it++)
		{
			if(!current.doesElementExist(it->second.getName()))
			{
				if(!silent)
					throw Exception("Missing element \"" + it->second.getName() + "\" in " + current.getFullName() + ".", __FILE__, __LINE__, Exception::ModuleException);
			}
			else
			{
				int id = current.getElementIndex(it->second.getName());
	
				const AbstractPipelineLayout::ComponentKind kind = current.getElementKind(id);
	
				switch(kind)
				{
					case AbstractPipelineLayout::FILTER :
						{
							int gid = current.getElementID(id);
							Filter& filter = pipeline[gid];	
							c += it->second.applyTo(pipeline, filter, forceWrite);
						}
						break;
					case AbstractPipelineLayout::PIPELINE :
						c += it->second.applyTo(pipeline, current.pipelineLayout(id), forceWrite);
						break;
					default : 
						throw Exception("UniformsLoader::Node::applyTo - Unknown element type (internal error).", __FILE__, __LINE__, Exception::ModuleException);
				}
			}
		}

		return c;
	}

	int UniformsLoader::Node::applyTo(Pipeline& pipeline, Filter& filter, bool forceWrite, bool silent) const
	{
		UNUSED_PARAMETER(pipeline)

		int c = 0;

		if(!empty() && !isFilter())
		{
			if(!silent)
				throw Exception("Element \"" + filter.getFullName() + "\" is not registered as a filter by this Object.", __FILE__, __LINE__, Exception::ModuleException);
			else
				return 0;
		}

		for(std::map<std::string, UniformsLoader::Resource>::const_iterator it=resources.begin(); it!=resources.end(); it++)
			c += it->second.applyTo(filter, forceWrite);

		return c;
	}

	VanillaParserSpace::Element UniformsLoader::Node::getCodeElement(void) const
	{
		VanillaParserSpace::Element e;

		e.noName = true;
		e.noArgument = true;
		e.noBody = true;

		if(empty())
			return e;

		if(isFilter())
			e.strKeyword = keywords[KW_UL_FILTER];
		else
			e.strKeyword = keywords[KW_UL_PIPELINE];

		e.name = name;
		e.noName = false;

		e.noArgument = true;
		e.noBody = false;

		if(isFilter())
		{
			for(ResourceConstIterator it=resourceBegin(); it!=resourceEnd(); )
			{
				e.body += it->second.getCodeElement().getCode();

				it++;

				if(it!=resourceEnd())
					e.body += '\n';
			}
		}
		else
		{
			for(NodeConstIterator it=nodeBegin(); it!=nodeEnd(); )
			{
				e.body += it->second.getCodeElement().getCode();

				it++;

				if(it!=nodeEnd())
					e.body += '\n';
			}
		}

		return e;
	}

// UniformsLoader :
	/**
	\fn UniformsLoader::UniformsLoader(void)
	\brief UniformsLoader constructor.
	**/
	UniformsLoader::UniformsLoader(void)
	{ }

	/**
	\fn UniformsLoader::UniformsLoader(const UniformsLoader& cpy)
	\brief UniformsLoader copy constructor.
	\param cpy The object to be copied.
	**/
	UniformsLoader::UniformsLoader(const UniformsLoader& cpy)
	 : 	nodes(cpy.nodes)
	{ }

	UniformsLoader::~UniformsLoader(void)
	{
		nodes.clear();
	}

	/**
	\fn void UniformsLoader::load(std::string source, LoadingFilter loadingFilter, int lineOffset)
	\brief Loads a set of uniforms variables for one, or multiple, pipelines.
	\param source Either a filename or directly the source string (in this case it must contain at least one newline character '\\n').
	\param loadingFilter Select a specific method of loading.
	\param lineOffset Set the line number offset for error reporting. 
	**/
	void UniformsLoader::load(std::string source, LoadingFilter loadingFilter, int lineOffset)
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
				throw Exception("UniformsLoader::load - Cannot load file : \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);


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

			// Clear the line offset, start counting in the file : 
			lineOffset = 1;
		}
		// else : "already loaded"

		// Load :
		try
		{
			VanillaParser parser(source, filename, lineOffset);

			for(std::vector<VanillaParserSpace::Element>::iterator it=parser.elements.begin(); it!=parser.elements.end(); it++)
			{
				Node tmp(*it);

				if(!tmp.empty())
				{
					NodeIterator similarIt = nodes.find(tmp.getName());

					if(similarIt==nodes.end())
					{
						if(loadingFilter==LoadAll || loadingFilter==NewOnly || loadingFilter==NewOnlySilent)
							nodes[tmp.getName()] = tmp;
						else if(loadingFilter==ReplaceOnly)
							throw Exception("The element with the name \"" + tmp.getName() + "\" is not currently registered (loadingFilter=ReplaceOnly).", it->sourceName, it->startLine, Exception::ClientScriptException);
						//else loadingFilter==ReplaceOnlySilent, nothing
					}
					else
					{
						if(loadingFilter==LoadAll || loadingFilter==ReplaceOnly || loadingFilter==ReplaceOnlySilent)
							similarIt->second.softCopy(tmp, loadingFilter);
						else if(loadingFilter==NewOnly)
							throw Exception("The element with the name \"" + tmp.getName() + "\" is already registered (loadingFilter=NewOnly).", it->sourceName, it->startLine, Exception::ClientScriptException);
						//else loadingFilter==NewOnlySilent, nothing
					}
				}
			}
		}
		catch(Exception& e)
		{
			if(!filename.empty())
			{
				Exception m("Unable to load uniforms data from file \"" + filename + "\" : ", filename, 0, Exception::ClientScriptException);
				m << e;
				throw m;
			}
			else
			{
				Exception m("Unable to load uniforms data : ", "", 0, Exception::ClientScriptException);
				m << e;
				throw m;
			}
		}
	}

	/**
	\fn void UniformsLoader::load(Pipeline& pipeline, LoadingFilter loadingFilter)
	\brief Load the set of uniforms variables from a pipeline.
	\param pipeline The pipeline to extract the data from.
	\param loadingFilter Select a specific method of loading.
	**/
	void UniformsLoader::load(Pipeline& pipeline, LoadingFilter loadingFilter)
	{
		Node tmp(pipeline.getTypeName(), pipeline, pipeline);
		std::map<std::string, UniformsLoader::Node>::iterator it=nodes.find(pipeline.getTypeName());

		if(it==nodes.end())
		{
			if(loadingFilter==LoadAll || loadingFilter==NewOnly || loadingFilter==NewOnlySilent)
				nodes[tmp.getName()] = tmp;
			else if(loadingFilter==ReplaceOnly)
				throw Exception("The element with the name \"" + tmp.getName() + "\" is not currently registered (loadingFilter=ReplaceOnly).", __FILE__, __LINE__, Exception::ModuleException);
			//else loadingFilter==ReplaceOnlySilent, nothing.
		}
		else
		{
			if(loadingFilter==LoadAll || loadingFilter==ReplaceOnly || loadingFilter==ReplaceOnlySilent)
				it->second.softCopy(tmp, loadingFilter);
			else if(loadingFilter==NewOnly)
				throw Exception("The element with the name \"" + tmp.getName() + "\" is already registered (loadingFilter=NewOnly).", __FILE__, __LINE__, Exception::ModuleException);
			//else loadingFilter==NewOnlySilent, nothing.
		}
	}

	/**
	\fn void UniformsLoader::load(const UniformsLoader& subLoader, LoadingFilter loadingFilter)
	\brief Load data from another UniformsLoader object.
	\param subLoader Other object.
	\param loadingFilter Select a specific method of loading.
	**/
	void UniformsLoader::load(const UniformsLoader& subLoader, LoadingFilter loadingFilter)
	{
		for(NodeConstIterator it = subLoader.nodes.begin(); it!=subLoader.nodes.end(); it++)
		{
			std::map<std::string, UniformsLoader::Node>::iterator similarIt = nodes.find(it->first);
			
			if(similarIt==nodes.end())
			{
				if(loadingFilter==LoadAll || loadingFilter==NewOnly || loadingFilter==NewOnlySilent)
					nodes[it->first] = it->second;
				else if(loadingFilter==ReplaceOnly)
					throw Exception("The element with the name \"" + it->second.getName() + "\" is not currently registered (loadingFilter=ReplaceOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==ReplaceOnlySilent, nothing.
			}
			else
			{
				if(loadingFilter==LoadAll || loadingFilter==ReplaceOnly || loadingFilter==ReplaceOnlySilent)
					similarIt->second.softCopy(it->second, loadingFilter);
				else if(loadingFilter==NewOnly)
					throw Exception("The element with the name \"" + it->second.getName() + "\" is already registered (loadingFilter=NewOnly).", __FILE__, __LINE__, Exception::ModuleException);
				//else loadingFilter==NewOnlySilent, nothing.
			}
		}
	}

	/**
	\fn bool UniformsLoader::empty(void) const
	\brief Test if this object has some data loaded.
	\return True if there is data loaded.
	**/
	bool UniformsLoader::empty(void) const
	{
		return nodes.empty();
	}

	/**
	\fn void UniformsLoader::clear(void)
	\brief Clear the full set of data.
	**/
	void UniformsLoader::clear(void)
	{
		nodes.clear();
	}

	/**
	\fn void UniformsLoader::clear(const std::string& name)
	\brief Clear the pipeline with the corresponding name.
	\param name The name of the pipeline to be erased.
	**/
	void UniformsLoader::clear(const std::string& name)
	{
		std::map<std::string, Node>::iterator it=nodes.find(name);

		if(it==nodes.end())
			throw Exception("UniformsLoader::clear - No pipeline named \"" + name + "\" was found.", __FILE__, __LINE__, Exception::ModuleException);
		else
			nodes.erase(it);
	}

	/**
	\fn bool UniformsLoader::hasPipeline(const std::string& name) const
	\brief Test if a pipeline has values attached in this UniformsLoader object (knowing its name).
	\param name The name of the pipeline to test (see Component::getName()).
	\return True if a pipeline with the correct name has values loaded in this UniformsLoader object.
	**/
	bool UniformsLoader::hasPipeline(const std::string& name) const
	{
		return nodes.find(name)!=nodes.end();
	}

	/**
	\fn std::vector<std::string> UniformsLoader::getPipelinesTypeNames(void) const
	\brief Get the typenames of all the pipelines which have data loaded.
	\return A vector of strings, each one is a known pipeline typename (see AbstractComponentLayout::getTypeName()).
	**/
	std::vector<std::string> UniformsLoader::getPipelinesTypeNames(void) const
	{
		std::vector<std::string> namesList;

		for(std::map<std::string, Node>::const_iterator it=nodes.begin(); it!=nodes.end(); it++)
			namesList.push_back(it->first);

		return namesList;
	}

	/**
	\fn const UniformsLoader::Node& UniformsLoader::getRootNode(const std::string& name) const
	\brief Access the container node of a pipeline type.
	\param name The type name of the targeted pipeline.
	\return A (constant) reference to the UniformsLoader::Node object of the correct typename. Raise an exception if any error occur.
	**/
	const UniformsLoader::Node& UniformsLoader::getRootNode(const std::string& name) const
	{
		std::map<const std::string, Node>::const_iterator it=nodes.find(name);

		if(it==nodes.end())
			throw Exception("UniformsLoader::getRootNode - No pipeline named \"" + name + "\" was found.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn UniformsLoader::Node& UniformsLoader::getRootNode(const std::string& name)
	\brief Access the container node of a pipeline type.
	\param name The type name of the targeted pipeline.
	\return A (constant) reference to the UniformsLoader::Node object of the correct typename. Raise an exception if any error occur.
	**/
	UniformsLoader::Node& UniformsLoader::getRootNode(const std::string& name)
	{
		
		std::map<const std::string, Node>::iterator it=nodes.find(name);

		if(it==nodes.end())
			throw Exception("UniformsLoader::getRootNode - No pipeline named \"" + name + "\" was found.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second;
	}

	/**
	\fn UniformsLoader::NodeConstIterator UniformsLoader::rootNodeBegin(void) const
	\brief Get the 'begin' iterator on the nodes list.
	\return A UniformsLoader::NodeConstIterator on 'begin'. 
	**/
	UniformsLoader::NodeConstIterator UniformsLoader::rootNodeBegin(void) const
	{
		return nodes.begin();
	}

	/**
	\fn UniformsLoader::NodeConstIterator UniformsLoader::rootNodeEnd(void) const
	\brief Get the 'end' iterator on the nodes list.
	\return A UniformsLoader::NodeConstIterator on 'end'. 
	**/
	UniformsLoader::NodeConstIterator UniformsLoader::rootNodeEnd(void) const
	{
		return nodes.end();
	}

	/**
	\fn UniformsLoader::NodeIterator UniformsLoader::rootNodeBegin(void)
	\brief Get the 'begin' iterator on the nodes list.
	\return A UniformsLoader::NodeIterator on 'begin'. 
	**/
	UniformsLoader::NodeIterator UniformsLoader::rootNodeBegin(void)
	{
		return nodes.begin();
	}

	/**
	\fn UniformsLoader::NodeIterator UniformsLoader::rootNodeEnd(void)
	\brief Get the 'end' iterator on the nodes list.
	\return A UniformsLoader::NodeIterator on 'end'. 
	**/
	UniformsLoader::NodeIterator UniformsLoader::rootNodeEnd(void)
	{
		return nodes.end();
	}

	/**
	\fn int UniformsLoader::applyTo(Pipeline& pipeline, bool forceWrite, bool silent) const
	\brief Copy the possibly loaded set of uniforms variables to a pipeline (for the corresponding name of the Pipeline instance).
	\param pipeline The pipeline to which the data has to be copied (if relevant).
	\param forceWrite If set to false, only the variable marked as modified will be loaded (see UniformsLoader::Resource::modified).
	\param silent If true, no exception will be reported (aside from internal exception).
	\return The total number of uniforms variables actually copied.
	**/
	int UniformsLoader::applyTo(Pipeline& pipeline, bool forceWrite, bool silent) const
	{
		std::map<std::string, Node>::const_iterator it=nodes.find(pipeline.getTypeName());

		if(it==nodes.end())
		{
			if(!silent)
				throw Exception("UniformsLoader::clear - No pipeline with type name \"" + pipeline.getTypeName() + "\" was found.", __FILE__, __LINE__, Exception::ModuleException);
			else
				return 0;
		}
		else
		{
			int c = 0;

			try
			{
			 	c = it->second.applyTo(pipeline, pipeline, forceWrite);
			}
			catch(Exception& e)
			{
				if(!silent)
				{
					Exception m("UniformsLoader::applyTo - Exception while modifying pipeline " + pipeline.getFullName() + " : ", __FILE__, __LINE__, Exception::ModuleException);
					m << e;
					throw m;
				}
			}	

			return c;
		}
	}

	/**
	\fn std::string UniformsLoader::getCode(void) const
	\brief Return the corresponding code for the current set of variables, for one, or multiple, pipelines. The format is human-readable.
	\return A standard string containing the structured data.
	**/
	std::string UniformsLoader::getCode(void) const
	{
		std::string code;

		for(std::map<std::string, Node>::const_iterator it=nodes.begin(); it!=nodes.end(); )
		{
			if(!it->second.empty())
				code += it->second.getCodeElement().getCode();

			it++;

			if(it!=nodes.end())
				code += '\n';
		}

		return code;
	}

	/**
	\fn std::string UniformsLoader::getCode(const std::string& name) const
	\brief Return the corresponding code of the corresponding pipeline. The format is human-readable.
	\param name The name of the pipeline to target.
	\return A standard string containing the structured data. Raise an Exception if no pipeline is found.
	**/
	std::string UniformsLoader::getCode(const std::string& name) const
	{
		std::map<std::string, Node>::const_iterator it=nodes.find(name);

		if(it==nodes.end())
			throw Exception("UniformsLoader::getCode - No pipeline named \"" + name + "\" found.", __FILE__, __LINE__, Exception::ModuleException);
		else
			return it->second.getCodeElement().getCode();
	}

	/**
	\fn void UniformsLoader::writeToFile(const std::string& filename) const
	\brief Write the corresponding code for the current set of variables, for one, or multiple, pipelines to a file. The format is human-readable. Note : in order to append, you must load the original first.
	\param filename The filename of the file to write to.
	**/
	void UniformsLoader::writeToFile(const std::string& filename) const
	{
		std::fstream file;

		file.open(filename.c_str(), std::ios_base::out);

		if(!file.is_open())
			throw Exception("UniformsLoader::writeToFile - Cannot open file : \"" + filename + "\" for writing.", __FILE__, __LINE__, Exception::ModuleException);

		file << getCode();

		file.close();
	}

	/**
	\fn const char* UniformsLoader::getKeyword(UniformsLoaderKeyword k)
	\brief Get the actual keyword string.
	\param k The index of the keyword.
	\return A const pointer to a C-style character string.
	**/
	const char* UniformsLoader::getKeyword(UniformsLoaderKeyword k)
	{
		if(k>=0 && k<UL_NumKeywords)
			return keywords[k];
		else if(k==UL_UnknownKeyword)
			return "<Unknown Keyword>";
		else
			throw Exception("LayoutLoader::getKeyword - Invalid keyword of index " + toString(k) + ".", __FILE__, __LINE__, Exception::ModuleException);
	}

