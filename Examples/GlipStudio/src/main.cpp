#include "./mainInterface.hpp"

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
