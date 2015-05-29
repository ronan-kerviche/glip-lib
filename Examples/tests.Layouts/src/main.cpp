// Includes
	#include <iostream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QDesktopWidget>
	#include <QtCore/QTimer>
	#include <QGLWidget>

// Namespaces
    using namespace Glip::CoreGL;
    using namespace Glip::CorePipeline;

int main(int argc, char** argv)
{
    std::cout << "Test Layouts" << std::endl;

    QApplication app(argc, argv);

    try
    {
        // Create a filter layout :
        //std::fstream file;
        //file.open("./Filters/gradient2.glsl");
        HdlTextureFormat fmt(640, 480, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);
        ShaderSource src("./Filters/gradient2.glsl");
        FilterLayout fl("TestFilterLayout", fmt, src);

        std::cout << "Type : " << fl.getTypeName() << std::endl;
        std::cout << "Input ports : " << std::endl;
        for(int i=0; i<fl.getNumInputPort(); i++)
            std::cout << "    <" << fl.getInputPortName(i) << '>' << std::endl;
        std::cout << "Output ports : " << std::endl;
        for(int i=0; i<fl.getNumOutputPort(); i++)
            std::cout << "    <" << fl.getOutputPortName(i) << '>' << std::endl;

        // Create a pipeline layout :
        PipelineLayout pl("PipelineTest");
        std::cout << "Name : " << pl.getFullName() << std::endl;
        pl.addInput("Input1");
        pl.addInput("Input2");
        pl.addOutput("Output1");
        pl.addOutput("Output2");
        pl.addOutput("Output3");
        std::cout << "Adding Test1" << std::endl;
        pl.add(fl, "Test1");
        std::cout << "Its name : " << std::endl;
        std::cout << pl.filterLayout(0).getFullName() << std::endl;
        std::cout << pl.componentLayout(0).getFullName() << std::endl;
        std::cout << "Name written" << std::endl;
        pl.add(fl, "Test2");
        std::cout << "Connection à l'entrée" << std::endl;
        pl.connectToInput("Input1", "Test1", "tex0");
        std::cout << "Connection effectuée" << std::endl;

        pl.connectToInput("Input2", "Test2", "tex0");
        pl.connectToOutput("Test1", "red",  "Output1");
        pl.connectToOutput("Test2", "blue", "Output2");
        //pl.connectToOutput("Test1", "red", "Output2"); Exception OK
        pl.connectToOutput("Test1", "green", "Output3");

        std::cout << "Pipeline name : " << pl.getTypeName() << std::endl;
        std::cout << "Checking Pipeline Layout : " << std::endl;
        pl.check();
        std::cout << "Done... (*)" << std::endl;

        std::cout << "Nom (test) : " << pl.componentLayout(0).getFullName() << std::endl;

        std::cout << "OK!" << std::endl;

        // Create a super pipeline layout :
        PipelineLayout PL("SuperPipelineTest");
        std::cout << "* Adding In1" << std::endl;
        PL.addInput("In1");
        std::cout << "* Adding Out1" << std::endl;
        PL.addOutput("Out1");
        std::cout << "* Adding pl1" << std::endl;
        PL.add(pl, "pl1");
        std::cout << "* Adding pl2" << std::endl;
        PL.add(pl, "pl2");
        std::cout << "* Connection 1" << std::endl;
        PL.connectToInput("In1", "pl1", "Input1");
        std::cout << "* Connection 2" << std::endl;
        PL.connectToInput("In1", "pl1", "Input2");
        std::cout << "* Connection 3" << std::endl;
        PL.connectToInput("In1", "pl2", "Input1");
        std::cout << "* Connection 4" << std::endl;
        PL.connectToInput("In1", "pl2", "Input2");
        std::cout << "* Connection 5" << std::endl;
        PL.connectToOutput("pl1", "Output3", "Out1");

        std::cout << "Checking Super Pipeline : " << std::endl;
        PL.check();
        std::cout << "Done..." << std::endl;

        std::cout << "Test index : " << PL.getElementIndex("pl2") << std::endl;
    }
    catch(std::exception& e)
    {
        std::cout << "Caught an exception : " << std::endl << e.what() << std::endl;
    }

    std::cout << "End Test Layouts" << std::endl;

    return 0;
}

