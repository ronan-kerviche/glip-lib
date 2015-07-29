/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : HdlShader.cpp                                                                             */
/*     Original Date : August 7th 2010                                                                           */
/*                                                                                                               */
/*     Description   : OpenGL Pixel and Fragment Shader Handle                                                   */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    HdlShader.cpp
 * \brief   OpenGL Pixel and Fragment Shader Handle
 * \author  R. KERVICHE
 * \date    August 7th 2010
**/

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "Core/Exception.hpp"
#include "Core/HdlShader.hpp"
#include "devDebugTools.hpp"

using namespace Glip::CoreGL;

// HdlShader :
	/**
	\fn    HdlShader::HdlShader(GLenum _type, const ShaderSource& src)
	\brief HdlShader constructor.
	\param _type The kind of shader it will be : GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER.
	\param src   The source code used.
	**/
	HdlShader::HdlShader(GLenum _type, const ShaderSource& src)
	 : 	ShaderSource(src),
		type(_type)
	{
		NEED_EXTENSION(GLEW_VERSION_2_0)
		//NEED_EXTENSION(GLEW_ARB_shader_objects)
	
		switch(type)
		{
			case GL_FRAGMENT_SHADER :
				
				NEED_EXTENSION(GLEW_ARB_fragment_shader)
				NEED_EXTENSION(GLEW_ARB_fragment_program)
				break;
			case GL_VERTEX_SHADER : 
				NEED_EXTENSION(GLEW_ARB_vertex_shader)
				NEED_EXTENSION(GLEW_ARB_vertex_program)				
				break;
			case GL_COMPUTE_SHADER : 
				NEED_EXTENSION(GLEW_ARB_compute_shader)
				break;
			case GL_TESS_CONTROL_SHADER :
			case GL_TESS_EVALUATION_SHADER :
				NEED_EXTENSION(GLEW_ARB_tessellation_shader)
				break;
			case GL_GEOMETRY_SHADER :
				NEED_EXTENSION(GLEW_ARB_geometry_shader4)
				break;
			default : 
				throw Exception("HdlShader::HdlShader - Unknown shader type : \"" + getGLEnumNameSafe(type) + "\".", __FILE__, __LINE__, Exception::GLException);
		}
		

		if(!src.requiresCompatibility())
			FIX_MISSING_GLEW_CALL(glBindFragDataLocation, glBindFragDataLocationEXT)

		FIX_MISSING_GLEW_CALL(glCompileShader, glCompileShaderARB)
		FIX_MISSING_GLEW_CALL(glLinkProgram, glLinkProgramARB)
		FIX_MISSING_GLEW_CALL(glGetUniformLocation, glGetUniformLocationARB)
		FIX_MISSING_GLEW_CALL(glUniform1i,  glUniform1iARB)
		FIX_MISSING_GLEW_CALL(glUniform2i,  glUniform2iARB)
		FIX_MISSING_GLEW_CALL(glUniform3i,  glUniform3iARB)
		FIX_MISSING_GLEW_CALL(glUniform4i,  glUniform4iARB)
		FIX_MISSING_GLEW_CALL(glUniform1f,  glUniform1fARB)
		FIX_MISSING_GLEW_CALL(glUniform2f,  glUniform2fARB)
		FIX_MISSING_GLEW_CALL(glUniform3f,  glUniform3fARB)
		FIX_MISSING_GLEW_CALL(glUniform4f,  glUniform4fARB)

		const GLenum listCorrectEnum[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER};
		if(!belongsToGLEnums(type, listCorrectEnum))
			throw Exception("HdlShader::HdlShader - Invalid enum : " + getGLEnumNameSafe(type) + ".", __FILE__, __LINE__, Exception::GLException);

		// create the shader
		shader = glCreateShader(type);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glCreateShader()")
		#endif

		if(shader==0)
		{
			GLenum err = glGetError();
			throw Exception("HdlShader::HdlShader - Unable to create the shader from " + getSourceName() + ". OpenGL error : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
		}

		// send the source code
		const char* data = getSourceCstr();
		glShaderSource(shader, 1, (const GLchar**)&data, NULL);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glShaderSource()")
		#endif

		// compile the shader source code
		glCompileShader(shader);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glCompileShader()")
		#endif

		// check the compilation
		GLint compile_status = GL_TRUE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status)")
		#endif

		if(compile_status != GL_TRUE)
		{
			GLint logSize;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize)")
			#endif

			char *log = new char[logSize+1]; // +1 <=> '/0'
			memset(log, 0, logSize+1);

			glGetShaderInfoLog(shader, logSize, &logSize, log);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glGetShaderInfoLog()")
			#endif

			log[logSize] = 0;

			Exception detailedLog = this->errorLog(std::string(log));

			delete[] log;

			// Clean other resources :
			glDeleteShader(shader);

			throw detailedLog;
		}
	}

	HdlShader::~HdlShader(void)
	{
		glDeleteShader(shader);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::~HdlShader", "glDeleteShader()")
		#endif
	}

	/**
	\fn    GLuint HdlShader::getShaderID(void) const
	\brief Returns the ID of the shader for OpenGL.
	\return The ID handled by the driver.
	**/
	GLuint HdlShader::getShaderID(void) const
	{
		return shader;
	}

	/**
	\fn    GLenum HdlShader::getType(void) const
	\brief Returns the type of the shader for OpenGL 
	
	Return the type of the shader, among : GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER.

	\return The type of this shader.
	**/
	GLenum HdlShader::getType(void) const
	{
		return type;
	}

// HdlProgram :
	/**
	\fn HdlProgram::HdlProgram(void)
	\brief HdlProgram constructor.
	**/
	HdlProgram::HdlProgram(void)
	 : 	valid(false),
		program(0)
	{
		std::memset(attachedShaders, 0, numShaderTypes*sizeof(GLuint));

		// create the program
		program = glCreateProgram();

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::HdlProgram(void)", "glCreateProgram()")
		#endif

		if(program==0)
		{
			GLenum err = glGetError();
			throw Exception("HdlProgram::HdlProgram - Program can't be created. Last OpenGL error : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
		}
	}

	HdlProgram::~HdlProgram(void)
	{
		for(int k=0; k<numShaderTypes; k++)
		{
			if(attachedShaders[k]!=0)
			{
				glDetachShader(program, attachedShaders[k]);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("HdlProgram::~HdlProgram", "glDetachShader(program, attachedShaders[k])")
				#endif
			}
		}

		glDeleteProgram(program);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::~HdlProgram", "glDeleteProgram(program)")
		#endif
	}

	/**
	\fn    bool HdlProgram::isValid(void) const
	\brief Check if the program is valid.
	\return True if the Program is valid, false otherwise.
	**/
	bool HdlProgram::isValid(void) const
	{
		return (program!=0) && valid;
	}

	/**
	\fn    void HdlProgram::updateShader(const HdlShader& shader, bool linkNow)
	\brief Change a shader in the program.
	\param shader The shader to add.
	\param linkNow Link the program now.
	**/
	void HdlProgram::updateShader(const HdlShader& shader, bool linkNow)
	{
		unsigned int k = HandleOpenGL::getShaderTypeIndex(shader.getType());

		// Dettach previous : 
		if(attachedShaders[k]!=0)
		{
			glDetachShader(program, attachedShaders[k]);
			attachedShaders[k] = false;

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlProgram::update", "glDetachShader(program, attachedShaders[" + getGLEnumNameSafe(shaderType) + "])")
			#endif
		}

		attachedShaders[k] = shader.getShaderID();
		glAttachShader(program, attachedShaders[k]);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::update", "glAttachShader(program, attachedShaders[" + getGLEnumNameSafe(shaderType) + "])")
		#endif

		// Link the program
		if(linkNow) 
			link();
	}

	/**
	\fn    bool HdlProgram::link(void)
	\brief Link the program.
	\return False in case of failure, true otherwise.

	<b>WARNING</b> : This step might remove all the uniform values previously set.
	**/
	void HdlProgram::link(void)
	{
		valid = false;

		// Link them
		glLinkProgram(program);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::link", "glLinkProgram()")
		#endif

		// Look for some error during the linking
		GLint link_status = GL_TRUE;
		glGetProgramiv(program, GL_LINK_STATUS, &link_status);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::link", "glGetProgramiv(program, GL_LINK_STATUS, &link_status)")
		#endif

		if(link_status!=GL_TRUE)
		{
			// get the log
			GLint logSize;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlProgram::link", "glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize)")
			#endif

			char *log = new char[logSize+1]; // +1 <=> '/0'
			memset(log, 0, logSize+1);

			glGetProgramInfoLog(program, logSize, &logSize, log);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlProgram::link", "glGetProgramInfoLog()")
			#endif

			log[logSize] = 0;

			std::string logstr(log);
			delete[] log;

			throw Exception("HdlProgram::link - Error during Program linking : \n" + logstr, __FILE__, __LINE__, Exception::ClientShaderException);
		}
		else
		{
			use();

			// Clean :
			activeUniforms.clear();
			activeTypes.clear();

			// Update available uniforms of the following types :
			const GLenum interestTypes[] = {GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_DOUBLE, GL_DOUBLE_VEC2, GL_DOUBLE_VEC3, GL_DOUBLE_VEC4, GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4, GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_DOUBLE_MAT2, GL_DOUBLE_MAT3, GL_DOUBLE_MAT4, GL_UNSIGNED_INT};
			const int numAllowedTypes = sizeof(interestTypes) / sizeof(GLenum);

			// Get number of uniforms :
			GLint numUniforms = 0;
			glGetProgramiv(	program, GL_ACTIVE_UNIFORMS, &numUniforms);

			const int maxLength = 1024;
			char buffer[maxLength];
			GLenum type;
			GLint actualSize, actualSizeName;
			for(int k=0; k<numUniforms; k++)
			{
				glGetActiveUniform( program, k, maxLength-1, &actualSizeName, &actualSize, &type, buffer);

				if(std::find(interestTypes, interestTypes + numAllowedTypes, type)!=interestTypes + numAllowedTypes)
				{
 					activeUniforms.push_back(buffer);
					activeTypes.push_back(type);
				}
			}

			valid = true;
		}		
	}

	/**
	\fn    void HdlProgram::use(void)
	\brief Start using the program with OpenGL.
	**/
	void HdlProgram::use(void)
	{
		glUseProgram(program);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::use", "glUseProgram()")
		#endif
	}

	/**
	\fn    const std::vector<std::string>& HdlProgram::getUniformsNames(void) const
	\brief Get access to the list of uniform variables names of supported types managed by the program (GL based). 

	Some variable reported by the Driver might not be accessible. You can use HdlProgram::isUniformVariableValid to detect such variables.
	
	\return Access to a string based vector.
	**/
	const std::vector<std::string>& HdlProgram::getUniformsNames(void) const
	{
		return activeUniforms;
	}

	/**
	\fn    const std::vector<GLenum>& HdlProgram::getUniformsTypes(void) const
	\brief Get access to the list of uniform variables types corresponding to the names provided by HdlProgram::getUniformsNames (GL based).

	Some variable reported by the Driver might not be accessible. You can use HdlProgram::isUniformVariableValid to detect such variables.

	\return Access to a GLenum based vector, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	**/
	const std::vector<GLenum>& HdlProgram::getUniformsTypes(void) const
	{
		return activeTypes;
	}

	/**
	\fn    void HdlProgram::setFragmentLocation(const std::string& fragName, int frag)
	\brief Link the name of a fragment output variable to a fragment unit.
	\param fragName Name of the fragment output variable.
	\param frag     Index of the desired fragment unit.

	Note that the program should be linked after this call in order to push the changes.
	**/
	void HdlProgram::setFragmentLocation(const std::string& fragName, int frag)
	{
		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlProgram::setFragmentLocation - FragName : " << fragName << std::endl;
		#endif
		glBindFragDataLocation(program, frag, fragName.c_str());

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::setFragmentLocation - Error while setting fragment location \"" + fragName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum type, int v0, int v1=0, int v2=0, int v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/
	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum type, unsigned int v0, unsigned int v1=0, unsigned int v2=0, unsigned int v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/
	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum type, float v0, float v1=0, float v2=0, float v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/

	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum t, int* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/
	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum t, unsigned int* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/
	/**
	\fn    void HdlProgram::setVar(const std::string& varName, GLenum t, float* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/

	#define GENsetVarA( argT1, argT2, argT3)   \
		void HdlProgram::setVar(const std::string& varName, GLenum t, argT1 v0, argT1 v1, argT1 v2, argT1 v3) \
		{ \
			use(); \
			GLint loc = glGetUniformLocation(program, varName.c_str()); \
			\
			if(loc==-1) \
				throw Exception("HdlProgram::setVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException); \
			\
			switch(t) \
			{ \
				case GL_##argT2 : 		glUniform1##argT3 (loc, v0);			break; \
				case GL_##argT2##_VEC2 : 	glUniform2##argT3 (loc, v0, v1);		break; \
				case GL_##argT2##_VEC3 : 	glUniform3##argT3 (loc, v0, v1, v2);		break; \
				case GL_##argT2##_VEC4 : 	glUniform4##argT3 (loc, v0, v1, v2, v3);	break; \
				default :			throw Exception("HdlProgram::setVar - Unknown variable type or type mismatch for \"" + getGLEnumNameSafe(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException); \
			} \
			\
			GLenum err = glGetError(); \
			if(err!=GL_NO_ERROR) \
				throw Exception("HdlProgram::setVar - An error occurred when loading data of type \"" + getGLEnumNameSafe(t) + "\" in variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException); \
		} \

	#define GENsetVarB( argT1, argT2, argT3)   \
		void HdlProgram::setVar(const std::string& varName, GLenum t, argT1* v) \
		{ \
			use(); \
			GLint loc = glGetUniformLocation(program, varName.c_str()); \
			\
			if(loc==-1) \
				throw Exception("HdlProgram::setVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException); \
			\
			switch(t) \
			{ \
				case GL_##argT2 : 		glUniform1##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC2 : 	glUniform2##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC3 : 	glUniform3##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC4 : 	glUniform4##argT3##v(loc, 1, v);			break; \
				default :			throw Exception("HdlProgram::setVar - Unknown variable type or type mismatch for \"" + getGLEnumNameSafe(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException); \
			} \
			\
			GLenum err = glGetError(); \
			if(err!=GL_NO_ERROR) \
				throw Exception("HdlProgram::setVar - An error occurred when loading data of type \"" + getGLEnumNameSafe(t) + "\" in variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException); \
		}

	GENsetVarA( int, INT, i)
	GENsetVarB( int, INT, i)
	GENsetVarA( unsigned int, UNSIGNED_INT, ui)
	GENsetVarB( unsigned int, UNSIGNED_INT, ui)
	GENsetVarA( float, FLOAT, f)

	#undef GENsetVar

	// Last one, with matrices :
	void HdlProgram::setVar(const std::string& varName, GLenum t, float* v)
	{
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::setVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		switch(t)
		{
			case GL_FLOAT : 	glUniform1fv(loc, 1, v);			break;
			case GL_FLOAT_VEC2 : 	glUniform2fv(loc, 1, v);			break;
			case GL_FLOAT_VEC3 : 	glUniform3fv(loc, 1, v);			break;
			case GL_FLOAT_VEC4 : 	glUniform4fv(loc, 1, v);			break;
			case GL_FLOAT_MAT2 :	glUniformMatrix2fv(loc, 1, GL_FALSE, v);	break;
			case GL_FLOAT_MAT3 :	glUniformMatrix3fv(loc, 1, GL_FALSE, v);	break;
			case GL_FLOAT_MAT4 :	glUniformMatrix4fv(loc, 1, GL_FALSE, v);	break;
			default :		throw Exception("HdlProgram::setVar - Unknown variable type or type mismatch for \"" + getGLEnumNameSafe(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
		}

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::setVar - An error occurred when loading data of type \"" + getGLEnumNameSafe(t) + "\" in variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn void HdlProgram::setVar(const std::string& varName, const HdlDynamicData& data)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param data The dynamic object to be used as source.
	**/
	void HdlProgram::setVar(const std::string& varName, const HdlDynamicData& data)
	{
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::setVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		switch(data.getGLType())
		{
			case GL_BYTE :			glUniform1i(loc, data.get(0));								break;
			case GL_UNSIGNED_BYTE : 	glUniform1ui(loc, data.get(0));								break;
			case GL_SHORT :			glUniform1i(loc, data.get(0));								break;
			case GL_UNSIGNED_SHORT :	glUniform1ui(loc, data.get(0));								break;
			case GL_FLOAT : 		glUniform1fv(loc, 1, reinterpret_cast<const GLfloat*>(data.getPtr()));			break;
			case GL_FLOAT_VEC2 : 		glUniform2fv(loc, 1, reinterpret_cast<const GLfloat*>(data.getPtr()));			break;
			case GL_FLOAT_VEC3 : 		glUniform3fv(loc, 1, reinterpret_cast<const GLfloat*>(data.getPtr()));			break;
			case GL_FLOAT_VEC4 : 		glUniform4fv(loc, 1, reinterpret_cast<const GLfloat*>(data.getPtr()));			break;
			case GL_DOUBLE :		throw Exception("HdlProgram::setVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC2 :		throw Exception("HdlProgram::setVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC3 :		throw Exception("HdlProgram::setVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC4 :		throw Exception("HdlProgram::setVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_INT :			glUniform1iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC2 :		glUniform2iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC3 :		glUniform3iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC4 :		glUniform4iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT :		glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC2 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC3 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC4 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_BOOL :			throw Exception("HdlProgram::setVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC2 :		throw Exception("HdlProgram::setVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC3 :		throw Exception("HdlProgram::setVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC4 :		throw Exception("HdlProgram::setVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_FLOAT_MAT2 :		glUniformMatrix2fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			case GL_FLOAT_MAT3 :		glUniformMatrix3fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			case GL_FLOAT_MAT4 :		glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			default :			throw Exception("HdlProgram::setVar - Unknown variable type or type mismatch for \"" + getGLEnumNameSafe(data.getGLType()) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
		}

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::setVar - An error occurred when loading data of type \"" + getGLEnumNameSafe(data.getGLType()) + "\" in variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, int* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, int* ptr)
	{
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		glGetUniformiv(program, loc, ptr);

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, unsigned int* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, unsigned int* ptr)
	{
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		glGetUniformuiv(program, loc, ptr);

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, float* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, float* ptr)
	{
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if (loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		glGetUniformfv(program, loc, ptr);

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn void HdlProgram::getVar(const std::string& varName, HdlDynamicData& data)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName Name of the fragment output variable.
	\param data The dynamic object to be used as target.
	**/
	void HdlProgram::getVar(const std::string& varName, HdlDynamicData& data)
	{
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__, Exception::GLException);

		switch(data.getGLType())
		{
			case GL_BYTE :			throw Exception("HdlProgram::getVar - Byte type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_UNSIGNED_BYTE : 	throw Exception("HdlProgram::getVar - Unsigned Byte type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_SHORT :			throw Exception("HdlProgram::getVar - Short type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_UNSIGNED_SHORT :	throw Exception("HdlProgram::getVar - Unsigned Short type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_FLOAT : 		
			case GL_FLOAT_VEC2 : 		
			case GL_FLOAT_VEC3 : 		
			case GL_FLOAT_VEC4 : 		glGetUniformfv(program, loc, reinterpret_cast<GLfloat*>(data.getPtr()));	break;
			case GL_DOUBLE :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC2 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC3 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_DOUBLE_VEC4 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_INT :			
			case GL_INT_VEC2 :		
			case GL_INT_VEC3 :		
			case GL_INT_VEC4 :		glGetUniformiv(program, loc, reinterpret_cast<GLint*>(data.getPtr()));		break;
			case GL_UNSIGNED_INT :		
			case GL_UNSIGNED_INT_VEC2 :	
			case GL_UNSIGNED_INT_VEC3 :	
			case GL_UNSIGNED_INT_VEC4 :	glGetUniformuiv(program, loc, reinterpret_cast<GLuint*>(data.getPtr()));	break;
			case GL_BOOL :			throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC2 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC3 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_BOOL_VEC4 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
			case GL_FLOAT_MAT2 :		
			case GL_FLOAT_MAT3 :		
			case GL_FLOAT_MAT4 :		glGetUniformfv(program, loc, reinterpret_cast<GLfloat*>(data.getPtr()));	break;
			default :			throw Exception("HdlProgram::getVar - Unknown variable type or type mismatch for \"" + getGLEnumNameSafe(data.getGLType()) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__, Exception::GLException);
		}

		GLenum err = glGetError();
		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + getGLEnumNameSafe(err) + " - " + getGLErrorDescription(err), __FILE__, __LINE__, Exception::GLException);
	}

	/**
	\fn    bool HdlProgram::isUniformVariableValid(const std::string& varName)
	\brief Check if a variable is valid from its name.
	\param varName The name of the uniform variable to read from.
	\return True if the name is valid (see glGetUniformLocation at http://www.opengl.org/sdk/docs/man/xhtml/glGetUniformLocation.xml) or false otherwise.
	**/
	bool HdlProgram::isUniformVariableValid(const std::string& varName)
	{
		if(program==0)
			return false;
		else
			return glGetUniformLocation(program, varName.c_str()) != -1;
	}	

// Static tools :
	/**
	\fn    int HdlProgram::maxVaryingVar(void)
	\brief Returns the maximum number of varying variables available.
	\return The maximum number of varying variables.
	**/
	int HdlProgram::maxVaryingVar(void)
	{
		GLint param;

		glGetIntegerv(GL_MAX_VARYING_FLOATS, &param);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::maxVaryingVar", "glGetIntegerv()")
		#endif

		return param;
	}

	/**
	\fn    void HdlProgram::stopProgram(void)
	\brief Stop using a program
	**/
	void HdlProgram::stopProgram(void)
	{
		glUseProgram(0);
	}

