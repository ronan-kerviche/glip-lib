// Include
	#include "imageContentInfo.hpp"
	#include <cstring>

// Namespaces


int main(int argc, char** argv)
{
	std::cout << "GLIP-Lib Test multiple pipelines" << std::endl;

	try
	{
		ImageContentInformation* d = new ImageContentInformation(800, 600, argc, argv);
		return d->exec();
	}
	catch(std::exception& e)
	{
		std::cout << "Exception caught : " << std::endl;
		std::cout << e.what() << std::endl;
	}
}

