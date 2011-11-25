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

		// Create a pipeline
		std::fstream file;
		file.open("./Filters/gradient2.glsl");
		if(!file.is_open())
		    throw Exception("UPPER LEVEL - File not opened", __FILE__, __LINE__);
		HdlTextureFormat fmt(640, 480, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
		ShaderSource src(file, "./Filters/gradient2.glsl");
		FilterLayout fl("TestFilterLayout", fmt, src);

		std::cout << "Type : " << fl.getType() << std::endl;
		std::cout << "Input ports : " << std::endl;
		for(int i=0; i<fl.getNumInputPort(); i++)
		    std::cout << "    <" << fl.getInputPortName(i) << '>' << std::endl;
		std::cout << "Output ports : " << std::endl;
		for(int i=0; i<fl.getNumOutputPort(); i++)
		    std::cout << "    <" << fl.getOutputPortName(i) << '>' << std::endl;

		PipelineLayout pl("PipelineTest");
		pl.addInput("Input1");
		pl.addInput("Input2");
		pl.addOutput("Output1");
		pl.addOutput("Output2");
		pl.addOutput("Output3");
		pl.add(fl, "Test1");
		pl.add(fl, "Test2");
		pl.connectToInput("Input1", "Test1", "tex0");
		pl.connectToInput("Input2", "Test2", "tex0");
		pl.connectToOutput("Test1", "red",  "Output1");
		pl.connectToOutput("Test2", "blue", "Output2");
		pl.connectToOutput("Test1", "green", "Output3");

		std::cout << ">> Creating the pipeline..." << std::endl;

		Pipeline p(pl, "TestPipeline");

		std::cout << " === Creating test ended successfully === " << std::endl;
		return app.exec();
	}
	catch(std::exception& e)
	{
	    std::cout << "Exception caught : " << std::endl;
	    std::cout << e.what() << std::endl;
	}
}

