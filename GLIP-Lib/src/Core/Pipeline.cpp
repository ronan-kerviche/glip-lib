/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Pipeline.cpp                                                                              */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Pipeline object                                                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Pipeline.cpp
 * \brief   Pipeline object
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include <limits>
#include <map>
#include "Exception.hpp"
#include "Pipeline.hpp"
#include "Component.hpp"
#include "HdlFBO.hpp"
#include "devDebugTools.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// __ReadOnly_PipelineLayout
	/**
	\fn __ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const std::string& type)
	\brief __ReadOnly_PipelineLayout constructor.
	\param type Typename of the pipeline.
	**/
	__ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const std::string& type)
	 : __ReadOnly_ComponentLayout(type)
	{ }

	/**
	\fn __ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const __ReadOnly_PipelineLayout& c)
	\brief __ReadOnly_PipelineLayout constructor.
	\param c Copy.
	**/
	__ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const __ReadOnly_PipelineLayout& c)
	 : __ReadOnly_ComponentLayout(c)
	{
		// Copy of the whole vectors : 
		elementsKind   	= c.elementsKind;
		elementsName	= c.elementsName;
		elementsID     	= c.elementsID;
		connections	= c.connections;

		for(int i=0; i<c.elementsLayout.size(); i++)
		{
			switch(elementsKind[i])
			{
				case FILTER:
					elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(new __ReadOnly_FilterLayout(c.filterLayout(i))));
					break;
				case PIPELINE:
					elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(new __ReadOnly_PipelineLayout(c.pipelineLayout(i))));
					break;
				default:
					throw Exception("__ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout - Unknown type for copy for element in " + getFullName(), __FILE__, __LINE__);
			}
		}
		//std::cout << "end copy of pipeline layout for " << getFullName() << std::endl;
	}

	__ReadOnly_PipelineLayout::~__ReadOnly_PipelineLayout(void)
	{
		for(int k=0; k<elementsLayout.size(); k++)
		{
			switch(elementsKind[k])
			{
				case FILTER:
					delete &filterLayout(k);
					break;
				case PIPELINE:
					delete &pipelineLayout(k);
					break;
				default:
					throw Exception("__ReadOnly_PipelineLayout::~__ReadOnly_PipelineLayout - Unknown type for delete for element in " + getFullName(), __FILE__, __LINE__);
			}
		}
		
		connections.clear();
		elementsKind.clear();
		elementsName.clear();
		elementsID.clear();
	}

	/**
	\fn int __ReadOnly_PipelineLayout::getElementID(int i)
	\brief Get element ID in global structure.
	\param i The ID of the element in the local pipeline layout.
	\return The ID of the element in the global structure or raise an exception if any errors occur.
	**/
	int __ReadOnly_PipelineLayout::getElementID(int i) const
	{
		checkElement(i);

		if(elementsID[i]==ELEMENT_NOT_ASSOCIATED)
			throw Exception("__ReadOnly_PipelineLayout::getElementID - Element " + to_string(i) + " is not associated. Is this object part of a Pipeline?", __FILE__, __LINE__);

		return elementsID[i];
	}

	/**
	\fn int __ReadOnly_PipelineLayout::getElementID(const std::string& name)
	\brief Get element ID in global structure.
	\param name The ID of the element in the local pipeline layout.
	\return The ID of the element in the global structure or raise an exception if any errors occur.
	**/
	int __ReadOnly_PipelineLayout::getElementID(const std::string& name) const
	{
		return getElementID( getElementIndex(name) );
	}

	/**
	\fn void __ReadOnly_PipelineLayout::setElementID(int i, int ID)
	\brief Set element ID in global structure or raise an exception if any errors occur.
	\param i The ID of the element in the local pipeline layout.
	\param ID The ID of the element in the global structure.
	**/
	void __ReadOnly_PipelineLayout::setElementID(int i, int ID)
	{
		checkElement(i);
		elementsID[i] = ID;
	}

	/**
	\fn __ReadOnly_PipelineLayout::Connection __ReadOnly_PipelineLayout::getConnection(int i) const
	\brief Get the connection by its ID.
	\param i The ID of the connection.
	\return A copy of the corresponding Connection object or raise an exception if any errors occur.
	**/
	__ReadOnly_PipelineLayout::Connection __ReadOnly_PipelineLayout::getConnection(int i) const
	{
		if(i<0 || i>=connections.size())
			throw Exception("__ReadOnly_PipelineLayout::getConnection - Bad connection ID for "  + getFullName() + ", ID : " + to_string(i), __FILE__, __LINE__);
		return connections[i];
	}

	/**
	\fn void __ReadOnly_PipelineLayout::checkElement(int i) const
	\brief Check if element exists and raise an exception if any errors occur.
	\param i The ID of the element.
	**/
	void __ReadOnly_PipelineLayout::checkElement(int i) const
	{
		if(i<0 || i>=elementsLayout.size())
			throw Exception("__ReadOnly_PipelineLayout::checkElement - Bad element ID for "  + getFullName() + ", ID : " + to_string(i), __FILE__, __LINE__);
	}

	/**
	\fn int __ReadOnly_PipelineLayout::getNumElements(void) const
	\brief Get the number of elements.
	\return Number of elements.
	**/
	int __ReadOnly_PipelineLayout::getNumElements(void) const
	{
		return elementsLayout.size();
	}

	/**
	\fn int __ReadOnly_PipelineLayout::getNumConnections(void) const
	\brief Get the number of connections.
	\return Number of connections.
	**/
	int __ReadOnly_PipelineLayout::getNumConnections(void) const
	{
		return connections.size();
	}

	/**
	\fn void __ReadOnly_PipelineLayout::getInfoElements(int& numFilters, int& numPipelines)
	\brief Get the total number of Filters and Pipelines contained by this pipeline.
	\param numFilters The total number of filters.
	\param numPipelines The total number of pipelines (including this one).
	**/
	void __ReadOnly_PipelineLayout::getInfoElements(int& numFilters, int& numPipelines)
	{
		int a, b;
		__ReadOnly_PipelineLayout* tmp = NULL;
		numFilters  	= 0;
		numPipelines	= 0;

		for(int i=0; i<elementsLayout.size(); i++)
		{
			switch(elementsKind[i])
			{
				case FILTER:
					numFilters++;
					break;
				case PIPELINE:
					tmp = reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[i]);
					tmp->getInfoElements(a,b);
					numFilters   += a;
					numPipelines += b+1;
					break;
				default:
					throw Exception("__ReadOnly_PipelineLayout::getInfoElements - Unknown type for element in " + getFullName(), __FILE__, __LINE__);
			}
		}

		numPipelines++; // include this
	}

	/**
	\fn int __ReadOnly_PipelineLayout::getElementIndex(const std::string& name) const
	\brief Get the ID of an element knowing its name.
	\param name The name of the element.
	\return The ID of the element or raise an exception if any errors occur.
	**/
	int __ReadOnly_PipelineLayout::getElementIndex(const std::string& name) const
	{
		std::vector<std::string>::const_iterator it = std::find(elementsName.begin(), elementsName.end(), name);

		if(it==elementsName.end())
			throw Exception("__ReadOnly_PipelineLayout::getElementIndex - Unable to find element \"" + name + "\" in pipeline layout " + getFullName() + ".", __FILE__, __LINE__);
		else
			return std::distance(elementsName.begin(), it);
	}

	/**
	\fn bool __ReadOnly_PipelineLayout::doesElementExist(const std::string& name) const
	\brief Check if an element exists knowing its name.
	\param name The name of the element.
	\return True if such an element exists, False otherwise.
	**/
	bool __ReadOnly_PipelineLayout::doesElementExist(const std::string& name) const
	{
		std::vector<std::string>::const_iterator it = std::find(elementsName.begin(), elementsName.end(), name);

		return it!=elementsName.end();
	}

	/**
	\fn ComponentKind __ReadOnly_PipelineLayout::getElementKind(int i) const
	\brief Get the kind of an element.
	\param i The ID of the element.
	\return The kind of the element or raise an exception if any errors occur.
	**/
	ComponentKind __ReadOnly_PipelineLayout::getElementKind(int i) const
	{
		checkElement(i);
		return elementsKind[i];
	}

	/**
	\fn const std::string& __ReadOnly_PipelineLayout::getElementName(int i) const
	\brief Get the name of an element.
	\param i The name of the element.
	\return The name of the element or raise an exception if any errors occur.
	**/
	const std::string& __ReadOnly_PipelineLayout::getElementName(int i) const
	{
		checkElement(i);
		return elementsName[i];
	}

	/**
	\fn __ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(int i) const
	\brief Get the component layout by its index.
	\param i The ID of the component.
	\return A reference to the component or raise an exception if any errors occur.
	**/
	__ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(int i) const
	{
		checkElement(i);

		//std::cout << "ACCESSING COMPONENT (int)" << std::endl;
		//return *elementsLayout[i];
		switch(elementsKind[i])
		{
			case FILTER:
				return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[i]);
			case PIPELINE:
				return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[i]);
			default :
				throw Exception("__ReadOnly_PipelineLayout::componentLayout - Type not recognized for element in " + getFullName() + ".",__FILE__, __LINE__);
		}
	}

	/**
	\fn __ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(const std::string& name) const
	\brief Get the component layout by its name.
	\param name The name of the element.
	\return A reference to the component or raise an exception if any errors occur.
	**/
	__ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(const std::string& name) const
	{
		int index = getElementIndex(name);
		//std::cout << "ACCESSING COMPONENT (int)" << std::endl;
		return componentLayout(index);
	}

	/**
	\fn __ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(int i) const
	\brief Get the filter layout by its index.
	\param i The ID of the filter layout.
	\return A reference to the filter layout or raise an exception if any errors occur.
	**/
	__ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(int i) const
	{
		//std::cout << "ACCESSING FILTER (int)" << std::endl;
		checkElement(i);
		if(getElementKind(i)!=FILTER)
			throw Exception("__ReadOnly_PipelineLayout::filterLayout - The element of index " + to_string(i) + " exists but is not a filter in pipeline " + getFullName() + ".", __FILE__, __LINE__);
		return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[i]);
	}

	/**
	\fn __ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(const std::string& name) const
	\brief Get the filter layout by its name.
	\param name The name of the filter layout.
	\return A reference to the filter layout or raise an exception if any errors occur.
	**/
	__ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(const std::string& name) const
	{
		//std::cout << "ACCESSING FILTER (name)" << std::endl;
		int index = getElementIndex(name);
		if(getElementKind(index)!=FILTER)
			throw Exception("__ReadOnly_PipelineLayout::filterLayout - The element " + name + " exists but is not a filter in pipeline " + getFullName() + ".", __FILE__, __LINE__);
		return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[index]);
	}

	/**
	\fn __ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(int i) const
	\brief Get the pipeline layout by its index.
	\param i The ID of the pipeline layout.
	\return A reference to the pipeline layout or raise an exception if any errors occur.
	**/
	__ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(int i) const
	{
		//std::cout << "ACCESSING PIPELINE (int)" << std::endl;
		checkElement(i);
		if(getElementKind(i)!=PIPELINE)
			throw Exception("__ReadOnly_PipelineLayout::pipelineLayout - The element of index " + to_string(i) + " exists but is not a pipeline in pipeline " + getFullName() + ".", __FILE__, __LINE__);
		return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[i]);
	}

	/**
	\fn __ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(const std::string& name) const
	\brief Get the pipeline layout by its name.
	\param name The name of the pipeline layout.
	\return A reference to the pipeline layout or raise an exception if any errors occur.
	**/
	__ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(const std::string& name) const
	{
		//std::cout << "ACCESSING PIPELINE (name)" << std::endl;
		int index = getElementIndex(name);
		if(getElementKind(index)!=PIPELINE)
			throw Exception("__ReadOnly_PipelineLayout::pipelineLayout - The element " + name + " exists but is not a pipeline in pipeline " + getFullName() + ".", __FILE__, __LINE__);
		return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[index]);
	}

	__ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(const std::vector<std::string>& path)
	{
		__ReadOnly_PipelineLayout* ptr = this;

		for(std::vector<std::string>::const_iterator it=path.begin(); it!=path.end(); it++)
			ptr = &ptr->pipelineLayout(*it);
		
		return *ptr;
	}

	/**
	\fn std::vector<__ReadOnly_PipelineLayout::Connection> __ReadOnly_PipelineLayout::getConnectionDestinations(int id, int p)
	\brief Get all destinations of an output port.
	\param id The ID of the output element.
	\param p The port of the output element.
	\return A vector of Connection object, all having output as (id,p) or raise an exception if any errors occur.
	**/
	std::vector<__ReadOnly_PipelineLayout::Connection> __ReadOnly_PipelineLayout::getConnectionDestinations(int id, int p)
	{
		if(id!=THIS_PIPELINE)
		{
			__ReadOnly_ComponentLayout& src = componentLayout(id);
			src.checkOutputPort(p);
		}
		else
			checkInputPort(p);

		// The Element and its port exist, now find their connexions
		std::vector<Connection> result;
		for(std::vector<Connection>::iterator it=connections.begin(); it!=connections.end(); it++)
			if( (*it).idOut==id && (*it).portOut==p) result.push_back(*it);

		return result;
	}

	/**
	\fn __ReadOnly_PipelineLayout::Connection __ReadOnly_PipelineLayout::getConnectionSource(int id, int p)
	\brief Get the source of an input port.
	\param id The ID of the input element.
	\param p The port of the input element.
	\return A Connection object, having input as (id,p) or raise an exception if any errors occur.
	**/
	__ReadOnly_PipelineLayout::Connection __ReadOnly_PipelineLayout::getConnectionSource(int id, int p)
	{
		std::string str;

		if(id!=THIS_PIPELINE)
		{
			__ReadOnly_ComponentLayout& src = componentLayout(id);
			src.checkInputPort(p);
		}
		else
			checkOutputPort(p);

		// The Element and its port exist, now find the connexion
		for(std::vector<Connection>::iterator it=connections.begin(); it!=connections.end(); it++)
			if( (*it).idIn==id && (*it).portIn==p) return (*it);

		if(id!=THIS_PIPELINE)
		{
			__ReadOnly_ComponentLayout& src = componentLayout(id);
			throw Exception("Element " + src.getFullName() + " has no source on output port " + src.getInputPortName(p) + " in pipeline " + getFullName() + ".", __FILE__, __LINE__);
		}
		else
			throw Exception("This Pipeline " + getFullName() + " has no source on output port " + getOutputPortName(p) + ".", __FILE__, __LINE__);
	}

	/**
	\fn std::string __ReadOnly_PipelineLayout::getConnectionDestinationsName(int source, int port)
	\brief Get the name of the elements linked to the output.
	\param source The ID of the output element.
	\param port The port of the output element.
	\return A standard string containing all the elements name linked to the output or raise an exception if any errors occur.
	**/
	std::string __ReadOnly_PipelineLayout::getConnectionDestinationsName(int source, int port)
	{
		std::vector<Connection> res = getConnectionDestinations(source, port);
		std::string result;

		std::cerr << "Using possibly deprecated function __ReadOnly_PipelineLayout::getConnectionDestinationsName." << std::endl;

		for(std::vector<Connection>::iterator it=res.begin(); it!=res.end(); it++)
		{
			__ReadOnly_ComponentLayout& tmp = componentLayout((*it).idIn);
			//OLD : result += tmp.getFullName() + SEPARATOR + tmp.getInputPortNameExtended((*it).portIn) + "\n";
			result += tmp.getInputPortName( (*it).portIn );
		}

		return result;
	}

	/**
	\fn std::string __ReadOnly_PipelineLayout::getConnectionDestinationsName(const std::string& source, const std::string& port)
	\brief Get the name of the elements linked to the output.
	\param source The name of the output element.
	\param port The name of the port of the output element.
	\return A standard string containing all the elements name linked to the output or raise an exception if any errors occur.
	**/
	std::string __ReadOnly_PipelineLayout::getConnectionDestinationsName(const std::string& source, const std::string& port)
	{
		int id = getElementIndex(source);
		int p  = componentLayout(id).getOutputPortID(port);
		return getConnectionDestinationsName(id, p);
	}

	/**
	\fn std::string __ReadOnly_PipelineLayout::getConnectionSourceName(int dest, int port)
	\brief Get the name of the element linked to the input.
	\param dest The ID of the input element.
	\param port The port of the input element.
	\return A standard string containing the element name linked to the input or raise an exception if any errors occur.
	**/
	std::string __ReadOnly_PipelineLayout::getConnectionSourceName(int dest, int port)
	{
		Connection c = getConnectionSource(dest, port);
		__ReadOnly_ComponentLayout& tmp = componentLayout(c.idOut);

		std::cerr << "Using possibly deprecated function __ReadOnly_PipelineLayout::getConnectionSourceName." << std::endl;
		//return tmp.getFullName() + SEPARATOR + tmp.getInputPortNameExtended(c.portOut) + "\n";

		return tmp.getInputPortName(c.portOut);
	}

	/**
	\fn std::string __ReadOnly_PipelineLayout::getConnectionSourceName(const std::string& dest, const std::string& port)
	\brief Get the name of the element linked to the input.
	\param dest The name of the input element.
	\param port The name of the port of the input element.
	\return A standard string containing the element name linked to the input or raise an exception if any errors occur.
	**/
	std::string __ReadOnly_PipelineLayout::getConnectionSourceName(const std::string& dest, const std::string& port)
	{
		int id = getElementIndex(dest);
		int p  = componentLayout(id).getOutputPortID(port);
		return getConnectionSourceName(id, p);
	}

	/**
	\fn bool __ReadOnly_PipelineLayout::check(bool exception)
	\brief Check the validity of the pipeline layout.
	\param exception If set to true, an exception would be raised if any error is found.
	\return true if valid, false otherwise.
	**/
	bool __ReadOnly_PipelineLayout::check(bool exception)
	{
		std::string res;

		for(int i=0; i<elementsLayout.size(); i++)
		{
			__ReadOnly_ComponentLayout& tmp = componentLayout(i);
			for(int j=0; j<tmp.getNumInputPort(); j++)
			{
				try
				{
					getConnectionSource(i, j);
				}
				catch(Exception& e)
				{
					res += e.what();
					res += '\n';
				}
				catch(std::exception& e)
				{
					res += e.what();
					res += '\n';
				}
			}

			for(int j=0; j<tmp.getNumOutputPort(); j++)
			{
				if(getConnectionDestinations(i,j).empty())
				{
					res += "__ReadOnly_PipelineLayout::check - Error : Element " + tmp.getFullName() + " output port " +  tmp.getOutputPortName(i) + " is not connected.";
					res += '\n';
				}
			}
		}

		for(int i=0; i<getNumOutputPort(); i++)
		{
			try
			{
				getConnectionSource(THIS_PIPELINE, i);
			}
			catch(Exception& e)
			{
				res += e.what();
				res += '\n';
			}
			catch(std::exception& e)
			{
				res += e.what();
				res += '\n';
			}
		}

		for(int i=0; i<getNumInputPort(); i++)
		{
			if(getConnectionDestinations(THIS_PIPELINE,i).empty())
			{
				res += "__ReadOnly_PipelineLayout::check - Error : Input port " + getInputPortName(i) + " is not connected inside the pipeline.";
				res += '\n';
			}
		}

		if(exception && !res.empty())
			throw Exception("check - The following errors has been found in the PipelineLayout " + getFullName() + " : \n" + res + ".", __FILE__, __LINE__);
		else
			return false;

		return true;
	}

// PipelineLayout
	/**
	\fn PipelineLayout::PipelineLayout(const std::string& type)
	\brief PipelineLayout constructor.
	\param type The typename.
	**/
	PipelineLayout::PipelineLayout(const std::string& type)
	 : __ReadOnly_ComponentLayout(type), ComponentLayout(type), __ReadOnly_PipelineLayout(type)
	{ }

	/**
	\fn PipelineLayout::PipelineLayout(const __ReadOnly_PipelineLayout& c)
	\brief PipelineLayout constructor.
	\param c Copy.
	**/
	PipelineLayout::PipelineLayout(const __ReadOnly_PipelineLayout& c)
	 : __ReadOnly_PipelineLayout(c), __ReadOnly_ComponentLayout(c), ComponentLayout(c)
	{ }

	/**
	\fn int PipelineLayout::add(const __ReadOnly_FilterLayout& filterLayout, const std::string& name)
	\brief Add a filter to the pipeline layout.
	\param filterLayout The filter layout.
	\param name The name of the element.
	\return The ID of the element added.
	**/
	int PipelineLayout::add(const __ReadOnly_FilterLayout& filterLayout, const std::string& name)
	{
		if(doesElementExist(name))
			throw Exception("PipelineLayout::add - An element with the name " + name + " already exists in pipeline " + getFullName() + ".", __FILE__, __LINE__);

		__ReadOnly_FilterLayout* tmp = new __ReadOnly_FilterLayout(filterLayout);
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "PipelineLayout::add : <" << name << '>' << std::endl;
		#endif
		elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(tmp));
		elementsKind.push_back(FILTER);
		elementsName.push_back(name);
		elementsID.push_back(ELEMENT_NOT_ASSOCIATED);
		return elementsLayout.size()-1;
	}

	/**
	\fn int PipelineLayout::add(const __ReadOnly_PipelineLayout& pipelineLayout, const std::string& name)
	\brief Add a subpipeline to the pipeline layout.
	\param pipelineLayout The pipeline layout.
	\param name The name of the element.
	\return The ID of the element added.
	**/
	int PipelineLayout::add(const __ReadOnly_PipelineLayout& pipelineLayout, const std::string& name)
	{
		if(doesElementExist(name))
			throw Exception("PipelineLayout::add - An element with the name " + name + " already exists in pipeline " + getFullName() + ".", __FILE__, __LINE__);

		__ReadOnly_PipelineLayout* tmp = new __ReadOnly_PipelineLayout(pipelineLayout);
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "PipelineLayout::add : <" << name << '>' << std::endl;
		#endif
		elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(tmp));
		elementsKind.push_back(PIPELINE);
		elementsName.push_back(name);
		elementsID.push_back(ELEMENT_NOT_ASSOCIATED);
		return elementsLayout.size()-1;
	}

	/**
	\fn int PipelineLayout::addInput(const std::string& name)
	\brief Add an input port to the pipeline layout.
	\param name The name of the new input port.
	\return The ID of the new input port.
	**/
	int PipelineLayout::addInput(const std::string& name)
	{
		return addInputPort(name);
	}

	/**
	\fn int PipelineLayout::addOutput(const std::string& name)
	\brief Add an output port to the pipeline layout.
	\param name The name of the new output port.
	\return The ID of the new output port.
	**/
	int PipelineLayout::addOutput(const std::string& name)
	{
		return addOutputPort(name);
	}

	/**
	\fn void PipelineLayout::connect(int filterOut, int portOut, int filterIn, int portIn)
	\brief Create a connection between two elements or an element and this pipeline and raise an exception if any errors occur.
	\param filterOut The ID of the output element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portOut The ID of the output port.
	\param filterIn The ID of the input element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portIn The ID of the input port.
	**/
	void PipelineLayout::connect(int filterOut, int portOut, int filterIn, int portIn)
	{
		if(filterOut==THIS_PIPELINE && filterIn==THIS_PIPELINE)
			throw Exception("PipelineLayout::connect - can't connect directly an input to an output in pipeline " + getFullName() + ", you don't need that.", __FILE__, __LINE__);

		if(filterOut!=THIS_PIPELINE)
		{
			__ReadOnly_ComponentLayout& fo = componentLayout(filterOut); // Source
			fo.checkOutputPort(portOut);
		}
		else
			checkInputPort(portOut);

		if(filterIn!=THIS_PIPELINE)
		{
			__ReadOnly_ComponentLayout& fi = componentLayout(filterIn);  // Destination
			fi.checkInputPort(portIn);
		}
		else
			checkOutputPort(portIn);

		// Check if a connexion already exist to the destination :
		for(std::vector<Connection>::iterator it=connections.begin(); it!=connections.end(); it++)
			if( (*it).idIn==filterIn && (*it).portIn==portIn)
			{
				if(filterIn!=THIS_PIPELINE)
				{
					throw Exception("PipelineLayout::connect - A connexion already exists to the destination : " + componentLayout(filterIn).getFullName() + " on port " + componentLayout(filterIn).getInputPortName(portIn) + " in pipeline " + getFullName() + ".", __FILE__, __LINE__);
				}
				else
				{
					throw Exception("PipelineLayout::connect - A connexion already exists to this pipeline output : " + getFullName() + " on port " + getInputPortName(portIn) + " in pipeline " + getFullName() + ".", __FILE__, __LINE__);
				}
			}

		Connection c;
		c.idOut   = filterOut;
		c.portOut = portOut;
		c.idIn    = filterIn;
		c.portIn  = portIn;

		//std::cout << "Connexion de " << filterOut << ':' << portOut << " à " << filterIn << ':' << portIn << std::endl;

		connections.push_back(c);
	}

	/**
	\fn void PipelineLayout::connect(const std::string& filterOut, const std::string& portOut, const std::string& filterIn, const std::string& portIn)
	\brief Create a connection between two elements or an element and this pipeline and raise an exception if any errors occur.
	\param filterOut The name of the output element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portOut The name of the output port.
	\param filterIn The name of the input element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portIn The name of the input port.
	**/
	void PipelineLayout::connect(const std::string& filterOut, const std::string& portOut, const std::string& filterIn, const std::string& portIn)
	{
		int fi = getElementIndex(filterIn),
		fo = getElementIndex(filterOut),
		pi = componentLayout(filterIn).getInputPortID(portIn),
		po = componentLayout(filterOut).getOutputPortID(portOut);

		connect(fo, po, fi, pi); // Check-in done twice but...
	}

	/**
	\fn void PipelineLayout::connectToInput(int port, int filterIn,  int portIn)
	\brief Create a connection between an input port of this pipeline and one of its element and raise an exception if any errors occur.
	\param port The ID of the port for this pipeline.
	\param filterIn The ID of the input element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portIn The ID of the input port.
	**/
	void PipelineLayout::connectToInput(int port, int filterIn,  int portIn)
	{
		connect(THIS_PIPELINE, port, filterIn, portIn);
	}

	/**
	void PipelineLayout::connectToInput(const std::string& port, const std::string& filterIn, const std::string& portIn)
	\brief Create a connection between an input port of this pipeline and one of its element and raise an exception if any errors occur.
	\param port The name of the port for this pipeline.
	\param filterIn The name of the input element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portIn The name of the input port.
	**/
	void PipelineLayout::connectToInput(const std::string& port, const std::string& filterIn, const std::string& portIn)
	{
		try
		{
			int p  = getInputPortID(port),
			fi = getElementIndex(filterIn),
			pi = componentLayout(filterIn).getInputPortID(portIn);
			connect(THIS_PIPELINE, p, fi, pi);
		}
		catch(Exception& e)
		{
			Exception m("PipelineLayout::connectToInput (str) - Caught an exception for the object " + getFullName() + ".", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("PipelineLayout::connectToInput (str) - Caught an exception for the object " + getFullName() + ".", __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn void PipelineLayout::connectToOutput(int filterOut, int portOut, int port)
	\brief Create a connection between an output port of this pipeline and one of its element and raise an exception if any errors occur.
	\param filterOut The ID of the output element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portOut The ID of the output port.
	\param port The ID of the port for this pipeline.
	**/
	void PipelineLayout::connectToOutput(int filterOut, int portOut, int port)
	{
		connect(filterOut, portOut, THIS_PIPELINE, port);
	}

	/**
	\fn void PipelineLayout::connectToOutput(const std::string& filterOut, const std::string& portOut, const std::string& port)
	\brief Create a connection between an output port of this pipeline and one of its element and raise an exception if any errors occur.
	\param filterOut The name of the output element (a FilterLayout, a PipelineLayout or THIS_PIPELINE).
	\param portOut The name of the output port.
	\param port The name of the port for this pipeline.
	**/
	void PipelineLayout::connectToOutput(const std::string& filterOut, const std::string& portOut, const std::string& port)
	{
		try
		{
			int p  = getOutputPortID(port),
			fo = getElementIndex(filterOut),
			po = componentLayout(filterOut).getOutputPortID(portOut);
			connect(fo, po, THIS_PIPELINE, p);
		}
		catch(Exception& e)
		{
			Exception m("PipelineLayout::connectToOutput (str) - Caught an exception for the object " + getFullName() + ".", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("PipelineLayout::connectToOutput (str) - Caught an exception for the object " + getFullName() + ".", __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn void PipelineLayout::autoConnect(void)
	\brief Create automatically all connections based on the name of the input and output ports and the name of the input and output textures of all Filters.

	This function will automatically make the connection based on the name of the input and output ports and the name of the different textures in the shaders used.

	All of the names of the output textures (defined with <i>out vec4</i> in the fragment shaders) and the input ports of this pipeline <b>must be unique</b>, it will raise an exception otherwise.
	**/
	void PipelineLayout::autoConnect(void)
	{
		try
		{
			// Check for previous existing connections
			if(getNumConnections()!=0)
				throw Exception("PipelineLayout::autoConnect - Layout for " + getFullName() + " has already connections and thus is not eligible to auto-connect.", __FILE__, __LINE__);

			// Check for double names in outputs :
			std::map<std::string,int> outputNames;

			// Push the inputs of this :
			for(int i=0; i<getNumInputPort(); i++)
				outputNames[getInputPortName(i)] = THIS_PIPELINE;

			// Add all of the outputs of all sub components :
			for(int i=0; i<getNumElements(); i++)
			{
				__ReadOnly_ComponentLayout& cp = componentLayout(i);

				for(int j=0; j<cp.getNumOutputPort(); j++)
				{
					std::string name = cp.getOutputPortName(j);

					// Check for doubles :
					std::map<std::string,int>::iterator it = outputNames.find(name);
					if(it!=outputNames.end())
						throw Exception("PipelineLayout::autoConnect - Found another output having the same name (" + name + " for PipelineLayout " + getFullName() + ".", __FILE__, __LINE__);
					else
						outputNames[name] = i;
				}
			}

			// Layout is clean : start auto-connect
			for(int i=0; i<getNumElements(); i++)
			{
				__ReadOnly_ComponentLayout& cp = componentLayout(i);
				for(int j=0; j<cp.getNumInputPort(); j++)
				{
					std::map<std::string,int>::iterator it = outputNames.find(cp.getInputPortName(j));

					if(it==outputNames.end())
						throw Exception("PipelineLayout::autoConnect - No elements were found having an output named " + cp.getInputPortName(j) + " for PipelineLayout " + getFullName() + ".", __FILE__, __LINE__);

					if(it->second!=THIS_PIPELINE)
						connect(getElementName(it->second), it->first, getElementName(i), cp.getInputPortName(j));
					else
						connectToInput(it->first, getElementName(i), cp.getInputPortName(j));
				}
			}

			// Connect output ports :
			for(int i=0; i<getNumOutputPort(); i++)
			{
				std::map<std::string,int>::iterator it = outputNames.find(getOutputPortName(i));

				if(it==outputNames.end())
					throw Exception("PipelineLayout::autoConnect - No elements were found having an output named " +getOutputPortName(i) + " for PipelineLayout " + getFullName() + ".", __FILE__, __LINE__);

				if(it->second!=THIS_PIPELINE)
					connectToOutput(getElementName(it->second), it->first, getOutputPortName(i));
				else
					throw Exception("PipelineLayout::autoConnect - can't connect directly an input to an output in pipeline " + getFullName() + ", you don't need that.", __FILE__, __LINE__);
			}
		}
		catch(Exception& e)
		{
			Exception m("PipelineLayout::autoConnect - An error occured while building connection for " + getFullName() + ".", __FILE__, __LINE__);
			throw m+e;
		}
	}

// Pipeline
	Pipeline::Pipeline(const __ReadOnly_PipelineLayout& p, const std::string& name, bool fake)
	 : __ReadOnly_ComponentLayout(p), __ReadOnly_PipelineLayout(p), Component(p, "(Intermediate : " + name + ")"), perfsMonitoring(false), queryObject(0)
	{ }

	/**
	\fn Pipeline::Pipeline(__ReadOnly_PipelineLayout& p, const std::string& name)
	\brief Pipeline constructor.
	\param p Pipeline layout.
	\param name Name of the pipeline.
	**/
	Pipeline::Pipeline(const __ReadOnly_PipelineLayout& p, const std::string& name)
	 : __ReadOnly_ComponentLayout(p), __ReadOnly_PipelineLayout(p), Component(p, name), perfsMonitoring(false), queryObject(0)
	{
		cleanInput();
		//outputBuffer.assign(getNumOutputPort(), 0);
                //outputBufferPort.assign(getNumOutputPort(), 0);

		std::vector<Connection> connections;
		int idx = THIS_PIPELINE;
		build(idx, filtersList, filtersGlobalIDsList, connections, *this);
		allocateBuffers(connections);
	}

	Pipeline::~Pipeline(void)
	{
		cleanInput();

		for(std::vector<HdlFBO*>::iterator it = buffersList.begin(); it!=buffersList.end(); it++)
			delete (*it);
		buffersList.clear();

		for(std::vector<Filter*>::iterator it = filtersList.begin(); it!=filtersList.end(); it++)
			delete (*it);
		filtersList.clear();

		if(queryObject>0)
			glDeleteQueries(1, &queryObject);
	}

	/**
	\fn void Pipeline::cleanInput(void)
	\brief Clean all inputs from previously acquired texture pointers.
	**/
	void Pipeline::cleanInput(void)
	{
		inputsList.clear();
	}

	void Pipeline::build(int& currentIdx, std::vector<Filter*>& filters, std::map<int, int>& filtersGlobalID, std::vector<Connection>& connections, __ReadOnly_PipelineLayout& originalLayout)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "BUILD" << std::endl;
		#endif

		const int thisPipelineIdx = currentIdx;
		currentIdx++;

		// Extract and build elements, get the connections done too : 
		try
		{
			std::vector<int> 	localToGlobalIdx;
			std::vector<Connection> localConnections;

			// First extract the elements : 
			for(int k=0; k<getNumElements(); k++)
			{
				ComponentKind currentElKind = getElementKind(k);

				if(currentElKind==FILTER)
				{
					#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
						std::cout << "    Adding a new filter" << std::endl;
					#endif
					originalLayout.setElementID(k, currentIdx);
					localToGlobalIdx.push_back(currentIdx);

					filters.push_back(new Filter(filterLayout(k), getElementName(k)));	

					// Save the link to the global ID : 
					filtersGlobalID[currentIdx] = filters.size()-1;	
					
					currentIdx++;
					#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
						std::cout << "    Adding : " << filters.back()->getFullName() << std::endl;
						std::cout << "    ID     : " << originalLayout.getElementID(k) << std::endl;
					#endif
				}
				else if(currentElKind==PIPELINE)
				{
					#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
							std::cout << "    Adding a new Pipeline" << std::endl;
					#endif					
					originalLayout.setElementID(k, currentIdx);
					localToGlobalIdx.push_back(currentIdx);

					// Create a sub-pipeline : 
					Pipeline tmpPipeline( pipelineLayout(k), getElementName(k), false);
					tmpPipeline.build(currentIdx, filters, filtersGlobalID, localConnections, pipelineLayout(k));

					currentIdx++;
					#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
						std::cout << "    Adding : " << pipelineLayout(k).getFullName() << std::endl;
						std::cout << "    ID     : " << originalLayout.getElementID(k) << std::endl;
					#endif
				}
				else
					throw Exception("Unknown exception kind.", __FILE__, __LINE__);
			}

			// Then change the connections : 
			// The main rule is : each pipeline scale MUST untangle and shorten all of ITS connection before passing to the upper level.
			std::vector<Connection> innerOutputConnections;
			for(int k=0; k<getNumConnections(); k++)
			{
				bool saveConnection = true;
				Connection c = getConnection(k);
	
				// If the output is this pipeline : 
				if(c.idOut==THIS_PIPELINE)
					c.idOut = thisPipelineIdx;
				else if(getElementKind(c.idOut)==PIPELINE) // or is a pipeline...
				{
					const int elIdx = localToGlobalIdx[c.idOut];

					// We have identify a connection for which the source is actually the output of a pipeline.
					// This means that this connection is either already identified in innerOutputConnections
					// (and thus removed from localConections) or that we have to find first.

					int idx = -1;
					for(int l=0; l<innerOutputConnections.size(); l++)
					{
						if(innerOutputConnections[l].idIn==elIdx && innerOutputConnections[l].portIn==c.portOut)
						{
							idx = l;
							break;
						}
					}

					// Otherwise, try to find it in localConnections and push it in innerOutputConnections :
					if(idx<0)
					{
						for(int l=0; l<localConnections.size(); l++)
						{
							if(localConnections[l].idIn==elIdx && localConnections[l].portIn==c.portOut)
							{
								idx = innerOutputConnections.size();
								innerOutputConnections.push_back( localConnections[l] );
								localConnections.erase( localConnections.begin()+l );		// We know the nature of this connection now, and we won't use it anymore in localConnections
								break;
							}
						}
					}

					// Manage a possible error : 
					if(idx<0)
						throw Exception("Unable to find interior connection to element " + componentLayout(c.idOut).getFullName() + ", port : " + to_string(c.portOut) + ".", __FILE__, __LINE__);

					// Finally : shorten the connection :
					c.idOut = innerOutputConnections[idx].idOut;
					c.portOut = innerOutputConnections[idx].portOut;
				}
				else // otherwise...
					c.idOut = localToGlobalIdx[c.idOut];

				// If the input is this pipeline : 
				if(c.idIn==THIS_PIPELINE)
					c.idIn = thisPipelineIdx;
				else if(getElementKind(c.idIn)==PIPELINE) // or is a pipeline...
				{
					const int elIdx = localToGlobalIdx[c.idIn];

					// Shorten paths : 
					for(int l=0; l<localConnections.size(); l++)
					{
						if(localConnections[l].idOut==elIdx && localConnections[l].portOut==c.portIn) // If the connections correspond to the other end.
						{
							localConnections[l].idOut = c.idOut;
							localConnections[l].portOut = c.portOut;
						}
					}

					saveConnection = false;
				}
				else // otherwise... 
					c.idIn = localToGlobalIdx[c.idIn];

				// Save connection : 
				if(saveConnection)
					connections.push_back(c);
			}

			// Finally, append to the connections list : 
			connections.insert(connections.end(), localConnections.begin(), localConnections.end());
		}
		catch(Exception& e)
		{
			Exception m("Pipeline::build - Error while building the pipeline " + getFullName() + " : ", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("Pipeline::build - Error (std) while building the pipeline " + getFullName() + " : ", __FILE__, __LINE__);
			throw m+e;
		}

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "END BUILD" << std::endl;
		#endif
	}

	void Pipeline::allocateBuffers(std::vector<Connection>& connections)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "ALLOCATE" << std::endl;
		#endif

		try
		{
			// The input is a list of all the connections, untangle, where the ID -1 is reserved for this pipeline.
			std::vector<Connection> 	remainingConnections = connections;	// The remaining (not processed) connections.
			std::vector<int>		requestedInputConnections;		// The number of connections not satisfied for this filter.
			std::vector<int>		bufferOccupancy;			// The number of filter which will use the buffer as inputs.
			std::vector<ActionHub>		tmpActions;				// The temporary actions list.
			bool allProcessed = false;

			// Initialize the outputs : 
			OutputHub blankOutput;
			blankOutput.bufferIdx = -1;
			blankOutput.outputIdx = -1;
			outputsList.assign( getNumOutputPort(), blankOutput );

			// Setup the requirements counters : 
			for(int k=0; k<filtersList.size(); k++)
			{
				ActionHub hub;

				hub.inputBufferIdx.assign( filtersList[k]->getNumInputPort(), -1);
				hub.inputArgumentIdx.assign( filtersList[k]->getNumInputPort(), -1);
				hub.bufferIdx		= -1;
				hub.filterIdx 		= k;

				tmpActions.push_back(hub);

				// Set the number of inputs not satisfied to be equal to the number of inputs : 
				requestedInputConnections.push_back( filtersList[k]->getNumInputPort() );
			}

			// Initialize by decrementing the connections to this pipeline inputs : 
			for(int k=0; k<remainingConnections.size(); k++)
			{
				if(remainingConnections[k].idOut==THIS_PIPELINE)
				{
					// Set up the links : 
					const int fid = filtersGlobalIDsList[remainingConnections[k].idIn];

					tmpActions[fid].inputBufferIdx[ remainingConnections[k].portIn ] 	= THIS_PIPELINE;
					tmpActions[fid].inputArgumentIdx[ remainingConnections[k].portIn ]	= remainingConnections[k].portOut;
					requestedInputConnections[fid]--;

					// Remove : 
					remainingConnections.erase( remainingConnections.begin() + k );
					k--;
				}
			}

			// Generate the final actions list and buffers : 
			do
			{

				#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
					std::cout << "    Finding candidates : " << std::endl;
				#endif
				// Explore the list to find the possibles solutions :
				std::vector<int> candidatesIdx;
				for(int k=0; k<requestedInputConnections.size(); k++)
				{
					//std::cout << k << " -> " << requestedInputConnections[k] << std::endl;
					if(requestedInputConnections[k]==0)
					{
						candidatesIdx.push_back(k);
						#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
							std::cout << "        Adding : " << filtersList[k]->getFullName() << std::endl;
						#endif
					}
				}

				if(candidatesIdx.empty())
					throw Exception("No available filter matches input conditions.", __FILE__, __LINE__);

				// Find best candidate along the priority order (from highest to lowest) : 
				// 1 - A filter that has exactly the same format as a buffer which is currently unused, this buffer must as large as possible.
				// 2 - A filter that has the largest format which is not allocated yet.
				// 3 - A filter that has the smallest format already allocated (but not available).
				// 4 - (reserved)
				// 5 - No decision.

				HdlTextureFormat fmt(0,0,GL_RGB,GL_UNSIGNED_BYTE);	// A format (in case we need to create one).
				int fIdx = -1;						// The index of the filter chosen.
				int bIdx = -1;						// The buffer index (if one already exists).
				int currentDecision = 5;				// No decision.

				for(int k=0; k<candidatesIdx.size(); k++)
				{
					// Try to find a match in the buffers : 
					bool noMatch = true;
					for(int l=0; l<buffersList.size(); l++)
					{
						// If this buffer is a match : 
						if( *filtersList[ candidatesIdx[k] ] == *buffersList[l] )
						{
							noMatch = false;

							// If this buffer is free : 
							if(bufferOccupancy[l]==0)
							{
								// If the current choice is lower in priority or larger in buffer size : 
								if( currentDecision>1 || ((currentDecision==1) && fmt.getSize()<buffersList[l]->getSize()) )
								{
									fIdx = candidatesIdx[k];
									bIdx = l;
									currentDecision = 1;
									fmt = *buffersList[l];
								}
							}
							else // The buffer is not free :
							{
								// If no other choices were made : 
								if(currentDecision>3 || ( (currentDecision==3) && (fmt.getSize()<buffersList[l]->getSize()) ) )
								{
									fIdx = candidatesIdx[k];
									bIdx = -1;
									currentDecision = 3;
									fmt = *buffersList[l];
								}
							}
						}
					}

					if(noMatch && currentDecision>=2 && filtersList[ candidatesIdx[k] ]->getSize() > fmt.getSize())
					{
						fIdx = candidatesIdx[k];
						bIdx = -1;
						currentDecision = 2;
						fmt = *filtersList[ candidatesIdx[k] ];
					}
				}

				#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
					std::cout << "    Decision : " << currentDecision << std::endl;
					std::cout << "    Filter   : " << filtersList[fIdx]->getFullName() << std::endl;
					std::cout << "    Buffer   : " << bIdx << std::endl;
				#endif

				if(currentDecision>=5)
					throw Exception("No correct decision was made.", __FILE__, __LINE__);

				// Create the action : 
				if(currentDecision==1)
				{
					tmpActions[ fIdx ].bufferIdx = bIdx;

					// Push : 
					actionsList.push_back( tmpActions[ fIdx ] );
				}
				else if(currentDecision==2 || currentDecision==3)
				{
					// Create a new buffer : 
					buffersList.push_back( new HdlFBO(fmt, filtersList[ fIdx ]->getNumOutputPort() ) );
					bufferOccupancy.push_back(0);

					bIdx = buffersList.size()-1;
					tmpActions[ fIdx ].bufferIdx = bIdx;
					
					// Push : 
					actionsList.push_back( tmpActions[ fIdx ] );
				}
				else
					throw Exception("Internal error : Unkown action decision (" + to_string(currentDecision) + ").", __FILE__, __LINE__);

				// Find all the buffers read by the current actions and decrease their occupancy : 
				for(int k=0; k<actionsList.back().inputBufferIdx.size(); k++)
				{
					if( actionsList.back().inputBufferIdx[k]!=THIS_PIPELINE )
						bufferOccupancy[ actionsList.back().inputBufferIdx[k] ]--;
				}

				// Lock down this filter as "done" : 
				requestedInputConnections[fIdx] = -1;

				// Update the buffer occupancy to the new number of output and at the same time decrease the number of requests : 
				for(int k=0; k<remainingConnections.size(); k++)
				{
					if(filtersGlobalIDsList[remainingConnections[k].idOut]==fIdx)
					{
						if(remainingConnections[k].idIn==THIS_PIPELINE)
						{
							outputsList[ remainingConnections[k].portIn ].bufferIdx 	= bIdx;
							outputsList[ remainingConnections[k].portIn ].outputIdx		= remainingConnections[k].portOut;
							
							bufferOccupancy[bIdx] = std::numeric_limits<int>::max();
						}
						else
						{
							// Set up the links : 
							const int fid = filtersGlobalIDsList[remainingConnections[k].idIn];
					
							tmpActions[fid].inputBufferIdx[ remainingConnections[k].portIn ] 	= bIdx;
							tmpActions[fid].inputArgumentIdx[ remainingConnections[k].portIn ]	= remainingConnections[k].portOut;
							requestedInputConnections[fid]--;

							// Remove : 
							remainingConnections.erase( remainingConnections.begin() + k );
							k--;

							bufferOccupancy[bIdx]++;
						}
					}
				}

				// Test if all were processed : 
				allProcessed 	= true;
				bool noNextStep = true;
				for(int k=0; k<requestedInputConnections.size(); k++)
				{
					if(requestedInputConnections[k]>=0)
						allProcessed = false;
					if(requestedInputConnections[k]==0)
						noNextStep = false;
				}

				if(noNextStep && !allProcessed)
					throw Exception("The pipeline building process is stuck as some elements remains but cannot be integrated as they lack input.", __FILE__, __LINE__);
			}
			while(!allProcessed);

			// Final tests : 
			if(filtersList.size()!=actionsList.size())
				throw Exception("Some filters were omitted because their connections scheme does not allow usage.", __FILE__, __LINE__);

		}
		catch(Exception& e)
		{
			Exception m("Pipeline::allocateBuffers - Error while allocating the buffers in the pipeline " + getFullName() + " : ", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("Pipeline::allocateBuffers - Error (std) while allocating the buffers in the pipeline " + getFullName() + " : ", __FILE__, __LINE__);
			throw m+e;
		}

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "END ALLOCATE" << std::endl;
		#endif

	}

	/**
	\fn int Pipeline::getNumActions(void)
	\brief Get the number of filters applied during processing.
	\return Number of filters applied during processing.
	**/
	int Pipeline::getNumActions(void)
	{
		return actionsList.size();
	}

	/**
	\fn int Pipeline::getSize(void)
	\brief Get the size in bytes of the elements on the GPU for this pipeline.
	\param  askDriver If true, it will use HdlTexture::getSizeOnGPU() to determine the real size (might be slower).
	\return Size in bytes.
	**/
	int Pipeline::getSize(bool askDriver)
	{
		int size = 0;

		#ifdef __GLIPLIB_VERBOSE__
			std::cout << "Pipeline::getSize for " << getFullName() << " (" << buffersList.size() << " buffers)" <<std::endl;
		#endif

		for(int i=0; i<buffersList.size(); i++)
		{
			int fsize = buffersList[i]->getSize(askDriver);

			#ifdef __GLIPLIB_VERBOSE__
				std::cout << "    - Buffer " << i << " : " << fsize/(1024.0*1024.0) << "MB (W:" << buffersList[i]->getWidth() << ", H:" << buffersList[i]->getHeight() << ",T:" << buffersList[i]->getAttachmentCount() << ')' << std::endl;
			#endif
			size += fsize;
		}

		#ifdef __GLIPLIB_VERBOSE__
			std::cout << "Pipeline::getSize END" << std::endl;
		#endif

		return size;
	}

	/**
	\fn void Pipeline::process(void)
	\brief Apply the pipeline.
	**/
	void Pipeline::process(void)
	{
		clock_t 	timing 		= 0,
				totalTiming 	= 0;

		if(!GLEW_VERSION_3_3 && perfsMonitoring)
		{
			totalTiming = clock();
			timing = clock();
		}

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "Pipeline::process - Processing : " << getFullName() << std::endl;
		#endif

		for(int k = 0; k<actionsList.size(); k++)
		{
			ActionHub* 	action 	= &actionsList[k];
			Filter* 	f 	= filtersList[ action->filterIdx ];
			HdlFBO* 	t 	= buffersList[ action->bufferIdx ];

			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "    applying filter : " << f->getFullName() << "..." << std::endl;
			#endif

			for(int l=0; l<f->getNumInputPort(); l++)
			{
				int bufferID 	= action->inputBufferIdx[l];
				int portID 	= action->inputArgumentIdx[l];

				#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
					std::cout << "        conecting buffer " << bufferID << " on port " << portID << std::endl;
				#endif

				if(bufferID==THIS_PIPELINE)
					f->setInputForNextRendering(l, inputsList[portID]);
				else
					f->setInputForNextRendering(l, (*buffersList[bufferID])[portID]);
			}

			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "        Processing using buffer " << action->bufferIdx << "..." << std::endl;
			#endif

			if(perfsMonitoring)
			{
				if(GLEW_VERSION_3_3)
					glBeginQuery(GL_TIME_ELAPSED, queryObject);
				else
					timing = clock();

				//glFlush();
			}

			f->process(*t);

			if(perfsMonitoring)
			{
				//glFlush();

				if(GLEW_VERSION_3_3)
				{
					GLuint64 querytime;
					glEndQuery(GL_TIME_ELAPSED);
					glGetQueryObjectui64v(queryObject, GL_QUERY_RESULT, &querytime);
					perfs[k] = static_cast<double>(querytime)/1e6;
				}
				else
				{
					timing = clock() - timing;
					perfs[k] = static_cast<double>(timing)/static_cast<double>(CLOCKS_PER_SEC)*1000.0f;
				}
			}

			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "        Done." << std::endl;
			#endif
		}

		if(perfsMonitoring)
		{
			if(GLEW_VERSION_3_3)
			{
				totalPerf = 0;
				for(int i=0; i<perfs.size(); i++)
					totalPerf += perfs[i];
			}
			else
			{
				totalTiming = clock() - totalTiming;
				totalPerf   = static_cast<double>(totalTiming)/static_cast<double>(CLOCKS_PER_SEC)*1000.0f;
			}
		}

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "Pipeline::process - Done for pipeline : " << getFullName() << std::endl;
		#endif
	}

	/**
	\fn Pipeline& Pipeline::operator<<(HdlTexture& texture)
	\brief Add a data as input to the pipeline. The user must maintain the texture in memory while this Pipeline hasn't received a Pipeline::Process or Pipeline::Reset signal.
	\param texture The data to use.
	\return This pipeline or raise an exception if any errors occur.
	**/
	Pipeline& Pipeline::operator<<(HdlTexture& texture)
	{
		if(inputsList.size()>=getNumInputPort())
			throw Exception("Pipeline::operator<<(HdlTexture&) - Too much arguments given to Pipeline " + getFullName() + ".", __FILE__, __LINE__);

		inputsList.push_back(&texture);

		return *this;
	}

	/**
	\fn Pipeline& Pipeline::operator<<(Pipeline& pipeline)
	\brief Add all the output of variable pipeline as input of this pipeline. The user must maintain the texture(s) in memory while this Pipeline hasn't received a Pipeline::Process or Pipeline::Reset signal.
	\param pipeline The pipeline outputs to append.
	\return This pipeline or raise an exception if any errors occur.
	**/
	Pipeline& Pipeline::operator<<(Pipeline& pipeline)
	{
		for(int i=0; i<pipeline.getNumOutputPort(); i++)
		{
			if(inputsList.size()>=getNumInputPort())
				throw Exception("Pipeline::operator<<(Pipeline&) - Too much arguments given to Pipeline " + getFullName() + ".", __FILE__, __LINE__);

			inputsList.push_back(&pipeline.out(i));
		}

		return *this;
	}

	/**
	\fn Pipeline& Pipeline::operator<<(ActionType a)
	\brief Apply operation on previously input data.
	\param a The ActionType (Process or Reset arguments).
	\return This pipeline or raise an exception if any errors occur.
	**/
	Pipeline& Pipeline::operator<<(ActionType a)
	{
		// Check the number of arguments given :
		if(inputsList.size()!=getNumInputPort())
			throw Exception("Pipeline::operator<<(ActionType) - Too few arguments given to Pipeline " + getFullName() + ".", __FILE__, __LINE__);

		switch(a)
		{
			case Process:
				process();
			case Reset:            // After Process do Reset of the Input
				cleanInput();
				break;
			default:
				throw Exception("Pipeline::operator<<(ActionType) - Unknown action for Pipeline " + getFullName() + ".", __FILE__, __LINE__);
		}

		return *this;
	}

	/**
	\fn HdlTexture& Pipeline::out(int i)
	\brief Return the output of the pipeline.
	\param i The ID of the output port.
	\return A reference to the corresponding output texture or raise an exception if any errors occur.
	**/
	HdlTexture& Pipeline::out(int i)
	{
		checkOutputPort(i);
		/*int bufferID 		= useBuffer[outputBuffer[i]];
		int bufferPortID 	= outputBufferPort[i];
		return *((*buffers[bufferID])[bufferPortID]);*/
		return (*(*buffersList[ outputsList[i].bufferIdx ])[ outputsList[i].outputIdx ]);
	}

	/**
	\fn HdlTexture& Pipeline::out(const std::string& portName)
	\brief Return the output of the pipeline.
	\param portName The name of the output port.
	\return A reference to the corresponding output texture or raise an exception if any errors occur.
	**/
	HdlTexture& Pipeline::out(const std::string& portName)
	{
		int index = getInputPortID(portName);
		return out(index);
	}

	/**
	\fn Filter& Pipeline::operator[](int filterID)
	\brief Access to the filter of described index.
	\param filterID The index of the filter, obtained with Pipeline::getFilterID.
	\return The index or raise an exception if any errors occur.
	**/
	Filter& Pipeline::operator[](int filterID)
	{
		try
		{
			return *filtersList[ filtersGlobalIDsList[filterID] ];
		}
		catch(Exception& e)
		{
			Exception m("Pipeline::operator[int] - Error while processing request on filter ID : " + to_string(filterID) + ".", __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("Pipeline::operator[int] - Error while processing request on filtr ID : " + to_string(filterID) + ".", __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn void Pipeline::enablePerfsMonitoring(void)
	\brief Enable performances monitoring.

	Any following process on this instance will record time to perform the pipeline (time per element and total time). Calling this function twice will not reset previous results.
	**/
	void Pipeline::enablePerfsMonitoring(void)
	{
		if(!perfsMonitoring)
		{
			perfsMonitoring = true;
			perfs.assign(filtersList.size(),0.0f);
			totalPerf = 0.0f;

			if(GLEW_VERSION_3_3)
			{
				if(queryObject==0)
					glGenQueries(1, &queryObject);
			}
		}
	}

	/**
	\fn void Pipeline::disablePerfsMonitoring(void)
	\brief Disable performances monitoring.

	Stops a monitoring session on this instance. Do nothing if no session was started.
	**/
	void Pipeline::disablePerfsMonitoring(void)
	{
		if(perfsMonitoring)
		{
			perfsMonitoring = false;
			perfs.clear();
			totalPerf = 0.0;
		}
	}

	/**
	\fn double Pipeline::getTiming(int id)
	\brief Get last result of performance monitoring IF it is still enabled.
	\param filterID The ID of the filter.
	\return Time in milliseconds needed to apply the filter (not counting binding operation).
	**/
	double Pipeline::getTiming(int filterID)
	{
		if(perfsMonitoring)
			return perfs[ filtersGlobalIDsList[filterID] ];
		else
			throw Exception("Pipeline::getTiming - Monitoring is disabled.", __FILE__, __LINE__);
	}

	/**
	\fn double Pipeline::getTiming(int action, std::string& filterName)
	\brief Get last result of performance monitoring IF it is still enabled.
	\param action The ID of the filter.
	\param filterName A reference string that will contain the name of the filter indexed by action at the end of the function.
	\return Time in milliseconds needed to apply the filter (not counting binding operation).
	**/
	double Pipeline::getTiming(int action, std::string& filterName)
	{
		if(perfsMonitoring)
		{
			if(action<0 || action>=actionsList.size())
				throw Exception("Pipeline::getTiming - Action index is outside of range.", __FILE__, __LINE__);
			else
			{
				filterName = filtersList[ actionsList[action].filterIdx ]->getFullName();
				return perfs[action];
			}
		}
		else
			throw Exception("Pipeline::getTiming - Monitoring is disabled.", __FILE__, __LINE__);
	}


	/**
	\fn double Pipeline::getTotalTiming(void)
	\brief Get total time ellapsed for last run.
	\return Time in milliseconds needed to apply the whole pipeline (counting everything and flushing after each filter).
	**/
	double Pipeline::getTotalTiming(void)
	{
		if(perfsMonitoring)
			return totalPerf;
		else
			throw Exception("Pipeline::getTotalTiming - Monitoring is disabled.", __FILE__, __LINE__);
	}

