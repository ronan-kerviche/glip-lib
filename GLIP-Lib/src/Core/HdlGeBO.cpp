/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : HdlGeBO.cpp                                                                               */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel Buffer Object Handle                                                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlGeBO.cpp
 * \brief   OpenGL Generic handle for Buffer Objects (VBO, PBO, VAO, ...)
 * \author  R. KERVICHE
 * \date    February 6th 2011
*/

#include "Core/Exception.hpp"
#include "Core/HdlGeBO.hpp"

using namespace Glip::CoreGL;

// Data
	bool HdlGeBO::binding[4] = {false, false, false, false};
	bool HdlGeBO::mapping[4] = {false, false, false, false};

// Functions
	/**
	\fn    HdlGeBO::HdlGeBO(int _size, GLenum infoTarget, GLenum infoUsage)
	\brief HdlGeBO Construtor.

	\param _size      Size of the buffer, in bytes.
	\param infoTarget Target kind, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
	\param infoUsage  Usage kind among GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
	**/
	HdlGeBO::HdlGeBO(int _size, GLenum infoTarget, GLenum infoUsage)
	 : size(_size)
	{
		NEED_EXTENSION(GLEW_VERSION_1_5)
		NEED_EXTENSION(GLEW_ARB_pixel_buffer_object)
		NEED_EXTENSION(GLEW_ARB_vertex_buffer_object)
		FIX_MISSING_GLEW_CALL(glGenBuffers, glGenBuffersARB)
		FIX_MISSING_GLEW_CALL(glBindBuffer, glBindBufferARB)
		FIX_MISSING_GLEW_CALL(glBufferData, glBufferDataARB)
		FIX_MISSING_GLEW_CALL(glBufferSubData, glBufferSubDataARB)
		FIX_MISSING_GLEW_CALL(glDeleteBuffers, glDeleteBuffersARB)
		FIX_MISSING_GLEW_CALL(glMapBuffer, glMapBufferARB)
		FIX_MISSING_GLEW_CALL(glUnmapBuffer, glUnmapBufferARB)

		// Generate the buffer
		glGenBuffers(1, &bufferId);

		if(bufferId==0)
		{
			GLenum err = glGetError();
			throw Exception("HdlGeBO::HdlGeBO - Buffer Object can't be created. OpenGL error " + getGLEnumNameSafe(err) + " : " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
		}

		// Bind it
		glBindBuffer(infoTarget, bufferId);

		// Allocate some space
		glBufferData(infoTarget, size, NULL, infoUsage);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlGeBO::HdlGeBO", "glBufferData()")
		#endif

		// Release point
		HdlGeBO::unbind(infoTarget);

		buildTarget = infoTarget;
		buildUsage  = infoUsage;

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlGeBO::HdlGeBO - New GeBO : " << getGLErrorDescription(glGetError()) << std::endl;
		#endif
	}

	/**
	\fn    HdlGeBO::HdlGeBO(GLuint id, int _size, GLenum infoTarget, GLenum infoUsage)
	\brief HdlGeBO Construtor.

	\param id    GLuint id of the Buffer Object to be mapped in.
	\param _size Size of the buffer, in bytes.
	\param infoTarget Target kind, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
	\param infoUsage  Usage kind among GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
	**/
	HdlGeBO::HdlGeBO(GLuint id, int _size, GLenum infoTarget, GLenum infoUsage)
	 : size(_size)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cerr << "HdlGeBO::HdlGeBO ERROR : a copy was made" << std::endl;
		#endif

		// Just copy the link
		bufferId = id;

		// And data
		buildTarget = infoTarget;
		buildUsage  = infoUsage;
	}

	HdlGeBO::~HdlGeBO(void)
	{
		// Delete the object
		glDeleteBuffers(1, &bufferId);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlGeBO::~HdlGeBO", "glDeleteBuffers()")
		#endif
	}

	/**
	\fn    unsigned int HdlGeBO::getSize(void)
	\brief Get the size of the Buffer Object.

	\return Size of the BO in bytes.
	**/
	int HdlGeBO::getSize(void)
	{
		return size;
	}

	/**
	\fn    GLuint HdlGeBO::getID(void)
	\brief Get the ID of the Buffer Object.

	\return ID of the Buffer Object
	**/
	GLuint HdlGeBO::getID(void)
	{
		return bufferId;
	}

	/**
	\fn    GLenum HdlGeBO::getTarget(void)
	\brief Get the target of the Buffer Object.

	\return Target of the Buffer Object, among : GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
	**/
	GLenum HdlGeBO::getTarget(void)
	{
		return buildTarget;
	}

	/**
	\fn    GLenum HdlGeBO::getUsage(void)
	\brief Get the usage of the Buffer Object.

	\return ID of the usage, among : GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB, GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB, GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB.
	**/
	GLenum HdlGeBO::getUsage(void)
	{
		return buildUsage;
	}

	/**
	\fn    void HdlGeBO::bind(GLenum target)
	\brief Bind the Buffer Object to target.
	\param target The target (GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB), default is the target specified for this object.
	**/
	void HdlGeBO::bind(GLenum target)
	{
		if(target==GL_NONE) target = getTarget();

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			if(binding[getIDTarget(target)])
				std::cout << "HdlGeBO::bind - Rebinding over : " << getGLEnumNameSafe(target) << std::endl;
		#endif

		glBindBuffer(target, bufferId);
		binding[getIDTarget(target)] = true;
	}

	/**
	\fn    void* HdlGeBO::map(GLenum target, GLenum access)
	\brief Map the Buffer Object into the CPU memory.

	\param access Kind of access, among GL_READ_ONLY_ARB, GL_WRITE_ONLY_ARB, GL_READ_WRITE_ARB, default can be used is target is GL_PIXEL_UNPACK_BUFFER_ARB or GL_PIXEL_PACK_BUFFER_ARB (will use respectively GL_WRITE_ONLY_ARB or GL_READ_ONLY_ARB). Will raise an exception otherwise.
	\param target Target mapping point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB, default is the target specified for this object.

	\return Pointer in CPU memory.
	**/
	void* HdlGeBO::map(GLenum access, GLenum target)
	{
		if(target==GL_NONE) target = getTarget();
		if(access==GL_NONE)
		{
			if(target==GL_PIXEL_UNPACK_BUFFER_ARB)
				access=GL_WRITE_ONLY_ARB;
			else if(target==GL_PIXEL_PACK_BUFFER_ARB)
				access=GL_READ_ONLY_ARB;
			else
				throw Exception("HdlGeBO::map - You must provide an acces type (R/W) for target " + getGLEnumNameSafe(target), __FILE__, __LINE__, Exception::GLException);
		}

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlGeBO::map - Infos : " << std::endl;
			std::cout << "    glDebug : " << std::endl;
			debugGL();
		#endif

		HdlGeBO::unmap(target);

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "    Unmap - target : " << getGLEnumNameSafe(target) << " access : " << getGLEnumNameSafe(access) << " : " << getGLErrorDescription(glGetError()) << std::endl;
		#endif

		bind(target);

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "    Bind : " << getGLErrorDescription(glGetError()) << std::endl;
			std::cout << "HdlGeBO::map - Done." << std::endl;
		#endif

		mapping[getIDTarget(target)] = true;
		void* ptr = glMapBuffer(target, access);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlGeBO::map", "glMapBuffer()")
		#endif

		return ptr;
	}

	/**
	\fn    void HdlGeBO::write(const void* data)
	\brief Write data to a Buffer Object with classical glBufferData method.
	\param data The data to write (must be the same size than the GeBO).
	**/
	void HdlGeBO::write(const void* data)
	{
		bind();

		glBufferData(getTarget(), static_cast<GLsizeiptr>(size), reinterpret_cast<const GLvoid *>(data), getUsage());

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlGeBO::write", "glBufferData()")
		#endif
	}

	/**
	\fn    void HdlGeBO::subWrite(const void* data, int size, int offset)
	\brief Write data to a Buffer Object with classical glBufferSubData method.
	\param data The data to write.
	\param size Size, in bytes, of the subset.
	\param offset Offset to apply, in bytes, before writting.
	**/
	void HdlGeBO::subWrite(const void* data, int size, int offset)
	{
		bind();

		glBufferSubData(getTarget(),  static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size),  reinterpret_cast<const GLvoid *>(data));

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlGeBO::subWrite", "glBufferSubData()")
		#endif
	}


// Static tools
	int HdlGeBO::getIDTarget(GLenum target)
	{
		switch(target)
		{
			case GL_ARRAY_BUFFER_ARB :		return 0;
			case GL_ELEMENT_ARRAY_BUFFER_ARB :	return 1;
			case GL_PIXEL_UNPACK_BUFFER_ARB :	return 2;
			case GL_PIXEL_PACK_BUFFER_ARB :		return 3;
			default :
				throw Exception("HdlGeBO::getIDTarget - Unknown target : " + getGLEnumNameSafe(target), __FILE__, __LINE__, Exception::GLException);
		}
	}

	/**
	\fn    void HdlGeBO::unbind(GLenum target)
	\brief Unbind any Buffer Object.
	\param target Target binding point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
	**/
	void HdlGeBO::unbind(GLenum target)
	{
		glBindBuffer(target, 0);
		binding[getIDTarget(target)] = false;
	}

	/**
	\fn    void HdlGeBO::unmap(GLenum target)
	\brief Unmap any Buffer Object.
	\param target Target binding point, among GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB.
	**/
	void HdlGeBO::unmap(GLenum target)
	{
		if(isMapped(target))
		{
			glUnmapBuffer(target);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlGeBO::unmap", "glUnmapBuffer()")
			#endif

			mapping[getIDTarget(target)] = false;


			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "HdlGeBO::unmap - Infos : " << std::endl;
				debugGL();
				std::cout << "    " << getGLErrorDescription(glGetError()) << std::endl;
			#endif
		}
	}

	/**
	\fn bool HdlGeBO::isBound(GLenum target)
	\brief Test if the target is bound.
	\param target The target (GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB).
	\return true if the target is bound.
	**/
	bool HdlGeBO::isBound(GLenum target)
	{
		return binding[getIDTarget(target)];
	}

	/**
	\fn bool HdlGeBO::isMapped(GLenum target)
	\brief Test if the target is mapped.
	\param target The target (GL_ARRAY_BUFFER_ARB, GL_ELEMENT_ARRAY_BUFFER_ARB, GL_PIXEL_UNPACK_BUFFER_ARB, GL_PIXEL_PACK_BUFFER_ARB).
	\return true if the target is mapped.
	**/
	bool HdlGeBO::isMapped(GLenum target)
	{
		return mapping[getIDTarget(target)];
	}
