#include "Test_ffmpeg.hpp"

int main(int argc, char** argv)
{
	std::cout << "GLIP-Lib Test FFMPEG" << std::endl;

	try
	{
		TestFFMPEGApplication d(argc, argv);
		return d.exec();
	}
	catch(std::exception& e)
	{
		std::cout << "Exception caught : " << std::endl;
		std::cout << e.what() << std::endl;
	}
}
