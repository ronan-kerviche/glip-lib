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
					int	rows,
						columns;
					GLenum 	type,
						supportingType;
					bool	floatingPointType,
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

Supported types : <i>GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT</i>; <i>GL_FLOAT, GL_DOUBLE, GL_INT, GL_UNSIGNED_INT, GL_BOOL</i>.
The indexing is row major (image-like). Most of the accessors do NOT perform tests on coordinate validity.

Example :  
\code
// Get a GL_UNSIGNED_CHAR buffer for an image : 
HdlDynamicTable* table = HdlDynamicTable::build(GL_UNSIGNED_CHAR, 640, 480, 3);

// Set some data ("first pixel to medium green") : 
data->set(0,   0, 0, 0);
data->set(128, 0, 0, 1);
data->set(0,   0, 0, 2);

// Delete :
delete data;
\endcode
**/
			class GLIP_API HdlDynamicTable
			{
				private :
					int	rows,
						columns,
						slices;
					GLenum 	type;

				protected : 
					HdlDynamicTable(const GLenum& _type, int _columns, int _rows, int _slices);

				public :
					virtual ~HdlDynamicTable(void);

					const GLenum& getGLType(void) const;
					const int& getNumRows(void) const;
					const int& getNumColumns(void) const;
					const int& getNumSlices(void) const;
					int getNumElements(void) const;
					bool isInside(const int& j, const int& i, const int& d) const;
					int getIndex(const int& j, const int& i, const int& d) const;

					/**
					\fn virtual float getf(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (i, j) casted to float.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to float. Raise an exception if any error occurs.
					**/
					virtual float getf(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void setf(const float& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (i, j) from a float value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void setf(const float& value, const int& j=0, const int& i=0, const int& d=0) = 0;

					/**
					\fn virtual double get(const int& j=0, const int& i=0, const int& d=0) const = 0;
					\brief Get the value at coordinates (i, j) casted to double.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					\return The value at the requested coordinates converted to double. Raise an exception if any error occurs.
					**/
					virtual double get(const int& j=0, const int& i=0, const int& d=0) const = 0;

					/**
					\fn virtual void set(const double& value, const int& j=0, const int& i=0, const int& d=0) = 0;
					\brief Set the value at coordinates (i, j) from a double value.
					\param value The new value.
					\param j The index of the column.
					\param i The index of the row.
					\param d The index of the slice.
					**/
					virtual void set(const double& value, const int& j=0, const int& i=0, const int& d=0) = 0;

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
					virtual HdlDynamicTable& operator=(const HdlDynamicTable& cpy) = 0;

					static HdlDynamicTable* build(const GLenum& type, const int& _columns, const int& _rows, const int& _slices);
					static HdlDynamicTable* copy(const HdlDynamicTable& cpy);
			};

			template<typename T>
			class GLIP_API HdlDynamicTableSpecial : public HdlDynamicTable
			{
				private : 
					T* data;

					// Forbidden : 
					HdlDynamicTableSpecial(const HdlDynamicTableSpecial& cpy);
					HdlDynamicTableSpecial& operator=(const HdlDynamicTableSpecial& cpy);

				protected :
					HdlDynamicTableSpecial(const GLenum& _type, int _columns=1, int _rows=1, int _slices=1);

					friend class HdlDynamicTable;

				public :
					virtual ~HdlDynamicTableSpecial(void);

					float getf(const int& j=0, const int& i=0, const int& d=0) const;
					void setf(const float& value, const int& j=0, const int& i=0, const int& d=0);
					double get(const int& j=0, const int& i=0, const int& d=0) const;
					void set(const double& value, const int& j=0, const int& i=0, const int& d=0);

					const void* getPtr(void) const;
					void* getPtr(void);

					HdlDynamicTable& operator=(const HdlDynamicTable& cpy);
			};

			// Template implementation :
				template<typename T>
				HdlDynamicTableSpecial<T>::HdlDynamicTableSpecial(const GLenum& _type, int _columns, int _rows, int _slices)
				 : 	HdlDynamicTable(_type, _columns, _rows, _slices),
					data(NULL)
				{
					data = new T[getNumElements()];

					std::memset(data, 0, getNumElements()*sizeof(T));
				}

				template<typename T>
				HdlDynamicTableSpecial<T>::~HdlDynamicTableSpecial(void)
				{
					delete[] data;
					data = NULL;
				}

				template<typename T>
				float HdlDynamicTableSpecial<T>::getf(const int& j, const int& i, const int& d) const
				{
					return static_cast<float>(data[ getIndex(j, i, d) ]);
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::setf(const float& value, const int& j, const int& i, const int& d)
				{
					data[ getIndex(j, i, d) ] = static_cast<T>(value);
				}

				template<typename T>
				double HdlDynamicTableSpecial<T>::get(const int& j, const int& i, const int& d) const
				{
					return static_cast<double>(data[ getIndex(j, i, d) ]);
				}

				template<typename T>
				void HdlDynamicTableSpecial<T>::set(const double& value, const int& j, const int& i, const int& d)
				{
					data[ getIndex(j, i, d) ] = static_cast<T>(value);
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
				HdlDynamicTable& HdlDynamicTableSpecial<T>::operator=(const HdlDynamicTable& cpy)
				{
					if(cpy.getGLType()!=getGLType())
						throw Exception("HdlDynamicTableSpecial<T>::operator= - Data types do not match (target : \"" + glParamName(cpy.getGLType()) + "\"; source : \"" + glParamName(cpy.getGLType()) + "\").", __FILE__, __LINE__);
				
					std::memcpy(data, cpy.getPtr(), getNumElements()*sizeof(T));
				
					return (*this);
				}
		}
	}

#endif

