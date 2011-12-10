// Include
    #include <iostream>
    #include <QApplication>
    #include "GLIPLib.hpp"
    #include "WindowRendering.hpp"

// Namespaces
    using namespace Glip;
    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

int main(int argc, char** argv)
{
    std::cout << "GLIP-Lib Test GL Context" << std::endl;

	try
	{
		// Init QT4 and OpenGL Context
		QApplication app(argc,argv);
		WindowRenderer window(640, 480);

		return app.exec();
	}
	catch(std::exception& e)
	{
	    std::cout << "Exception caught : " << std::endl;
	    std::cout << e.what() << std::endl;
	}
}

