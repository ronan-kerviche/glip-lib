/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-STUDIO                                                                                               */
/*     IDE for the OpenGL Image Processing LIBrary                                                               */
/*                                                                                                               */
/*     Author        : R. Kerviche                                                                               */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : main.cpp                                                                                  */
/*     Original Date : August 1th 2012                                                                           */
/*                                                                                                               */
/*     Description   : Main.                                                                                     */
/*                                                                                                               */
/* ************************************************************************************************************* */


#include "./GlipStudio.hpp"

int main(int argc, char** argv)
{
	int retCode = 0;

	try
	{
		GlipStudio application(argc, argv);
		retCode = application.exec();
	}
	catch(Exception& e)
	{
		std::cerr << "Exception caught : " << std::endl;
		std::cerr << e.what() << std::endl;
		retCode = -1;
	}

	return retCode;
}
