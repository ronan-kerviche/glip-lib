// Include
	#include "gameOfLife.hpp"

// Namespaces


int main(int argc, char** argv)
{
	std::cout << "GLIP-Lib Test GL Context" << std::endl;

	try
	{
		// Init QT4 and OpenGL Context
		GameOfLife g(640, 480, argc, argv);

		return g.exec();
	}
	catch(std::exception& e)
	{
		std::cout << "Exception caught : " << std::endl;
		std::cout << e.what() << std::endl;
	}
}

