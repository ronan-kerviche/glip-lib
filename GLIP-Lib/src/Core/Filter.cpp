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
			str << "in vec4 vertex;" << std::endl;

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

	// AbstractFilterLayout
	/**
	\fn AbstractFilterLayout::AbstractFilterLayout(const std::string& type, const HdlAbstractTextureFormat& f)
	\brief AbstractFilterLayout constructor.
	\param type The typename of the filter layout.
	\param f The texture format associated to all outputs of the filter.
	**/
	AbstractFilterLayout::AbstractFilterLayout(const std::string& type, const HdlAbstractTextureFormat& f)
	 : 	AbstractComponentLayout(type), 
		HdlAbstractTextureFormat(f), 
		vertexSource(NULL), 
		fragmentSource(NULL), 
		geometryModel(NULL), 
		clearing(true),
		blending(false),
		depthTesting(false),
		isStandardVertex(true),
		isStandardGeometry(true),
		sFactor(GL_ONE),
		dFactor(GL_ONE),
		blendingEquation(GL_FUNC_ADD),
		depthTestingFunction(GL_LESS)
	{ }

	/**
	\fn AbstractFilterLayout::AbstractFilterLayout(const AbstractFilterLayout& c)
	\brief AbstractFilterLayout constructor.
	\param c Copy.
	**/
	AbstractFilterLayout::AbstractFilterLayout(const AbstractFilterLayout& c)
	 :	AbstractComponentLayout(c), 
		HdlAbstractTextureFormat(c), 
		vertexSource(NULL), 
		fragmentSource(NULL), 
		geometryModel(NULL), 
		clearing(c.clearing),
		blending(c.blending), 
		depthTesting(c.depthTesting),
		isStandardVertex(c.isStandardVertex), 
		isStandardGeometry(c.isStandardGeometry),
		sFactor(c.sFactor),
		dFactor(c.dFactor),
		blendingEquation(c.blendingEquation),
		depthTestingFunction(c.depthTestingFunction)
	{
		try
		{
			if(c.vertexSource!=NULL)
				vertexSource   = new ShaderSource(*c.vertexSource);
			else
				throw Exception("AbstractFilterLayout::AbstractFilterLayout - vertexSource is NULL for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);

			if(c.fragmentSource!=NULL)
				fragmentSource = new ShaderSource(*c.fragmentSource);
			else
				throw Exception("AbstractFilterLayout::AbstractFilterLayout - fragmentSource is NULL for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);

			if(c.geometryModel!=NULL)
				geometryModel = new GeometryModel(*c.geometryModel);
			else
				throw Exception("AbstractFilterLayout::AbstractFilterLayout - geometryModel is NULL for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);
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

	AbstractFilterLayout::~AbstractFilterLayout(void)
	{
		delete vertexSource;
		delete fragmentSource;
		delete geometryModel;
	}

	/**
	\fn ShaderSource& AbstractFilterLayout::getVertexSource(void) const
	\brief Get the ShaderSource object of the vertex shader used by the filter.
	\return ShaderSource object reference.
	**/
	ShaderSource& AbstractFilterLayout::getVertexSource(void) const
	{
		if(vertexSource==NULL)
			throw Exception("FilterLayout::getVertexSource - The source has not been defined yet for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);

		return *vertexSource;
	}

	/**
	\fn ShaderSource& AbstractFilterLayout::getFragmentSource(void) const
	\brief Get the ShaderSource object of the fragment shader used by the filter.
	\return ShaderSource object reference.
	**/
	ShaderSource& AbstractFilterLayout::getFragmentSource(void) const
	{
		if(fragmentSource==NULL)
			throw Exception("FilterLayout::getFragmentSource - The source has not been defined yet for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);

		return *fragmentSource;
	}

	/**
	\fn GeometryModel& AbstractFilterLayout::getGeometryModel(void) const
	\brief Get the GeometryModel used by the filter.
	\return GeometryModel object reference.
	**/
	GeometryModel& AbstractFilterLayout::getGeometryModel(void) const
	{
		if(geometryModel==NULL)
			throw Exception("FilterLayout::getGeometryModel - The geometry has not been defined yet for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);

		return *geometryModel;
	}

	/**
	\fn bool AbstractFilterLayout::isClearingEnabled(void) const
	\return true if clearing is enabled.		
	**/
	bool AbstractFilterLayout::isClearingEnabled(void) const
	{
		return clearing;
	}
	
	/**
	\fn void AbstractFilterLayout::enableClearing(void)
	\brief Enables clearing operation (the destination buffer is cleared before each operation).
	**/
	void AbstractFilterLayout::enableClearing(void)
	{ 
		clearing = true;
	}
	
	/**
	\fn void AbstractFilterLayout::disableClearing(void)
	\brief Disables clearing operation.
	**/
	void AbstractFilterLayout::disableClearing(void)
	{
		clearing = false; 
	}

	/**
	\fn bool AbstractFilterLayout::isBlendingEnabled(void) const
	\return true if blending is enabled.	
	**/
	bool AbstractFilterLayout::isBlendingEnabled(void) const
	{ 
		return blending;  
	}

	/**
	\fn const GLenum& AbstractFilterLayout::getSFactor(void) const
	\return Get the blending factor to be applied to the source (only if blending is enabled).
	**/
	const GLenum& AbstractFilterLayout::getSFactor(void) const
	{
		return sFactor;
	}

	/**
	\fn const GLenum& AbstractFilterLayout::getDFactor(void) const
	\return Get the blending factor to be applied to the destination (only if blending is enabled).
	**/
	const GLenum& AbstractFilterLayout::getDFactor(void) const
	{
		return dFactor;
	}

	/**
	\fn const GLenum& AbstractFilterLayout::getBlendingEquation(void) const
	\return Get the equation used for the blending operation.
	**/
	const GLenum& AbstractFilterLayout::getBlendingEquation(void) const
	{
		return blendingEquation;
	}

	/**
	\fn void AbstractFilterLayout::enableBlending(const GLenum& _sFactor, const GLenum& _dFactor, const GLenum& _blendingEquation)
	\brief Enables blending operation (the alpha channel is used).
	\param _sFactor Blending factor to be applied to the source; among GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA and GL_SRC_ALPHA_SATURATE (see https://www.opengl.org/sdk/docs/man2/xhtml/glBlendFunc.xml for more information).
	\param _dFactor Blending factor to be applied to the destination; among GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA. GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_ALPHA and GL_ONE_MINUS_CONSTANT_ALPHA (see https://www.opengl.org/sdk/docs/man2/xhtml/glBlendFunc.xml for more information).
	\param _blendingEquation Setting the blending function, for mixing the source and destination color; among GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX (see https://www.opengl.org/sdk/docs/man2/xhtml/glBlendEquation.xml for more information).
	**/
	void AbstractFilterLayout::enableBlending(const GLenum& _sFactor, const GLenum& _dFactor, const GLenum& _blendingEquation)   		
	{
		sFactor = _sFactor;
		dFactor = _dFactor;
		blendingEquation = _blendingEquation;
		blending = true;  
	}

	/**
	\fn void AbstractFilterLayout::disableBlending(void)
	\brief Disables blending operation.
	**/
	void AbstractFilterLayout::disableBlending(void)
	{
		blending = false;
	}

	/**
	\fn bool AbstractFilterLayout::isDepthTestingEnabled(void) const
	\return True if the depth testing is enabled.
	**/
	bool AbstractFilterLayout::isDepthTestingEnabled(void) const
	{
		return depthTesting;
	}

	/**
	\fn const GLenum& AbstractFilterLayout::getDepthTestingFunction(void) const
	\return The depth testing function used if depth testing is enabled;
	**/
	const GLenum& AbstractFilterLayout::getDepthTestingFunction(void) const
	{
		return depthTestingFunction;
	}

	/**
	\fn void AbstractFilterLayout::enableDepthTesting(const GLenum& _depthTestingFunction)
	\brief Enable depth testing during filter processing.
	\param _depthTestingFunction Function used the depth testing; among GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL and GL_ALWAYS (see https://www.opengl.org/sdk/docs/man2/xhtml/glDepthFunc.xml for more information).
	**/
	void AbstractFilterLayout::enableDepthTesting(const GLenum& _depthTestingFunction)
	{
		depthTestingFunction = _depthTestingFunction;
		depthTesting = true;
	}	

	/**
	\fn void AbstractFilterLayout::disableDepthTesting(void)
	\brief Disable depth testing operation.
	**/
	void AbstractFilterLayout::disableDepthTesting(void)
	{
		depthTesting = false;
	}

	/**
	\fn bool AbstractFilterLayout::isStandardVertexSource(void) const
	\return true if the filter will be using a standard vertex program.
	\fn bool AbstractFilterLayout::isStandardGeometryModel(void) const
	\return true if the filter will be using a standard quad as geometry.
	**/
	bool AbstractFilterLayout::isStandardVertexSource(void) const	{ return isStandardVertex; }
	bool AbstractFilterLayout::isStandardGeometryModel(void) const	{ return isStandardGeometry; }

	// FilterLayout
	/**
	\fn FilterLayout::FilterLayout(const std::string& type, const HdlAbstractTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex, GeometryModel* geometry)
	\brief FilterLayout constructor.
	\param type The typename of the filter layout.
	\param fout The texture format of all the outputs.
	\param fragment The ShaderSource of the fragement shader.
	\param vertex The ShaderSource of the vertex shader (if left to NULL, the standard vertex shader is generated).
	\param geometry The geometry model to use in this filter (if left to NULL, the standard quad will be used, otherwise the object will be copied).
	**/
	FilterLayout::FilterLayout(const std::string& type, const HdlAbstractTextureFormat& fout, const ShaderSource& fragment, ShaderSource* vertex, GeometryModel* geometry)
	 : 	AbstractComponentLayout(type),  
		ComponentLayout(type),
		HdlAbstractTextureFormat(fout),
		AbstractFilterLayout(type, fout)
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
	\fn Filter::Filter(const AbstractFilterLayout& c, const std::string& name)
	\brief Filter constructor.
	\param c Filter layout.
	\param name The instance name.
	**/
	Filter::Filter(const AbstractFilterLayout& c, const std::string& name)
	:	AbstractComponentLayout(c), 
		Component(c, name),
		HdlAbstractTextureFormat(c), 
		AbstractFilterLayout(c),
		vertexShader(NULL), 
		fragmentShader(NULL), 
		prgm(NULL), 
		geometry(NULL) 
	{
		const int 	limInput  = HdlTexture::getMaxImageUnits(),
				limOutput = HdlFBO::getMaximumColorAttachment();

		firstRun	= true;
		broken		= true; // Wait for complete initialization.

		// Check for the number of input
		if(getNumInputPort()>limInput)
			throw Exception("Filter::Filter - Filter " + getFullName() + " has too many input port for hardware (Max : " + toString(limInput) + ", Current : " + toString(getNumInputPort()) + ").", __FILE__, __LINE__, Exception::CoreException);

		if(getNumOutputPort()>limOutput)
			throw Exception("Filter::Filter - Filter " + getFullName() + " has too many output port for hardware (Max : " + toString(limOutput) + ", Current : " + toString(getNumInputPort()) + ").", __FILE__, __LINE__, Exception::CoreException);

		// Build arguments table :
		arguments.assign(getNumInputPort(), reinterpret_cast<HdlTexture*>(NULL));

		try
		{
			// Build the shaders :
			vertexShader	= new HdlShader(GL_VERTEX_SHADER, getVertexSource());
			fragmentShader	= new HdlShader(GL_FRAGMENT_SHADER, getFragmentSource());
			prgm		= new HdlProgram(*vertexShader, *fragmentShader);
		}
		catch(Exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete prgm;

			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);
			m << e;
			throw m;
		}
		catch(std::exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete prgm;

			Exception m("Filter::Filter - Caught an exception while creating the shaders for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);
			m << e;
			throw m;
		}

		try
		{
			// Set the names of the samplers :
			for(int i=0; i<getNumInputPort(); i++)
				prgm->modifyVar(getInputPortName(i), GL_INT, i);

			if(!fragmentShader->requiresCompatibility())
			{
				for(int i=0; i<getNumOutputPort(); i++)
					prgm->setFragmentLocation(getOutputPortName(i), i);
			}

			prgm->stopProgram();
		}
		catch(Exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete prgm;

			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);
			m << e;
			throw m;
		}
		catch(std::exception& e)
		{
			delete vertexShader;
			delete fragmentShader;
			delete prgm;

			Exception m("Filter::Filter - Caught an exception while editing the samplers for " + getFullName(), __FILE__, __LINE__, Exception::CoreException);
			m << e;
			throw m;
		}

		// Build the geometry :
		geometry = new GeometryInstance( getGeometryModel(), GL_STATIC_DRAW_ARB );

		// Finally : 
		broken = false;
	}

	Filter::~Filter(void)
	{
		delete prgm;
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
			throw Exception("Filter::setInputForNextRendering - Index out of range", __FILE__, __LINE__, Exception::CoreException);
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
			throw Exception("Filter::process - Renderer doesn't have as many texture targets as Filter " + getFullName() + " has outputs.", __FILE__, __LINE__, Exception::CoreException);

		// Prepare the renderer	
			renderer.beginRendering(getNumOutputPort());
	
		// Enable states
			if(isDepthTestingEnabled())
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(getDepthTestingFunction());
			}
			else
				glDisable(GL_DEPTH_TEST);

			if(isBlendingEnabled())
			{
				glEnable(GL_BLEND);
				glBlendFunc(getSFactor(), getDFactor());
				glBlendEquation(getBlendingEquation());
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
			prgm->use();
			
		// Test on first run : 
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
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after initialization, GL error : " + glParamName(err) + ".", __FILE__, __LINE__, Exception::CoreException);
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
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after drawing operation, GL error : " + glParamName(err) + ".", __FILE__, __LINE__, Exception::CoreException);
				}
			}

		// Stop using the shader
			HdlProgram::stopProgram();

		// Remove from stack
			if(isDepthTestingEnabled())
				glDisable(GL_DEPTH_TEST);

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
					throw Exception("Filter::process : Exception caught on first run of filter " + getFullName() + ". The error occured after deinitialization, GL error : " + glParamName(err) + ".", __FILE__, __LINE__, Exception::CoreException);
				}
				else
					firstRun 	= false; // First run completed successfully.
			}
	}

	/**
	\fn HdlProgram& Filter::program(void)
	\brief Access the program carried.
	\return Reference to the program.
	**/
	HdlProgram& Filter::program(void)
	{
		return *prgm;
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

