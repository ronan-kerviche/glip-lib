// Includes
	#include <iostream>
	#include "GLIPLib.hpp"
	#include <QApplication>
	#include <QDesktopWidget>
	#include <QtCore/QTimer>

	using namespace Glip;

const char* testKey = " attribute const uniform varying layout centroid flat smooth noperspective patch sample break continue do for while switch case default if else subroutine in out inout float double int void bool true false invariant discard return mat2 mat3 mat4 dmat2 dmat3 dmat4 mat2x2 mat2x3 mat2x4 dmat2x2 dmat2x3 dmat2x4 mat3x2 mat3x3 mat3x4 dmat3x2 dmat3x3 dmat3x4 mat4x2 mat4x3 mat4x4 dmat4x2 dmat4x3 dmat4x4 vec2 vec3 vec4 ivec2 ivec3 ivec4 bvec2 bvec3 bvec4 dvec2 dvec3 dvec4 uint uvec2 uvec3 uvec4 lowp mediump highp precision sampler1D sampler2D sampler3D samplerCube sampler1DShadow sampler2DShadow samplerCubeShadow sampler1DArray sampler2DArray sampler1DArrayShadow sampler2DArrayShadow isampler1D isampler2D isampler3D isamplerCube isampler1DArray isampler2DArray usampler1D usampler2D usampler3D usamplerCube usampler1DArray usampler2DArray sampler2DRect sampler2DRectShadow isampler2DRect usampler2DRect samplerBuffer isamplerBuffer usamplerBuffer sampler2DMS isampler2DMS usampler2DMS sampler2DMSArray isampler2DMSArray usampler2DMSArray samplerCubeArray samplerCubeArrayShadow isamplerCubeArray usamplerCubeArray struct common partition active asm class union enum typedef template this packed goto inline noinline volatile public static extern external interface long short half fixed unsigned superp input output hvec2 hvec3 hvec4 fvec2 fvec3 fvec4 sampler3DRect filter image1D image2D image3D imageCube iimage1D iimage2D iimage3D iimageCube uimage1D uimage2D uimage3D uimageCube image1DArray image2DArray iimage1DArray iimage2DArray uimage1DArray uimage2DArray image1DShadow image2DShadow image1DArrayShadow image2DArrayShadow imageBuffer iimageBuffer uimageBuffer sizeof cast namespace using row_major cast blabla ;";

std::string genLineTest(int l)
{
	std::string res;

	for(int i=1; i<=l; i++)
		res += "Line " + to_string(i) + "\n";

	return res;
}

int main(int argc, char** argv)
{
	std::cout << "Test ShaderSource" << std::endl;

	QApplication app(argc, argv);

	/*//#define __BASIC_TEST__
	//#define __LOG_NVIDIA__
	//#define __LOG_AMDATI__
	#define __LOG_INTEL__

	#ifdef __BASIC_TEST__
		try
		{
			std::string filename = "./Filters/local_max_horiz.glsl";
			//std::fstream file;
			//file.open(filename.c_str());
			Glip::CoreGL::ShaderSource shader(filename);
			//Glip::CoreGL::ShaderSource shader(testKey);
			std::cout << "There are : " << shader.getInputVars().size() <<  " Input variables" << std::endl;
			for(unsigned int i=0; i<shader.getInputVars().size(); i++)
				std::cout << "    -> " << shader.getInputVars()[i] << std::endl;
			std::cout << "There are : " << shader.getOutputVars().size() << " Output variables" << std::endl;
			for(unsigned int i=0; i<shader.getInputVars().size(); i++)
				std::cout << "    -> " << shader.getOutputVars()[i] << std::endl;
		}
		catch(std::exception& e)
		{
			std::cout << "Caught an exception : " << std::endl << e.what() << std::endl;
		}
	#endif

	#ifdef __LOG_NVIDIA__

	#endif

	#ifdef __LOG_AMDATI__
		std::string log = "Fragment shader failed to compile with the following errors:\nERROR: 0:6: 'output' : Reserved word.\nERROR: 0:6: '' : Declaration must include a precision qualifier or the default precision must have been previously declared.\nERROR: 0:15: 'output' : syntax error parse error\nERROR:  compilation errors.  No code generated.\n";

		ShaderSource src(genLineTest(30));
		std::cout << "=> Log INTEL : " << std::endl;
		std::cout << src.errorLog(log) << std::endl;
	#endif

	#ifdef __LOG_INTEL__
		std::string log = "ERROR: 0:15: 'gl_TxCoord' : undeclared identifier\nERROR: 0:15: 'gl_TxCoord' :  left of '[' is not of type array, matrix, or vector\nERROR: 0:15: 't' :  field selection requires structure, vector, or matrix on left hand side\nERROR: 0:17: 'pw' : no matching overloaded function found (using implicit conversion)\nERROR: 0:20: 'x' : undeclared identifier\n";
		ShaderSource src(genLineTest(30));
		std::cout << "=> Log INTEL : " << std::endl;
		std::cout << src.errorLog(log) << std::endl;
	#endif

	std::cout << "End Test ShaderSource" << std::endl;*/

	// New :
	/*ShaderSource src("uniform sampler2D a, b=1, c; \n uniform sampler2D d2, e4=1, f25e; \n out vec2 o1, o2=2, o3; \n out vec4 o4, o5=1; \n uniform float alpha = 6.0f, beta, gamma=1.0f;\n uniform mat4 m1=16.0; uniform mat2 m2 =mat2(0,0,0,0), m3=0;");

	std::cout << "Inputs : " << std::endl;
	for(int k=0; k< src.getInputVars().size(); k++)
		std::cout << k << " > " <<  src.getInputVars()[k] << std::endl;

	std::cout << "Outputs : " << std::endl;
	for(int k=0; k< src.getOutputVars().size(); k++)
		std::cout << k << " > " <<  src.getOutputVars()[k] << std::endl;

	std::cout << "Variables : " << std::endl;
	for(int k=0; k< src.getUniformVars().size(); k++)
		std::cout << k << " > " <<  src.getUniformVars()[k] << " [" << glParamName(src.getUniformTypes()[k]) << "]" << std::endl;*/
	
	// LayoutLoaderParser :
	std::fstream file;
	file.open("tmp.txt");

	// Did it fail?
	if(!file.is_open())
		throw Exception("Can't open file for reading : tmp.txt.", __FILE__, __LINE__);

	// Set starting position
	file.seekg(0, std::ios::beg);

	std::string line, source;
	while(std::getline(file,line))
	{
		source += line;
		source += "\n";
	}

	file.close();

	// Parse : 
	Glip::Modules::VanillaParserSpace::VanillaParser parser(source);

	// Show : 
	std::cout << "Elements : " << std::endl;
	for(std::vector<Glip::Modules::VanillaParserSpace::Element>::iterator p=parser.elements.begin(); p!=parser.elements.end(); p++)
	{
		std::cout << "Element      : " << std::endl;
		std::cout << "   Keyword   : " << (*p).strKeyword << std::endl;

		std::cout << "   Name      : " << (*p).name;
		if( (*p).noName )
			std::cout << '*' << std::endl;
		else
			std::cout << std::endl;

		std::cout << "   Arguments : ";
		for(std::vector<std::string>::iterator q=(*p).arguments.begin(); q!=(*p).arguments.end(); q++)
			std::cout << *q << ", ";
		if( (*p).noArgument )
			std::cout << '*' << std::endl;
		else
			std::cout << std::endl;

		std::cout << "   Body      : " << (*p).body;
		if( (*p).noBody )
			std::cout << '*' << std::endl;
		else
			std::cout << std::endl;
	}

	return 0;
}

