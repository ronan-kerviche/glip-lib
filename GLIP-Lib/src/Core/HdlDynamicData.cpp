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
		supportingType(getRelatedGLSupportingType(_type)),
		rows(_rows),
		columns(_columns),
		floatingPointType(supportingType==GL_FLOAT || supportingType==GL_DOUBLE),
		integerType(supportingType==GL_BYTE || supportingType==GL_UNSIGNED_BYTE || supportingType==GL_SHORT || supportingType==GL_UNSIGNED_SHORT || supportingType==GL_INT || supportingType==GL_UNSIGNED_INT),
		booleanType(supportingType==GL_BOOL),
		unsignedType(supportingType==GL_UNSIGNED_BYTE || supportingType==GL_UNSIGNED_SHORT || supportingType==GL_UNSIGNED_INT)
	{ }

	HdlDynamicData::~HdlDynamicData(void)
	{ }

	GLenum HdlDynamicData::getRelatedGLSupportingType(const GLenum& t)
	{
		switch(t)
		{
			case GL_BYTE :
							return GL_BYTE;
			case GL_UNSIGNED_BYTE :
							return GL_UNSIGNED_BYTE;
			case GL_SHORT :
							return GL_SHORT;
			case GL_UNSIGNED_SHORT :
							return GL_UNSIGNED_SHORT;
			case GL_FLOAT :
			case GL_FLOAT_VEC2 :
			case GL_FLOAT_VEC3 :
			case GL_FLOAT_VEC4 :
							return GL_FLOAT;
			case GL_DOUBLE :
			case GL_DOUBLE_VEC2 :
			case GL_DOUBLE_VEC3 :
			case GL_DOUBLE_VEC4 :
							return GL_DOUBLE;
			case GL_INT :
			case GL_INT_VEC2 :
			case GL_INT_VEC3 :
			case GL_INT_VEC4 :
							return GL_INT;
			case GL_UNSIGNED_INT :
			case GL_UNSIGNED_INT_VEC2 :
			case GL_UNSIGNED_INT_VEC3 :
			case GL_UNSIGNED_INT_VEC4 :
							return GL_UNSIGNED_INT;
			case GL_BOOL :
			case GL_BOOL_VEC2 :
			case GL_BOOL_VEC3 :
			case GL_BOOL_VEC4 :
							return GL_BOOL;
			case GL_FLOAT_MAT2 : 
			case GL_FLOAT_MAT3 :
			case GL_FLOAT_MAT4 :
							return GL_FLOAT;
			default :
				throw Exception("HdlDynamicData::getRelatedGLSupportingType - Unknown GL type identifier : \"" + glParamName(t) + "\" (internal error).", __FILE__, __LINE__);
		}
	}

	/**
	\fn const GLenum& HdlDynamicData::getGLType(void) const
	\brief Get the type of the current data as the GL identifier.
	\return The GLenum corresponding to the type.
	**/
	const GLenum& HdlDynamicData::getGLType(void) const
	{
		return type;
	}

	/**
	\fn const GLenum& HdlDynamicData::getGLSupportingType(void) const
	\brief Get the type of the current data as the GL identifier.
	\return The GLenum corresponding to the type.
	**/
	const GLenum& HdlDynamicData::getGLSupportingType(void) const
	{
		return supportingType;
	}

	/**
	\fn const bool& HdlDynamicData::isFloatingPointType(void) const
	\brief Test if the content is of floating point type.
	\return True if the content is of floating point type.
	**/
	const bool& HdlDynamicData::isFloatingPointType(void) const
	{
		return floatingPointType;
	}

	/**
	\fn const bool& HdlDynamicData::isIntegerType(void) const
	\brief Test if the content is of integer type.
	\return True if the content is of integer type.
	**/
	const bool& HdlDynamicData::isIntegerType(void) const
	{
		return integerType;
	}

	/**
	\fn const bool& HdlDynamicData::isBooleanType(void) const
	\brief Test if the content is of boolean type.
	\return True if the content is of boolean type.
	**/
	const bool& HdlDynamicData::isBooleanType(void) const
	{
		return booleanType;
	}

	/**
	\fn const bool& HdlDynamicData::isUnsignedType(void) const
	\brief Test if the content is of unsigned type.
	\return True if the content is of unsigned type.
	**/
	const bool& HdlDynamicData::isUnsignedType(void) const
	{
		return unsignedType;
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

			GENERATE_ELM( GL_BYTE,			char,		1,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_BYTE,		unsigned char,	1,	1 )
		else	GENERATE_ELM( GL_SHORT,			short,		1,	1 )
		else	GENERATE_ELM( GL_UNSIGNED_SHORT,	unsigned short,	1,	1 )
		else	GENERATE_ELM( GL_FLOAT, 		float, 		1,	1 )
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

	/**
	\fn HdlDynamicData* HdlDynamicData::copy(const HdlDynamicData& cpy)
	\brief Copy dynamic data.
	\param cpy The original element to be copied.
	\return A data object allocated on the stack, that the user will have to delete once used. Raise an exception if any error occurs.
	**/
	HdlDynamicData* HdlDynamicData::copy(const HdlDynamicData& cpy)
	{
		HdlDynamicData* res = NULL;

		#define COPY_ELM( glType, CType, _rows, _cols) \
			if(cpy.getGLType()== glType ) \
			{ \
				HdlDynamicDataSpecial< CType >* d = new HdlDynamicDataSpecial< CType >(cpy.getGLType(), _rows, _cols); \
				std::memcpy(d->getPtr(), cpy.getPtr(), _rows * _cols * sizeof( CType )); \
				res = reinterpret_cast<HdlDynamicData*>(d); \
			}

			COPY_ELM( GL_BYTE,			char,		1,	1 )
		else	COPY_ELM( GL_UNSIGNED_BYTE,		unsigned char,	1,	1 )
		else	COPY_ELM( GL_SHORT,			short,		1,	1 )
		else	COPY_ELM( GL_UNSIGNED_SHORT,		unsigned short,	1,	1 )
		else	COPY_ELM( GL_FLOAT, 			float, 		1,	1 )
		else	COPY_ELM( GL_FLOAT_VEC2, 		float,		2,	1 )
		else	COPY_ELM( GL_FLOAT_VEC3, 		float, 		3,	1 )
		else	COPY_ELM( GL_FLOAT_VEC4,		float, 		4,	1 )
		else	COPY_ELM( GL_DOUBLE,			double,		1,	1 )
		else	COPY_ELM( GL_DOUBLE_VEC2,		double,		2,	1 )
		else	COPY_ELM( GL_DOUBLE_VEC3,		double,		3,	1 )
		else	COPY_ELM( GL_DOUBLE_VEC4,		double,		4,	1 )
		else	COPY_ELM( GL_INT,			int,		1,	1 )
		else	COPY_ELM( GL_INT_VEC2,			int,		2,	1 )
		else	COPY_ELM( GL_INT_VEC3,			int,		3,	1 )
		else	COPY_ELM( GL_INT_VEC4,			int,		4,	1 )
		else	COPY_ELM( GL_UNSIGNED_INT,		unsigned int,	1,	1 )
		else	COPY_ELM( GL_UNSIGNED_INT_VEC2,		unsigned int,	2,	1 )
		else	COPY_ELM( GL_UNSIGNED_INT_VEC3,		unsigned int,	3,	1 )
		else	COPY_ELM( GL_UNSIGNED_INT_VEC4,		unsigned int,	4,	1 )
		else	COPY_ELM( GL_BOOL,			int,		1,	1 )
		else	COPY_ELM( GL_BOOL_VEC2,			int,		2,	1 )
		else	COPY_ELM( GL_BOOL_VEC3,			int,		3,	1 )
		else	COPY_ELM( GL_BOOL_VEC4,			int,		4,	1 )
		else	COPY_ELM( GL_FLOAT_MAT2,		float,		2,	2 )
		else	COPY_ELM( GL_FLOAT_MAT3,		float,		3,	3 )
		else	COPY_ELM( GL_FLOAT_MAT4,		float,		4, 	4 )
		else
			throw Exception("HdlDynamicData::copy - Unknown GL type identifier : \"" + glParamName(cpy.getGLType()) + "\" (internal error).", __FILE__, __LINE__);

		return res;

		#undef COPY_ELM
	}

// Extern : 
	std::ostream& Glip::CoreGL::operator<<(std::ostream& os, const HdlDynamicData& d)
	{
		os << '[' << glParamName(d.getGLType()) << "; " << d.getNumRows() << 'x' << d.getNumColumns() << "]" << std::endl;
		for(int i=0; i<d.getNumRows(); i++)
		{
			for(int j=0; j<(d.getNumColumns()-1); j++)
				os << d.get(i, j) << "\t";

			os << d.get(i, (d.getNumColumns()-1) ) << std::endl;
		}

		return os;
	}

