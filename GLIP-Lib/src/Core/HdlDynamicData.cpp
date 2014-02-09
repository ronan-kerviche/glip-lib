/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTexture.cpp                                                                            */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Dynamic Allocator                                                                  */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlDynamicData.cpp
 * \brief   OpenGL Dynamic Memory Allocator
 * \author  R. KERVICHE
 * \date    February 9th 2014
**/
	// Includes :
	#include "Core/HdlDynamicData.hpp"

	// Namespaces :
	using namespace Glip;
	using namespace Glip::CoreGL;

// HdlDynamicDataSpecial :
	HdlDynamicData::HdlDynamicData(const GLenum& _type, int _rows, int _columns)
	 :	type(_type),
		rows(_rows),
		columns(_columns)
	{ }

	HdlDynamicData::~HdlDynamicData(void)
	{ }

	/**
	\fn const GLenum& HdlDynamicData::getType(void) const
	\brief Get the type of the current data as the GL identifier.

	\return The GLenum corresponding to the type.
	**/
	const GLenum& HdlDynamicData::getType(void) const
	{
		return type;
	}

	/**
	\fn const int& HdlDynamicData::getNumRows(void) const
	\brief Get the number of rows of the data.
	\return The number of rows of the matrix.
	**/
	const int& HdlDynamicData::getNumRows(void) const
	{
		return rows;
	}

	/**
	\fn const int& HdlDynamicData::getNumColumns(void) const
	\brief Get the number of columns of the data.
	\return The number of columns of the matrix.
	**/
	const int& HdlDynamicData::getNumColumns(void) const
	{
		return columns;
	}

	/**
	\fn int HdlDynamicData::getNumElements(void) const
	\brief Get the number of elements of the matrix (rows times columns).
	\return The number of elements in the matrix.
	**/
	int HdlDynamicData::getNumElements(void) const
	{
		return rows*columns;
	}

	/**
	\fn bool HdlDynamicData::isInside(const int& i, const int& j) const
	\brief Test if the coordinates are for a valid element inside the matrix.
	\param i The index of the row.
	\param j The index of the column.
	\return True if the coordinates are valid, false otherwise.
	**/
	bool HdlDynamicData::isInside(const int& i, const int& j) const
	{
		return (i>=0 && i<rows) && (j>=0 && j<columns);
	}

	/**
	\fn int HdlDynamicData::getIndex(const int& i, const int& j) const
	\brief Get the linear index corresponding to these coordinates.
	\param i The index of the row.
	\param j The index of the column.
	\return The linear index.
	**/
	int HdlDynamicData::getIndex(const int& i, const int& j) const
	{
		return j * rows + i;
	}

	/**
	\fn HdlDynamicData* HdlDynamicData::build(const GLenum& type)
	\brief Build dynamic data from a GL data identifier (see supported types in main description of HdlDynamicData).
	\param type The required GL type.
	\return A data object allocated on the stack, that the user will have to delete once used. Raise an exception if any error occurs.
	**/
	HdlDynamicData* HdlDynamicData::build(const GLenum& type)
	{
		HdlDynamicData* res = NULL;

		#define GENERATE_ELM( glType, CType, _rows, _cols) \
			if(type== glType ) \
			{ \
				HdlDynamicDataSpecial< CType >* d = new HdlDynamicDataSpecial< CType >(type, _rows, _cols); \
				res = reinterpret_cast<HdlDynamicData*>(d); \
			}

			GENERATE_ELM( GL_FLOAT, 		float, 		1,	1 )
		else	GENERATE_ELM( GL_FLOAT_VEC2, 		float,		2,	1 )
		else	GENERATE_ELM( GL_FLOAT_VEC3, 		float, 		3,	1 )
		else	GENERATE_ELM( GL_FLOAT_VEC4,		float, 		4,	1 )
		else	GENERATE_ELM( GL_DOUBLE,		double,		1,	1 )
		else	GENERATE_ELM( GL_DOUBLE_VEC2,		double,		2,	1 )
		else	GENERATE_ELM( GL_DOUBLE_VEC3,		double,		3,	1 )
		else	GENERATE_ELM( GL_DOUBLE_VEC4,		double,		4,	1 )
		else	GENERATE_ELM( GL_INT,			int,		1,	1 )
		else	GENERATE_ELM( GL_INT_VEC2,		int,		2,	1 )
		else	GENERATE_ELM( GL_INT_VEC3,		int,		3,	1 )
		else	GENERATE_ELM( GL_INT_VEC4,		int,		4,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_INT,		unsigned int,	1,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_INT_VEC2,	unsigned int,	2,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_INT_VEC3,	unsigned int,	3,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_INT_VEC4,	unsigned int,	4,	1 )
		else	GENERATE_ELM( GL_BOOL,			int,		1,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC2,		int,		2,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC3,		int,		3,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC4,		int,		4,	1 )
		else	GENERATE_ELM( GL_FLOAT_MAT2,		float,		2,	2 )
		else	GENERATE_ELM( GL_FLOAT_MAT3,		float,		3,	3 )
		else	GENERATE_ELM( GL_FLOAT_MAT4,		float,		4, 	4 )
		else
			throw Exception("HdlDynamicData::build - Unknown GL type identifier : \"" + glParamName(type) + "\".", __FILE__, __LINE__);

		return res;

		#undef GENERATE_ELM
	}

// Extern : 
	std::ostream& Glip::CoreGL::operator<<(std::ostream& os, const HdlDynamicData& d)
	{
		os << '[' << glParamName(d.getType()) << "; " << d.getNumRows() << 'x' << d.getNumColumns() << "]" << std::endl;
		for(int i=0; i<d.getNumRows(); i++)
		{
			for(int j=0; j<(d.getNumColumns()-1); j++)
				os << d.get(i, j) << "\t";

			os << d.get(i, (d.getNumColumns()-1) ) << std::endl;
		}

		return os;
	}

