/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : GPLv3                                                                                     */
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
#include "Devices.hpp"
#include "Exception.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// Tools - InputDevice
	/**
	\fn InputDevice::InputDevice(const std::string& name)
	\brief InputDevice constructor.
	\param name The name of the component.
	**/
	InputDevice::InputDevice(const std::string& name)
	 : t(NULL), newImage(false), imagesMissed(0), ObjectName(name, "InputDevice")
	{ }

	InputDevice::~InputDevice(void)
	{
		if(t!=NULL)
			delete t;
	}

	/**
	\fn void InputDevice::allocateNewImage(const __ReadOnly_HdlTextureFormat& fmt)
	\brief Create a new image or replace the existing one if format differs.
	\param fmt The new format of the texture.
	**/
	void InputDevice::allocateNewImage(const __ReadOnly_HdlTextureFormat& fmt)
	{
		if(t==NULL)
			t = new HdlTexture(fmt);
		else
		{
			if(*t!=fmt)
			{
				delete t;
				t = new HdlTexture(fmt);
			}
		}
	}

	/**
	\fn void InputDevice::declareNewImage(void)
	\brief Declare that a new image arrived.
	**/
	void InputDevice::declareNewImage(void)
	{
		if(t!=NULL)
		{
			if(newImage)
				imagesMissed++;
			newImage = true;
		}
	}

	/**
	\fn bool InputDevice::isNewImage(void)
	\brief Check if there is a new image.
	\return true if there is a new image.
	**/
	bool InputDevice::isNewImage(void)
	{
		return newImage;
	}

	/**
	\fn bool InputDevice::isNewImage(void)
	\brief Get the number of images missed (texture() wasn't call for new image).
	\return The number of images missed.
	**/
	int InputDevice::getMissedImagesCount(void)
	{
		return imagesMissed;
	}

	/**
	\fn HdlTexture& InputDevice::texture(void)
	\brief Get the current image.
	\return Reference to the current image or raise an exception if any errors occur.
	**/
	HdlTexture& InputDevice::texture(void)
	{
		if(t==NULL)
			throw Exception("InputDevice::texture - No texture was created", __FILE__, __LINE__);
		newImage = false;
		return *t;
	}

// Tools - OutputDevice
	/**
	\fn OutputDevice::OutputDevice(const std::string& name)
	\brief OutputDevice constructor.
	\param name The name of the component.
	**/
	OutputDevice::OutputDevice(const std::string& name)
	 : ObjectName(name, "OutputDevice")
	{ }

	/**
	\fn OutputDevice& OutputDevice::operator<<(HdlTexture& t)
	\brief Connection to the component, will call process().
	\param t Input texture.
	**/
	OutputDevice& OutputDevice::operator<<(HdlTexture& t)
	{
		process(t);
		return *this;
	}
