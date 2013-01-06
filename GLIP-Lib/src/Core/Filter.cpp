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
#include "Filter.hpp"
#include "HdlTexture.hpp"
#include "HdlShader.hpp"
#include "HdlVBO.hpp"
#include "HdlFBO.hpp"
#include "devDebugTools.hpp"

    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

// Tools
	/**
	\fn std::string getStandardVertexSource(int nUnits, int versionNumber=0)
	\brief Build the standard vertex shader.
	\param nUnits Number of texturing units to be used.
	\param versionNumber The targeted version as an integer. It won't be included by default (0).
	\return Standard string containing the GLSL code.
	**/
	std::string getStandardVertexSource(int nUnits, int versionNumber=0)
	{
		std::stringstream str;

		if(versionNumber>0)
			str << "#version " << versionNumber << std::endl;

		str << "void main() \n { \n    gl_FrontColor  = gl_Color; \n";

		for(int i=0; i<nUnits; i++)
			str << "    gl_TexCoord[" << i << "] = gl_TextureMatrix[" << i << "] * gl_MultiTexCoord" << i << "; \n";

		str << "    gl_Position = gl_ModelViewMatrix * gl_Vertex; \n } \n";

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
	 : __ReadOnly_ComponentLayout(type), __ReadOnly_HdlTextureFormat(f), vertexSource(NULL), fragmentSource(NULL), blending(false), clearing(true)
	{ }

	/**
	\fn __ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const __ReadOnly_FilterLayout& c)
	\brief __ReadOnly_FilterLayout constructor.
	\param c Copy.
	**/
	__ReadOnly_FilterLayout::__ReadOnly_FilterLayout(const __ReadOnly_FilterLayout& c)
	 : __ReadOnly_ComponentLayout(c), __ReadOnly_HdlTextureFormat(c), vertexSource(NULL), fragmentSource(NULL), blending(c.blending), clearing(c.clearing)
	{
		if(c.vertexSource!=NULL)
			vertexSource   = new ShaderSource(*c.vertexSource);
		else
			throw Exception("__ReadOnly_FilterLayout::__ReadOnly_FilterLayout - vertexSource is NULL for " + getNameExtended(), __FILE__, __LINE__);

		if(c.fragmentSource!=NULL)
			fragmentSource = new ShaderSource(*c.fragmentSource);
		else
			throw Exception("__ReadOnly_FilterLayout::__ReadOnly_FilterLayout - fragmentSource is NULL for " + getNameExtended(), __FILE__, __LINE__);
	}

	__ReadOnly_FilterLayout::~__ReadOnly_FilterLayout(void)
	{
		delete vertexSource;
		delete fragmentSource;
	}

	/**
	\fn ShaderSource& __ReadOnly_FilterLayout::getVertexSource(void) const
	\brief Get the ShaderSource object of the vertex shader used by the filter.
	\return ShaderSource object reference.
	**/
	ShaderSource& __ReadOnly_FilterLayout::getVertexSource(void) const
	{
		if(vertexSource==NULL)
			throw Exception("FilterLayout::getVertexSource - The source has not been defined yet for " + getNameExtended(), __FILE__, __LINE__);

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
			throw Exception("FilterLayout::getFragmentSource - The source has not been defined yet for " + getNameExtended(), __FILE__, __LINE__);

		return *fragmentSource;
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

	// FilterLayout
	/**
	\fn FilterLayout::FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex)
	\brief FilterLayout constructor.
	\param type The typename of the filter layout.
	\param fout The texture format of all the outputs.
	\param fragment The ShaderSource of the fragement shader.
	\param vertex [Optional] The ShaderSource of the vertex shader (if left to NULL, the standard vertex shader is generated).
	**/
	FilterLayout::FilterLayout(const std::string& type, const __ReadOnly_HdlTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex)
	 : __ReadOnly_HdlTextureFormat(fout), __ReadOnly_ComponentLayout(type), ComponentLayout(type), __ReadOnly_FilterLayout(type, fout)
	{
		fragmentSource = new ShaderSource(fragment);

		if(vertex!=NULL)
			vertexSource = new ShaderSource(*vertex);

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

			//TODO Verify that variables names in vertex and fragment are different.
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
				std::cout << getStandardVertexSource(varsIn.size(), fragmentSource->getVersion()) << std::endl;
			#endif
			vertexSource = new ShaderSource(getStandardVertexSource(varsIn.size(), fragmentSource->getVersion()));
		}
	}

// Filter
	/**
	\fn Filter::Filter(const __ReadOnly_FilterLayout& c)
	\brief Filter constructor.
	\param c Filter layout.
	**/
	Filter::Filter(const __ReadOnly_FilterLayout& c)
	: Component(c, c.getName()), __ReadOnly_FilterLayout(c), __ReadOnly_ComponentLayout(c), __ReadOnly_HdlTextureFormat(c), vertexShader(NULL), fragmentShader(NULL), program(NULL), vbo(NULL)
	{
		const int 	limInput  = HdlTexture::getMaxImageUnits(),
				limOutput = HdlFBO::getMaximumColorAttachment();

		// Check for the number of input
		if(getNumInputPort()>limInput)
			throw Exception("Filter::Filter - Filter " + getNameExtended() + " has too many input port for hardware (Max : " + to_string(limInput) + ", Current : " + to_string(getNumInputPort()), __FILE__, __LINE__);

		if(getNumInputPort()>limOutput)
			throw Exception("Filter::Filter - Filter " + getNameExtended() + " has too many output port for hardware (Max : " + to_string(limOutput) + ", Current : " + to_string(getNumInputPort()), __FILE__, __LINE__);

		// Build arguments table :
		arguments.assign(getNumInputPort(), reinterpret_cast<HdlTexture*>(NULL));

		try
		{
			// Build the shaders :
			vertexShader   = new HdlShader(GL_VERTEX_SHADER, getVertexSource());
			fragmentShader = new HdlShader(GL_FRAGMENT_SHADER, getFragmentSource());
			program        = new HdlProgram(*vertexShader, *fragmentShader);
		}
		catch(Exception& e)
		{
			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}

		try
		{
			// Set the names of the samplers :
			for(int i=0; i<getNumInputPort(); i++)
				program->modifyVar(getInputPortName(i), HdlProgram::Var, i);

			if(!fragmentShader->requiresCompatibility())
			{
				for(int i=0; i<getNumOutputPort(); i++)
					program->setFragmentLocation(getOutputPortName(i), i);
			}

			program->stopProgram();
		}
		catch(Exception& e)
		{
			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}
		catch(std::exception& e)
		{
			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getNameExtended(), __FILE__, __LINE__);
			throw m+e;
		}

		/*try
		{
			// Create a basic geometry :
			vbo = HdlVBO::generate2DStandardQuad();
		}
		catch(Exception& e)
		{
			throw Exception("Filter::Filter - Caught an exception while creating the geometry for " + getNameExtended() + " : \n" + e.what(), __FILE__, __LINE__);
		}
		catch(std::exception& e)
		{
			throw Exception("Filter::Filter - Caught an exception while creating the geometry for " + getNameExtended() + " : \n" + e.what(), __FILE__, __LINE__);
		}*/

		// Set up the data on the program :
		if(!fragmentShader->requiresCompatibility())
		{
			for(int i=0; i<getNumInputPort(); i++)
				program->setFragmentLocation(getInputPortName(i), i);
		}
	}

	Filter::~Filter(void)
	{
		if(program==NULL)
			throw Exception("Filter::~Filter - Internal error : program is NULL", __FILE__, __LINE__);
		if(vertexShader==NULL)
			throw Exception("Filter::~Filter - Internal error : vertexShader is NULL", __FILE__, __LINE__);
		if(fragmentShader==NULL)
			throw Exception("Filter::~Filter - Internal error : fragmentShader is NULL", __FILE__, __LINE__);
		/*if(vbo==NULL)
			throw Exception("Filter::~Filter - Internal error : vbo is NULL", __FILE__, __LINE__);*/
		if(vbo!=NULL)
			delete vbo;

		delete program;
		delete vertexShader;
		delete fragmentShader;
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
			throw Exception("Filter::process - Renderer doesn't have as many texture targets as Filter " + getNameExtended() + " has outputs.", __FILE__, __LINE__);

		// Prepare the renderer
			renderer.beginRendering(getNumOutputPort());
			//std::cout << "Begin rendering 		: "; glErrors(true, false);

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
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			//std::cout << "Clearing states 		: "; glErrors(true, false);

		// Link the textures
			for(int i=0; i<getNumInputPort(); i++)
			{
				//std::cout << "Using : " << arguments[i] << std::endl;
				/*if(arguments[i]==NULL)
					throw Exception("Argument is NULL", __FILE__, __LINE__);*/
				arguments[i]->bind(i);
			}
			//std::cout << "Binding	 		: "; glErrors(true, false);

		// Prepare geometry
			glLoadIdentity();

		// Load the shader
			program->use();
			//std::cout << "Using shader 		: "; glErrors(true, false);

		// Draw
			if(vbo!=NULL)
				vbo->draw();
			else
				HandleOpenGL::standardQuadVBO().draw();
			//std::cout << "drawing VBO 		: "; glErrors(true, false);

		// Stop using the shader
			HdlProgram::stopProgram();
			//std::cout << "Stop program 		: "; glErrors(true, false);

		// Remove from stack
			if(isBlendingEnabled())
			    glDisable(GL_BLEND);

		// Unload
			for(int i=0; i<getNumInputPort(); i++)
			{
				//std::cout << "Unbinding " << i << std::endl;
				HdlTexture::unbind(i);
			}
			//std::cout << "Unbinding 		: "; glErrors(true, false);

		// End rendering
			renderer.endRendering();
			//std::cout << "End rendering 		: "; glErrors(true, false);


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
	\fn void Filter::setGeometry(HdlVBO* v)
	\brief Push different geometry rendering, the Filter object will take care of deleting the data when needed. The object will take care to free previously used memory.
	\param v Pointer to the new geometry to use. If set to NULL it will use the standard quad from HandleOpenGL::standardQuadVBO().
	**/
	void Filter::setGeometry(HdlVBO* v)
	{
		/*if(v==NULL)
			throw Exception("Filter::setGeometry - You can't set a pointer to NULL for the geometry!", __FILE__, __LINE__);*/

		if(vbo!=NULL)
			delete vbo;

		vbo = v;
	}
