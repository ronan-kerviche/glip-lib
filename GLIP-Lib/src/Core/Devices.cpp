#include <iostream>
#include "Devices.hpp"
#include "HdlTexture.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// Tools - InputDevice
    InputDevice::InputDevice(const std::string& name) : texture(NULL), newImage(false), imagesMissed(0), ObjectName(name, "Input Device")
    { }

    InputDevice::~InputDevice(void)
    {
        delete texture;
    }

    void InputDevice::declareNewImage(void)
    {
        if(newImage)
            imagesMissed++;
        newImage = true;
    }

    bool InputDevice::isNewImage(void)
    {
        return newImage;
    }

    int InputDevice::getMissedImagesCount(void)
    {
        return imagesMissed;
    }

    HdlTexture* InputDevice::ownerTexturePtr(void)
    {
        return texture;
    }

    HdlTexture* InputDevice::texturePtr(void)
    {
        newImage = false;
        return texture;
    }

// Tools - OutputDevice
	OutputDevice::OutputDevice(const std::string& name) : texture(NULL), newImage(false), imagesMissed(0), ObjectName(name, "Output Device")
	{ }

	bool OutputDevice::isNewImage(void)
	{
		return newImage;
	}

	HdlTexture* OutputDevice::readTexture(void)
	{
		newImage = false;
		return texture;
	}

	void OutputDevice::giveTexture(HdlTexture* t)
	{
		if(t!=NULL)
		{
			if(newImage)
				imagesMissed++;

			newImage = true;
			texture = t;
		}
		else
			forgetLastTexture();
	}

	void OutputDevice::forgetLastTexture(void)
	{
		newImage = false;
	}

	int OutputDevice::getMissedImagesCount(void)
	{
		return imagesMissed;
	}
