/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Devices.cpp                                                                               */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Input and Output devices objects                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Devices.cpp
 * \brief   Input and Output devices objects
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include <iostream>
#include "Core/Devices.hpp"
#include "Core/Exception.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// InputDeviceLayout
	/**
	\fn InputDevice::InputDeviceLayout::InputDeviceLayout(const std::string& _typeName)
	\brief InputDeviceLayout constructor.
	\param _typeName The type name of the object.
	**/
	InputDevice::InputDeviceLayout::InputDeviceLayout(const std::string& _typeName)
	 : AbstractComponentLayout(_typeName), ComponentLayout(_typeName)
	{ }

	/**
	\fn InputDevice::InputDeviceLayout::InputDeviceLayout(const InputDeviceLayout& c)
	\brief InputDeviceLayout copy constructor.
	\param c The layout to copy.
	**/
	InputDevice::InputDeviceLayout::InputDeviceLayout(const InputDeviceLayout& c)
	 : AbstractComponentLayout(c), ComponentLayout(c)
	{ }

	/**
	\fn int InputDevice::InputDeviceLayout::addOutputPort(const std::string& name)
	\brief Add an output port to the layout.
	\param name The name of the new output port.
	\return The ID of the newly created port.
	**/
	int InputDevice::InputDeviceLayout::addOutputPort(const std::string& name)
	{
		return ComponentLayout::addOutputPort(name);
	}

// InputDevice
	/**
	\fn InputDevice::InputDevice(const InputDeviceLayout& layout, const std::string& name)
	\brief InputDevice constructor.
	\param layout The component layout.
	\param name The name of the component.
	**/
	InputDevice::InputDevice(const InputDeviceLayout& layout, const std::string& name)
	 : AbstractComponentLayout(layout), Component(layout, name)
	{
		texturesLinks.assign(getNumOutputPort(),NULL);
		newImages.assign(getNumOutputPort(),false);
		imagesMissedCount.assign(getNumOutputPort(), 0);
	}

	InputDevice::~InputDevice(void)
	{
		texturesLinks.clear();
		newImages.clear();
		imagesMissedCount.clear();
	}

	/**
	\fn void InputDevice::setTextureLink(HdlTexture* tex, int port)
	\brief Give a different texture as target. The use ensure the memory footprint of the object, no deletion is made by this object. The pointer must remain valid until replaced. A call is made to the corresponding InputDevice::declareNewImage.
	\param tex Pointer to the texture or raise an exception if any error occurs.
	\param port The index of the port. Default is first port.
	**/
	void InputDevice::setTextureLink(HdlTexture* tex, int port)
	{
		checkOutputPort(port);

		texturesLinks[port] = tex;
		declareNewImage(port);
	}

	/**
	\fn void InputDevice::setTextureLink(HdlTexture* tex, const std::string& port)
	\brief Give a different texture as target. The use ensure the memory footprint of the object, no deletion is made by this object. The pointer must remain valid until replaced. A call is made to the corresponding InputDevice::declareNewImage.
	\param tex Pointer to the texture or raise an exception if any error occurs.
	\param port The name of the port. Default is the first port.
	**/
	void InputDevice::setTextureLink(HdlTexture* tex, const std::string& port)
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		setTextureLink(tex, id);
	}

	/**
	\fn void InputDevice::declareNewImage(int port)
	\brief Force declaration of a new image.
	\param port The index of the port. Default is first port.
	**/
	void InputDevice::declareNewImage(int port)
	{
		checkOutputPort(port);

		if(newImages[port])
			imagesMissedCount[port]++;

		newImages[port] = true;
	}

	/**
	\fn void InputDevice::declareNewImage(const std::string& port)
	\brief Force declaration of a new image.
	\param port The name of the port. Default is the first port.
	**/
	void InputDevice::declareNewImage(const std::string& port)
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		declareNewImage(id);
	}

	/**
	\fn void InputDevice::declareNewImageOnAllPorts(void)
	\brief Force declaration of a new image on all ports.
	**/
	void InputDevice::declareNewImageOnAllPorts(void)
	{
		for(int i=0; i<getNumOutputPort(); i++)
			declareNewImage(i);
	}

	/**
	\fn bool InputDevice::isNewImage(int port) const
	\brief Check if there is a new image.
	\param port The index of the port. Default is first port.
	\return true if there is a new image.
	**/
	bool InputDevice::isNewImage(int port) const
	{
		checkOutputPort(port);

		return newImages[port];
	}

	/**
	\fn bool InputDevice::isNewImage(const std::string& port) const
	\brief Check if there is a new image.
	\param port The name of the port. Default is the first port.
	\return true if there is a new image.
	**/
	bool InputDevice::isNewImage(const std::string& port) const
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		return isNewImage(id);
	}

	/**
	\fn int	InputDevice::getMissedImagesCount(int port) const
	\brief Get the number of images missed (out() wasn't call for a new image, for this port).
	\param port The index of the port. Default is first port.
	\return The number of images missed.
	**/
	int InputDevice::getMissedImagesCount(int port) const
	{
		checkOutputPort(port);

		return imagesMissedCount[port];
	}

	/**
	\fn int	InputDevice::getMissedImagesCount(const std::string& port) const
	\brief Get the number of images missed (out() wasn't call for a new image, for this port).
	\param port The name of the port. Default is the first port.
	\return The number of images missed.
	**/
	int InputDevice::getMissedImagesCount(const std::string& port) const
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		return getMissedImagesCount(id);
	}

	/**
	\fn int InputDevice::getMissedImagesTotalCount(void) const
	\brief The total number of missed images.
	\return The total (across all output ports) number of images missed.
	**/
	int InputDevice::getMissedImagesTotalCount(void) const
	{
		int res = 0;

		for(std::vector<int>::const_iterator it=imagesMissedCount.begin(); it<imagesMissedCount.end(); it++)
			res += *it;

		return res;
	}

	/**
	\fn bool InputDevice::portHasValidOutput(int port) const
	\brief Check if a port has a texture assigned.
	\param port The index of the port. Default is first port.
	\return True if it has a texture assigned and InputDevice::out can be use on it.
	**/
	bool InputDevice::portHasValidOutput(int port) const
	{
		checkOutputPort(port);

		return texturesLinks[port]!=NULL;
	}

	/**
	\fn bool InputDevice::portHasValidOutput(const std::string& port) const
	\brief Check if a port has a texture assigned.
	\param port The name of the port. Default is the first port.
	\return True if it has a texture assigned and InputDevice::out can be use on it.
	**/
	bool InputDevice::portHasValidOutput(const std::string& port) const
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		return portHasValidOutput(id);
	}

	/**
	\fn HdlTexture& InputDevice::out(int port)
	\brief Get the current image for the port.
	\param port The index of the port. Default is first port.
	\return A reference to the current image or raise an exception if any errors occur.
	**/
	HdlTexture& InputDevice::out(int port)
	{
		if(!portHasValidOutput(port))
			throw Exception("InputDevice::out - Port " + getOutputPortName(port) + " has no valid output.", __FILE__, __LINE__);
		else
		{
			newImages[port] = false;
			return *texturesLinks[port];
		}
	}

	/**
	\fn HdlTexture& InputDevice::out(const std::string& port)
	\brief Get the current image for the port.
	\param port The name of the port. Default is the first port.
	\return A reference to the current image or raise an exception if any errors occur.
	**/
	HdlTexture& InputDevice::out(const std::string& port)
	{
		int id = 0;

		if(!port.empty())
			id = getOutputPortID(port);

		return out(id);
	}

// OutputDeviceLayout
	/**
	\fn OutputDevice::OutputDeviceLayout::OutputDeviceLayout(const std::string& _typeName)
	\brief OutputDeviceLayout constructor.
	\param _typeName The type name of the object.
	**/
	OutputDevice::OutputDeviceLayout::OutputDeviceLayout(const std::string& _typeName)
	 : AbstractComponentLayout(_typeName), ComponentLayout(_typeName)
	{ }

	/**
	\fn OutputDevice::OutputDeviceLayout::OutputDeviceLayout(const OutputDeviceLayout& c)
	\brief OutputDeviceLayout copy constructor.
	\param c The layout to copy.
	**/
	OutputDevice::OutputDeviceLayout::OutputDeviceLayout(const OutputDeviceLayout& c)
	 : AbstractComponentLayout(c), ComponentLayout(c)
	{ }

	/**
	\fn int OutputDevice::OutputDeviceLayout::addInputPort(const std::string& name)
	\brief Add an input port to the layout.
	\param name The name of the new output port.
	\return The ID of the newly created port.
	**/
	int OutputDevice::OutputDeviceLayout::addInputPort(const std::string& name)
	{
		return ComponentLayout::addInputPort(name);
	}

// OutputDevice
	/**
	\fn OutputDevice::OutputDevice(const OutputDeviceLayout& layout, const std::string& name)
	\brief OutputDevice constructor.
	\param layout The component layout.
	\param name The name of the component.
	**/
	OutputDevice::OutputDevice(const OutputDeviceLayout& layout, const std::string& name)
	 : AbstractComponentLayout(layout), Component(layout, name)
	{
		argumentsList.reserve(getNumInputPort());
	}

	OutputDevice::~OutputDevice(void)
	{
		argumentsList.clear();
	}

	/**
	\fn HdlTexture& OutputDevice::in(int port)
	\brief Access the texture on indexed port.
	\param port The index of the port. Default is first port.
	\return A reference to the texture bound to the port or raise an exception if any errors occur.
	**/
	HdlTexture& OutputDevice::in(int port)
	{
		checkInputPort(port);

		return *argumentsList[port];
	}

	/**
	\fn HdlTexture& OutputDevice::in(const std::string& port)
	\brief Access the texture on indexed port.
	\param port The name of the port. Default is the first port.
	\return A reference to the texture bound to the port or raise an exception if any errors occur.
	**/
	HdlTexture& OutputDevice::in(const std::string& port)
	{
		int id = 0;

		if(!port.empty())
			id = getInputPortID(port);

		return in(id);
	}

	/**
	\fn OutputDevice& OutputDevice::operator<<(HdlTexture& texture)
	\brief Push a texture in the arguments list. The user must maintain the texture in memory while this OutputDevice hasn't received a OutputDevice::Process or OutputDevice::Reset signal.
	\param texture Input texture.
	\return A reference to this object.
	**/
	OutputDevice& OutputDevice::operator<<(HdlTexture& texture)
	{
		if(getNumInputPort()==0)
			throw Exception("OutputDevice::operator<<(HdlTexture&) - OutputDevice " + getFullName() + " has no configured input ports.", __FILE__, __LINE__);

		if(argumentsList.size()>=getNumInputPort())
			throw Exception("OutputDevice::operator<<(HdlTexture&) - Too much arguments given to OutputDevice " + getFullName() + ".", __FILE__, __LINE__);

		argumentsList.push_back(&texture);

		return *this;
	}

	/**
	\fn OutputDevice& OutputDevice::operator<<(Pipeline& pipeline)
	\brief Push all the output texture of pipeline in the arguments list. The user must maintain the textures in memory while this OutputDevice hasn't received a OutputDevice::Process or OutputDevice::Reset signal.
	\param pipeline A pipeline.
	\return A reference to this object.
	**/
	OutputDevice& OutputDevice::operator<<(Pipeline& pipeline)
	{
		if(getNumInputPort()==0)
			throw Exception("OutputDevice::operator<<(Pipeline&) - OutputDevice " + getFullName() + " has no configured input ports.", __FILE__, __LINE__);

		for(int i=0; i<pipeline.getNumOutputPort(); i++)
		{
			if(argumentsList.size()>=getNumInputPort())
				throw Exception("OutputDevice::operator<<(Pipeline&) - Too much arguments given to OutputDevice " + getFullName() + ".", __FILE__, __LINE__);

			argumentsList.push_back(&pipeline.out(i));
		}

		return *this;
	}

	/**
	\fn OutputDevice& OutputDevice::operator<<(OutputDevice::ActionType a)
	\brief Apply operation on previously input data.
	\param a The ActionType (Process or Reset arguments).
	\return This pipeline or raise an exception if any errors occur.
	**/
	OutputDevice& OutputDevice::operator<<(OutputDevice::ActionType a)
	{
		if(getNumInputPort()==0)
			throw Exception("OutputDevice::operator<<(OutputDevice::ActionType) - OutputDevice " + getFullName() + " has no configured input ports.", __FILE__, __LINE__);

		// Check the number of arguments given :
		if(argumentsList.size()!=getNumInputPort())
			throw Exception("OutputDevice::operator<<(ActionType) - Too few arguments given to OutputDevice " + getFullName() + ".", __FILE__, __LINE__);

		switch(a)
		{
			case Process:
				process();
			case Reset:
				argumentsList.clear();
				break;
			default:
				throw Exception("OutputDevice::operator<<(ActionType) - Unknown action for OutputDevice" + getFullName() + ".", __FILE__, __LINE__);
		}

		return *this;
	}

	/**
	\fn OutputDevice& OutputDevice::operator<<(Pipeline::ActionType a)
	\brief Apply operation on previously input data.
	\param a The ActionType (Process or Reset arguments).
	\return This pipeline or raise an exception if any errors occur.
	**/
	OutputDevice& OutputDevice::operator<<(Pipeline::ActionType a)
	{
		return (*this) << static_cast<OutputDevice::ActionType>(a);
	}
