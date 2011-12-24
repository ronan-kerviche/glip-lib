#ifndef __GLIP_DEVICES__
#define __GLIP_DEVICES__

    // Includes
        #include "NamingLayout.hpp"

namespace Glip
{
	namespace CoreGL
	{
		class HdlTexture;
	}

	using namespace CoreGL;

	namespace CorePipeline
	{
		/** TODO :
		Add multiple frame cyclic buffer
		**/

		// Objects
		class InputDevice : public ObjectName
		{
			private :
				// Data
				HdlTexture* 	t;
				bool        	newImage;
				int         	imagesMissed;
			protected :
				// Tools
				InputDevice(const std::string& name);

				void        	declareNewImage(void);
			public :
				// Tools
				~InputDevice(void);
				bool        	isNewImage(void);
				int         	getMissedImagesCount(void);
				HdlTexture& 	texture(void);
		};

		class OutputDevice : public ObjectName
		{
			private :

			protected :
				// Tools
				OutputDevice(const std::string& name);

				virtual void process(HdlTexture& t) = 0;
			public :
				// Tools
				OutputDevice& operator<<(HdlTexture& t);
		};
	}
}

#endif // DEVICES_HPP_INCLUDED
