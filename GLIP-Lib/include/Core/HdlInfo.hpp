#ifndef HDLINFO_H
#define HDLINFO_H
#include <string>
#include <map>

namespace Glip
{
    namespace CoreGL
    {
        typedef std::map<std::string, bool> ext_map_type;

        struct glinfo_data
        {
            std::string vendor;
            std::string renderer;
            std::string version;
            int redBits;
            int greenBits;
            int blueBits;
            int alphaBits;
            int depthBits;
            int stencilBits;
            int maxTextureSize;
            int maxLights;
            int maxAttribStacks;
            int maxModelViewStacks;
            int maxProjectionStacks;
            int maxClipPlanes;
            int maxTextureStacks;
        };

        class HdlInfo
        {
            public:
            static HdlInfo* instance();

            /*
            *   Load and check extension
            */
            bool init(std::string extensionfile);

            /*
            *   Load config of your Gpu
            */
            bool loadConfig();

            /*
            *	Check extension supported by Gpu in the List
            */
            void checkExtSupportedByGpu();

            /*
            *	Check in List if all extension used by lib is supported by Gpu
            */
            bool checkExtUsedByLib(std::string extensionfile);

            /*
            *	Check if extension ext is support by Gpu
            * 	@return: True if extension is pported by Gpu
            */
            bool isExtensionSupported(std::string ext);

            private:
            ext_map_type extentions;
            glinfo_data *glinfo;
            static HdlInfo* p_instance;
        };
    }
}


#endif
