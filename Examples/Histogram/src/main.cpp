#include "header.hpp"
#include "WindowRendering.hpp"


int main(int argc, char** argv) 
{
    std::cout << "GLIP-Lib's Histogram code sample" << std::endl;

	if(argc<=2)
	{
		std::cerr << "Too few arguments to start : try 0 and VYUV for a built-in webcam!" << std::endl;
		return -1;
	}
	else
	{
		// Init GLIP-LIB's StreamManager 
		StreamManager mainManager;

		// Init QT4 and OpenGL Context
		QApplication app(argc, argv);
		WindowRenderer window(640, 480); 

		// Init OpenGL Handles and GLIP-LIB
		HandleOpenGL::init();
		UnicapHub();

		std::cout << "Found " << UnicapHub::getNumDevices() << " devices" << std::endl;
		if( !HdlInfo::instance()->init("extentionsNeeded.list") )
		{
			std::cerr << "A needed OpenGL extension is not available on your Hardware" << std::endl;
			std::cerr << "The program will abort..." << std::endl;
			return -1;
		}
	
		// Create the Application 
		std::cout << "Camera : " << (int)(argv[1][0]-'0') << std::endl;
		std::cout << "FOURCC : " << argv[2] << std::endl;
		
		ApplicationWebcam appli((int)(argv[1][0]-'0'), getFOURCC(argv[2])); 

		// Connect Processing part to Display Part 
		StreamManager::connect(window.in(0), appli.out(1)); 

		// Debug
		StreamManager::debugInformation();
  
		// Run
		if( appli.isEnabled() )
    		return app.exec();
		else
		{
			std::cerr << "Can't run the program - bad camera settings?" << std::endl;
			return -1;
		}
	}      
}       
 
