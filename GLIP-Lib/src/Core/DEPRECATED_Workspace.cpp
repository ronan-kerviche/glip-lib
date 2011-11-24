/*#include "Workspace.hpp"
#include "Devices.hpp"

using namespace Glip::CorePipeline;

// Tools
    Workspace::Workspace(const std::string& str) : name(str)
    { }

    Workspace::~Workspace(void)
    {
        for(std::vector<InputDevice*>::iterator it=inputDevicesList.begin(); it!=inputDevicesList.end(); it++)
            delete (*it);

        for(std::vector<OutputDevice*>::iterator it=outputDevicesList.begin(); it!=outputDevicesList.end(); it++)
            delete (*it);

        for(std::vector<Filter*>::iterator it=filtersList.begin(); it!=filtersList.end(); it++)
            delete (*it);

        inputDevicesList.clear();
        outputDevicesList.clear();
        filtersList.clear();
    }

    void Workspace::addInput(InputDevice* d)
    {
        inputDevicesList.push_back(d);
    }

    void Workspace::addOutput(OutputDevice* d)
    {
        outputDevicesList.push_back(d);
    }*/
