#include "Pipeline.hpp"
#include "Component.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// __ReadOnly_PipelineLayout
    __ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const std::string& type) : __ReadOnly_ComponentLayout(type)
    { }

    __ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout(const __ReadOnly_PipelineLayout& c) : __ReadOnly_ComponentLayout(c)
    {
        //std::cout << "Starting copy of pipeline layout for " << getNameExtended() << std::endl;
        // Copy of the whole vector
        elementsKind   = c.elementsKind;
        elementsID     = c.elementsID;

        for(int i=0; i<c.elementsLayout.size(); i++)
        {
            switch(elementsKind[i])
            {
                case FILTER:
                    elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(new __ReadOnly_FilterLayout(c.filterLayout(i))));
                    break;
                case PIPELINE:
                    elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(new __ReadOnly_PipelineLayout(c.pipelineLayout(i))));
                    break;
                default:
                    throw Exception("__ReadOnly_PipelineLayout::__ReadOnly_PipelineLayout - Unknown type for copy", __FILE__, __LINE__);
            }
        }
        //std::cout << "end copy of pipeline layout for " << getNameExtended() << std::endl;
    }

    int __ReadOnly_PipelineLayout::getElementID(int i)
    {
        checkElement(i);
        return elementsID[i];
    }

    void __ReadOnly_PipelineLayout::setElementID(int i, int ID)
    {
        checkElement(i);
        elementsID[i] = ID;
    }

    void __ReadOnly_PipelineLayout::checkElement(int i) const
    {
        if(i<0 || i>=elementsLayout.size())
            throw Exception("FilterLayout - Bad input port ID for "  + getNameExtended() + " ID : " + to_string(i), __FILE__, __LINE__);
    }

    int __ReadOnly_PipelineLayout::getNumElements(void)
    {
        return elementsLayout.size();
    }

    int __ReadOnly_PipelineLayout::getElementIndex(const std::string& name) const
    {
        int index;

        try
        {
            index = getIndexByNameFct(name, elementsLayout.size(), __ReadOnly_PipelineLayout::componentName, reinterpret_cast<const void*>(this));
        }
        catch(std::exception& e)
        {
            Exception m("getElementIndex - Caught an exception while looking for " + name + " in " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }

        return index;
    }

    ComponentKind __ReadOnly_PipelineLayout::getElementKind(int i) const
    {
        checkElement(i);
        return elementsKind[i];
    }

    ObjectName& __ReadOnly_PipelineLayout::componentName(int i, const void* obj)
    {
        return reinterpret_cast<const __ReadOnly_PipelineLayout*>(obj)->componentLayout(i);
    }

    __ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(int i) const
    {
        checkElement(i);

        //std::cout << "ACCESSING COMPONENT (int)" << std::endl;
        //return *elementsLayout[i];
        switch(elementsKind[i])
        {
            case FILTER:
                return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[i]);
            case PIPELINE:
                return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[i]);
            default :
                throw Exception("__ReadOnly_PipelineLayout::componentLayout - Type not recognized",__FILE__, __LINE__);
        }
    }

    __ReadOnly_ComponentLayout& __ReadOnly_PipelineLayout::componentLayout(const std::string& name) const
    {
        int index = getElementIndex(name);
        //std::cout << "ACCESSING COMPONENT (int)" << std::endl;
        return componentLayout(index);
    }

    __ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(int i) const
    {
        //std::cout << "ACCESSING FILTER (int)" << std::endl;
        checkElement(i);
        if(getElementKind(i)!=FILTER)
            throw Exception("__ReadOnly_PipelineLayout::filterLayout - This element exists but is not a filter!");
         return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[i]);
    }

    __ReadOnly_FilterLayout& __ReadOnly_PipelineLayout::filterLayout(const std::string& name) const
    {
        //std::cout << "ACCESSING FILTER (name)" << std::endl;
        int index = getElementIndex(name);
        if(getElementKind(index)!=FILTER)
            throw Exception("__ReadOnly_PipelineLayout::filterLayout - This element exists but is not a filter!");
        return *reinterpret_cast<__ReadOnly_FilterLayout*>(elementsLayout[index]);
    }

    __ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(int i) const
    {
        //std::cout << "ACCESSING PIPELINE (int)" << std::endl;
        checkElement(i);
        if(getElementKind(i)!=PIPELINE)
            throw Exception("__ReadOnly_PipelineLayout::pipelineLayout - This element exists but is not a pipeline!");
        return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[i]);
    }

    __ReadOnly_PipelineLayout& __ReadOnly_PipelineLayout::pipelineLayout(const std::string& name) const
    {
        //std::cout << "ACCESSING PIPELINE (name)" << std::endl;
        int index = getElementIndex(name);
        if(getElementKind(index)!=PIPELINE)
            throw Exception("__ReadOnly_PipelineLayout::pipelineLayout - This element exists but is not a pipeline!");
        return *reinterpret_cast<__ReadOnly_PipelineLayout*>(elementsLayout[index]);
    }

    std::vector<__ReadOnly_PipelineLayout::Connexion> __ReadOnly_PipelineLayout::getConnexionDestinations(int id, int p)
    {
        if(id!=THIS_PIPELINE)
        {
            __ReadOnly_ComponentLayout& src = componentLayout(id);
            src.checkOutputPort(p);
        }
        else
            checkInputPort(p);

        // The Element and its port exist, now find their connexions
        std::vector<Connexion> result;
        for(std::vector<Connexion>::iterator it=connexions.begin(); it!=connexions.end(); it++)
            if( (*it).idOut==id && (*it).portOut==p) result.push_back(*it);

        return result;
    }

    __ReadOnly_PipelineLayout::Connexion __ReadOnly_PipelineLayout::getConnexionSource(int id, int p)
    {
        std::string str;

        if(id!=THIS_PIPELINE)
        {
            __ReadOnly_ComponentLayout& src = componentLayout(id);
            src.checkInputPort(p);
        }
        else
            checkOutputPort(p);

        // The Element and its port exist, now find the connexion
        for(std::vector<Connexion>::iterator it=connexions.begin(); it!=connexions.end(); it++)
            if( (*it).idIn==id && (*it).portIn==p) return (*it);

        if(id!=THIS_PIPELINE)
        {
            __ReadOnly_ComponentLayout& src = componentLayout(id);
            throw Exception("Element " + src.getNameExtended() + " has no source on output port " + src.getInputPortNameExtended(p), __FILE__, __LINE__);
        }
        else
            throw Exception("This Pipeline " + getNameExtended() + " has no source on output port " + getOutputPortNameExtended(p), __FILE__, __LINE__);
    }

    std::string __ReadOnly_PipelineLayout::getConnexionDestinationsName(int source, int port)
    {
        std::vector<Connexion> res = getConnexionDestinations(source, port);
        std::string result;

        for(std::vector<Connexion>::iterator it=res.begin(); it!=res.end(); it++)
        {
            __ReadOnly_ComponentLayout& tmp = componentLayout((*it).idIn);
            result += tmp.getNameExtended() + SEPARATOR + tmp.getInputPortNameExtended((*it).portIn) + "\n";
        }

        return result;
    }

    std::string __ReadOnly_PipelineLayout::getConnexionDestinationsName(const std::string& source, const std::string& port)
    {
        int id = getElementIndex(source);
        int p  = componentLayout(id).getOutputPortID(port);
        return getConnexionDestinationsName(id, p);
    }

    std::string __ReadOnly_PipelineLayout::getConnexionSourceName(int dest, int port)
    {
        Connexion c = getConnexionSource(dest, port);
        __ReadOnly_ComponentLayout& tmp = componentLayout(c.idOut);
        return tmp.getNameExtended() + SEPARATOR + tmp.getInputPortNameExtended(c.portOut) + "\n";
    }

    std::string __ReadOnly_PipelineLayout::getConnexionSourceName(const std::string& dest, const std::string& port)
    {
        int id = getElementIndex(dest);
        int p  = componentLayout(id).getOutputPortID(port);
        return getConnexionSourceName(id, p);
    }

    bool __ReadOnly_PipelineLayout::check(bool exception)
    {
        std::string res;

        for(int i=0; i<elementsLayout.size(); i++)
        {
            __ReadOnly_ComponentLayout& tmp = componentLayout(i);
            for(int j=0; j<tmp.getNumInputPort(); j++)
            {
                try
                {
                    getConnexionSource(i, j);
                }
                catch(std::exception& e)
                {
                    res += e.what();
                    res += '\n';
                }
            }
        }

        for(int i=0; i<getNumOutputPort(); i++)
        {
            try
            {
                getConnexionSource(THIS_PIPELINE, i);
            }
            catch(std::exception& e)
            {
                res += e.what();
                res += '\n';
            }
        }

        if(exception && !res.empty())
            throw Exception("check - The following errors has been found in the PipelineLayout " + getNameExtended() + " : \n" + res, __FILE__, __LINE__);
        else
            return false;

        return true;
    }

// PipelineLayout
    PipelineLayout::PipelineLayout(const std::string& type) : __ReadOnly_ComponentLayout(type), ComponentLayout(type), __ReadOnly_PipelineLayout(type)
    { }

    PipelineLayout::PipelineLayout(__ReadOnly_PipelineLayout& c) : __ReadOnly_PipelineLayout(c), __ReadOnly_ComponentLayout(c), ComponentLayout(c)
    { }

    int PipelineLayout::add(const __ReadOnly_FilterLayout& filterLayout, const std::string& name)
    {
        __ReadOnly_FilterLayout* tmp = new __ReadOnly_FilterLayout(filterLayout);
        tmp->setName(name);
        elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(tmp));
        elementsKind.push_back(FILTER);
        elementsID.push_back(ELEMENT_NOT_ASSOCIATED);
        return elementsLayout.size()-1;
    }

    int PipelineLayout::add(const __ReadOnly_PipelineLayout& pipelineLayout, const std::string& name)
    {
        __ReadOnly_PipelineLayout* tmp = new __ReadOnly_PipelineLayout(pipelineLayout);
        tmp->setName(name);
        elementsLayout.push_back(reinterpret_cast<__ReadOnly_ComponentLayout*>(tmp));
        elementsKind.push_back(PIPELINE);
        elementsID.push_back(ELEMENT_NOT_ASSOCIATED);
        return elementsLayout.size()-1;
    }

    int PipelineLayout::addInput(const std::string& name)
    {
        return addInputPort(name);
    }

    int PipelineLayout::addOutput(const std::string& name)
    {
        return addOutputPort(name);
    }

    void PipelineLayout::connect(int filterOut, int portOut, int filterIn, int portIn)
    {
        if(filterOut==THIS_PIPELINE && filterIn==THIS_PIPELINE)
            throw Exception("PipelineLayout::connect - can't connect directly an input to an output, you don't need that!", __FILE__, __LINE__);

        if(filterOut!=THIS_PIPELINE)
        {
            __ReadOnly_ComponentLayout& fo = componentLayout(filterOut); // Source
            fo.checkOutputPort(portOut);
        }
        else
            checkInputPort(portOut);

        if(filterIn!=THIS_PIPELINE)
        {
            __ReadOnly_ComponentLayout& fi = componentLayout(filterIn);  // Destination
            fi.checkInputPort(portIn);
        }
        else
            checkOutputPort(portIn);

        // Check if a connexion already exist to the destination :
        for(std::vector<Connexion>::iterator it=connexions.begin(); it!=connexions.end(); it++)
            if( (*it).idIn==filterIn && (*it).portIn==portIn)
                if(filterIn!=THIS_PIPELINE)
                    throw Exception("PipelineLayout::connect - A connexion already exists to the destination : " + componentLayout(filterIn).getNameExtended() + " on port " + componentLayout(filterIn).getInputPortNameExtended(portIn), __FILE__, __LINE__);
                else
                    throw Exception("PipelineLayout::connect - A connexion already exists to this pipeline output : " + getNameExtended() + " on port " + getInputPortNameExtended(portIn), __FILE__, __LINE__);

        Connexion c;
        c.idOut   = filterOut;
        c.portOut = portOut;
        c.idIn    = filterIn;
        c.portIn  = portIn;

        //std::cout << "Connexion de " << filterOut << ':' << portOut << " à " << filterIn << ':' << portIn << std::endl;

        connexions.push_back(c);
    }

    void PipelineLayout::connect(const std::string& filterOut, const std::string& portOut, const std::string& filterIn, const std::string& portIn)
    {
        int fi = getElementIndex(filterIn),
            fo = getElementIndex(filterOut),
            pi = componentLayout(filterIn).getInputPortID(portIn),
            po = componentLayout(filterOut).getOutputPortID(portOut);

        connect(fo, po, fi, pi); // Check-in done twice but...
    }

    void PipelineLayout::connectToInput(int port, int filterIn,  int portIn)
    {
        connect(THIS_PIPELINE, port, filterIn, portIn);
    }

    void PipelineLayout::connectToInput(const std::string& port, const std::string& filterIn, const std::string& portIn)
    {
        try
        {
            int p  = getInputPortID(port),
                fi = getElementIndex(filterIn),
                pi = componentLayout(filterIn).getInputPortID(portIn);
            connect(THIS_PIPELINE, p, fi, pi);
        }
        catch(std::exception& e)
        {
            Exception m("PipelineLayout::connectToInput (str) - Caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }
    }

    void PipelineLayout::connectToOutput(int filterOut, int portOut, int port)
    {
        connect(filterOut, portOut, THIS_PIPELINE, port);
    }

    void PipelineLayout::connectToOutput(const std::string& filterOut, const std::string& portOut, const std::string& port)
    {
        try
        {
            int p  = getOutputPortID(port),
                fo = getElementIndex(filterOut),
                po = componentLayout(filterOut).getOutputPortID(portOut);
            connect(fo, po, THIS_PIPELINE, p);
        }
        catch(std::exception& e)
        {
            Exception m("PipelineLayout::connectToOutput (str) - Caught an exception for the object " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }
    }

// Pipeline
    Pipeline::Pipeline(__ReadOnly_PipelineLayout& p, const std::string& name) : __ReadOnly_ComponentLayout(p), __ReadOnly_PipelineLayout(p), Component(p, name)
    {
        input.resize(getNumInputPort());
        output.resize(getNumOutputPort());

        for(TablePtr::iterator it = input.begin(); it!=input.end(); it++)
            (*it) = NULL;

        for(TablePtr::iterator it = output.begin(); it!=output.end(); it++)
            (*it) = NULL;

        build();
    }

    void Pipeline::cleanInput(void)
    {
        input.clear();
        input.resize(getNumInputPort()); // Usefull?
    }

    void Pipeline::build(void)
    {
        std::cout << "Building pipeline " << getNameExtended() << std::endl;

        try
        {
            // 1st Step

            // Push this as the first item of the wait list
            // While the wait list is not empty
            //     Take the first/last element in the list and look for all its elements
            //          If the element is a pipeline, push it in the wait list
            //          If the element is a filter, create an instance of it and set its ID
            //     Done.
            // Done.

            std::list<__ReadOnly_PipelineLayout*> waitList;
            waitList.push_back(this);


            while(!waitList.empty())
            {
                __ReadOnly_PipelineLayout* tmp = waitList.front();
                for(int i=0; i<tmp->getNumElements(); i++)
                {
                    switch(tmp->getElementKind(i))
                    {
                        case FILTER :
                            //std::cout << "    Adding a new component" << std::endl;
                            filters.push_back(new Filter(tmp->filterLayout(i)));
                            tmp->setElementID(i, filters.size()-1);
                            //std::cout << "    Adding : " << filters.back().getNameExtended() << std::endl;
                            //std::cout << "    ID     : " << filters.size()-1 << std::endl;
                            break;
                        case PIPELINE :
                            waitList.push_back(&tmp->pipelineLayout(i));
                            break;
                        default :
                            throw Exception("Pipeline::build - Element type not recognized for " + tmp->componentLayout(i).getNameExtended(), __FILE__, __LINE__);
                    }
                }
                waitList.pop_front();
            }

            // 2nd Step

            // Build the links

            // 3rd Step

            // Analyze memory needs
        }
        catch(std::exception& e)
        {
            Exception m("Pipeline::build - Error while building the pipeline " + getNameExtended(), __FILE__, __LINE__);
            throw m+e;
        }
    }

    void Pipeline::process(void)
    {
        // TODO
    }

    Pipeline& Pipeline::operator<<(HdlTexture& t)
    {
        if(input.size()>=getNumInputPort())
            throw Exception("Pipeline::operator<<(HdlTexture&) - Too much arguments given to Pipeline " + getNameExtended(), __FILE__, __LINE__);

        input.push_back(&t);
    }

    Pipeline& Pipeline::operator<<(ActionType a)
    {
        // Check the number of arguments given :
        if(input.size()!=getNumInputPort())
            throw Exception("Pipeline::operator<<(HdlTexture&) - Too few arguments given to Pipeline " + getNameExtended(), __FILE__, __LINE__);

        switch(a)
        {
            case Process:
                process();
            case Reset:            // After Process do Reset of the Input
                cleanInput();
                break;
            default:
                throw Exception("Pipeline::operator<<(ActionType) - Unknown action for Pipeline " + getNameExtended(), __FILE__, __LINE__);
        }
    }

    HdlTexture& Pipeline::out(int i)
    {
        checkOutputPort(i);

        if(output[i]==NULL)
            throw Exception("Pipeline::operator<<(ActionType) - Output is NULL for Pipeline " + getNameExtended() + " port ID : " + to_string(i), __FILE__, __LINE__);

        return *output[i];
    }

    HdlTexture& Pipeline::out(const std::string& portName)
    {
        int index = getInputPortID(portName);
        HdlTexture* tmp = output[index];

        if(tmp==NULL)
            throw Exception("Pipeline::operator<<(ActionType) - Output is NULL for Pipeline " + getNameExtended() + " port ID : " + portName, __FILE__, __LINE__);

        return *tmp;
    }

    Filter& Pipeline::operator[](const std::string& name)
    {
        try
        {
            __ReadOnly_PipelineLayout* p = this;
            // Parse the identification name and return a filter if so
            std::vector<std::string> tree = ObjectName::parse(name);
            //std::cout << tree[0] << std::endl;
            //std::cout << tree[1] << std::endl;
            std::string filter = tree.back();
            tree.pop_back();

            for(std::vector<std::string>::iterator it=tree.begin(); it!=tree.end(); it++)
            {
                //std::cout << "->" << *it << std::endl;
                __ReadOnly_PipelineLayout& tmp = p->pipelineLayout(*it);
                //std::cout << "Test name : " << tmp.getName() << std::endl;
                p = &tmp;
            }

            //std::cout << "Filter : " << filter << std::endl;
            //std::cout << "Num elem : " << p->getNumElements() << std::endl;
            int id = p->getElementIndex(filter);
            //std::cout << "ID1 " << id << std::endl;
            //std::cout << "INDEX : " << p->getElementID(id) << std::endl;

            if(p->getElementKind(id)!=FILTER)
                throw Exception("Pipeline::operator[] - The element " + name + " isn't a filter", __FILE__, __LINE__);

            if(p->getElementID(id)==ELEMENT_NOT_ASSOCIATED)
                throw Exception("Pipeline::operator[] - The element " + name + " exists but wasn't associated (internal error)", __FILE__, __LINE__);

            //std::cout << "Test : " << std::endl;
            //std::cout << "NameTest : " << filters[p->getElementID(id)].getNameExtended() << std::endl;

            return *filters[p->getElementID(id)];
        }
        catch(std::exception& e)
        {
            Exception m("Pipeline::operator[] - Error while processing request on " + name, __FILE__, __LINE__);
            throw m+e;
        }
    }
