/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-COMPUTE                                                                                              */
/*     Command-Line Utility for the OpenGL Image Processing LIBrary                                              */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : CreateWindowlessContext.cpp                                                               */
/*     Original Date : August 18th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Create window-less GL functions (still require a display system to be available).         */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "CreateWindowlessContext.hpp"

void createWindowlessContext(std::string displayName)
{
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
	glXMakeContextCurrentARBProc glXMakeContextCurrentARB = (glXMakeContextCurrentARBProc) glXGetProcAddressARB((const GLubyte *) "glXMakeContextCurrent");

	// Suppress "-Wunused-variable" for function pointer.
	(void) glXMakeContextCurrentARB;

	const char *displayNameCStr = (displayName.empty() ? NULL : displayName.c_str());
	Display* display = XOpenDisplay( displayNameCStr );

	if(display==NULL)
	{
		if(displayName.empty())
			throw Glip::Exception("createWindowlessContext - Could not open a display (default).", __FILE__, __LINE__, Glip::Exception::GLException);
		else
			throw Glip::Exception("createWindowlessContext - Could not open the display \"" + displayName + "\".", __FILE__, __LINE__, Glip::Exception::GLException);
	}

	static int visualAttribs[] = { None };
	int numberOfFramebufferConfigurations = 0;
	GLXFBConfig* fbConfigs = glXChooseFBConfig( display, DefaultScreen(display), visualAttribs, &numberOfFramebufferConfigurations);

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	GLXContext openGLContext = glXCreateContextAttribsARB( display, fbConfigs[0], 0, True, context_attribs);

	int pbufferAttribs[] = {
		GLX_PBUFFER_WIDTH,  32,
		GLX_PBUFFER_HEIGHT, 32,
		None
	};

	GLXPbuffer pbuffer = glXCreatePbuffer( display, fbConfigs[0], pbufferAttribs );

	// clean up:
	XFree(fbConfigs);
	XSync(display, False);

	if(!glXMakeContextCurrent(display, pbuffer, pbuffer, openGLContext))
		throw Glip::Exception("createWindowlessContext - Could not setup GL context.", __FILE__, __LINE__, Glip::Exception::GLException);
}

