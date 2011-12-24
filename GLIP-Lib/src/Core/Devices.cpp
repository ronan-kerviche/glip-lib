#include <iostream>
#include "Devices.hpp"
#include "HdlTexture.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// Tools - InputDevice
	InputDevice::InputDevice(const std::string& name) : t(NULL), newImage(false), imagesMissed(0), ObjectName(name, "Input Device")
	{ }

	InputDevice::~InputDevice(void)
	{
		delete t;
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

	HdlTexture& InputDevice::texture(void)
	{
		newImage = false;
		return *t;
	}

// Tools - OutputDevice
	OutputDevice::OutputDevice(const std::string& name) : ObjectName(name, "Output Device")
	{ }

	OutputDevice& OutputDevice::operator<<(HdlTexture& t)
	{
		process(t);
		return *this;
	}
