/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : Filter.cpp                                                                                */
/*     Original Date : August 15th 2011                                                                          */
/*                                                                                                               */
/*     Description   : Filter object                                                                             */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    Filter.cpp
 * \brief   Filter object
 * \author  R. KERVICHE
 * \date    August 15th 2011
**/

#include <algorithm>
#include "Core/Exception.hpp"
#include "Core/Filter.hpp"
#include "Core/HdlTexture.hpp"
#include "Core/HdlShader.hpp"
#include "Core/HdlVBO.hpp"
#include "Core/HdlFBO.hpp"
#include "devDebugTools.hpp"
#include "Core/Geometry.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// Tools
	/**
	\fn std::string getStandardVertexSource(int versionNumber=0)
	\brief Build the standard vertex shader.
	\param versionNumber The targeted version as an integer. It won't be included by default (0).
	\return Standard string containing the GLSL code.
	**/
	std::string getStandardVertexSource(int versionNumber=0)
	{
		std::stringstream str;

		if(versionNumber>0)
			str << "#version " << versionNumber << std::endl;
		else if(versionNumber>130)
			str << "in vec3 vertex;" << std::endl;

		str << "void main()" << std::endl;
		str << "{" << std::endl;
		//str << "    gl_FrontColor = gl_Color;" << std::endl;

		/*for(int i=0; i<nUnits; i++)
			str << "    gl_TexCoord[" << i << "] = gl_TextureMatrix[" << i << "] * gl_MultiTexCoord" << i << "; \n";

		str << "    gl_Position = gl_ModelViewMatrix * gl_Vertex; \n } \n";*/

		str << "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;" << std::endl;

		if(versionNumber<=130)
			str << "    gl_Position = ftransform();" << std::endl;
		else
			str << "    gl_Position = vertex;" << std::endl;

		str << "}" << std::endl;

		return std::string(str.str());
	}

	// __ReadOnly_FilterLayout
	/**
	\fn __ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& f)
	\brief __ReadOnly_FilterLayout constructor.
	\param type The typename of the filter layout.
	\param f The texture format associated to all outputs of the filter.
	**/
	__ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& f)
	 : __ReadOnly_ComponentLayout(type), __ReadOnly_HdlTextureFormat(f), vertexSource(NULL), fragmentSource(NULL), geometryModel(NULL), blending(false), clearing(true), isStandardVertex(true), isStandardGeometry(true)
	{ }

	/**
	\fn __ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const __ReadOnly_FilterLayout& c)
	\brief __ReadOnly_FilterLayout constructor.
	\param c Copy.
	**/
	__ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const __ReadOnly_FilterLayout& c)
	 : __ReadOnly_ComponentLayout(c), __ReadOnly_HdlTextureFormat(c), vertexSource(NULL), fragmentSource(NULL), geometryModel(NULL), blending(c.blending), clearing(c.clearing), isStandardVertex(c.isStandardVertex), isStandardGeometry(c.isStandardGeometry)
	{
		try
		{
			if(c.vertexSource!=NULL)
				vertexSource   = new ShaderSource(*c.vertexSource);
			else
				throw Exception("__ReadOnly_FilterLayout::__ReadOnly_FilterLayout - vertexSource is NULL for " + getFullName(), __FILE__, __LINE__);

			if(c.fragmentSource!=NULL)
				fragmentSource = new ShaderSource(*c.fragmentSource);
			else
				throw Exception("__ReadOnly_FilterLayout::__ReadOnly_FilterLayout - fragmentSource is NULL for " + getFullName(), __FILE__, __LINE__);

			if(c.geometryModel!=NULL)
				geometryModel = new GeometryModel(*c.geometryModel);
			else
				throw Exception("__ReadOnly_FilterLayout::__ReadOnly_FilterLayout - geometryModel is NULL for " + getFullName(), __FILE__, __LINE__);
		}
		catch(Exception& e)
		{
			// Clean : 
			delete vertexSource;
			delete fragmentSource;
			delete geometryModel;

			vertexSource 	= NULL;
			fragmentSource	= NULL;
			geometryModel	= NULL;

			throw e;
		}
		
	}

	__ReadOnly_FilterLayout::~__ReadOnly_FilterLayout(void)
	{
		delete vertexSource;
		delete fragmentSource;
		delete geometryModel;
	}

	/**
	\fn ShaderSource& __ReadOnly_FilterLayout::getVertexSource(void) const
	\brief Get the ShaderSource object of the vertex shader used by the filter.
	\return ShaderSource object reference.
	**/
	ShaderSource& __ReadOnly_FilterLayout::getVertexSource(void) const
	{
		if(vertexSource==NULL)
			throw Exception("FilterLayout::getVertexSource - The source has not been defined yet for " + getFullName(), __FILE__, __LINE__);

		return *vertexSource;
	}

	/**
	\fn ShaderSource& __ReadOnly_FilterLayout::getFragmentSource(void) const
	\brief Get the ShaderSource object of the fragment shader used by the filter.
	\return ShaderSource object reference.
	**/
	ShaderSource& __ReadOnly_FilterLayout::getFragmentSource(void) const
	{
		if(fragmentSource==NULL)
			throw Exception("FilterLayout::getFragmentSource - The source has not been defined yet for " + getFullName(), __FILE__, __LINE__);

		return *fragmentSource;
	}

	/**
	\fn GeometryModel& __ReadOnly_FilterLayout::getGeometryModel(void) const
	\brief Get the GeometryModel used by the filter.
	\return GeometryModel object reference.
	**/
	GeometryModel& __ReadOnly_FilterLayout::getGeometryModel(void) const
	{
		if(geometryModel==NULL)
			throw Exception("FilterLayout::getGeometryModel - The geometry has not been defined yet for " + getFullName(), __FILE__, __LINE__);

		return *geometryModel;
	}

	/**
	\fn bool __ReadOnly_FilterLayout::isBlendingEnabled(void) const
	\return true if blending is enabled.
	\fn void __ReadOnly_FilterLayout::enableBlending(void)
	\brief Enables blending operation (the alpha channel is used).
	\fn void __ReadOnly_FilterLayout::disableBlending(void)
	\brief Disables blending operation.
	\fn bool __ReadOnly_FilterLayout::isClearingEnabled(void) const
	\return true if clearing is enabled.
	\fn void __ReadOnly_FilterLayout::enableClearing(void)
	\brief Enables clearing operation (the destination buffer is cleared before each operation).
	\fn void __ReadOnly_FilterLayout::disableClearing(void)
	\brief Disables clearing operation.
	**/
	bool __ReadOnly_FilterLayout::isBlendingEnabled(void) const	{ return blending;  }
	void __ReadOnly_FilterLayout::enableBlending(void)   		{ blending = true;  }
	void __ReadOnly_FilterLayout::disableBlending(void)  		{ blending = false; }
	bool __ReadOnly_FilterLayout::isClearingEnabled(void) const	{ return clearing;  }
	void __ReadOnly_FilterLayout::enableClearing(void)   		{ clearing = true;  }
	void __ReadOnly_FilterLayout::disableClearing(void)  		{ clearing = false; }

	/**
	\fn bool __ReadOnly_FilterLayout::isStandardVertexSource(void) const
	\return true if the filter will be using a standard vertex program.
	\fn bool __ReadOnly_FilterLayout::isStandardGeometryModel(void) const
	\return true if the filter will be using a standard quad as geometry.
	**/
	bool __ReadOnly_FilterLayout::isStandardVertexSource(void) const	{ return isStandardVertex; }
	bool __ReadOnly_FilterLayout::isStandardGeometryModel(void) const	{ return isStandardGeometry; }

	// FilterLayout
	/**
	\fn FilterLayout::FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex, GeometryModel* geometry)
	\brief FilterLayout constructor.
	\param type The typename of the filter layout.
	\param fout The texture format of all the outputs.
	\param fragment The ShaderSource of the fragement shader.
	\param vertex The ShaderSource of the vertex shader (if left to NULL, the standard vertex shader is generated).
	\param geometry The geometry model to use in this filter (if left to NULL, the standard quad will be used, otherwise the object will be copied).
	**/
	FilterLayout::FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex, GeometryModel* geometry)
	 : __ReadOnly_HdlTextureFormat(fout), __ReadOnly_ComponentLayout(type), ComponentLayout(type), __ReadOnly_FilterLayout(type, fout)
	{
		fragmentSource = new ShaderSource(fragment);

		if(vertex!=NULL)
			vertexSource = new ShaderSource(*vertex);

		if(geometry!=NULL)
		{
			geometryModel = new GeometryModel(*geometry);
			isStandardGeometry = false;
		}
		else
		{
			geometryModel = new GeometryPrimitives::StandardQuad;
			isStandardGeometry = true;
		}

		// Analyze sources to get the variables and the outputs
		std::vector<std::string> varsIn  = fragmentSource->getInputVars();
		if(vertexSource!=NULL)
		{
			// Add also vertex inputs if needed
			std::vector<std::string> varsInVertex = vertexSource->getInputVars();

			// Push them!
			for(std::vector<std::string>::iterator it=varsInVertex.begin(); it!=varsInVertex.end(); it++)
			{
				if(std::find(varsIn.begin(), varsIn.end(), *it)==varsIn.end())
					varsIn.push_back(*it);
				else
				{
					#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
						std::cout << "FilterLayout::FilterLayout - omitting variable name : " << *it << " in " << type << " because it already exits in the Fragment Shader." << std::endl;
					#endif
				}
			}
		}
		std::vector<std::string> varsOut = fragmentSource->getOutputVars();

		// Build Ports :
		for(std::vector<std::string>::iterator it=varsIn.begin(); it!=varsIn.end(); it++)
			addInputPort(*it);

		for(std::vector<std::string>::iterator it=varsOut.begin(); it!=varsOut.end(); it++)
			addOutputPort(*it);

		// If there is no vertexSource
		if(vertexSource==NULL)
		{
			// Build one :
			#ifdef __GLIPLIB_DEVELOPMENT_VERBOSE__
				std::cout << "FilterLayout::FilterLayout - Using for vertex shader : " << std::endl;
				std::cout << getStandardVertexSource(fragmentSource->getVersion()) << std::endl;
			#endif

			vertexSource = new ShaderSource(getStandardVertexSource(fragmentSource->getVersion()));

			isStandardVertex = true;
		}
		else
			isStandardVertex = false;
	}

// Filter
	/**
	\fn Filter::Filter(const __ReadOnly_FilterLayout& c, const std::string& name)
	\brief Filter constructor.
	\param c Filter layout.
	\param name The instance name.
	**/
	Filter::Filter(const __ReadOnly_FilterLayout& c, const std::string& name)
	: Component(c, name), __ReadOnly_FilterLayout(c), __ReadOnly_ComponentLayout(c), __ReadOnly_HdlTextureFormat(c), vertexShader(NULL), fragmentShader(NULL), program(NULL), geometry(NULL) 
	{
		const int 	limInput  = HdlTexture::getMaxImageUnits(),
				limOutput = HdlFBO::getMaximumColorAttachment();

		firstRun	= true;
		broken		= true; // Wait for complete initialization.

		// Check for the number of input
		if(getNumInputPort()>limInput)
			throw Exception("Filter::Filter - Filter " + getFullName() + " has too many input port for hardware (Max : " + to_string(limInput) + ", Current : " + to_string(getNumInputPort()) + ").", __FILE__, __LINE__);

		if(getNumOutputPort()>limOutput)
			throw Exception("Filter::Filter - Filter " + getFullName() + " has too many output port for hardware (Max : " + to_string(limOutput) + ", Current : " + to_string(getNumInputPort()) + ").", __FILE__, __LINE__);

		// Build arguments table :
		arguments.assign(getNumInputPort(), reinterpret_cast<HdlTexture*>(NULL));

		try
		{
			// Build the shaders :
			vertexShader	= new HdlShader(GL_VERTEX_SHADER, getVertexSource());
			fragmentShader	= new HdlShader(GL_FRAGMENT_SHADER, getFragmentSource());
			program		= new HdlProgram(*vertexShader, *fragmentShader);
		}
		catch(Exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete program;

			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getFullName(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete program;

			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getFullName(), __FILE__, __LINE__);
			throw m+e;
		}

		try
		{
			// Set the names of the samplers :
			for(int i=0; i<getNumInputPort(); i++)
				program->modifyVar(getInputPortName(i), GL_INT, i);

			if(!fragmentShader->requiresCompatibility())
			{
				for(int i=0; i<getNumOutputPort(); i++)
					program->setFragmentLocation(getOutputPortName(i), i);
			}

			program->stopProgram();
		}
		catch(Exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete program;

			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getFullName(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete program;

			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getFullName(), __FILE__, __LINE__);
			throw m+e;
		}

		// Build the geometry :
		geometry = new GeometryInstance( getGeometryModel(), GL_STATIC_DRAW_ARB );

		// Finally : 
		broken = false;
	}

	Filter::~Filter(void)
	{
		/*if(program==NULL)
			throw Exception("Filter::~Filter - Internal error : program is NULL", __FILE__, __LINE__);
		if(vertexShader==NULL)
			throw Exception("Filter::~Filter - Internal error : vertexShader is NULL", __FILE__, __LINE__);
		if(fragmentShader==NULL)
			throw Exception("Filter::~Filter - Internal error : fragmentShader is NULL", __FILE__, __LINE__);
		if(vbo!=NULL)
			delete vbo;*/

		delete program;
		delete vertexShader;
		delete fragmentShader;
		delete geometry;
	}

	/**
	\fn void Filter::setInputForNextRendering(int id, HdlTexture* ptr)
	\brief Sets input texture for next rendering.
	\param id Index of the input.
	\param ptr Pointer to a valid texture object.
	**/
	void Filter::setInputForNextRendering(int id, HdlTexture* ptr)
	{
		if(id<0 || id>getNumInputPort())
			throw Exception("Filter::setInputForNextRendering - Index out of range", __FILE__, __LINE__);
		arguments[id] = ptr;
		//std::cout << "Adding : " << arguments[id] << " at " << id << std::endl;
	}

	/**
	\fn void Filter::process(HdlFBO& renderer)
	\brief Start the rendering process.
	\param renderer The FBO to use as target.
	**/
	void Filter::process(HdlFBO& renderer)
	{
		if(renderer.getAttachmentCount()<getNumOutputPort())
			throw Exception("Filter::process - Renderer doesn't have as many texture targets as Filter " + getFullName() + " has outputs.", __FILE__, __LINE__);

		// Prepare the renderer
			renderer.beginRendering(getNumOutputPort());
			
		// Enable states
			if(isBlendingEnabled())
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
			}
			else
				glDisable(GL_BLEND);

			if(isClearingEnabled())
			{
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

		// Link the textures
			for(int i=0; i<getNumInputPort(); i++)
			{
				//std::cout << "Using : " << arguments[i] << std::endl;
				arguments[i]->bind(i);
			}

		// Prepare geometry
			glLoadIdentity();

		// Load the shader
			program->use();
			
		// Test on first run ; 
			if(firstRun)
			{
				const GLenum err = glGetError();

				if(err!=GL_NO_ERROR)
				{
					// Force end rendering : 
					HdlProgram::stopProgram();
					renderer.endRendering();

					firstRun 	= false;
					broken 		= true;
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after initialization, GL error : " + glParamName(err) + ".", __FILE__, __LINE__);
				}
			}

		// Draw
			geometry->draw();

		// Test on first run ; 
			if(firstRun)
			{
				const GLenum err = glGetError();

				if(err!=GL_NO_ERROR)
				{
					// Force end rendering : 
					HdlProgram::stopProgram();
					renderer.endRendering();

					firstRun 	= false;
					broken 		= true;
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after drawing operation, GL error : " + glParamName(err) + ".", __FILE__, __LINE__);
				}
			}

		// Stop using the shader
			HdlProgram::stopProgram();

		// Remove from stack
			if(isBlendingEnabled())
			    glDisable(GL_BLEND);

		// Unload
			for(int i=0; i<getNumInputPort(); i++)
			{
				//std::cout << "Unbinding " << i << std::endl;
				HdlTexture::unbind(i);
			}

		// End rendering
			renderer.endRendering();

		// Test on first run ; 
			if(firstRun)
			{
				const GLenum err = glGetError();

				if(err!=GL_NO_ERROR)
				{
					firstRun 	= false;
					broken 		= true;
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after deinitialization, GL error : " + glParamName(err) + ".", __FILE__, __LINE__);
				}
				else
					firstRun 	= false; // First run completed successfully.
			}
	}

	/**
	\fn HdlProgram& Filter::prgm(void)
	\brief Access the program carried.
	\return Reference to the program.
	**/
	HdlProgram& Filter::prgm(void)
	{
		return *program;
	}

	/**
	\fn bool Filter::wentThroughFirstRun(void) const
	\brief Check if the filter was already applied, at least once.
	\return True if the filter was already applied.
	**/
	bool Filter::wentThroughFirstRun(void) const
	{
		return !firstRun;
	}

	/**
	\fn bool Filter::isBroken(void) const
	\brief Check if the filter is broken (its initialization failed, or an error occured during its first run).
	\return True if the filter is broken and should not be used.
	**/
	bool Filter::isBroken(void) const
	{
		return broken;
	}

