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
#include "Core/Exception.hpp"
#include "Core/Component.hpp"
#include "devDebugTools.hpp"

	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

//AbstractComponentLayout
	/**
	\fn AbstractComponentLayout::AbstractComponentLayout(const std::string& type)
	\brief AbstractComponentLayout constructor.
	\param type The typename of the object.
	**/
	AbstractComponentLayout::AbstractComponentLayout(const std::string& _typeName)
	 : 	typeName(_typeName)
	{ }

	/**
	\fn AbstractComponentLayout::AbstractComponentLayout(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts)
	\brief AbstractComponentLayout constructor.
	\param _typeName The typename of the object.
	\param _inputPorts The list of the names of the input ports.
	\param _outputPorts The list of the names of the input ports.
	**/
	AbstractComponentLayout::AbstractComponentLayout(const std::string& _typeName, const std::vector<std::string>& _inputPorts, const std::vector<std::string>& _outputPorts)
	 : 	typeName(_typeName), 
		inputPorts(_inputPorts),
		outputPorts(_outputPorts)
	{ }

	/**
	\fn AbstractComponentLayout::AbstractComponentLayout(const AbstractComponentLayout& c)
	\brief AbstractComponentLayout constructor.
	\param c Copy.
	**/
	AbstractComponentLayout::AbstractComponentLayout(const AbstractComponentLayout& c)
	 : 	typeName(c.typeName),
		inputPorts(c.inputPorts),
		outputPorts(c.outputPorts)
	{
		#ifdef __DEVELOPMENT_VERBOSE__
			std::cout << "Copying AbstractComponentLayout " << getName() << std::endl;
		#endif
	}

	AbstractComponentLayout::~AbstractComponentLayout(void)
	{ }

	/**
	\fn std::string AbstractComponentLayout::getFullName(void) const
	\brief Returns the full name of this component (might be overloaded).
	\return The full name of this component (possible name and typename).
	**/
	std::string AbstractComponentLayout::getFullName(void) const
	{
		return "<ComponentLayout> (type : \"" + getTypeName() + "\")";
	}

	/**
	\fn const std::string& AbstractComponentLayout::getTypeName(void) const
	\brief Returns the typename of this component.
	\return The typename of this component.
	**/
	const std::string& AbstractComponentLayout::getTypeName(void) const
	{
		return typeName;
	}

	/**
	\fn void AbstractComponentLayout::checkInputPort(int i) const
	\brief Check the validity of an input port. Raise an exception if any errors occur.
	\param i Index of the input port to test.
	**/
	void AbstractComponentLayout::checkInputPort(int i) const
	{
		if(i<0 || i>=getNumInputPort())
			throw Exception("ComponentLayout - Bad input port ID for "  + getFullName() + " (ID = " + toString(i) + ").", __FILE__, __LINE__, Exception::CoreException);
	}

	/**
	\fn void AbstractComponentLayout::checkOutputPort(int i) const
	\brief Check the validity of an output port. Raise an exception if any errors occur.
	\param i Index of the output port to test.
	**/
	void AbstractComponentLayout::checkOutputPort(int i) const
	{
		if(i<0 || i>=getNumOutputPort())
			throw Exception("ComponentLayout - Bad output port ID for " + getFullName() + " (ID = " + toString(i) + ").", __FILE__, __LINE__, Exception::CoreException);
	}

	/**
	\fn int AbstractComponentLayout::getNumInputPort(void) const
	\brief Get the number of input ports.
	\return Number of input ports.
	**/
	int AbstractComponentLayout::getNumInputPort(void) const
	{
		return inputPorts.size();
	}

	/**
	\fn const std::string& AbstractComponentLayout::getInputPortName(int id) const
	\brief Return the name of an input port. Raise an exception if any errors occur.
	\param id The index of the input port.
	\return The name of the input port as a standard string.
	**/
	const std::string& AbstractComponentLayout::getInputPortName(int id) const
	{
		checkInputPort(id);
		return inputPorts[id];
	}

	/**
	\fn int AbstractComponentLayout::getInputPortID(const std::string& name) const
	\brief Get the index of an input port knowing its name. Raise an exception if any errors occur.
	\param name The name of the input port.
	\return The index of the input port.
	**/
	int AbstractComponentLayout::getInputPortID(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumInputPort()==1)
				return 0;
			else
				throw Exception("AbstractComponentLayout::getInputPortID - Unable to use wildcard '*' in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);
		}

		std::vector<std::string>::const_iterator it = std::find(inputPorts.begin(), inputPorts.end(), name);

		if(it==inputPorts.end())
			throw Exception("AbstractComponentLayout::getInputPortID - Unable to find input port \"" + name + "\" in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);

		return std::distance(inputPorts.begin(), it);
	}

	/**
	\fn bool AbstractComponentLayout::doesInputPortExist(const std::string& name) const
	\brief Check if an input port exists, knowing its name
	\param name The name of the input port.
	\return True if an input port named correctly exists, False otherwise.
	**/
	bool AbstractComponentLayout::doesInputPortExist(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumInputPort()==1)
				return true;
			else
				throw Exception("AbstractComponentLayout::doesInputPortExist( - Unable to use wildcard '*' in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);
		}

		std::vector<std::string>::const_iterator it = std::find(inputPorts.begin(), inputPorts.end(), name);

		return it!=inputPorts.end();
	}

	/**
	\fn int AbstractComponentLayout::getNumOutputPort(void) const
	\brief Get the number of output ports.
	\return Number of output ports.
	**/
	int AbstractComponentLayout::getNumOutputPort(void) const
	{
		return outputPorts.size();
	}

	/**
	\fn const std::string& AbstractComponentLayout::getOutputPortName(int id) const
	\brief Return the name of an output port. Raise an exception if any errors occur.
	\param id The index of the output port.
	\return The name of the output port as a standard string.
	**/
	const std::string& AbstractComponentLayout::getOutputPortName(int id) const
	{
		checkOutputPort(id);
		return outputPorts[id];
	}

	/**
	\fn int AbstractComponentLayout::getOutputPortID(const std::string& name) const
	\brief Get the index of an output port knowing its name. Raise an exception if any errors occur.
	\param name The name of the output port.
	\return The index of the output port.
	**/
	int AbstractComponentLayout::getOutputPortID(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumOutputPort()==1)
				return 0;
			else
				throw Exception("AbstractComponentLayout::getOutputPortID - Unable to use wildcard '*' in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);
		}

		std::vector<std::string>::const_iterator it = std::find(outputPorts.begin(), outputPorts.end(), name);

		if(it==outputPorts.end())
			throw Exception("AbstractComponentLayout::getOutputPortID - Unable to find output port \"" + name + "\" in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);

		return std::distance(outputPorts.begin(), it);
	}

	/**
	\fn bool AbstractComponentLayout::doesOutputPortExist(const std::string& name) const
	\brief Check if an output port exists, knowing its name
	\param name The name of the output port.
	\return True if an output port named correctly exists, False otherwise.
	**/
	bool AbstractComponentLayout::doesOutputPortExist(const std::string& name) const
	{
		if(name=="*")
		{
			if(getNumOutputPort()==1)
				return true;
			else
				throw Exception("AbstractComponentLayout::doesOutputPortExist - Unable to use wildcard '*' in component " + getFullName() + ".", __FILE__, __LINE__, Exception::CoreException);
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
	 : 	AbstractComponentLayout(_typeName)
	{ }

	/**
	\fn ComponentLayout::ComponentLayout(const AbstractComponentLayout& c)
	\brief ComponentLayout constructor.
	\param c Copy.
	**/
	ComponentLayout::ComponentLayout(const AbstractComponentLayout& c)
	 : 	AbstractComponentLayout(c)
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
			throw Exception("ComponentLayout::addInputPort - Input port \"" + name + "\" already exists.", __FILE__, __LINE__, Exception::CoreException);
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
			throw Exception("ComponentLayout::addOutputPort - Output port \"" + name + "\" already exists.", __FILE__, __LINE__, Exception::CoreException);
		else
		{
			int id = outputPorts.size();
			outputPorts.push_back(name);
			return id;
		}
	}

	/**
	\fn void ComponentLayout::setInputPortName(int i, const std::string& newName)
	\brief Change the name of an input port.
	\param i The ID of the target input port.
	\param newName The name of the new input port.
	**/
	void ComponentLayout::setInputPortName(int i, const std::string& newName)
	{
		checkInputPort(i);

		inputPorts[i] = newName;
	}

	/**
	\fn void ComponentLayout::setOutputPortName(int i, const std::string& newName)
	\brief Change the name of an output port.
	\param i The ID of the target output port.
	\param newName The name of the new output port.
	**/
	void ComponentLayout::setOutputPortName(int i, const std::string& newName)
	{
		checkOutputPort(i);

		outputPorts[i] = newName;
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
	 : 	AbstractComponentLayout(_typeName, _inputPorts, _outputPorts), 
		instanceName(_instanceName)
	{ }

	/**
	\fn Component::Component(const AbstractComponentLayout& c, const std::string& _instanceName)
	\brief Component constructor.
	\param c The component layout.
	\param _instanceName The name of the component.
	**/
	Component::Component(const AbstractComponentLayout& c, const std::string& _instanceName)
	 : 	AbstractComponentLayout(c), 
		instanceName(_instanceName)
	{ }

	Component::~Component(void)
	{ }

	/**
	\fn std::string AbstractComponentLayout::getFullName(void) const
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
