/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Devices.hpp                                                                               */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Input and Output devices objects                                                          */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Devices.hpp
 * \brief   Input and Output devices objects
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#ifndef __GLIP_DEVICES__
#define __GLIP_DEVICES__

    // Includes
        #include "Component.hpp"
        #include "HdlTexture.hpp"
        #include "Pipeline.hpp"

	namespace Glip
	{

		using namespace CoreGL;

		namespace CorePipeline
		{
			// Objects
/**
\class InputDevice
\brief Input of data onto the GPU.

Using input device :
\code
class MyInputDevice : public InputDevice
{
	public :
		MyInputDevice(...)
		 : InputDevice(declareLayout(...), "instanceName"), ...
		{
			// Your init here...

			// Set the texture links :
			setTextureLink(texture1,0);

			// ...
		}

	private :
		InputDevice::InputDeviceLayout declareLayout(...)
		{
			// Create a layout here, from some arguments...
			InputDevice::InputDeviceLayout tmp("SomeTypeOfInputDevice");

			// Add output ports :
			tmp.addOutputPort("somePortName");
			// ...

			return tmp;
		}

		void someMethodToAddImage(void)
		{
			// Prepare a new image...

			// Declare new image by relinking it to output :
			setTextureLink(texture1,0);

			// Or just keep previous link and declare :
			//declareNewImage(0);
		}
}
\endcode
**/
			class InputDevice : public Component
			{
				private :
					// Data
					std::vector<HdlTexture*> 	texturesLinks;
					std::vector<bool>        	newImages;
					std::vector<int>         	imagesMissedCount;

				protected :
					// Sub structure :
/**
\class InputDeviceLayout
\brief InputDevice layout structure.
**/
					class InputDeviceLayout : public ComponentLayout
					{
						public :
							InputDeviceLayout(const std::string& _typeName);
							InputDeviceLayout(const InputDeviceLayout& c);

							int addOutputPort(const std::string& name);
							//int addInputPort(const std::string& name);
					};

					// Tools
					InputDevice(const InputDeviceLayout& layout, const std::string& name);

					void 		setTextureLink(HdlTexture* tex, int port=0);
					void 		setTextureLink(HdlTexture* tex,const std::string& port);
					void		declareNewImage(int port=0);
					void		declareNewImage(const std::string& port);
					void		declareNewImageOnAllPorts(void);
				public :
					// Tools
					~InputDevice(void);

					bool 		isNewImage(int port=0) const;
					bool		isNewImage(const std::string& port) const;
					int		getMissedImagesCount(int port=0) const;
					int		getMissedImagesCount(const std::string& port) const;
					int		getMissedImagesTotalCount(void) const;
					bool		portHasValidOutput(int port=0) const;
					bool		portHasValidOutput(const std::string& port) const;
					HdlTexture&	out(int port=0);
					HdlTexture& 	out(const std::string& port);
			};

/**
\class OutputDevice
\brief Output of data from the GPU.

Using output device :
\code
class MyOutputDevice : public OutputDevice
{
	public :
		MyOutputDevice(...)
		 : OutputDevice( declareLayout(...), "instanceName"), ....
		{
			// Your init code here...
		}

	private :
		OutputDevice::OutputDeviceLayout declareLayout(...)
		{
			// Create a layout here, from some arguments...
			OutputDevice::OutputDeviceLayout tmp("SomeTypeOfOutputDevice");

			// Add input ports :
			tmp.addInputPort("somePortName");
			// ...

			return tmp;
		}

		void process(void)
		{
			// Process argument lists with :
			for(int i=0; i<getNumInputPort(); i++)
			{
				// Read the i'th argument with :
				in(i);
			}
		}
}
\endcode
**/
			class OutputDevice : public Component
			{
				public :
					///Actions enumeration.
					enum ActionType
					{
						///To start process.
						Process	= Pipeline::Process,
						///To reset argument chain.
						Reset	= Pipeline::Reset
					};

				private :
					int				currentArgId;
					std::vector<HdlTexture*>	argumentsList;

					void clearArgList(void);

				protected :
					// Sub structure :
/**
\class OutputDeviceLayout
\brief OutputDevice layout structure.
**/
					class OutputDeviceLayout : public ComponentLayout
					{
						public :
							OutputDeviceLayout(const std::string& _typeName);
							OutputDeviceLayout(const OutputDeviceLayout& c);

							//int addOutputPort(const std::string& name);
							int addInputPort(const std::string& name);
					};

					// Tools
					OutputDevice(const OutputDeviceLayout& layout, const std::string& name);

					HdlTexture& in(int port=0);
					HdlTexture& in(const std::string& port);

					/**
					\fn virtual void process(void) = 0;
					\brief The user-defined processing stage. This will be called upon the reception of a Process signal. Use OutputDevice::in functions to read input.
					**/
					virtual void process(void) = 0;

				public :
					~OutputDevice(void);

					// Tools
					OutputDevice& operator<<(HdlTexture& texture);
					OutputDevice& operator<<(Pipeline& pipeline);
					OutputDevice& operator<<(OutputDevice::ActionType a);
					OutputDevice& operator<<(Pipeline::ActionType a);
			};
		}
	}

#endif // DEVICES_HPP_INCLUDED
