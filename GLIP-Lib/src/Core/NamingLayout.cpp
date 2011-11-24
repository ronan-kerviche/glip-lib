#include "NamingLayout.hpp"
#include "Exception.hpp"

using namespace Glip::CorePipeline;
using namespace Glip;

// ObjectName
    ObjectName::ObjectName(const std::string& nm, const std::string& tp)
        : portID(NO_PORT), name(nm), type(tp)
    { }
    ObjectName::ObjectName(const std::string& name, int port)
        : portID(port), name(name), type("")
    { }

    ObjectName::ObjectName(const ObjectName& c)
        : portID(c.portID), name(c.name), type(c.type)
    { }

    void ObjectName::checkName(void)
    {
        #define EXCEPTION(s) throw Exception("ObjectName - Name " + name + " contains illegal symbol : " + s, __FILE__, __LINE__);
        if( name.find(SEPARATOR)!=std::string::npos )
            EXCEPTION(SEPARATOR)
        if( name.find(BEGIN_TYPE)!=std::string::npos )
            EXCEPTION(BEGIN_TYPE)
        if( name.find(END_TYPE)!=std::string::npos )
            EXCEPTION(END_TYPE)
        if( name.find(BEGIN_PORT)!=std::string::npos )
            EXCEPTION(BEGIN_PORT)
        if( name.find(END_PORT)!=std::string::npos )
            EXCEPTION(END_PORT)
        #undef EXCEPTION
    }

    void ObjectName::checkType(void)
    {
        #define EXCEPTION(s) throw Exception("ObjectName - Type " + type + " contains illegal symbol : " + s, __FILE__, __LINE__);
        if( type.find(SEPARATOR)!=std::string::npos )
            EXCEPTION(SEPARATOR)
        if( type.find(BEGIN_TYPE)!=std::string::npos )
            EXCEPTION(BEGIN_TYPE)
        if( type.find(END_TYPE)!=std::string::npos )
            EXCEPTION(END_TYPE)
        if( type.find(BEGIN_PORT)!=std::string::npos )
            EXCEPTION(BEGIN_PORT)
        if( type.find(END_PORT)!=std::string::npos )
            EXCEPTION(END_PORT)
        #undef EXCEPTION
    }

    void ObjectName::setName(const std::string& s)
    {
        name = s;
    }

    const std::string& ObjectName::getName(void) const
    {
        return name;
    }

    std::string ObjectName::getNameExtended(void) const
    {
        if(portID==NO_PORT)
            return name + BEGIN_TYPE + to_string(type) + END_TYPE;
        else
            return name + BEGIN_PORT + to_string(portID) + END_PORT;
    }

    const std::string& ObjectName::getType(void) const
    {
        return type;
    }

    void ObjectName::removeQualifier(std::string& str, const std::string& beginQual, const std::string& endQual)
    {

        // Remove any qualifiers
        size_t beg = 0,
               end = 0;
        while( (beg = str.find(beginQual, end))!=std::string::npos )
        {
            if( (end = str.find(endQual, beg))==std::string::npos )
                throw Exception("ObjectName::parse - Unbalanced " + beginQual + " in " + str, __FILE__, __LINE__);
            else
                str.erase(beg, end-beg);
        }

        // Check for remaining qualificators :
        if( str.find(endQual, end)!=std::string::npos )
            throw Exception("ObjectName::parse - Unbalanced " + endQual + " in " + str, __FILE__, __LINE__);
    }

    std::vector<std::string> ObjectName::parse(const std::string& field)
    {
        if(field.empty())
            throw Exception("ObjectName::parse - Can't parse an empty string", __FILE__, __LINE__);

        std::string BP, EP, BT, ET, prc;
        BP = BEGIN_PORT;
        EP = END_PORT;
        BT = BEGIN_TYPE;
        ET = END_TYPE;
        prc = field;
        removeQualifier(prc, BP, EP);
        removeQualifier(prc, BT, ET);

        // Now parse the solutions
        std::vector<std::string> res;
        std::string sep = SEPARATOR;
        size_t beg = 0,
               end = 0;

        do
        {
            end = prc.find(sep, beg);
            res.push_back(prc.substr(beg, end-beg));
            if(end!=std::string::npos)
                beg = end + sep.length();
            else
                beg = end;
        }while(beg<prc.length());

        return res;
    }

// Tools
    int Glip::CorePipeline::getIndexByNameFct(const std::string& str, const int ln, ObjectName& (*f)(int, const void*), const void* obj)
    {
        if(ln==0)
            throw Exception("getIndexByNameFct (Vector) - Vector of items is empty", __FILE__, __LINE__);

        for(int i=0; i<ln; i++)
        {
            if(f(i, obj).getName()==str)
                return i;
        }

        throw Exception("getIndexByNameFct (Vector) - No Object named : " + str, __FILE__, __LINE__);
    }
