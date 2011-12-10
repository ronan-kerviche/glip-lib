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
                        HdlTexture* 	texture;
                        bool        	newImage;
                        int         	imagesMissed;
                protected :
                    // Tools
                        InputDevice(const std::string& name);

                        void        	declareNewImage(void);
                        HdlTexture* 	ownerTexturePtr(void);
                public :
                    // Tools
                        ~InputDevice(void);
                        bool        	isNewImage(void);
                        int         	getMissedImagesCount(void);
                        HdlTexture* 	texturePtr(void);
            };

            class OutputDevice : public ObjectName
            {
                private :
                    // Data
                        HdlTexture* 	texture;
                        bool       	newImage;
                        int         	imagesMissed;
                protected :
                    // Tools
                        OutputDevice(const std::string& name);
                        bool        	isNewImage(void);
                        HdlTexture* 	readTexture(void);
                public :
                    // Tools
                        void        	giveTexture(HdlTexture*);
                        void		forgetLastTexture(void);
                        int         	getMissedImagesCount(void);
            };
    }
}

#endif // DEVICES_HPP_INCLUDED
