/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
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
	\fn size_t HdlDynamicTable::getOffset(const int& j, const int& i, const int& d) const
	\brief Get the distance in memory, from the beginning of the table to the targeted element.
	\param j The index of the column.
	\param i The index of the row.
	\param d The index of the slice.
	\return The distance to the beginning of the table, in bytes.
	**/
	size_t HdlDynamicTable::getOffset(const int& j, const int& i, const int& d) const
	{
		return static_cast<size_t>(i)*getRowSize() + static_cast<size_t>(j*getNumSlices() + d)*getElementSize();
	}
	
	/**
	\fn size_t HdlDynamicTable::getRowOffset(const int& i) const
	\brief Get the offset for the beginning of a row.
	\param i The index of the row.
	\return The distance to the beginning of the table, in bytes.
	**/
	size_t HdlDynamicTable::getRowOffset(const int& i) const
	{
		return static_cast<size_t>(i)*getRowSize();
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
	\fn void HdlDynamicTable::writeBytes(const void* value, size_t length, size_t offset)
	\brief Write a shapeless value array at the given position.
	\param value Shapeless value.
	\param length The amount of data to write (in bytes).
	\param offset Offset in the table.
	**/
	void HdlDynamicTable::writeBytes(const void* value, size_t length, size_t offset)
	{
		std::memcpy(reinterpret_cast<char*>(getPtr())+offset, value, length);
	}

	/**
	\fn void HdlDynamicTable::memset(unsigned char c)
	\brief Clear the array.
	\param c Character to be used to clear the array.
	**/
	void HdlDynamicTable::memset(unsigned char c)
	{
		std::memset(getPtr(), c, getSize());
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

