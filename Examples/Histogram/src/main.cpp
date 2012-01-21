#include "header.hpp"

int main(int argc, char** argv)
{
	std::cout << "GLIP-Lib's Histogram code sample" << std::endl;

	try
	{
		HistogramApplication app(argc, argv);
		std::cout << "-- Start --" << std::endl;
		return app.exec();
	}
	catch(std::exception& e)
	{
		std::cerr << "Error : " << e.what() << std::endl;
	}
}

