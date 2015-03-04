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
	 :	rows(_rows),
		columns(_columns),
		type(_type),
		supportingType(getRelatedGLSupportingType(_type)),	
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
				throw Exception("HdlDynamicData::getRelatedGLSupportingType - Unknown GL type identifier : \"" + getGLEnumNameSafe(t) + "\" (internal error).", __FILE__, __LINE__, Exception::CoreException);
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
	\brief Get the supporting type of the current data as the GL identifier.
	\return The GLenum corresponding to the supporting type (ex. with GL_FLOAT type, the supporting will also be GL_FLOAT, and for GL_INT_VEC2 it will be GL_INT).
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
	\fn void HdlDynamicData::getCoordinates(const int& index, int& i, int& j) const
	\brief Get the coordinates from the linear index.
	\param index The linear index.
	\param i The index of the row (as output).
	\param j The index of the column (as output).
	**/
	void HdlDynamicData::getCoordinates(const int& index, int& i, int& j) const
	{
		i = index % rows;
		j = index / rows;
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
		else	GENERATE_ELM( GL_BOOL,			bool,		1,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC2,		bool,		2,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC3,		bool,		3,	1 )
		else	GENERATE_ELM( GL_BOOL_VEC4,		bool,		4,	1 )
		else	GENERATE_ELM( GL_FLOAT_MAT2,		float,		2,	2 )
		else	GENERATE_ELM( GL_FLOAT_MAT3,		float,		3,	3 )
		else	GENERATE_ELM( GL_FLOAT_MAT4,		float,		4, 	4 )
		else
			throw Exception("HdlDynamicData::build - Unknown GL type identifier : \"" + getGLEnumNameSafe(type) + "\".", __FILE__, __LINE__, Exception::CoreException);

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
		else	COPY_ELM( GL_BOOL,			bool,		1,	1 )
		else	COPY_ELM( GL_BOOL_VEC2,			bool,		2,	1 )
		else	COPY_ELM( GL_BOOL_VEC3,			bool,		3,	1 )
		else	COPY_ELM( GL_BOOL_VEC4,			bool,		4,	1 )
		else	COPY_ELM( GL_FLOAT_MAT2,		float,		2,	2 )
		else	COPY_ELM( GL_FLOAT_MAT3,		float,		3,	3 )
		else	COPY_ELM( GL_FLOAT_MAT4,		float,		4, 	4 )
		else
			throw Exception("HdlDynamicData::copy - Unknown GL type identifier : \"" + getGLEnumNameSafe(cpy.getGLType()) + "\" (internal error).", __FILE__, __LINE__, Exception::CoreException);

		return res;

		#undef COPY_ELM
	}

// Extern : 
	std::ostream& Glip::CoreGL::operator<<(std::ostream& os, const HdlDynamicData& d)
	{
		os << '[' << getGLEnumName(d.getGLType()) << "; " << d.getNumRows() << 'x' << d.getNumColumns() << "]" << std::endl;
		for(int i=0; i<d.getNumRows(); i++)
		{
			for(int j=0; j<(d.getNumColumns()-1); j++)
				os << d.get(i, j) << "\t";

			os << d.get(i, (d.getNumColumns()-1) ) << std::endl;
		}

		return os;
	}

// HdlDynamicTable :
	HdlDynamicTable::HdlDynamicTable(const GLenum& _type, int _columns, int _rows, int _slices, bool _normalized, int _alignment, bool _proxy)
	 :	rows(_rows),
		columns(_columns),
		slices(_slices),	
		alignment(_alignment),
		proxy(_proxy),
		normalized(_normalized),	
		type(_type)	
	{ }

	HdlDynamicTable::~HdlDynamicTable(void)
	{ }

	/**
	\fn const GLenum& HdlDynamicTable::getGLType(void) const
	\brief Get the type of the current data as the GL identifier.
	\return The GLenum corresponding to the type.
	**/
	const GLenum& HdlDynamicTable::getGLType(void) const
	{
		return type;
	}
	
	/**
	\fn bool HdlDynamicTable::isFloatingPointType(void) const
	\brief Test if the data is floatting point.
	\return True if the data is floatting point.
	**/
	bool HdlDynamicTable::isFloatingPointType(void) const
	{
		return (type==GL_FLOAT) || (type==GL_DOUBLE);
	}

	/**
	\fn bool HdlDynamicTable::isIntegerType(void) const
	\brief Test if the data is integer.
	\return True if the data is integer.
	**/
	bool HdlDynamicTable::isIntegerType(void) const
	{
		return (type==GL_BYTE) && (type==GL_UNSIGNED_BYTE) && (type==GL_SHORT) && (type==GL_UNSIGNED_SHORT) && (type==GL_INT) && (type==GL_UNSIGNED_INT);
	}

	/**
	\fn bool HdlDynamicTable::isFloatingPointType(void) const
	\brief Test if the data is boolean.
	\return True if the data is boolean.
	**/
	bool HdlDynamicTable::isBooleanType(void) const
	{
		return (type==GL_BOOL);
	}

	/**
	\fn bool HdlDynamicTable::isFloatingPointType(void) const
	\brief Test if the data is unsigned.
	\return True if the data is unsigned.
	**/
	bool HdlDynamicTable::isUnsignedType(void) const
	{
		return (type==GL_UNSIGNED_BYTE) || (type==GL_UNSIGNED_SHORT) || (type==GL_UNSIGNED_INT);
	}

	/**
	\fn const int& HdlDynamicTable::getNumRows(void) const
	\brief Get the number of rows of the data.
	\return The number of rows of the table.
	**/
	const int& HdlDynamicTable::getNumRows(void) const
	{
		return rows;
	}

	/**
	\fn const int& HdlDynamicTable::getNumColumns(void) const
	\brief Get the number of columns of the data.
	\return The number of columns of the table.
	**/
	const int& HdlDynamicTable::getNumColumns(void) const
	{
		return columns;
	}

	/**
	\fn const int& HdlDynamicTable::getNumSlices(void) const
	\brief Get the number of slices of the data.
	\return The number of slices of the table.
	**/
	const int& HdlDynamicTable::getNumSlices(void) const
	{
		return slices;
	}

	/**
	\fn int HdlDynamicTable::getNumElements(void) const
	\brief Get the number of elements of the table (rows times columns times slices).
	\return The number of elements in the table.
	**/
	int HdlDynamicTable::getNumElements(void) const
	{
		return rows*columns*slices;
	}

	/**
	\fn const int& HdlDynamicTable::getAlignment(void) const
	\brief Get the data alignment.
	\return The row alignment in memory, in bytes.
	**/
	const int& HdlDynamicTable::getAlignment(void) const
	{
		return alignment;
	}

	/**
	\fn size_t HdlDynamicTable::getSliceSize(void) const
	\brief Get the size of one slice, in bytes.
	\return The size of one slice, or HdlDynamicTable::getNumSlices times HdlDynamicTable::getElementSize, in bytes.
	**/
	size_t HdlDynamicTable::getSliceSize(void) const
	{
		return static_cast<size_t>(getNumSlices())*getElementSize();
	}

	/**
	\fn size_t HdlDynamicTable::getRowSize(void) const
	\brief Get the size of one row, in bytes.
	\return The size of one row, in bytes, and accounting for the alignment.
	**/
	size_t HdlDynamicTable::getRowSize(void) const
	{
		return static_cast<size_t>(getNumColumns()*getNumSlices()*getElementSize() + (getAlignment()-1)) & ~static_cast<size_t>(getAlignment() - 1);
	}

	/**
	\fn size_t HdlDynamicTable::getSize(void) const
	\brief Get the size of whole table, in bytes.
	\return The size of the whole table, in bytes, and accounting for the alignment.
	**/
	size_t HdlDynamicTable::getSize(void) const
	{
		return static_cast<size_t>(getNumRows()) * getRowSize();
	}

	/**
	\fn size_t HdlDynamicTable::getPosition(const int& j, const int& i, const int& d) const
	\brief Get the distance in memory, from the beginning of the table to the targeted element.
	\param j The index of the column.
	\param i The index of the row.
	\param d The index of the slice.
	\return The distance to the beginning of the table in bytes.
	**/
	size_t HdlDynamicTable::getPosition(const int& j, const int& i, const int& d) const
	{
		return static_cast<size_t>(i)*getRowSize() + static_cast<size_t>(j*getNumSlices() + d)*getElementSize();
	}

	/**
	\fn bool HdlDynamicTable::isProxy(void) const
	\brief Test if the table is actually a proxy.
	\return True if this table is a proxy to some data.
	**/
	bool HdlDynamicTable::isProxy(void) const
	{
		return proxy;
	}

	/**
	\fn bool HdlDynamicTable::isNormalized(void) const
	\brief Test if the table is normalized.
	\return True if this table is normalized (floatting point data in the range [0, 1]).
	**/
	bool HdlDynamicTable::isNormalized(void) const
	{
		return normalized;
	}

	/**
	\fn bool HdlDynamicTable::isInside(const int& i, const int& j, const int& d) const
	\brief Test if the coordinates are for a valid element inside the table.
	\param j The index of the column.
	\param i The index of the row.
	\param d The index of the slice.
	\return True if the coordinates are valid, false otherwise.
	**/
	bool HdlDynamicTable::isInside(const int& j, const int& i, const int& d) const
	{
		return (i>=0 && i<rows) && (j>=0 && j<columns) && (d>=0 && d<slices);
	}

	/**
	\fn int HdlDynamicTable::getIndex(const int& i, const int& j, const int& d) const
	\brief Get the linear index corresponding to these coordinates.
	\param j The index of the column.
	\param i The index of the row.
	\param d The index of the slice.
	\return The linear index.
	**/
	int HdlDynamicTable::getIndex(const int& j, const int& i, const int& d) const
	{
		return (i * columns + j) * slices + d;
	}

	/**
	\fn void HdlDynamicTable::writeBytes(const void* value, size_t length, void* position)
	\brief Write a shapeless value array at the given position.
	\param value Shapeless value.
	\param length The amount of data to write (in bytes).
	\param position Position in the table (direct data access).
	**/
	void HdlDynamicTable::writeBytes(const void* value, size_t length, void* position)
	{
		std::memcpy(position, value, length);
	}

	/**
	\fn HdlDynamicTable* HdlDynamicTable::build(const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized, int _alignment)
	\brief Build dynamic data from a GL data identifier (see supported types in main description of HdlDynamicData).
	\param type The required GL type.
	\param _columns The number of columns of the table.
	\param _rows The number of rows of the table.
	\param _slices The number of slices of the table.
	\param _normalized True if the data is normalized.
	\param _alignment Data alignment (per row, should be either 1, 4, or 8).
	\return A data object allocated on the stack, that the user will have to delete once used. Raise an exception if any error occurs.
	**/
	HdlDynamicTable* HdlDynamicTable::build(const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized, int _alignment)
	{
		HdlDynamicTable* res = NULL;

		_normalized = _normalized && ((type==GL_FLOAT) || (type==GL_DOUBLE));

		#define GENERATE_ELM(glType, CType) \
			if(type== glType ) \
			{ \
				HdlDynamicTableSpecial< CType >* d = new HdlDynamicTableSpecial< CType >(type, _columns, _rows, _slices, _normalized, _alignment); \
				res = reinterpret_cast<HdlDynamicTable*>(d); \
			}

		#define ERROR_ELM(glType) \
			if(type== glType ) \
				throw Exception("HdlDynamicTable::build - Unable to build a table of type \"" + getGLEnumNameSafe(type) + "\" : Illegal type.", __FILE__, __LINE__, Exception::CoreException);
			

			GENERATE_ELM( 	GL_BYTE,		char)
		else	GENERATE_ELM( 	GL_UNSIGNED_BYTE,	unsigned char)
		else	GENERATE_ELM( 	GL_SHORT,		short)
		else	GENERATE_ELM( 	GL_UNSIGNED_SHORT,	unsigned short)
		else	GENERATE_ELM( 	GL_FLOAT, 		float)
		else	ERROR_ELM( 	GL_FLOAT_VEC2)
		else	ERROR_ELM( 	GL_FLOAT_VEC3)
		else	ERROR_ELM( 	GL_FLOAT_VEC4)
		else	GENERATE_ELM( 	GL_DOUBLE,		double)
		else	ERROR_ELM( 	GL_DOUBLE_VEC2)
		else	ERROR_ELM(	GL_DOUBLE_VEC3)
		else	ERROR_ELM( 	GL_DOUBLE_VEC4)
		else	GENERATE_ELM( 	GL_INT,			int)
		else	ERROR_ELM( 	GL_INT_VEC2)
		else	ERROR_ELM( 	GL_INT_VEC3)
		else	ERROR_ELM( 	GL_INT_VEC4)
		else	GENERATE_ELM( 	GL_UNSIGNED_INT,	unsigned int)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC2)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC3)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC4)
		else	GENERATE_ELM( 	GL_BOOL,		bool)
		else	ERROR_ELM( 	GL_BOOL_VEC2)
		else	ERROR_ELM( 	GL_BOOL_VEC3)
		else	ERROR_ELM( 	GL_BOOL_VEC4)
		else	ERROR_ELM( 	GL_FLOAT_MAT2)
		else	ERROR_ELM(  	GL_FLOAT_MAT3)
		else	ERROR_ELM( 	GL_FLOAT_MAT4)
		else
			throw Exception("HdlDynamicTable::build - Unknown GL type identifier : \"" + getGLEnumNameSafe(type) + "\".", __FILE__, __LINE__, Exception::CoreException);

		return res;

		#undef GENERATE_ELM
		#undef ERROR_ELM
	}

	/**
	\fn HdlDynamicTable* HdlDynamicTable::buildProxy(void* buffer, const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized, int _alignment)
	\brief Build dynamic data proxy from a GL data identifier and a buffer pointer (see supported types in main description of HdlDynamicData).
	\param buffer The original data (will not be deleted, the user must guarantee that this object is destroyed when the original data is).
	\param type The required GL type.
	\param _columns The number of columns of the table.
	\param _rows The number of rows of the table.
	\param _slices The number of slices of the table.
	\param _normalized True if the data is normalized.
	\param _alignment Data alignment (per row, should be either 1, 4, or 8).
	\return A data object allocated on the stack, that the user will have to delete once used. Raise an exception if any error occurs.
	**/
	HdlDynamicTable* HdlDynamicTable::buildProxy(void* buffer, const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized, int _alignment)
	{
		HdlDynamicTable* res = NULL;

		_normalized = _normalized && ((type==GL_FLOAT) || (type==GL_DOUBLE));

		#define GENERATE_ELM(glType, CType) \
			if(type== glType ) \
			{ \
				HdlDynamicTableSpecial< CType >* d = new HdlDynamicTableSpecial< CType >(buffer, type, _columns, _rows, _slices, _normalized, _alignment); \
				res = reinterpret_cast<HdlDynamicTable*>(d); \
			}

		#define ERROR_ELM(glType) \
			if(type== glType ) \
				throw Exception("HdlDynamicTable::build - Unable to build a table of type \"" + getGLEnumNameSafe(type) + "\" : Illegal type.", __FILE__, __LINE__, Exception::CoreException);
			

			GENERATE_ELM( 	GL_BYTE,		char)
		else	GENERATE_ELM( 	GL_UNSIGNED_BYTE,	unsigned char)
		else	GENERATE_ELM( 	GL_SHORT,		short)
		else	GENERATE_ELM( 	GL_UNSIGNED_SHORT,	unsigned short)
		else	GENERATE_ELM( 	GL_FLOAT, 		float)
		else	ERROR_ELM( 	GL_FLOAT_VEC2)
		else	ERROR_ELM( 	GL_FLOAT_VEC3)
		else	ERROR_ELM( 	GL_FLOAT_VEC4)
		else	GENERATE_ELM( 	GL_DOUBLE,		double)
		else	ERROR_ELM( 	GL_DOUBLE_VEC2)
		else	ERROR_ELM(	GL_DOUBLE_VEC3)
		else	ERROR_ELM( 	GL_DOUBLE_VEC4)
		else	GENERATE_ELM( 	GL_INT,			int)
		else	ERROR_ELM( 	GL_INT_VEC2)
		else	ERROR_ELM( 	GL_INT_VEC3)
		else	ERROR_ELM( 	GL_INT_VEC4)
		else	GENERATE_ELM( 	GL_UNSIGNED_INT,	unsigned int)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC2)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC3)
		else	ERROR_ELM( 	GL_UNSIGNED_INT_VEC4)
		else	GENERATE_ELM( 	GL_BOOL,		bool)
		else	ERROR_ELM( 	GL_BOOL_VEC2)
		else	ERROR_ELM( 	GL_BOOL_VEC3)
		else	ERROR_ELM( 	GL_BOOL_VEC4)
		else	ERROR_ELM( 	GL_FLOAT_MAT2)
		else	ERROR_ELM(  	GL_FLOAT_MAT3)
		else	ERROR_ELM( 	GL_FLOAT_MAT4)
		else
			throw Exception("HdlDynamicTable::build - Unknown GL type identifier : \"" + getGLEnumNameSafe(type) + "\".", __FILE__, __LINE__, Exception::CoreException);

		return res;

		#undef GENERATE_ELM
		#undef ERROR_ELM
	}

	/**
	\fn HdlDynamicData* HdlDynamicTable::copy(const HdlDynamicData& cpy)
	\brief Copy dynamic data.
	\param cpy The original element to be copied.
	\return A data object allocated on the stack, that the user will have to delete once used. Raise an exception if any error occurs.
	**/
	HdlDynamicTable* HdlDynamicTable::copy(const HdlDynamicTable& cpy)
	{
		HdlDynamicTable* res = NULL;

		#define COPY_ELM( glType, CType) \
			if(cpy.getGLType()== glType ) \
			{ \
				HdlDynamicTableSpecial< CType >* d = new HdlDynamicTableSpecial< CType >(cpy.getGLType(), cpy.getNumColumns(), cpy.getNumRows(), cpy.getNumSlices()); \
				std::memcpy(d->getPtr(), cpy.getPtr(), cpy.getNumElements() * sizeof( CType )); \
				res = reinterpret_cast<HdlDynamicTable*>(d); \
			}

			COPY_ELM( GL_BYTE,			char)
		else	COPY_ELM( GL_UNSIGNED_BYTE,		unsigned char)
		else	COPY_ELM( GL_SHORT,			short)
		else	COPY_ELM( GL_UNSIGNED_SHORT,		unsigned short)
		else	COPY_ELM( GL_FLOAT, 			float)
		else	COPY_ELM( GL_DOUBLE,			double)
		else	COPY_ELM( GL_INT,			int)
		else	COPY_ELM( GL_UNSIGNED_INT,		unsigned int)
		else	COPY_ELM( GL_BOOL,			int)
		else
			throw Exception("HdlDynamicTable::copy - Unknown GL type identifier : \"" + getGLEnumNameSafe(cpy.getGLType()) + "\" (internal error).", __FILE__, __LINE__, Exception::CoreException);

		return res;

		#undef COPY_ELM
	}

	namespace Glip
	{
		namespace CoreGL
		{
			template<>
			float HdlDynamicTableSpecial<float>::normalize(const float& t)
			{
				return t;
			}

			template<>
			float HdlDynamicTableSpecial<double>::normalize(const double& t)
			{
				return t;
			}

			template<>
			float HdlDynamicTableSpecial<float>::denormalize(const float& t)
			{
				return t;
			}

			template<>
			double HdlDynamicTableSpecial<double>::denormalize(const float& t)
			{
				return t;
			}
		}
	}

// HdlDynamicTableIterator :
	/**
	\fn HdlDynamicTableIterator::HdlDynamicTableIterator(HdlDynamicTable& _table)
	\brief Create an iterator over a table. Table must exist at all time. The iterator is initialized to the beginning of the table.
	\param _table The table to run over.
	**/
	HdlDynamicTableIterator::HdlDynamicTableIterator(HdlDynamicTable& _table)
	 :	table(_table),
		i(0),
		j(0),
		d(0),
		position(reinterpret_cast<unsigned char*>(_table.getPtr()))
	{ }

	/**
	\fn HdlDynamicTableIterator::HdlDynamicTableIterator(const HdlDynamicTableIterator& copy)
	\brief Copy constructor.
	\param copy Original iterator.
	**/
	HdlDynamicTableIterator::HdlDynamicTableIterator(const HdlDynamicTableIterator& copy)
	 :	table(copy.table),
		i(copy.i),
		j(copy.j),
		d(copy.d),
		position(copy.position)
	{ }

	HdlDynamicTableIterator::~HdlDynamicTableIterator(void)
	{ }

	void HdlDynamicTableIterator::checkSliceUpperBorder(void)
	{
		if(d>=table.getNumSlices())
		{
			d = 0;
			j++;
		}
	}

	void HdlDynamicTableIterator::checkSliceLowerBorder(void)
	{
		if(d<0)
		{
			d = table.getNumSlices()-1;
			j--;
		}
	}

	void HdlDynamicTableIterator::checkRowUpperBorder(void)
	{
		if(j>=table.getNumColumns())
		{
			j = 0;
			i++;

			position = reinterpret_cast<unsigned char*>(table.getRowPtr(i));
		}
	}

	void HdlDynamicTableIterator::checkRowLowerBorder(void)
	{
		if(j<0)
		{
			j = table.getNumColumns()-1;
			i--;

			position = reinterpret_cast<unsigned char*>(table.getRowPtr(i)) + static_cast<size_t>(table.getNumColumns() * table.getNumSlices() - 1) * table.getElementSize();
		}
	}
	
	/**
	\fn const HdlDynamicTable& HdlDynamicTableIterator::getTable(void) const
	\brief Access the parent table.
	\return A constant reference to the parent table of this iterator.
	**/
	const HdlDynamicTable& HdlDynamicTableIterator::getTable(void) const
	{
		return table;
	}

	/**
	\fn HdlDynamicTable& HdlDynamicTableIterator::getTable(void)
	\brief Access the parent table.
	\return A reference to the parent table of this iterator.
	**/
	HdlDynamicTable& HdlDynamicTableIterator::getTable(void)
	{
		return table;
	}

	/**
	\fn bool HdlDynamicTableIterator::isValid(void) const
	\brief Check if the iterator is still valid.
	\return True if the iterator is valid (can be used for I/O operation).
	**/
	bool HdlDynamicTableIterator::isValid(void) const
	{
		return table.isInside(j, i, d);
	}

	/**
	\fn const int& HdlDynamicTableIterator::getRowIndex(void) const
	\brief Get the current row index.
	\return The current row index (might not be inside the table).
	**/
	const int& HdlDynamicTableIterator::getRowIndex(void) const
	{
		return i;
	}

	/**
	\fn const int& HdlDynamicTableIterator::getColumnIndex(void) const
	\brief Get the current column index.
	\return The current column index (might not be inside the table).
	**/
	const int& HdlDynamicTableIterator::getColumnIndex(void) const
	{
		return j;
	}

	/**
	\fn const int& HdlDynamicTableIterator::getSliceIndex(void) const
	\brief Get the current slice index.
	\return The current slice index (might not be inside the table).
	**/
	const int& HdlDynamicTableIterator::getSliceIndex(void) const
	{
		return d;
	}

	/**
	\fn int HdlDynamicTableIterator::getDistanceToBottomBorder(void) const
	\brief Get the number of elements between the current position and the bottom border (this element is included).
	\return Height minus current row index.
	**/
	int HdlDynamicTableIterator::getDistanceToBottomBorder(void) const
	{
		return table.getNumRows() - getRowIndex();
	}

	/**
	\fn int HdlDynamicTableIterator::getDistanceToRightBorder(void) const
	\brief Get the number of elements between the current position and the right border (this element is included).
	\return Width minus current column index.
	**/
	int HdlDynamicTableIterator::getDistanceToRightBorder(void) const
	{
		return table.getNumColumns() - getColumnIndex();
	}

	/**
	\fn void HdlDynamicTableIterator::nextElement(void)
	\brief Move to the next element.
	**/
	void HdlDynamicTableIterator::nextElement(void)
	{
		d++;
		position += table.getElementSize();
		
		checkSliceUpperBorder();
		checkRowUpperBorder();
	}

	/**
	\fn void HdlDynamicTableIterator::previousElement(void)
	\brief Move to the previous element.
	**/
	void HdlDynamicTableIterator::previousElement(void)
	{
		d--;
		position -= table.getElementSize();

		checkSliceLowerBorder();
		checkRowLowerBorder();
	}

	/**
	\fn void HdlDynamicTableIterator::nextSlice(void)
	\brief Move to the beginning of the next slice.
	**/
	void HdlDynamicTableIterator::nextSlice(void)
	{
		position += static_cast<size_t>(table.getNumSlices()-d) * table.getElementSize();
		d = 0;
		j++;
		
		checkSliceUpperBorder();
		checkRowUpperBorder();
	}

	/**
	\fn void HdlDynamicTableIterator::previousSlice(void)
	\brief Move to the beginning of the previous slice.
	**/
	void HdlDynamicTableIterator::previousSlice(void)
	{
		position -= static_cast<size_t>(table.getNumSlices() + d) * table.getElementSize();
		d = 0;
		j--;
		
		checkSliceLowerBorder();
		checkRowLowerBorder();
	}

	/**
	\fn void HdlDynamicTableIterator::sliceBegin(void)
	\brief Move to the beginning of the current slice.
	**/
	void HdlDynamicTableIterator::sliceBegin(void)
	{
		position -= static_cast<size_t>(d) * table.getElementSize();
	}

	/**
	\fn void HdlDynamicTableIterator::sliceEnd(void)
	\brief Move to the end of the current slice (last element).
	**/
	void HdlDynamicTableIterator::sliceEnd(void)
	{
		position += static_cast<size_t>(table.getNumSlices()-d-1) * table.getElementSize();
	}

	/**
	\fn void HdlDynamicTableIterator::nextRow(void)
	\brief Move to the beginning of the next row.
	**/
	void HdlDynamicTableIterator::nextRow(void)
	{
		d = 0;
		j = 0;
		i++;
		
		position = reinterpret_cast<unsigned char*>(table.getRowPtr(i));
	}

	/**
	\fn void HdlDynamicTableIterator::previousRow(void)
	\brief Move to the beginning of the previous row
	**/
	void HdlDynamicTableIterator::previousRow(void)
	{
		d = 0;
		j = 0;
		i--;
		
		position = reinterpret_cast<unsigned char*>(table.getRowPtr(i));
	}

	/**
	\fn void HdlDynamicTableIterator::rowBegin(void)
	\brief Move to the beginning of the current row.
	**/
	void HdlDynamicTableIterator::rowBegin(void)
	{
		d = 0;
		j = 0;
		
		position = reinterpret_cast<unsigned char*>(table.getRowPtr(i));
	}

	/**
	\fn void HdlDynamicTableIterator::rowEnd(void)
	\brief Move to the end of the current row (last element).
	**/
	void HdlDynamicTableIterator::rowEnd(void)
	{
		d = table.getNumSlices()-1;
		j = table.getNumColumns()-1;
		
		position = reinterpret_cast<unsigned char*>(table.getRowPtr(i)) + static_cast<size_t>(table.getNumColumns() * table.getNumSlices() - 1) * table.getElementSize();
	}

	/**
	\fn void HdlDynamicTableIterator::tableBegin(void)
	\brief Move to the beginning of the table.
	**/
	void HdlDynamicTableIterator::tableBegin(void)
	{
		i = 0;
		j = 0;
		d = 0;

		position = reinterpret_cast<unsigned char*>(table.getPtr());
	}

	/**
	\fn void HdlDynamicTableIterator::tableEnd(void)
	\brief Move to the end of the table (last element).
	**/
	void HdlDynamicTableIterator::tableEnd(void)
	{
		i = table.getNumRows()-1;
		j = table.getNumColumns()-1;
		d = table.getNumSlices()-1;

		position = reinterpret_cast<unsigned char*>(table.getPtr()) + table.getSize() - table.getElementSize();
	}

	/**
	\fn void HdlDynamicTableIterator::jumpTo(const int& _j, const int& _i, const int& _d)
	\brief Jump to a different element.
	\param _j The index of the column.
	\param _i The index of the row.
	\param _d The index of the slice.
	**/
	void HdlDynamicTableIterator::jumpTo(const int& _j, const int& _i, const int& _d)
	{
		j = _j;
		i = _i;
		d = _d;
		position = reinterpret_cast<unsigned char*>(table.getPtr()) + table.getPosition(_j, _i, _d);	
	}

	/**
	\fn const void* HdlDynamicTableIterator::getPtr(void) const
	\brief Get the pointer to the current position.
	\return Pointer to the current position.
	**/
	const void* HdlDynamicTableIterator::getPtr(void) const
	{
		return reinterpret_cast<void*>(position);
	}

	/**
	\fn void* HdlDynamicTableIterator::getPtr(void)
	\brief Get the pointer to the current position.
	\return Pointer to the current position.
	**/
	void* HdlDynamicTableIterator::getPtr(void)
	{
		return reinterpret_cast<void*>(position);
	}

	/**
	\fn float HdlDynamicTableIterator::readf(void) const
	\brief Read the current value as a floatting point value
	\return The current value casted as a float.
	**/
	float HdlDynamicTableIterator::readf(void) const
	{
		return table.readf(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writef(const float& value)
	\brief Write a floatting point value to the current position.
	\param value The floatting point value to write.
	**/
	void HdlDynamicTableIterator::writef(const float& value)
	{
		table.writef(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn double HdlDynamicTableIterator::readd(void) const
	\brief Read the current value as a floatting point value
	\return The current value casted as a double.
	**/
	double HdlDynamicTableIterator::readd(void) const
	{
		return table.readd(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writed(const double& value)
	\brief Write a floatting point value to the current position.
	\param value The floatting point value to write.
	**/
	void HdlDynamicTableIterator::writed(const double& value)
	{
		table.writed(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn long long HdlDynamicTableIterator::readl(void) const
	\brief Read the current value as an integer value
	\return The current value casted as a long long.
	**/
	long long HdlDynamicTableIterator::readl(void) const
	{
		return table.readl(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writel(const long long& value)
	\brief Write an integer value to the current position.
	\param value The integer value to write.
	**/
	void HdlDynamicTableIterator::writel(const long long& value)
	{
		table.writel(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn int HdlDynamicTableIterator::readi(void) const
	\brief Read the current value as an integer value
	\return The current value casted as an int.
	**/
	int HdlDynamicTableIterator::readi(void) const
	{
		return table.readi(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writei(const int& value)
	\brief Write an integer value to the current position.
	\param value The integer value to write.
	**/
	void HdlDynamicTableIterator::writei(const int& value)
	{
		table.writei(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn unsigned char HdlDynamicTableIterator::readb(void) const
	\brief Read the current value as an integer value
	\return The current value casted as an int.
	**/
	unsigned char HdlDynamicTableIterator::readb(void) const
	{
		return table.readb(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writeb(const unsigned char& value)
	\brief Write an integer value to the current position.
	\param value The integer value to write.
	**/
	void HdlDynamicTableIterator::writeb(const unsigned char& value)
	{
		table.writeb(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn float HdlDynamicTableIterator::readNormalized(void) const
	\brief Read the current value as a normalized value. 
	\return he current value casted in a normalized range.
	**/
	float HdlDynamicTableIterator::readNormalized(void) const
	{
		return table.readNormalized(reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writeNormalized(const float& value)
	\brief Write a normalized value to the current position.
	\param value The normalized value to write.
	**/
	void HdlDynamicTableIterator::writeNormalized(const float& value)
	{
		table.writeNormalized(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::write(const void* value)
	\brief Write shapless data to the current position (assuming same type as the table).
	\param value The shapeless value to write. 
	**/
	void HdlDynamicTableIterator::write(const void* value)
	{
		table.write(value, reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::writeBytes(void* value, size_t length)
	\brief Write a shapeless value array at the given position.
	\param value Shapeless value array.
	\param length The amount of data to write (in bytes).
	**/
	void HdlDynamicTableIterator::writeBytes(const void* value, size_t length)
	{
		table.writeBytes(value, length, reinterpret_cast<void*>(position));
	}

	/**
	\fn void HdlDynamicTableIterator::write(const HdlDynamicTableIterator& it)
	\brief Write the content from another iterator.
	\param it Other iterator to copy from.
	**/
	void HdlDynamicTableIterator::write(const HdlDynamicTableIterator& it)
	{
		// Test elligibility to fast copy : 
		if(it.getTable().getGLType()==getTable().getGLType())
			write(it.getPtr());
		else
			writeNormalized(it.readNormalized());
	}

