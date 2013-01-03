/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : NamingLayout.cpp                                                                          */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Filter object                                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    NamingLayout.cpp
 * \brief   Object name tools
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include "NamingLayout.hpp"
#include "Exception.hpp"

using namespace Glip::CorePipeline;
using namespace Glip;

// ObjectName
	/**
	\fn ObjectName::ObjectName(const std::string& nm, const std::string& tp)
	\brief ObjectName Constructor for a pipeline element.
	\param nm Name of the element.
	\param tp Typename of the element.
	**/
	ObjectName::ObjectName(const std::string& nm, const std::string& tp)
	 : name(nm), type(tp), portID(NO_PORT)
	{
		checkName();
		checkType();
	}

	/**
	\fn ObjectName::ObjectName(const std::string& name, int port)
	\brief ObjectName Constructor for a port.
	\param name Name of the element.
	\param port Index of the port.
	**/
	ObjectName::ObjectName(const std::string& name, int port)
	 : name(name), type(""), portID(port)
	{
		checkName();
	}

	/**
	\fn ObjectName::ObjectName(const ObjectName& c)
	\brief ObjectName Constructor.
	\param c Copy.
	**/
	ObjectName::ObjectName(const ObjectName& c)
	 : name(c.name), type(c.type), portID(c.portID)
	{ }

	ObjectName::~ObjectName(void)
	{
		name.clear();
		type.clear();
	}

	/**
	\fn void ObjectName::checkName(void)
	\brief Check the validity of the name. Raise an exception if any errors occur.
	**/
	void ObjectName::checkName(void)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "ObjectName::checkName - Checking name : \"" << name << '\"' << std::endl;
		#endif

		#define EXCEPTION(s) throw Exception("ObjectName - Name \"" + name + "\" contains illegal symbol : " + s, __FILE__, __LINE__);
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
		if( name.find(' ')!=std::string::npos )
			EXCEPTION("<space>")
		if( name.find('\t')!=std::string::npos )
			EXCEPTION("<tabulation>")
		if( name.find('\n')!=std::string::npos )
			EXCEPTION("<newline>")
		if( name.find(';')!=std::string::npos )
			EXCEPTION(";")
		if( name.find('*')!=std::string::npos )
			EXCEPTION("*")
		if( name.find('?')!=std::string::npos )
			EXCEPTION("?")
		if( name.find('+')!=std::string::npos )
			EXCEPTION("+")
		if( name.find('-')!=std::string::npos )
			EXCEPTION("-")
		if( name.find('/')!=std::string::npos )
			EXCEPTION("/")
		if( name.find('\\')!=std::string::npos )
			EXCEPTION("\\")
		#undef EXCEPTION
	}

	/**
	\fn void ObjectName::checkType(void)
	\brief Check the validity of the typename. Raise an exception if any errors occur.
	**/
	void ObjectName::checkType(void)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "ObjectName::checkName - Checking type : \"" << name << '\"' << std::endl;
		#endif

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
		if( name.find(' ')!=std::string::npos )
			EXCEPTION("<space>")
		if( name.find('\t')!=std::string::npos )
			EXCEPTION("<tabulation>")
		if( name.find('\n')!=std::string::npos )
			EXCEPTION("<newline>")
		if( name.find(';')!=std::string::npos )
			EXCEPTION(";")
		if( name.find('*')!=std::string::npos )
			EXCEPTION("*")
		if( name.find('?')!=std::string::npos )
			EXCEPTION("?")
		if( name.find('+')!=std::string::npos )
			EXCEPTION("+")
		if( name.find('-')!=std::string::npos )
			EXCEPTION("-")
		if( name.find('/')!=std::string::npos )
			EXCEPTION("/")
		if( name.find('\\')!=std::string::npos )
			EXCEPTION("\\")
		#undef EXCEPTION
	}

	/**
	\fn void ObjectName::setName(const std::string& s)
	\brief Change the name of the object.
	\param s The new name.
	**/
	void ObjectName::setName(const std::string& s)
	{
		name = s;
		checkName();
	}

	/**
	\fn const std::string& ObjectName::getName(void) const
	\brief Get the name of the object.
	\return The name of the object.
	**/
	const std::string& ObjectName::getName(void) const
	{
		return name;
	}

	/**
	\fn std::string ObjectName::getNameExtended(void) const
	\brief Get the name and the typename of the object in the same string.
	\return The name and typename of the object.
	**/
	std::string ObjectName::getNameExtended(void) const
	{
		if(portID==NO_PORT)
			return std::string(name + BEGIN_TYPE + to_string(type) + END_TYPE);
		else
			return std::string(name + BEGIN_PORT + to_string(portID) + END_PORT);
	}

	/**
	\fn const std::string& ObjectName::getType(void) const
	\brief Get the typename of the object.
	\return The typename of the object.
	**/
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

	/**
	\fn std::vector<std::string> ObjectName::parse(const std::string& field)
	\brief Parse the input string in order to navigate into a pipeline architecture as a tree.
	\param field The input sequence.
	\return A vector of string representing the consecutive elements (from trunk to leaves).
	**/
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
	/**
	\fn bool Glip::CorePipeline::doesElementExistByNameFct(const std::string& str, const int ln, ObjectName& (*f)(int, const void*), const void* obj)
	\brief Get the Index of an object return by a selection function.
	\param str The name of the object.
	\param ln The number of selected items.
	\param f The selection function.
	\param obj The object handling the data.
	\return The index of the desired object or raise an exception if any errors occur.
	**/
	bool Glip::CorePipeline::doesElementExistByNameFct(const std::string& str, const int ln, ObjectName& (*f)(int, const void*), const void* obj)
	{
		if(ln==0)
			return false;

		for(int i=0; i<ln; i++)
		{
			if(f(i, obj).getName()==str)
				return true;
		}

		return false;
	}

	/**
	\fn int Glip::CorePipeline::getIndexByNameFct(const std::string& str, const int ln, ObjectName& (*f)(int, const void*), const void* obj)
	\brief Get the Index of an object return by a selection function.
	\param str The name of the object.
	\param ln The number of selected items.
	\param f The selection function.
	\param obj The object handling the data.
	\return The index of the desired object or raise an exception if any errors occur.
	**/
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
