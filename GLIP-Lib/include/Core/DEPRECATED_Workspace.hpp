
/*#ifndef __GLIP_WORKSPACE__
#define __GLIP_WORKSPACE__

    // Includes
        #include <vector>
        #include "Devices.hpp"

namespace Glip
{
    namespace CorePipeline
    {
        // Prototype
            class Filter;
            class InputDevice;
            class OutputDevice;

        // Object
            class Workspace
            {
                private :
                    // Connexions data type
                        struct Connexion
                        {
                            int port;
                            int filter;

                            Connexion(int p, int f);
                            Connexion(const Connexion&);
                        };

                    // Data
                        std::string                name;
                        bool                       locked;
                        std::vector<InputDevice*>  inputDevicesList;
                        std::vector<OutputDevice*> outputDevicesList;
                        std::vector<int>           inputSocket;
                        std::vector<int>           outputSocket;
                        std::vector<Filter*>       filtersList;
                        std::vector<bool>          enabled;
                        std::vector<Connexion>     Output;

                public :
                    // Tools
                        Workspace(const std::string&);
                        ~Workspace(void);

                        void lock(void);
                        bool isLocked(void);
                        void unlock(void);

                        // The above functions need the unlocked state
                            void addInput(InputDevice*);
                            void addOutput(OutputDevice*);

                        // The above functions don't need the unlocked state
                            void setEnableState(ObjectName);

                        // Standard Canvas
                            virtual void build(void) = 0;
                            virtual void process(void);
            };
    }
}

#endif*/
