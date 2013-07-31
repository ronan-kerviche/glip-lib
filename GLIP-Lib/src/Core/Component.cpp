/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Component.cpp                                                                             */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Components of pipelines                                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Component.cpp
 * \brief   Components of pipelines.
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include <algorithm>
#include "Exception.hpp"
#include "Component.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

//__ReadOnly_ComponentLayout
	/**
	\fn __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& type)
	\brief __ReadOnly_ComponentLayout constructor.
	\param type The typename of the object.
	**/
	__ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& _typeName)
	 : typeName(_typeName)
	{ }

	/**
	\fn __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts)
	\brief __ReadOnly_ComponentLayout constructor.
	\param _typeName The typename of the object.
	\param _inputPorts The list of the names of the input ports.
	\param _outputPorts The list of the names of the input ports.
	**/
	__ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts)
	 : typeName(_typeName), inputPorts(_inputPorts), outputPorts(_outputPorts)
	{ }

	/**
	\fn __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout& c)
	\brief __ReadOnly_ComponentLayout constructor.
	\param c Copy.
	**/
	__ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout& c)
	{
		#ifdef __DEVELOPMENT_VERBOSE__
			std::cout << "Copying ReadOnly Component Layout " << getName() << std::endl;
		#endif

		typeName	= c.typeName;
		inputPorts	= c.inputPorts;
		outputPorts	= c.outputPorts;

		#ifdef __DEVELOPMENT_VERBOSE__
			std::cout << "Size : " << inputPorts.size() << std::endl;
		#endif
	}

	__ReadOnly_ComponentLayout::~__ReadOnly_ComponentLayout(void)
	{ }

	/**
	\fn std::string __ReadOnly_ComponentLayout::getFullName(void) const
	\brief Returns the full name of this component (might be overloaded).
	\return The full name of this component (possible name and typename).
	**/
	std::string __ReadOnly_ComponentLayout::getFullName(void) const
	{
		return "<ComponentLayout> (type : \"" + getTypeName() + "\")";
	}

	/**
	\fn const std::string& __ReadOnly_ComponentLayout::getTypeName(void) const
	\brief Returns the typename of this component.
	\return The typename of this component.
	**/
	const std::string& __ReadOnly_ComponentLayout::getTypeName(void) const
	{
		return typeName;
	}

	/**
	\fn void __ReadOnly_ComponentLayout::checkInputPort(int i) const
	\brief Check the validity of an input port. Raise an exception if any errors occur.
	\param i Index of the input port to test.
	**/
	void __ReadOnly_ComponentLayout::checkInputPort(int i) const
	{
		if(i<0 || i>=getNumInputPort())
			throw Exception("ComponentLayout - Bad input port ID for "  + getFullName() + " ID : " + to_string(i), __FILE__, __LINE__);
	}

	/**
	\fn void __ReadOnly_ComponentLayout::checkOutputPort(int i) const
	\brief Check the validity of an output port. Raise an exception if any errors occur.
	\param i Index of the output port to test.
	**/
	void __ReadOnly_ComponentLayout::checkOutputPort(int i) const
	{
		if(i<0 || i>=getNumOutputPort())
			throw Exception("ComponentLayout - Bad output port ID for " + getFullName() + " ID : " + to_string(i), __FILE__, __LINE__);
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getNumInputPort(void) const
	\brief Get the number of input ports.
	\return Number of input ports.
	**/
	int __ReadOnly_ComponentLayout::getNumInputPort(void) const
	{
		return inputPorts.size();
	}

	/**
	\fn const std::string& __ReadOnly_ComponentLayout::getInputPortName(int id) const
	\brief Return the name of an input port. Raise an exception if any errors occur.
	\param id The index of the input port.
	\return The name of the input port as a standard string.
	**/
	const std::string& __ReadOnly_ComponentLayout::getInputPortName(int id) const
	{
		checkInputPort(id);
		return inputPorts[id];
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getInputPortID(const std::string& name) const
	\brief Get the index of an input port knowing its name. Raise an exception if any errors occur.
	\param name The name of the input port.
	\return The index of the input port.
	**/
	int __ReadOnly_ComponentLayout::getInputPortID(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumInputPort()==1)
				return 0;
			else
				throw Exception("__ReadOnly_ComponentLayout::getInputPortID - Unable to use wildcard '*' in component \"" + getFullName() + "\".", __FILE__, __LINE__);
		}

		std::vector<std::string>::const_iterator it = std::find(inputPorts.begin(), inputPorts.end(), name);

		if(it==inputPorts.end())
			throw Exception("__ReadOnly_ComponentLayout::getInputPortID - Unable to find input port \"" + name + "\" in component \"" + getFullName() + "\".", __FILE__, __LINE__);

		return std::distance(inputPorts.begin(), it);
	}

	/**
	\fn bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	\brief Check if an input port exists, knowing its name
	\param name The name of the input port.
	\return True if an input port named correctly exists, False otherwise.
	**/
	bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumInputPort()==1)
				return true;
			else
				throw Exception("__ReadOnly_ComponentLayout::doesInputPortExist( - Unable to use wildcard '*' in component \"" + getFullName() + "\".", __FILE__, __LINE__);
		}

		std::vector<std::string>::const_iterator it = std::find(inputPorts.begin(), inputPorts.end(), name);

		return it!=inputPorts.end();
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getNumOutputPort(void) const
	\brief Get the number of output ports.
	\return Number of output ports.
	**/
	int __ReadOnly_ComponentLayout::getNumOutputPort(void) const
	{
		return outputPorts.size();
	}

	/**
	\fn const std::string& __ReadOnly_ComponentLayout::getOutputPortName(int id) const
	\brief Return the name of an output port. Raise an exception if any errors occur.
	\param id The index of the output port.
	\return The name of the output port as a standard string.
	**/
	const std::string& __ReadOnly_ComponentLayout::getOutputPortName(int id) const
	{
		checkOutputPort(id);
		return outputPorts[id];
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getOutputPortID(const std::string& name) const
	\brief Get the index of an output port knowing its name. Raise an exception if any errors occur.
	\param name The name of the output port.
	\return The index of the output port.
	**/
	int __ReadOnly_ComponentLayout::getOutputPortID(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumOutputPort()==1)
				return 0;
			else
				throw Exception("__ReadOnly_ComponentLayout::getOutputPortID - Unable to use wildcard '*' in component \"" + getFullName() + "\".", __FILE__, __LINE__);
		}

		std::vector<std::string>::const_iterator it = std::find(outputPorts.begin(), outputPorts.end(), name);

		if(it==outputPorts.end())
			throw Exception("__ReadOnly_ComponentLayout::getOutputPortID - Unable to find output port \"" + name + "\" in component \"" + getFullName() + "\".", __FILE__, __LINE__);

		return std::distance(outputPorts.begin(), it);
	}

	/**
	\fn bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	\brief Check if an input port exists, knowing its name
	\param name The name of the input port.
	\return True if an input port named correctly exists, False otherwise.
	**/
	bool __ReadOnly_ComponentLayout::doesOutputPortExist(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumOutputPort()==1)
				return true;
			else
				throw Exception("__ReadOnly_ComponentLayout::doesOutputPortExist - Unable to use wildcard '*' in component \"" + getFullName() + "\".", __FILE__, __LINE__);
		}

		std::vector<std::string>::const_iterator it = std::find(outputPorts.begin(), outputPorts.end(), name);

		return it!=outputPorts.end();
	}

// ComponentLayout
	/**
	\fn ComponentLayout::ComponentLayout(const std::string& _typeName)
	\brief ComponentLayout constructor.
	\param _typeName Typename of the component.
	**/
	ComponentLayout::ComponentLayout(const std::string& _typeName)
	 : __ReadOnly_ComponentLayout(_typeName)
	{ }

	/**
	\fn ComponentLayout::ComponentLayout(const __ReadOnly_ComponentLayout& c)
	\brief ComponentLayout constructor.
	\param c Copy.
	**/
	ComponentLayout::ComponentLayout(const __ReadOnly_ComponentLayout& c)
	 : __ReadOnly_ComponentLayout(c)
	{ }

	ComponentLayout::~ComponentLayout(void)
	{ }

	/**
	\fn int ComponentLayout::addInputPort(const std::string& name)
	\brief Add an input port.
	\param name The name of the new input port.
	\return The ID of the new input port.
	**/
	int ComponentLayout::addInputPort(const std::string& name)
	{
		if(doesInputPortExist(name))
			throw Exception("ComponentLayout::addInputPort - Input port " + name + " already exists.", __FILE__, __LINE__);
		else
		{
			int id = inputPorts.size();
			inputPorts.push_back(name);
			return id;
		}
	}

	/**
	\fn int ComponentLayout::addOutputPort(const std::string& name)
	\brief Add an output port.
	\param name The name of the new output port.
	\return The ID of the new output port.
	**/
	int ComponentLayout::addOutputPort(const std::string& name)
	{
		if(doesOutputPortExist(name))
			throw Exception("ComponentLayout::addOutputPort - Output port " + name + " already exists.", __FILE__, __LINE__);
		else
		{
			int id = outputPorts.size();
			outputPorts.push_back(name);
			return id;
		}
	}

// Component :
	/**
	\fn Component::Component(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts, const std::string& _instanceName)
	\brief Component constructor.
	\param _typeName The typename of the object.
	\param _inputPorts The list of the names of the input ports.
	\param _outputPorts The list of the names of the input ports.
	\param _instanceName The name of the component.
	**/
	Component::Component(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts, const std::string& _instanceName)
	 : __ReadOnly_ComponentLayout(_typeName, _inputPorts, _outputPorts), instanceName(_instanceName)
	{ }

	/**
	\fn Component::Component(const __ReadOnly_ComponentLayout& c, const std::string& _instanceName)
	\brief Component constructor.
	\param c The component layout.
	\param _instanceName The name of the component.
	**/
	Component::Component(const __ReadOnly_ComponentLayout& c, const std::string& _instanceName)
	 : __ReadOnly_ComponentLayout(c), instanceName(_instanceName)
	{ }

	Component::~Component(void)
	{ }

	/**
	\fn std::string __ReadOnly_ComponentLayout::getFullName(void) const
	\brief Returns the full name of this component (might be overloaded).
	\return The full name of this component (name and typename).
	**/
	std::string Component::getFullName(void) const
	{
		return "\"" + getName() + "\" (type : \"" + getTypeName() + "\")";
	}

	/**
	\fn const std::string& Component::getName(void) const
	\brief Returns the name of this component.
	\return The name of this component.
	**/
	const std::string& Component::getName(void) const
	{
		return instanceName;
	}
