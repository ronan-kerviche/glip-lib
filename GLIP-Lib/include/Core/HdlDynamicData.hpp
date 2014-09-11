/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlTexture.hpp                                                                            */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Dynamic Allocator                                                                  */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlDynamicData.hpp
 * \brief   OpenGL Dynamic Memory Allocator
 * \author  R. KERVICHE
 * \date    February 9th 2014
**/

#ifndef __HdlDynamicDataSpecial_INCLUDE__
#define __HdlDynamicDataSpecial_INCLUDE__

	// Include :
	#include <iostream>
	#include <cstring>
	#include <limits>
	#include "Core/Exception.hpp"
	#include "Core/LibTools.hpp"
	#include "Core/OglInclude.hpp"

	namespace Glip
	{
		namespace CoreGL
		{

/**
\class HdlDynamicData
\brief Dynamic allocator for GL types (run-time resolution of type).

Supported types : <i>GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT</i>; <i>GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_DOUBLE, GL_DOUBLE_VEC2, GL_DOUBLE_VEC3, GL_DOUBLE_VEC4, GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4, GL_UNSIGNED_INT, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4, GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4</i>.
The indexing is column major (matrix-like).

Example :  
\code
// Get a GL_INT_VEC2 buffer : 
HdlDynamicData* data = HdlDynamicData::build(GL_INT_VEC2);

// Set some data : 
data->set(17, 0);
data->set(23, 1);

// Print : 
std::cout << "Vector of 2 integers : " << std::endl;
std::cout << (*data);

// Delete :
delete data;
\endcode
**/
			class GLIP_API HdlDynamicData
			{
				private : 
					const int	rows,
							columns;
					const GLenum 	type,
							supportingType;
					const bool	floatingPointType,
							integerType,
							booleanType,
							unsignedType;

					// Forbidden : 
					HdlDynamicData(const HdlDynamicData&);
					//HdlDynamicData& operator=(const HdlDynamicData& cpy);

					// Tool : 
					static GLenum getRelatedGLSupportingType(const GLenum& t);

				protected : 
					HdlDynamicData(const GLenum& _type, int _rows=1, int _columns=1);

				public : 
					virtual ~HdlDynamicData(void);

					const GLenum& getGLType(void) const;
					const GLenum& getGLSupportingType(void) const;
					const bool& isFloatingPointType(void) const;
					const bool& isIntegerType(void) const;
					const bool& isBooleanType(void) const;
					const bool& isUnsignedType(void) const;
					const int& getNumRows(void) const;
					const int& getNumColumns(void) const;
					int getNumElements(void) const;
					bool isInside(const int& i, const int& j) const;
					int getIndex(const int& i, const int& j) const;
					void getCoordinates(const int& index, int& i, int& j) const;

					/**
					\fn virtual float getf(const int& i=0, const int& j=0) const = 0;
					\brief Get the value at coordinates (i, j) casted to float.
					\param i The index of the row.
					\param j The index of the column.
					\return The value at the requested coordinates converted to float. Raise an exception if any error occurs.
					**/
					virtual float getf(const int& i=0, const int& j=0) const = 0;

					/**
					\fn virtual void setf(const float& value, const int& i=0, const int& j=0) = 0;
					\brief Set the value at coordinates (i, j) from a float value.
					\param value The new value.
					\param i The index of the row.
					\param j The index of the column.
					**/
					virtual void setf(const float& value, const int& i=0, const int& j=0) = 0;

					/**
					\fn virtual double get(const int& i=0, const int& j=0) const = 0;
					\brief Get the value at coordinates (i, j) casted to double.
					\param i The index of the row.
					\param j The index of the column.
					\return The value at the requested coordinates converted to double. Raise an exception if any error occurs.
					**/
					virtual double get(const int& i=0, const int& j=0) const = 0;

					/**
					\fn virtual void set(const double& value, const int& i=0, const int& j=0) = 0;
					\brief Set the value at coordinates (i, j) from a double value.
					\param value The new value.
					\param i The index of the row.
					\param j The index of the column.
					**/
					virtual void set(const double& value, const int& i=0, const int& j=0) = 0;

					/**
					\fn virtual const void* getPtr(void) const = 0;
					\brief Get the pointer to the data (const).
					\return The pointer to the data casted to const void*.
					**/
					virtual const void* getPtr(void) const = 0;

					/**
					\fn virtual void* getPtr(void) = 0;
					\brief Get the pointer to the data.
					\return The pointer to the data casted to void*.
					**/
					virtual void* getPtr(void) = 0;

					/**
					\fn virtual operator=(const HdlDynamicData& cpy) = 0;
					\brief Copy the value from another HdlDynamicData object. Type MUST match, will raise an exception otherwise.
					\param cpy The object to be copied.
					\return Reference to this object.
					**/
					virtual HdlDynamicData& operator=(const HdlDynamicData& cpy) = 0;

					friend std::ostream& operator<<(std::ostream& os, const HdlDynamicData& d);
	
					static HdlDynamicData* build(const GLenum& type);
					static HdlDynamicData* copy(const HdlDynamicData& cpy);
			};

			GLIP_API_FUNC std::ostream& operator<<(std::ostream& os, const HdlDynamicData& d);

			template<typename T>
			class GLIP_API HdlDynamicDataSpecial : public HdlDynamicData
			{
				private : 
					T* data;

					// Forbidden : 
					HdlDynamicDataSpecial(const HdlDynamicDataSpecial& cpy);
					HdlDynamicDataSpecial& operator=(const HdlDynamicDataSpecial& cpy);

				protected :
					HdlDynamicDataSpecial(const GLenum& _type, int _rows=1, int _columns=1);

					friend class HdlDynamicData;

				public :
					virtual ~HdlDynamicDataSpecial(void);

					float getf(const int& i=0, const int& j=0) const;
					void setf(const float& value, const int& i=0, const int& j=0);
					double get(const int& i=0, const int& j=0) const;
					void set(const double& value, const int& i=0, const int& j=0);

					const void* getPtr(void) const;
					void* getPtr(void);

					HdlDynamicData& operator=(const HdlDynamicData& cpy);
			};

			// Template implementation :
				template<typename T>
				HdlDynamicDataSpecial<T>::HdlDynamicDataSpecial(const GLenum& _type, int _rows, int _columns)
				 : 	HdlDynamicData(_type, _rows, _columns),
					data(NULL)
				{
					data = new T[getNumElements()];

					std::memset(data, 0, getNumElements()*sizeof(T));
				}

				template<typename T>
				HdlDynamicDataSpecial<T>::~HdlDynamicDataSpecial(void)
				{
					delete[] data;
					data = NULL;
				}

				template<typename T>
				float HdlDynamicDataSpecial<T>::getf(const int& i, const int& j) const
				{
					if(!isInside(i, j))
						throw Exception("HdlDynamicDataSpecial<T>::getf - Out of range.", __FILE__, __LINE__);
					else
						return static_cast<float>(data[ getIndex(i, j) ]);
				}

				template<typename T>
				void HdlDynamicDataSpecial<T>::setf(const float& value, const int& i, const int& j)
				{
					if(!isInside(i, j))
						throw Exception("HdlDynamicDataSpecial<T>::setf - Out of range.", __FILE__, __LINE__);
					else
						data[ getIndex(i, j) ] = static_cast<T>(value);
				}

				template<typename T>
				double HdlDynamicDataSpecial<T>::get(const int& i, const int& j) const
				{
					if(!isInside(i, j))
						throw Exception("HdlDynamicDataSpecial<T>::get - Out of range.", __FILE__, __LINE__);
					else
						return static_cast<double>(data[ getIndex(i, j) ]);
				}

				template<typename T>
				void HdlDynamicDataSpecial<T>::set(const double& value, const int& i, const int& j)
				{
					if(!isInside(i, j))
						throw Exception("HdlDynamicDataSpecial<T>::set - Out of range.", __FILE__, __LINE__);
					else
						data[ getIndex(i, j) ] = static_cast<T>(value);
				}

				template<typename T>
				const void* HdlDynamicDataSpecial<T>::getPtr(void) const
				{
					return reinterpret_cast<void*>(data);
				}

				template<typename T>
				void* HdlDynamicDataSpecial<T>::getPtr(void)
				{
					return reinterpret_cast<void*>(data);
				}

				template<typename T>
				HdlDynamicData& HdlDynamicDataSpecial<T>::operator=(const HdlDynamicData& cpy)
				{
					if(cpy.getGLType()!=getGLType())
						throw Exception("HdlDynamicDataSpecial<T>::operator= - Data types do not match (target : \"" + glParamName(cpy.getGLType()) + "\"; source : \"" + glParamName(cpy.getGLType()) + "\").", __FILE__, __LINE__);
				
					std::memcpy(data, cpy.getPtr(), getNumElements()*sizeof(T));
				
					return (*this);
				}

/**
\class HdlDynamicTable
\brief Dynamic table allocator for GL types (run-time resolution of type).

Supported types : <i>GL_BOOL, GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE</i>.
The indexing is <b>row major</b> and the slices are interleaved ("RGBRGBRGB..." image-like). Most of the accessors do NOT perform tests on coordinate validity.

Example :  
\code
// Get a GL_UNSIGNED_CHAR buffer for an image : 
HdlDynamicTable* table = HdlDynamicTable::build(GL_UNSIGNED_CHAR, 640, 480, 3);

// Set some data ("first pixel to medium green") : 
data->seti(0,   0, 0, 0);
data->seti(128, 0, 0, 1);
data->seti(0,   0, 0, 2);

// Delete :
delete data;
\endcode
**/
			class GLIP_API HdlDynamicTable
			{
				private :
					const int	rows,
							columns,
							slices,
							alignment;
					const bool	proxy,
							normalized;
					const GLenum 	type;

				protected : 
					HdlDynamicTable(const GLenum& _type, int _columns, int _rows, int _slices, bool _normalized=false, int _alignment=1, bool _proxy=false);

				public :
					virtual ~HdlDynamicTable(void);

					const GLenum& getGLType(void) const;
					bool isFloatingPointType(void) const;
					bool isIntegerType(void) const;
					bool isBooleanType(void) const;
					bool isUnsignedType(void) const;
					const int& getNumRows(void) const;
					const int& getNumColumns(void) const;
					const int& getNumSlices(void) const;
					int getNumElements(void) const;
					const int& getAlignment(void) const;

					/**
					\fn virtual size_t getElementSize(void) const = 0;
					\brief Get the size of one element, in bytes.
					\return The size, in bytes.
					**/
					virtual size_t getElementSize(void) const = 0;

					size_t getSliceSize(void) const;
					size_t getRowSize(void) const;
					size_t getSize(void) const;
					bool isProxy(void) const;
					bool isNormalized(void) const;
					bool isInside(const int& j, const int& i, const int& d) const;
					int getIndex(const int& j, const int& i, const int& d) const;
					size_t getPosition(const int& j, const int& i, const int& d) const;

					/**
					\fn virtual float getf(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) casted to float.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to float.
					**/
					virtual float getf(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void setf(const float& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (j, i, d) from a float value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setf(const float& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual double getd(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) casted to double.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to double.
					**/
					virtual double getd(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void setd(const double& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (j, i, d) from a double value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setd(const double& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual long long getl(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) casted to long long.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to long long.
					**/
					virtual long long getl(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void setl(const long long& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (j, i, d) from an integer value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setl(const long long& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual int geti(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) casted to int.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to int.
					**/
					virtual int geti(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void seti(const int& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (j, i, d) from an integer value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void seti(const int& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual unsigned char getb(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) casted to unsigned char.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to unsigned char.
					**/
					virtual unsigned char getb(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void setb(const unsigned char& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (j, i, d) from an unsigned char value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setb(const unsigned char& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual float getNormalized(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (j, i, d) in a normalized range.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to a normalized range.
					**/
					virtual float getNormalized(const int& j=0, const int& i=0, const int& d=0) const = 0;
					
					/**
					\fn virtual void setNormalized(const float& value, const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Set the value at coordinates (j, i, d) from a normalized value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setNormalized(const float& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual void* get(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get direct pointer to the data.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return Shapeless pointer to the data.
					**/
					virtual void* get(const int& j=0, const int& i=0, const int& d=0) const = 0;
					
					/**
					\fn virtual void set(void* value, const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Set data, assuming the same underlying type.
					\param value The new value (assumed to be of the same type).
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void set(void* value, const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual float readf(const void* position) const = 0;
					\brief Reading a floatting point value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to float.
					**/
					virtual float readf(const void* position) const = 0;

					/**
					\fn virtual void writef(const float& value, void* position) = 0;
					\brief Write a floatting point value at the given position (presumed within the table).
					\param value Floatting point value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writef(const float& value, void* position) = 0;

					/**
					\fn virtual double readd(const void* position) const = 0;
					\brief Reading a floatting point value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to double.
					**/
					virtual double readd(const void* position) const = 0;

					/**
					\fn virtual void writed(const double& value, void* position) = 0;
					\brief Write a floatting point value at the given position (presumed within the table).
					\param value Floatting point value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writed(const double& value, void* position) = 0;

					/**
					\fn virtual long long readl(const void* position) const = 0;
					\brief Reading an integer value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to long long.
					**/
					virtual long long readl(const void* position) const = 0;

					/**
					\fn virtual void writel(const long long& value, void* position) = 0;
					\brief Write an integer value at the given position (presumed within the table).
					\param value Integer value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writel(const long long& value, void* position) = 0;

					/**
					\fn virtual int readi(const void* position) const = 0;
					\brief Reading an integer value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to int.
					**/
					virtual int readi(const void* position) const = 0;

					/**
					\fn virtual void writei(const int& value, void* position) = 0;
					\brief Write an integer value at the given position (presumed within the table).
					\param value Integer value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writei(const int& value, void* position) = 0;

					/**
					\fn virtual unsigned char readb(const void* position) const = 0;
					\brief Reading an integer value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to unsigned char.
					**/
					virtual unsigned char readb(const void* position) const = 0;

					/**
					\fn virtual void writeb(const unsigned char& value, void* position) = 0;
					\brief Write an integer value at the given position (presumed within the table).
					\param value Integer value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writeb(const unsigned char& value, void* position) = 0;

					/**
					\fn virtual float readNormalized(const void* position) const = 0;
					\brief Reading an integer value from the given position (presumed within the table).
					\param position Position in the table (direct data access).
					\return Value at the given position, casted to a normalized range.
					**/
					virtual float readNormalized(const void* position) const = 0;

					/**
					\fn virtual void writeNormalized(const float& value, void* position) = 0;
					\brief Write a normalized value at the given position (presumed within the table).
					\param value Normalized value to write.
					\param position Position in the table (direct data access).
					**/
					virtual void writeNormalized(const float& value, void* position) = 0;

					/**
					\fn virtual void write(const void* value, void* position) = 0;
					\brief Write a shapeless value at the given position (presumed within the table and of the same type).
					\param value Shapeless value (of the same type as for the table).
					\param position Position in the table (direct data access).
					**/
					virtual void write(const void* value, void* position) = 0;

					void writeBytes(const void* value, size_t length, void* position);

					/**
					\fn virtual const void* getPtr(void) const = 0;
					\brief Get the pointer to the data (const).
					\return The pointer to the data casted to const void*.
					**/
					virtual const void* getPtr(void) const = 0;

					/**
					\fn virtual void* getPtr(void) = 0;
					\brief Get the pointer to the data.
					\return The pointer to the data casted to void*.
					**/
					virtual void* getPtr(void) = 0;

					/**
					\fn virtual const void* getRowPtr(int i) const = 0;
					\brief Get the pointer to the begin of the targeted row (const).
					\param i The row index.
					\return The pointer to the begin of the targeted row casted to const void*.
					**/
					virtual const void* getRowPtr(int i) const = 0;

					/**
					\fn virtual void* getRowPtr(int i) = 0;
					\brief Get the pointer to the begin of the targeted row.
					\param i The row index.
					\return The pointer to the begin of the targeted row casted to void*.
					**/
					virtual void* getRowPtr(int i) = 0;

					/**
					\fn virtual operator=(const HdlDynamicData& cpy) = 0;
					\brief Copy the value from another HdlDynamicData object. Type MUST match, will raise an exception otherwise.
					\param cpy The object to be copied.
					\return Reference to this object.
					**/
					virtual HdlDynamicTable& operator=(const HdlDynamicTable& cpy) = 0;

					static HdlDynamicTable* build(const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized=false, int _alignment=1);
					static HdlDynamicTable* buildProxy(void* buffer, const GLenum& type, const int& _columns, const int& _rows, const int& _slices, bool _normalized=false, int _alignment=1);
					static HdlDynamicTable* copy(const HdlDynamicTable& cpy);
			};

			template<typename T>
			class GLIP_API HdlDynamicTableSpecial : public HdlDynamicTable
			{
				private : 
					unsigned char* data;

					// Forbidden : 
					HdlDynamicTableSpecial(const HdlDynamicTableSpecial& cpy);
					HdlDynamicTableSpecial& operator=(const HdlDynamicTableSpecial& cpy);

				protected :
					HdlDynamicTableSpecial(const GLenum& _type, int _columns=1, int _rows=1, int _slices=1, bool _normalized=false, int _alignment=1);
					HdlDynamicTableSpecial(void* _data, const GLenum& _type, int _columns=1, int _rows=1, int _slices=1, bool _normalized=false, int _alignment=1);

					friend class HdlDynamicTable;

				public :
					virtual ~HdlDynamicTableSpecial(void);

					size_t getElementSize(void) const;

					float getf(const int& j=0, const int& i=0, const int& d=0) const;
					void setf(const float& value, const int& j=0, const int& i=0, const int& d=0);
					double getd(const int& j=0, const int& i=0, const int& d=0) const;
					void setd(const double& value, const int& j=0, const int& i=0, const int& d=0);
					long long getl(const int& j=0, const int& i=0, const int& d=0) const;
					void setl(const long long& value, const int& j=0, const int& i=0, const int& d=0);
					int geti(const int& j=0, const int& i=0, const int& d=0) const;
					void seti(const int& value, const int& j=0, const int& i=0, const int& d=0);
					unsigned char getb(const int& j=0, const int& i=0, const int& d=0) const;
					void setb(const unsigned char& value, const int& j=0, const int& i=0, const int& d=0);
					float getNormalized(const int& j=0, const int& i=0, const int& d=0) const;
					void setNormalized(const float& value, const int& j=0, const int& i=0, const int& d=0);
					void* get(const int& j=0, const int& i=0, const int& d=0) const;
					void set(void* value, const int& j=0, const int& i=0, const int& d=0) const ;

					float readf(const void* position) const;
					void writef(const float& value, void* position);
					double readd(const void* position) const;
					void writed(const double& value, void* position);
					long long readl(const void* position) const;
					void writel(const long long& value, void* position);
					int readi(const void* position) const;
					void writei(const int& value, void* position);
					unsigned char readb(const void* position) const;
					void writeb(const unsigned char& value, void* position);
					float readNormalized(const void* position) const;
					void writeNormalized(const float& value, void* position);
					void write(const void* value, void* position);

					const void* getPtr(void) const;
					void* getPtr(void);
					const void* getRowPtr(int i) const;
					void* getRowPtr(int i);

					HdlDynamicTable& operator=(const HdlDynamicTable& cpy);

					static float normalize(const T& t);
					static T denormalize(const float& t);
			};

			// Template implementation :
				template<typename T>
				HdlDynamicTableSpecial<T>::HdlDynamicTableSpecial(const GLenum& _type, int _columns, int _rows, int _slices, bool _normalized, int _alignment)
				 : 	HdlDynamicTable(_type, _columns, _rows, _slices, _normalized, _alignment),
					data(NULL)
				{
					data = new unsigned char[getSize()];

					std::memset(data, 0, getSize());
				}

				template<typename T>
				HdlDynamicTableSpecial<T>::HdlDynamicTableSpecial(void* _data, const GLenum& _type, int _columns, int _rows, int _slices, bool _normalized, int _alignment)
				 : 	HdlDynamicTable(_type, _columns, _rows, _slices, _normalized, _alignment, true),
					data(reinterpret_cast<unsigned char*>(_data))
				{ }

				template<typename T>
				HdlDynamicTableSpecial<T>::~HdlDynamicTableSpecial(void)
				{
					if(!isProxy())
					{
						delete[] data;
						data = NULL;
					}
				}

				template<typename T>
				size_t HdlDynamicTableSpecial<T>::getElementSize(void) const
				{
					return sizeof(T);
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::getf(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return static_cast<float>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return HdlDynamicTableSpecial<float>::denormalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setf(const float& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = HdlDynamicTableSpecial<float>::normalize(value);
				}

				template<typename T>
				double HdlDynamicTableSpecial<T>::getd(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return static_cast<double>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return HdlDynamicTableSpecial<double>::denormalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setd(const double& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = HdlDynamicTableSpecial<double>::normalize(value);
				}

				template<typename T>
				long long HdlDynamicTableSpecial<T>::getl(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return static_cast<long long>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return HdlDynamicTableSpecial<long long>::denormalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setl(const long long& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = HdlDynamicTableSpecial<long long>::normalize(value);
				}

				template<typename T>
				int HdlDynamicTableSpecial<T>::geti(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return static_cast<int>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return HdlDynamicTableSpecial<int>::denormalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::seti(const int& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = HdlDynamicTableSpecial<int>::normalize(value);
				}

				template<typename T>
				unsigned char HdlDynamicTableSpecial<T>::getb(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return static_cast<unsigned char>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return HdlDynamicTableSpecial<unsigned char>::denormalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setb(const unsigned char& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = HdlDynamicTableSpecial<unsigned char>::normalize(value);
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::getNormalized(const int& j, const int& i, const int& d) const
				{
					if(!isNormalized())
						return HdlDynamicTableSpecial<T>::normalize(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
					else
						return static_cast<float>(*reinterpret_cast<T*>(data + getPosition(j, i, d)));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setNormalized(const float& value, const int& j, const int& i, const int& d)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(HdlDynamicTableSpecial<float>::denormalize(value));
					else
						(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = static_cast<T>(value);
				}

				template<typename T>
				void* HdlDynamicTableSpecial<T>::get(const int& j, const int& i, const int& d) const
				{
					return reinterpret_cast<void*>(data + getPosition(j, i, d));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::set(void* value, const int& j, const int& i, const int& d) const
				{
					(*reinterpret_cast<T*>(data + getPosition(j, i, d))) = (*reinterpret_cast<T*>(value));
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::readf(const void* position) const
				{
					if(!isNormalized())
						return static_cast<float>(*reinterpret_cast<const T*>(position));
					else
						return HdlDynamicTableSpecial<float>::denormalize(*reinterpret_cast<const T*>(position));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::writef(const float& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<float>::normalize(value);
				}
		
				template<typename T>
				double HdlDynamicTableSpecial<T>::readd(const void* position) const
				{
					if(!isNormalized())
						return static_cast<double>(*reinterpret_cast<const T*>(position));
					else
						return HdlDynamicTableSpecial<double>::denormalize(*reinterpret_cast<const T*>(position));
				}
		
				template<typename T>
				void HdlDynamicTableSpecial<T>::writed(const double& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<double>::normalize(value);
				}

				template<typename T>
				long long HdlDynamicTableSpecial<T>::readl(const void* position) const
				{
					if(!isNormalized())
						return static_cast<long long>(*reinterpret_cast<const T*>(position));
					else
						return HdlDynamicTableSpecial<long long>::denormalize(*reinterpret_cast<const T*>(position));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::writel(const long long& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<long long>::normalize(value);
				}

				template<typename T>
				int HdlDynamicTableSpecial<T>::readi(const void* position) const
				{
					if(!isNormalized())
						return static_cast<int>(*reinterpret_cast<const T*>(position));
					else
						return HdlDynamicTableSpecial<int>::denormalize(*reinterpret_cast<const T*>(position));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::writei(const int& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<int>::normalize(value);
				}

				template<typename T>
				unsigned char HdlDynamicTableSpecial<T>::readb(const void* position) const
				{
					if(!isNormalized())
						return static_cast<unsigned char>(*reinterpret_cast<const T*>(position));
					else
						return HdlDynamicTableSpecial<unsigned char>::denormalize(*reinterpret_cast<const T*>(position));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::writeb(const unsigned char& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
					else
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<int>::normalize(value);
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::readNormalized(const void* position) const
				{
					if(!isNormalized())
						return HdlDynamicTableSpecial<T>::normalize(*reinterpret_cast<const T*>(position));
					else
						return static_cast<float>(*reinterpret_cast<const T*>(position));
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::writeNormalized(const float& value, void* position)
				{
					if(!isNormalized())
						(*reinterpret_cast<T*>(position)) = HdlDynamicTableSpecial<T>::denormalize(value);
					else
						(*reinterpret_cast<T*>(position)) = static_cast<T>(value);
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::write(const void* value, void* position)
				{
					(*reinterpret_cast<T*>(position)) = (*reinterpret_cast<const T*>(value));
				}

				template<typename T>
				const void* HdlDynamicTableSpecial<T>::getPtr(void) const
				{
					return reinterpret_cast<void*>(data);
				}

				template<typename T>
				void* HdlDynamicTableSpecial<T>::getPtr(void)
				{
					return reinterpret_cast<void*>(data);
				}

				template<typename T>
				const void* HdlDynamicTableSpecial<T>::getRowPtr(int i) const
				{
					return reinterpret_cast<void*>(data + i *getRowSize());
				}

				template<typename T>
				void* HdlDynamicTableSpecial<T>::getRowPtr(int i)
				{
					return reinterpret_cast<void*>(data + i *getRowSize());
				}

				template<typename T>
				HdlDynamicTable& HdlDynamicTableSpecial<T>::operator=(const HdlDynamicTable& cpy)
				{
					if(cpy.getGLType()!=getGLType())
						throw Exception("HdlDynamicTableSpecial<T>::operator= - Data types do not match (target : \"" + glParamName(cpy.getGLType()) + "\"; source : \"" + glParamName(cpy.getGLType()) + "\").", __FILE__, __LINE__);
					if(cpy.getNumRows()!=getNumRows() || cpy.getNumColumns()!=getNumColumns() || cpy.getNumSlices()!=getNumSlices())
						throw Exception("HdlDynamicTableSpecial<T>::operator= - Data format does not match.", __FILE__, __LINE__);
				
					if(cpy.getAlignment()==getAlignment())
						std::memcpy(data, cpy.getPtr(), getSize());
					else
					{
						// Scan every row : 
						for(int i=0; i<cpy.getNumRows(); i++)
							std::memcpy(getRowPtr(i), cpy.getRowPtr(i), std::min(getRowSize(), cpy.getRowSize()));
					}
				
					return (*this);
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::normalize(const T& t)
				{
					return (static_cast<float>(t) - static_cast<float>(std::numeric_limits<T>::min())) / (static_cast<float>(std::numeric_limits<T>::max()) - static_cast<float>(std::numeric_limits<T>::min()));
				}

				template<typename T>
				T HdlDynamicTableSpecial<T>::denormalize(const float& t)
				{
					return static_cast<T>(t * (static_cast<float>(std::numeric_limits<T>::max()) - static_cast<float>(std::numeric_limits<T>::min())) + static_cast<float>(std::numeric_limits<T>::min()));
				}

/**
\class HdlDynamicTableIterator
\brief Iterator-like element for HdlDynamicTable
**/
			class GLIP_API HdlDynamicTableIterator
			{
				private : 
					HdlDynamicTable& 	table;
					int 			i,
								j,
								d;
					unsigned char* 		position;

					void checkSliceUpperBorder(void);
					void checkSliceLowerBorder(void);
					void checkRowUpperBorder(void);
					void checkRowLowerBorder(void);

				public : 
					HdlDynamicTableIterator(HdlDynamicTable& _table);
					HdlDynamicTableIterator(const HdlDynamicTableIterator& copy);
					virtual ~HdlDynamicTableIterator(void);

					// Information : 
					const HdlDynamicTable& getTable(void) const;
					HdlDynamicTable& getTable(void);
					bool isValid(void) const;
					const int& getRowIndex(void) const;
					const int& getColumnIndex(void) const;
					const int& getSliceIndex(void) const;
					int getDistanceToBottomBorder(void) const;
					int getDistanceToRightBorder(void) const;					

					// Move : 
					void nextElement(void);
					void previousElement(void);
					void nextSlice(void);
					void previousSlice(void);
					void sliceBegin(void);
					void sliceEnd(void);
					void nextRow(void);
					void previousRow(void);
					void rowBegin(void);
					void rowEnd(void);
					void tableBegin(void);
					void tableEnd(void);
					void jumpTo(const int& _j, const int& _i, const int& _d);

					// Read/write : 
					const void* getPtr(void) const;
					void* getPtr(void);
					float readf(void) const;
					void writef(const float& value);
					double readd(void) const;
					void writed(const double& value);
					long long readl(void) const;
					void writel(const long long& value);
					int readi(void) const;
					void writei(const int& value);
					unsigned char readb(void) const;
					void writeb(const unsigned char& value);
					float readNormalized(void) const;
					void writeNormalized(const float& value);
					void write(const void* value);
					void writeBytes(const void* value, size_t length);
					void write(const HdlDynamicTableIterator& it);
			};
		}
	}

#endif

