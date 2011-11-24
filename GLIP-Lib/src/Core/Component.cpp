#include "Exception.hpp"
#include "Component.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

//__ReadOnly_ComponentLayout
    __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const std::string& type) : ObjectName("(Template)", type)
    { }

    __ReadOnly_ComponentLayout::__ReadOnly_ComponentLayout(const __ReadOnly_ComponentLayout& c) : ObjectName(c)
    {
        std::cout << "Copying ReadOnly Component Layout " << getName() << std::endl;
        inputPortDescription  = c.inputPortDescription;
        outputPortDescription = c.outputPortDescription;
        std::cout << "Size : " << inputPortDescription.size() << std::endl;
    }

    void __ReadOnly_ComponentLayout::checkInputPort(int i)
    {
        if((i)<0 || (i)>=getNumInputPort())
            throw Exception("ComponentLayout - Bad input port ID for "  + getNameExtended() + " ID : " + to_string(i), __FILE__, __LINE__);
    }

    void __ReadOnly_ComponentLayout::checkOutputPort(int i)
    {
        if((i)<0 || (i)>=getNumOutputPort())
            throw Exception("ComponentLayout - Bad output port ID for " + getNameExtended() + " ID : " + to_string(i), __FILE__, __LINE__);
    }

    int __ReadOnly_ComponentLayout::getNumInputPort(void)
    {
        return inputPortDescription.size();
    }

    const std::string& __ReadOnly_ComponentLayout::getInputPortName(int id)
    {
        checkInputPort(id);
        return inputPortDescription[id].getName();
    }

    std::string __ReadOnly_ComponentLayout::getInputPortNameExtended(int id)
    {
        checkInputPort(id);
        return inputPortDescription[id].getNameExtended();
    }

    int __ReadOnly_ComponentLayout::getInputPortID(const std::string& name)
    {
        try
        {
            return getIndexByName(name, inputPortDescription);
        }
        catch(std::exception& e)
        {
            Exception m("__ReadOnly_ComponentLayout::getInputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }
    }

    int __ReadOnly_ComponentLayout::getNumOutputPort(void)
    {
        return outputPortDescription.size();
    }

    const std::string& __ReadOnly_ComponentLayout::getOutputPortName(int id)
    {
        checkOutputPort(id);
        return outputPortDescription[id].getName();
    }

    std::string __ReadOnly_ComponentLayout::getOutputPortNameExtended(int id)
    {
        checkOutputPort(id);
        return outputPortDescription[id].getNameExtended();
    }

    int __ReadOnly_ComponentLayout::getOutputPortID(const std::string& name)
    {
        try
        {
            return getIndexByName(name, outputPortDescription);
        }
        catch(std::exception& e)
        {
            Exception m("__ReadOnly_ComponentLayout::getOutputPortID - caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }
    }

// ComponentLayout
    ComponentLayout::ComponentLayout(const std::string& type) : __ReadOnly_ComponentLayout(type)
    { }

    ComponentLayout::ComponentLayout(const __ReadOnly_ComponentLayout& c) : __ReadOnly_ComponentLayout(c)
    { }

    void ComponentLayout::setInputPortName(int id, const std::string& name)
    {
        checkInputPort(id);
        inputPortDescription[id].setName(name);
    }

    int ComponentLayout::addInputPort(const std::string& name)
    {
        int id = inputPortDescription.size();
        inputPortDescription.push_back(ObjectName(name, id));
        return id;
    }

    void ComponentLayout::setOutputPortName(int id, const std::string& name)
    {
        checkOutputPort(id);
        outputPortDescription[id].setName(name);
    }

    int ComponentLayout::addOutputPort(const std::string& name)
    {
        int id = outputPortDescription.size();
        outputPortDescription.push_back(ObjectName(name, id));
        return id;
    }

// Component :
    Component::Component(const __ReadOnly_ComponentLayout& c, const std::string& name) : __ReadOnly_ComponentLayout(c)
    {
        setName(name);
    }

