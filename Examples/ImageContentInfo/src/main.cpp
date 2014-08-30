// Include
	#include "imageContentInfo.hpp"
	#include <cstring>

// Namespaces


int main(int argc, char** argv)
{
	std::cout << "GLIP-Lib Test multiple pipelines" << std::endl;

	try
	{
		ImageContentInformation d(argc, argv);
		return d.exec();
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception caught : " << std::endl;
		std::cerr << e.what() << std::endl;
		return -1;
	}
}

