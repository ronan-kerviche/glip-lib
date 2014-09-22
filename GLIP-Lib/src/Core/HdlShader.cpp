/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
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

// HdlShader - Functions
	/**
	\fn    HdlShader::HdlShader(GLenum _type, const ShaderSource& src)
	\brief HdlShader constructor.
	\param _type The kind of shader it will be : either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
	\param src   The source code used.
	**/
	HdlShader::HdlShader(GLenum _type, const ShaderSource& src)
	 : ShaderSource(src)
	{
		NEED_EXTENSION(GLEW_VERSION_2_0)
		NEED_EXTENSION(GLEW_ARB_vertex_shader)
		NEED_EXTENSION(GLEW_ARB_fragment_shader)
		NEED_EXTENSION(GLEW_ARB_shader_objects)
		NEED_EXTENSION(GLEW_ARB_shading_language_100)
		NEED_EXTENSION(GLEW_ARB_vertex_program)
		NEED_EXTENSION(GLEW_ARB_fragment_program)

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

		type = _type; //either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER

		// create the shader
		shader = glCreateShader(type);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::HdlShader", "glCreateShader()")
		#endif

		if(shader==0)
			throw Exception("HdlShader::HdlShader - Unable to create the shader from " + getSourceName() + ". Last OpenGL error : " + glParamName(glGetError()) + ".", __FILE__, __LINE__);

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

			std::string err = this->errorLog(std::string(log));

			delete[] log;

			throw Exception("HdlShader::HdlShader - error while compiling the shader from " + getSourceName() + " : \n" + err, __FILE__, __LINE__);
		}
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
	\brief Returns the kind of the shader for OpenGL (either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
	\return The Kind.
	**/
	GLenum HdlShader::getType(void) const
	{
		return type;
	}

	HdlShader::~HdlShader(void)
	{
		glDeleteShader(shader);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlShader::~HdlShader", "glDeleteShader()")
		#endif
	}


// HdlProgram - Functions
	/**
	\fn HdlProgram::HdlProgram(void)
	\brief HdlProgram constructor.
	**/
	HdlProgram::HdlProgram(void)
	 : valid(false)
	{
		// create the program
		program = glCreateProgram();

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::HdlProgram(void)", "glCreateProgram()")
		#endif

		if(program==0)
			throw Exception("HdlProgram::HdlProgram - Program can't be created. Last OpenGL error : " + glParamName(glGetError()) + ".", __FILE__, __LINE__);

		attachedFragmentShader = attachedVertexShader = 0;
	}

	/**
	\fn    HdlProgram::HdlProgram(const HdlShader& shd1, const HdlShader& shd2)
	\brief HdlProgram constructor. Note that the shaders must be of different kinds.
	\param shd1 The first shader to link.
	\param shd2 The second shader to link.
	**/
	HdlProgram::HdlProgram(const HdlShader& shd1, const HdlShader& shd2)
	 : valid(false)
	{
		// create the program
		program = glCreateProgram();

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::HdlProgram(const HdlShader& shd1, const HdlShader& shd2)", "glCreateProgram()")
		#endif

		attachedFragmentShader = attachedVertexShader = 0;

		//attach the two Shaders
		update(shd1, false);
		update(shd2, false);

		// Link the program
		valid = link();
	}

	/**
	\fn    bool HdlProgram::isValid(void)
	\brief Check if the program is valid.
	\return True if the Program is valid, false otherwise.
	**/
	bool HdlProgram::isValid(void)
	{
		return valid;
	}

	/**
	\fn    bool HdlProgram::link(void)
	\brief Link the program.
	\return False in case of failure, true otherwise.
	**/
	bool HdlProgram::link(void)
	{
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
			//std::cout << "Error during Program linking" << std::endl;

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

			throw Exception("HdlProgram::link - Error during Program linking : \n" + logstr, __FILE__, __LINE__);
			return false;
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

			char buffer[1024];
			GLenum type;
			GLint actualSize, actualSizeName;
			for(int k=0; k<numUniforms; k++)
			{
				glGetActiveUniform( program, k, 1024, &actualSizeName, &actualSize, &type, buffer);

				if( std::find(interestTypes, interestTypes + numAllowedTypes, type) != interestTypes + numAllowedTypes)
				{
 					activeUniforms.push_back(buffer);
					activeTypes.push_back(type);
				}
			}
		}

		return true;
	}

	/**
	\fn    void HdlProgram::update(const HdlShader& shader, bool lnk)
	\brief Change a shader in the program.
	\param shader The shader to add.
	\param lnk Set to true if you want the program to be linked again.
	**/
	void HdlProgram::update(const HdlShader& shader, bool lnk)
	{
		if( shader.getType()==GL_VERTEX_SHADER )
		{
			if( attachedVertexShader !=0 )
			{
				glDetachShader(program, attachedVertexShader);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("HdlProgram::update", "glDetachShader(program, attachedVertexShader)")
				#endif
			}

			attachedVertexShader = shader.getShaderID();
			glAttachShader(program, attachedVertexShader);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlProgram::update", "glAttachShader(program, attachedVertexShader)")
			#endif
		}
		else //shader.getType()==GL_FRAGMENT_SHADER
		{
			if( attachedFragmentShader !=0 )
			{
				glDetachShader(program, attachedFragmentShader);

				#ifdef __GLIPLIB_TRACK_GL_ERRORS__
					OPENGL_ERROR_TRACKER("HdlProgram::update", "glDetachShader(program, attachedFragmentShader)")
				#endif
			}

			attachedFragmentShader = shader.getShaderID();
			glAttachShader(program, attachedFragmentShader);

			#ifdef __GLIPLIB_TRACK_GL_ERRORS__
				OPENGL_ERROR_TRACKER("HdlProgram::update", "glAttachShader(program, attachedFragmentShader)")
			#endif
		}

		// Link the program
		if(lnk) valid = link();
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
	**/
	void HdlProgram::setFragmentLocation(const std::string& fragName, int frag)
	{
		glGetError(); // clean error buffer, isolate from previous possible errors.

		#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
			std::cout << "HdlProgram::setFragmentLocation - FragName : " << fragName << std::endl;
		#endif
		glBindFragDataLocation(program, frag, fragName.c_str());

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::setFragmentLocation - Error while setting fragment location \"" + fragName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum type, int v0, int v1=0, int v2=0, int v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/
	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum type, unsigned int v0, unsigned int v1=0, unsigned int v2=0, unsigned int v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/
	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum type, float v0, float v1=0, float v2=0, float v3=0)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param type    Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v0      Corresponding value to assign.
	\param v1      Corresponding value to assign.
	\param v2      Corresponding value to assign.
	\param v3      Corresponding value to assign.
	**/

	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum t, int* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/
	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum t, unsigned int* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/
	/**
	\fn    void HdlProgram::modifyVar(const std::string& varName, GLenum t, float* v)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param t       Kind of variable in, see http://www.opengl.org/sdk/docs/man/xhtml/glGetActiveUniform.xml for possible types.
	\param v       Pointer to the values to assign.
	**/

	#define GENmodifyVarA( argT1, argT2, argT3)   \
		void HdlProgram::modifyVar(const std::string& varName, GLenum t, argT1 v0, argT1 v1, argT1 v2, argT1 v3) \
		{ \
			glGetError(); \
			use(); \
			GLint loc = glGetUniformLocation(program, varName.c_str()); \
			 \
			if(loc==-1) \
				throw Exception("HdlProgram::modifyVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__); \
			 \
			switch(t) \
			{ \
				case GL_##argT2 : 		glUniform1##argT3 (loc, v0);			break; \
				case GL_##argT2##_VEC2 : 	glUniform2##argT3 (loc, v0, v1);		break; \
				case GL_##argT2##_VEC3 : 	glUniform3##argT3 (loc, v0, v1, v2);		break; \
				case GL_##argT2##_VEC4 : 	glUniform4##argT3 (loc, v0, v1, v2, v3);	break; \
				default :		throw Exception("HdlProgram::modifyVar - Unknown variable type or type mismatch for \"" + glParamName(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__); \
			} \
			 \
			GLenum err = glGetError(); \
			 \
			if(err!=GL_NO_ERROR) \
				throw Exception("HdlProgram::modifyVar - An error occurred when loading data of type \"" + glParamName(t) + "\" in variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__); \
		} \

	#define GENmodifyVarB( argT1, argT2, argT3)   \
		void HdlProgram::modifyVar(const std::string& varName, GLenum t, argT1* v) \
		{ \
			glGetError(); \
			use(); \
			GLint loc = glGetUniformLocation(program, varName.c_str()); \
			 \
			if(loc==-1) \
				throw Exception("HdlProgram::modifyVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__); \
			 \
			switch(t) \
			{ \
				case GL_##argT2 : 		glUniform1##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC2 : 	glUniform2##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC3 : 	glUniform3##argT3##v(loc, 1, v);			break; \
				case GL_##argT2##_VEC4 : 	glUniform4##argT3##v(loc, 1, v);			break; \
				default :		throw Exception("HdlProgram::modifyVar - Unknown variable type or type mismatch for \"" + glParamName(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__); \
			} \
			 \
			GLenum err = glGetError(); \
			 \
			if(err!=GL_NO_ERROR) \
				throw Exception("HdlProgram::modifyVar - An error occurred when loading data of type \"" + glParamName(t) + "\" in variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__); \
		}

	GENmodifyVarA( int, INT, i)
	GENmodifyVarB( int, INT, i)
	GENmodifyVarA( unsigned int, UNSIGNED_INT, ui)
	GENmodifyVarB( unsigned int, UNSIGNED_INT, ui)
	GENmodifyVarA( float, FLOAT, f)

	#undef GENmodifyVar

	// Last one, with matrices :
	void HdlProgram::modifyVar(const std::string& varName, GLenum t, float* v)
	{
		glGetError();
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::modifyVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

		switch(t)
		{
			case GL_FLOAT : 	glUniform1fv(loc, 1, v);			break;
			case GL_FLOAT_VEC2 : 	glUniform2fv(loc, 1, v);			break;
			case GL_FLOAT_VEC3 : 	glUniform3fv(loc, 1, v);			break;
			case GL_FLOAT_VEC4 : 	glUniform4fv(loc, 1, v);			break;
			case GL_FLOAT_MAT2 :	glUniformMatrix2fv(loc, 1, GL_FALSE, v);	break;
			case GL_FLOAT_MAT3 :	glUniformMatrix3fv(loc, 1, GL_FALSE, v);	break;
			case GL_FLOAT_MAT4 :	glUniformMatrix4fv(loc, 1, GL_FALSE, v);	break;
			default :		throw Exception("HdlProgram::modifyVar - Unknown variable type or type mismatch for \"" + glParamName(t) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
		}

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::modifyVar - An error occurred when loading data of type \"" + glParamName(t) + "\" in variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn void HdlProgram::modifyVar(const std::string& varName, const HdlDynamicData& data)
	\brief Change a uniform variable in a shader. Raise an exception if any error occur.
	\param varName Name of the fragment output variable.
	\param data The dynamic object to be used as source.
	**/
	void HdlProgram::modifyVar(const std::string& varName, const HdlDynamicData& data)
	{
		glGetError();
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::modifyVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

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
			case GL_DOUBLE :		throw Exception("HdlProgram::modifyVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC2 :		throw Exception("HdlProgram::modifyVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC3 :		throw Exception("HdlProgram::modifyVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC4 :		throw Exception("HdlProgram::modifyVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_INT :			glUniform1iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC2 :		glUniform2iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC3 :		glUniform3iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_INT_VEC4 :		glUniform4iv(loc, 1, reinterpret_cast<const GLint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT :		glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC2 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC3 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_UNSIGNED_INT_VEC4 :	glUniform1uiv(loc, 1, reinterpret_cast<const GLuint*>(data.getPtr()));			break;
			case GL_BOOL :			throw Exception("HdlProgram::modifyVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC2 :		throw Exception("HdlProgram::modifyVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC3 :		throw Exception("HdlProgram::modifyVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC4 :		throw Exception("HdlProgram::modifyVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_FLOAT_MAT2 :		glUniformMatrix2fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			case GL_FLOAT_MAT3 :		glUniformMatrix3fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			case GL_FLOAT_MAT4 :		glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data.getPtr()));	break;
			default :			throw Exception("HdlProgram::modifyVar - Unknown variable type or type mismatch for \"" + glParamName(data.getGLType()) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
		}

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::modifyVar - An error occurred when loading data of type \"" + glParamName(data.getGLType()) + "\" in variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, int* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, int* ptr)
	{
		glGetError();
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if (loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

		glGetUniformiv(program, loc, ptr);

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, unsigned int* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, unsigned int* ptr)
	{
		glGetError();
		use();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if (loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

		glGetUniformuiv(program, loc, ptr);

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn    void HdlProgram::getVar(const std::string& varName, float* ptr)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName The name of the uniform variable to read from.
	\param ptr A pointer to a buffer with sufficient size in order to contain the full object (scalar, vector, matrix...).
	**/
	void HdlProgram::getVar(const std::string& varName, float* ptr)
	{
		glGetError();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if (loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

		glGetUniformfv(program, loc, ptr);

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn void HdlProgram::getVar(const std::string& varName, HdlDynamicData& data)
	\brief Read a uniform variable from a shader. Warning : this function does not perform any type or size check which might result in a buffer overflow if not used with care.
	\param varName Name of the fragment output variable.
	\param data The dynamic object to be used as target.
	**/
	void HdlProgram::getVar(const std::string& varName, HdlDynamicData& data)
	{
		glGetError();
		GLint loc = glGetUniformLocation(program, varName.c_str());

		if(loc==-1)
			throw Exception("HdlProgram::getVar - Wrong location, does this var exist : \"" + varName + "\"? Is it used in the program? (May be the GLCompiler swapped it because it is unused).", __FILE__, __LINE__);

		switch(data.getGLType())
		{
			case GL_BYTE :			throw Exception("HdlProgram::getVar - Byte type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_UNSIGNED_BYTE : 	throw Exception("HdlProgram::getVar - Unsigned Byte type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_SHORT :			throw Exception("HdlProgram::getVar - Short type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_UNSIGNED_SHORT :	throw Exception("HdlProgram::getVar - Unsigned Short type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_FLOAT : 		
			case GL_FLOAT_VEC2 : 		
			case GL_FLOAT_VEC3 : 		
			case GL_FLOAT_VEC4 : 		glGetUniformfv(program, loc, reinterpret_cast<GLfloat*>(data.getPtr()));	break;
			case GL_DOUBLE :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC2 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC3 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_DOUBLE_VEC4 :		throw Exception("HdlProgram::getVar - Double type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_INT :			
			case GL_INT_VEC2 :		
			case GL_INT_VEC3 :		
			case GL_INT_VEC4 :		glGetUniformiv(program, loc, reinterpret_cast<GLint*>(data.getPtr()));		break;
			case GL_UNSIGNED_INT :		
			case GL_UNSIGNED_INT_VEC2 :	
			case GL_UNSIGNED_INT_VEC3 :	
			case GL_UNSIGNED_INT_VEC4 :	glGetUniformuiv(program, loc, reinterpret_cast<GLuint*>(data.getPtr()));	break;
			case GL_BOOL :			throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC2 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC3 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_BOOL_VEC4 :		throw Exception("HdlProgram::getVar - Bool type not supported when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
			case GL_FLOAT_MAT2 :		
			case GL_FLOAT_MAT3 :		
			case GL_FLOAT_MAT4 :		glGetUniformfv(program, loc, reinterpret_cast<GLfloat*>(data.getPtr()));	break;
			default :			throw Exception("HdlProgram::getVar - Unknown variable type or type mismatch for \"" + glParamName(data.getGLType()) + "\" when modifying uniform variable \"" + varName + "\".", __FILE__, __LINE__);
		}

		GLenum err = glGetError();

		if(err!=GL_NO_ERROR)
			throw Exception("HdlProgram::getVar - An error occurred when reading variable \"" + varName + "\" : " + glParamName(err) + ".", __FILE__, __LINE__);
	}

	/**
	\fn    bool HdlProgram::isUniformVariableValid(const std::string& varName)
	\brief Check if a variable is valid from its name.
	\param varName The name of the uniform variable to read from.
	\return True if the name is valid (see glGetUniformLocation at http://www.opengl.org/sdk/docs/man/xhtml/glGetUniformLocation.xml) or false otherwise.
	**/
	bool HdlProgram::isUniformVariableValid(const std::string& varName)
	{
		return glGetUniformLocation(program, varName.c_str()) != -1;
	}

	HdlProgram::~HdlProgram(void)
	{
		glDetachShader(program, attachedFragmentShader);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::~HdlProgram", "glDetachShader(program, attachedFragmentShader)")
		#endif

		glDetachShader(program, attachedVertexShader);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::~HdlProgram", "glDetachShader(program, attachedVertexShader)")
		#endif

		glDeleteProgram(program);

		#ifdef __GLIPLIB_TRACK_GL_ERRORS__
			OPENGL_ERROR_TRACKER("HdlProgram::~HdlProgram", "glDeleteProgram(program)")
		#endif
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

