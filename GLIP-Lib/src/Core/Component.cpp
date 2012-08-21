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
	__ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& type)
	 : ObjectName("(Template)", type)
	{ }

	/**
	\fn __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout& c)
	\brief __ReadOnly_ComponentLayout constructor.
	\param c Copy.
	**/
	__ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout& c)
	 : ObjectName(c)
	{
		#ifdef __DEVELOPMENT_VERBOSE__
			std::cout << "Copying ReadOnly Component Layout " << getName() << std::endl;
		#endif

		inputPortDescription  = c.inputPortDescription;
		outputPortDescription = c.outputPortDescription;

		#ifdef __DEVELOPMENT_VERBOSE__
			std::cout << "Size : " << inputPortDescription.size() << std::endl;
		#endif
	}

	/**
	\fn void __ReadOnly_ComponentLayout::checkInputPort(int i) const
	\brief Check the validity of an input port. Raise an exception if any errors occur.
	\param i Index of the input port to test.
	**/
	void __ReadOnly_ComponentLayout::checkInputPort(int i) const
	{
		if((i)<0 || (i)>=getNumInputPort())
			throw Exception("ComponentLayout - Bad input port ID for "  + getNameExtended() + " ID : " + to_string(i), __FILE__, __LINE__);
	}

	/**
	\fn void __ReadOnly_ComponentLayout::checkOutputPort(int i) const
	\brief Check the validity of an output port. Raise an exception if any errors occur.
	\param i Index of the output port to test.
	**/
	void __ReadOnly_ComponentLayout::checkOutputPort(int i) const
	{
		if((i)<0 || (i)>=getNumOutputPort())
			throw Exception("ComponentLayout - Bad output port ID for " + getNameExtended() + " ID : " + to_string(i), __FILE__, __LINE__);
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getNumInputPort(void) const
	\brief Get the number of input ports.
	\return Number of input ports.
	**/
	int __ReadOnly_ComponentLayout::getNumInputPort(void) const
	{
		return inputPortDescription.size();
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
		return inputPortDescription[id].getName();
	}

	/**
	\fn std::string __ReadOnly_ComponentLayout::getInputPortNameExtended(int id) const
	\brief Return the name of an input port. Raise an exception if any errors occur.
	\param id The index of the input port.
	\return The name of the input port as a standard string.
	**/
	std::string __ReadOnly_ComponentLayout::getInputPortNameExtended(int id) const
	{
		checkInputPort(id);
		return inputPortDescription[id].getNameExtended();
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getInputPortID(const std::string& name) const
	\brief Get the index of an input port knowing its name. Raise an exception if any errors occur.
	\param name The name of the input port.
	\return The index of the input port.
	**/
	int __ReadOnly_ComponentLayout::getInputPortID(const std::string& name) const
	{
		try
		{
			return getIndexByName(name, inputPortDescription);
		}
		catch(Exception& e)
		{
			Exception m("__ReadOnly_ComponentLayout::getInputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("__ReadOnly_ComponentLayout::getInputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	\brief Check if an input port exists, knowing its name
	\param name The name of the input port.
	\return True if an input port named correctly exists, False otherwise.
	**/
	bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	{
		return doesInstanceExistByName(name, inputPortDescription);
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getNumOutputPort(void) const
	\brief Get the number of output ports.
	\return Number of output ports.
	**/
	int __ReadOnly_ComponentLayout::getNumOutputPort(void) const
	{
		return outputPortDescription.size();
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
		return outputPortDescription[id].getName();
	}

	/**
	\fn std::string __ReadOnly_ComponentLayout::getOutputPortNameExtended(int id) const
	\brief Return the name of an output port. Raise an exception if any errors occur.
	\param id The index of the output port.
	\return The name of the output port as a standard string.
	**/
	std::string __ReadOnly_ComponentLayout::getOutputPortNameExtended(int id) const
	{
		checkOutputPort(id);
		return outputPortDescription[id].getNameExtended();
	}

	/**
	\fn int __ReadOnly_ComponentLayout::getOutputPortID(const std::string& name) const
	\brief Get the index of an output port knowing its name. Raise an exception if any errors occur.
	\param name The name of the output port.
	\return The index of the output port.
	**/
	int __ReadOnly_ComponentLayout::getOutputPortID(const std::string& name) const
	{
		try
		{
			return getIndexByName(name, outputPortDescription);
		}
		catch(Exception& e)
		{
			Exception m("__ReadOnly_ComponentLayout::getOutputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("__ReadOnly_ComponentLayout::getOutputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
	}

	/**
	\fn bool __ReadOnly_ComponentLayout::doesInputPortExist(const std::string& name) const
	\brief Check if an input port exists, knowing its name
	\param name The name of the input port.
	\return True if an input port named correctly exists, False otherwise.
	**/
	bool __ReadOnly_ComponentLayout::doesOutputPortExist(const std::string& name) const
	{
		return doesInstanceExistByName(name, outputPortDescription);
	}

// ComponentLayout
	/**
	\fn ComponentLayout::ComponentLayout(const std::string& type)
	\brief ComponentLayout constructor.
	\param type Typename of the component.
	**/
	ComponentLayout::ComponentLayout(const std::string& type)
	 : __ReadOnly_ComponentLayout(type)
	{ }

	/**
	\fn ComponentLayout::ComponentLayout(const __ReadOnly_ComponentLayout& c)
	\brief ComponentLayout constructor.
	\param c Copy.
	**/
	ComponentLayout::ComponentLayout(const __ReadOnly_ComponentLayout& c)
	 : __ReadOnly_ComponentLayout(c)
	{ }

	/**
	\fn void ComponentLayout::setInputPortName(int id, const std::string& name)
	\brief Sets the name of an input port. Raise an exception if any errors occur.
	\param id The index of the input port.
	\param name The new name of the port.
	**/
	void ComponentLayout::setInputPortName(int id, const std::string& name)
	{
		checkInputPort(id);
		inputPortDescription[id].setName(name);
	}

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
			int id = inputPortDescription.size();
			inputPortDescription.push_back(ObjectName(name, id));
			return id;
		}
	}

	/**
	\fn void ComponentLayout::setOutputPortName(int id, const std::string& name)
	\brief Sets the name of an output port. Raise an exception if any errors occur.
	\param id The index of the output port.
	\param name The new name of the port.
	**/
	void ComponentLayout::setOutputPortName(int id, const std::string& name)
	{
		checkOutputPort(id);
		outputPortDescription[id].setName(name);
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
			int id = outputPortDescription.size();
			outputPortDescription.push_back(ObjectName(name, id));
			return id;
		}
	}

// Component :
	/**
	\fn Component::Component(const __ReadOnly_ComponentLayout& c, const std::string& name)
	\brief Component constructor.
	\param c The component layout.
	\param name The name of the component.
	**/
	Component::Component(const __ReadOnly_ComponentLayout& c, const std::string& name)
	 : __ReadOnly_ComponentLayout(c)
	{
		setName(name);
	}

