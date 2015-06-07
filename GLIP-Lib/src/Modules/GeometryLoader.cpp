/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE (ronan.kerviche@free.fr)                                                      */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : GeometryLoader.cpp                                                                        */
/*     Original Date : May 29th 2015                                                                             */
/*                                                                                                               */
/*     Description   : Geometry loaders for Wavefront Object (OBJ) and STL file formats.                         */
/*                                                                                                               */
/* ************************************************************************************************************* */

/**
 * \file    GeometryLoader.cpp
 * \brief   Geometry loaders for OBJ and STL file formats.
 * \author  R. KERVICHE
 * \date    May 29th 2015
**/

	// Includes
	#include <sstream>
	#include <cstring>
	#include "Core/Exception.hpp"
	#include "devDebugTools.hpp"
	#include "Modules/GeometryLoader.hpp"

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::CorePipeline::GeometryPrimitives;
	using namespace Glip::Modules;
	
// OBJLoader :
	OBJLoader::OBJLoader(void)
	 :	LayoutLoaderModule(	"LOAD_OBJ_GEOMETRY", 
					"Load a geometry from a Wavefront file (OBJ).\n"
					"Arguments : filename, geometryName[, strict].",
					2,
					3,
					0)
	{ }

	GLfloat OBJLoader::readNextNumber(const std::string& line, size_t& p, const int lineNumber, const std::string& sourceName)
	{
		const std::string spaces = " \t\r\n\f\v",
				  digits = ".+-,e0123456789";

		size_t b = line.find_first_not_of(spaces, p);
		
		if(b==std::string::npos)
			throw Exception("Cannot find coordinate start.", sourceName, lineNumber, Exception::ClientScriptException);

		if(digits.find(line[b])==std::string::npos)
			throw Exception("Cannot read digit.", sourceName, lineNumber, Exception::ClientScriptException);

		size_t e = line.find_first_not_of(digits, b);
		e = (e==std::string::npos) ? line.size() : e;

		GLfloat res = 0.0f;
		if(!fromString(line.substr(b, std::max(e-b,static_cast<size_t>(1))), res))
			throw Exception("Cannot read number.", sourceName, lineNumber, Exception::ClientScriptException);

		p = e;
		return res;
	}

	void OBJLoader::readFaceComponent(const std::string& line, size_t& p, std::vector<GLuint>& v, std::vector<GLuint>& n, std::vector<GLuint>& t, const int lineNumber, const std::string& sourceName)
	{
		const std::string spaces = " \t\r\n\f\v",
				  digits = "0123456789";

		size_t b = line.find_first_not_of(spaces, p);
		if(b==std::string::npos)
			throw Exception("Cannot find index start.", sourceName, lineNumber, Exception::ClientScriptException);
		if(digits.find(line[b])==std::string::npos)
			throw Exception("Cannot read digit.", sourceName, lineNumber, Exception::ClientScriptException);

		size_t e = line.find_first_not_of(digits, b);
		e = (e==std::string::npos) ? line.size() : e;
		
		GLuint	v1 = 0,
			v2 = 0,
			v3 = 0;
		//bool	hasSecond = false,
		//	hasThird = false;
		if(!fromString(line.substr(b, std::max(e-b,static_cast<size_t>(1))), v1))
			throw Exception("Cannot read number (first).", sourceName, lineNumber, Exception::ClientScriptException);
		
		if(e<line.size() && line[e]=='/')
		{
			//hasSecond = true;
			b = e + 1;

			if(b>=line.size())
				throw Exception("Cannot find number start (second).", sourceName, lineNumber, Exception::ClientScriptException);

			e = line.find_first_not_of(digits, b);
			e = (e==std::string::npos) ? line.size() : e;

			if(e>b)
			{
				if(!fromString(line.substr(b, std::max(e-b,static_cast<size_t>(1))), v2))
					throw Exception("Cannot read number (second).", sourceName, lineNumber, Exception::ClientScriptException);
			}

			if(e<line.size() && line[e]=='/')
			{
				//hasThird = true;
				b = e + 1;

				if(b>=line.size())
					throw Exception("Cannot find number start (third).", sourceName, lineNumber, Exception::ClientScriptException);

				e = line.find_first_not_of(digits, b);
				e = (e==std::string::npos) ? line.size() : e;

				if(e>b)
				{
					if(!fromString(line.substr(b, std::max(e-b,static_cast<size_t>(1))), v3))
						throw Exception("Cannot read number (third).", sourceName, lineNumber, Exception::ClientScriptException);
				}
			}
		}

		v.push_back(v1);
		t.push_back(v2);
		n.push_back(v3);
		p = line.find_first_of(spaces, e);
		p = line.find_first_not_of(spaces, p);
	}

	void OBJLoader::processLine(const std::string& line, UnshapedData& data, const bool strict, const int lineNumber, const std::string& sourceName, std::vector<GLuint>& vertexIndicesBuffer, std::vector<GLuint>& normalIndicesBuffer, std::vector<GLuint>& textureIndicesBuffer)
	{
		const std::string 	spaces = " \t\r\n\f\v",
					delimiters = " \t\r\n\f\v#/";
		
		size_t	p1 = std::string::npos,
			p2 = std::string::npos;

		p1 = line.find_first_not_of(spaces);
		p2 = line.find_first_of(delimiters, p1+1);
	
		// Empty line : 
		if(p1==std::string::npos)
			return ;

		const std::string header = line.substr(p1, std::max(p2-p1,static_cast<size_t>(1)));
		p1 = p2;

		// Comment line : 
		if(header=="#")
			return ;
		else if(header=="v")
		{
			GLfloat	x = readNextNumber(line, p1, lineNumber, sourceName),
				y = readNextNumber(line, p1, lineNumber, sourceName),
				z = readNextNumber(line, p1, lineNumber, sourceName);
			data.x.push_back(x);
			data.y.push_back(y);
			data.z.push_back(z);

			if(strict && line.find_first_not_of(spaces, p1)!=std::string::npos)
				throw Exception("Cannot read trailing information for vertex.", sourceName, lineNumber, Exception::ClientScriptException);
		}
		else if(header=="vn")
		{
			GLfloat	nx = readNextNumber(line, p1, lineNumber, sourceName),
				ny = readNextNumber(line, p1, lineNumber, sourceName),
				nz = readNextNumber(line, p1, lineNumber, sourceName);
			data.nx.push_back(nx);
			data.ny.push_back(ny);
			data.nz.push_back(nz);
			data.hasNormals = true;

			if(strict && line.find_first_not_of(spaces, p1)!=std::string::npos)
				throw Exception("Cannot read trailing information for normal.", sourceName, lineNumber, Exception::ClientScriptException);
		}
		else if(header=="vt")
		{
			GLfloat u = readNextNumber(line, p1, lineNumber, sourceName),
				v = readNextNumber(line, p1, lineNumber, sourceName);
			data.u.push_back(u);
			data.v.push_back(v);
			data.hasTexCoords = true;

			if(strict && line.find_first_not_of(spaces, p1)!=std::string::npos)
				throw Exception("Cannot read trailing information for texture coordinate.", sourceName, lineNumber, Exception::ClientScriptException);
		}
		else if(header=="f")
		{
			/*std::vector<GLuint> 	vertexIndices, 
						normalIndices,
						textureIndices;
			vertexIndices.reserve(3);
			normalIndices.reserve(3);
			textureIndices.reserve(3);*/

			// Ensure 3 reads : 
			readFaceComponent(line, p1, vertexIndicesBuffer, normalIndicesBuffer, textureIndicesBuffer, lineNumber, sourceName);
			readFaceComponent(line, p1, vertexIndicesBuffer, normalIndicesBuffer, textureIndicesBuffer, lineNumber, sourceName);
			readFaceComponent(line, p1, vertexIndicesBuffer, normalIndicesBuffer, textureIndicesBuffer, lineNumber, sourceName);

			// Continue while necessary : 
			while(p1!=std::string::npos)
				readFaceComponent(line, p1, vertexIndicesBuffer, normalIndicesBuffer, textureIndicesBuffer, lineNumber, sourceName);
			
			// Generate triangles from it :
			if(vertexIndicesBuffer.size()<=2)
				throw Exception("Unable to process face having less than three vertives.", sourceName, lineNumber, Exception::ClientScriptException);
			else if(vertexIndicesBuffer.size()==3)
			{
				data.av.push_back(vertexIndicesBuffer[0]);
				data.an.push_back(normalIndicesBuffer[0]);
				data.at.push_back(textureIndicesBuffer[0]);	
				data.bv.push_back(vertexIndicesBuffer[1]);
				data.bn.push_back(normalIndicesBuffer[1]);
				data.bt.push_back(textureIndicesBuffer[1]);
				data.cv.push_back(vertexIndicesBuffer[2]);
				data.cn.push_back(normalIndicesBuffer[2]);
				data.ct.push_back(textureIndicesBuffer[2]);
			}
			else
			{
				// Split convex polygons into triangles : 
				for(unsigned int k=2; k<vertexIndicesBuffer.size(); k++)
				{
					data.av.push_back(vertexIndicesBuffer[0]);
					data.an.push_back(normalIndicesBuffer[0]);
					data.at.push_back(textureIndicesBuffer[0]);	
					data.bv.push_back(vertexIndicesBuffer[k-1]);
					data.bn.push_back(normalIndicesBuffer[k-1]);
					data.bt.push_back(textureIndicesBuffer[k-1]);
					data.cv.push_back(vertexIndicesBuffer[k]);
					data.cn.push_back(normalIndicesBuffer[k]);
					data.ct.push_back(textureIndicesBuffer[k]);
				}
			}

			// And clear the buffers :
			vertexIndicesBuffer.clear();
			normalIndicesBuffer.clear();
			textureIndicesBuffer.clear();
		}
		else if(strict)
			throw Exception("Unable to process token \"" + header + "\".", sourceName, lineNumber, Exception::ClientScriptException);
	}

	void OBJLoader::completeModelData(UnshapedData& data)
	{
		#define COMPLETE_INDICES(AV, AN, AT)	for(unsigned int k=0; k<data.AV.size(); k++) \
							{ \
								if(data.hasNormals && data.AN[k]==0) data.AN[k] = data.AV[k]; \
								if(data.hasNormals && data.AT[k]==0) data.AT[k] = data.AV[k]; \
							}

		COMPLETE_INDICES(av, an, at)
		COMPLETE_INDICES(bv, bn, bt)
		COMPLETE_INDICES(cv, cn, ct)

		#undef COMPLETE_INDICES
	}

	bool OBJLoader::testIndices(const UnshapedData& data)
	{
		// Test sizes first : 
		if(data.x.size()!=data.y.size() || data.x.size()!=data.z.size() || ((data.nx.size()!=data.ny.size() || data.nx.size()!=data.nz.size()) && data.hasNormals) || ((data.u.size()!=data.v.size()) && data.hasTexCoords))
		{
			//#ifdef __VERBOSE__
				std::cout << "OBJLoader::testIndices - Error type A." << std::endl;
			//#endif
			return false;
		}
		
		if(data.av.size()!=data.bv.size() || data.av.size()!=data.cv.size() 
			|| ((data.av.size()!=data.an.size() || data.av.size()!=data.bn.size() || data.av.size()!=data.cn.size()) && data.hasNormals)
			|| ((data.av.size()!=data.at.size() || data.av.size()!=data.bt.size() || data.av.size()!=data.ct.size()) && data.hasTexCoords))
		{
			//#ifdef __VERBOSE__
				std::cout << "OBJLoader::testIndices - Error type B." << std::endl;
			//#endif
			return false;
		}

		// Test indices : 
		for(unsigned int k=0; k<data.av.size(); k++)
		{
			if( 	(data.av[k]<=0 || data.bv[k]<=0 || data.cv[k]<=0 || data.av[k]>data.x.size() || data.bv[k]>data.x.size() || data.cv[k]>data.x.size())
				|| (data.hasNormals && (data.an[k]<=0 || data.bn[k]<=0 || data.cn[k]<=0 || data.an[k]>data.nx.size() || data.bn[k]>data.nx.size() || data.cn[k]>data.nx.size()))
				|| (data.hasTexCoords && (data.at[k]<=0 || data.bt[k]<=0 || data.ct[k]<=0 || data.at[k]>data.u.size() || data.bt[k]>data.u.size() || data.ct[k]>data.u.size())) )
			{
				//#ifdef __VERBOSE__
					std::cout << "OBJLoader::testIndices - Error type C." << std::endl;
					std::cout << "    Error for Face " << k << "/" << data.av.size() << std::endl;
					std::cout << "    " << data.av[k] << ' ' << data.bv[k] << ' ' << data.cv[k] << " [" << data.x.size() << ']' << std::endl;
					std::cout << "    hasNormals : " << data.hasNormals << std::endl;
					if(data.hasNormals)
						std::cout << "    " << data.an[k] << ' ' << data.bn[k] << ' ' << data.cn[k] << " [" << data.nx.size() << ']' << std::endl;
					std::cout << "    hasTexCoords : " << data.hasTexCoords << std::endl;
					if(data.hasTexCoords)
						std::cout << "    " << data.at[k] << ' ' << data.bt[k] << ' ' << data.ct[k] << " [" << data.u.size() << ']' << std::endl;
				//#endif
				return false;
			}
		}

		// Ok : 
		return true;
	}

	void OBJLoader::reorderData(UnshapedData& data)
	{
		// Augment the unshaped data by generating all the needed triplet of vertices/normals/textures required by the object.
		// After this step, the 

		if(data.hasTexCoords || data.hasNormals)
		{
			// Generate an indices map with unique pairs of vertices/textures : 
			std::map<GLuint, std::map<GLuint, std::map<GLuint, GLuint> > > indicesMap;
			std::vector<GLfloat> 	_x,
						_y,
						_z,
						_nx,
						_ny,
						_nz,
						_u,
						_v;
			std::vector<GLuint> 	a,
						b,
						c;

			const size_t nReserve = data.x.size()+data.av.size(); // very rough estimate.
			_x.reserve(nReserve);
			_y.reserve(nReserve);
			_z.reserve(nReserve);
			_nx.reserve(nReserve);
			_ny.reserve(nReserve);
			_nz.reserve(nReserve);
			_u.reserve(nReserve);
			_v.reserve(nReserve);
			a.reserve(data.av.size());
			b.reserve(data.av.size());
			c.reserve(data.av.size());
	
			#define PROCESS(AV, AN, AT, AF)	for(size_t k=0; k<data.AV.size(); k++) \
							{ \
								std::map<GLuint, std::map<GLuint, std::map<GLuint, GLuint> > >::iterator it1; \
								std::map<GLuint, std::map<GLuint, GLuint> >::iterator it2; \
								std::map<GLuint, GLuint>::iterator it3; \
								bool found=false; \
								it1 = indicesMap.find(data.AV[k]); \
								if(it1!=indicesMap.end()) \
								{ \
									it2 = it1->second.find(data.AN[k]); \
									if(it2!=it1->second.end()) \
									{ \
										it3 = it2->second.find(data.AT[k]); \
										if(it3!=it2->second.end()) \
										{ \
											found = true; \
											AF.push_back(it3->second); \
										} \
										else \
											it2->second[data.AT[k]] = _x.size(); \
									} \
									else \
										it1->second[data.AN[k]][data.AT[k]] = _x.size(); \
								} \
								else \
									indicesMap[data.AV[k]][data.AN[k]][data.AT[k]] = _x.size(); \
								if(!found) \
								{ \
									AF.push_back(_x.size()); \
									_x.push_back(data.x[data.AV[k]-1]); \
									_y.push_back(data.y[data.AV[k]-1]); \
									_z.push_back(data.z[data.AV[k]-1]); \
									if(data.hasNormals) \
									{ \
										_nx.push_back(data.nx[data.AN[k]-1]); \
										_ny.push_back(data.ny[data.AN[k]-1]); \
										_nz.push_back(data.nz[data.AN[k]-1]); \
									} \
									if(data.hasTexCoords) \
									{ \
										_u.push_back(data.u[data.AT[k]-1]); \
										_v.push_back(data.v[data.AT[k]-1]); \
									} \
								} \
							}

			PROCESS(av, an, at, a)
			PROCESS(bv, bn, bt, b)
			PROCESS(cv, cn, ct, c)

			#undef PROCESS

			// Debug display : 
			//for(size_t k=0; k<a.size(); k++)
			//{
			//	if(data.av[k]!=indicesMap[a[k]].first || data.at[k]!=indicesMap[a[k]].second)
			//		std::cout << "A : " << data.av[k] << ' ' << data.at[k] << " >> " << a[k] << " : (" << indicesMap[a[k]].first << "; " << indicesMap[a[k]].second << ")" << std::endl;
			//	if(data.bv[k]!=indicesMap[b[k]].first || data.bt[k]!=indicesMap[b[k]].second)
			//		std::cout << "B : " << data.bv[k] << ' ' << data.bt[k] << " >> " << b[k] << " : (" << indicesMap[b[k]].first << "; " << indicesMap[b[k]].second << ")" << std::endl;
			//	if(data.cv[k]!=indicesMap[c[k]].first || data.ct[k]!=indicesMap[c[k]].second)
			//		std::cout << "C : " << data.cv[k] << ' ' << data.ct[k] << " >> " << c[k] << " : (" << indicesMap[c[k]].first << "; " << indicesMap[c[k]].second << ")" << std::endl;
			//}

			// Replace : 
			data.x = _x;
			data.y = _y;
			data.z = _z;
			data.nx = _nx;
			data.ny = _ny;
			data.nz = _nz;
			data.u = _u;
			data.v = _v;
			data.av = a;
			data.bv = b;
			data.cv = c;
		}
		else
		{
			// Simply shift the indices : 
			for(unsigned int k=0; k<data.av.size(); k++)
			{
				data.av[k]--;
				data.bv[k]--;
				data.cv[k]--;
			}
		}

		// Normals and texture indices are not needed anymore : 
		data.an.clear();
		data.bn.clear();
		data.cn.clear();
		data.at.clear();
		data.bt.clear();
		data.ct.clear();
	}

	LAYOUT_LOADER_MODULE_APPLY_IMPLEMENTATION( OBJLoader )
	{
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(pipelineList)
		UNUSED_PARAMETER(mainPipelineName)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(body)
		UNUSED_PARAMETER(bodyLine)
		UNUSED_PARAMETER(executionCode)

		GEOMETRY_MUST_NOT_EXIST(arguments[1])

		bool strict = false;

		if(arguments.size()>=3)
			strict = LayoutLoaderModule::getBoolean(arguments[2], sourceName, startLine);

		const std::string& filename = arguments[0];
		std::vector<std::string> possibleFilenames = findFile(filename, dynamicPaths);

		// Show some error : 
		if(possibleFilenames.empty())
		{
			if(dynamicPaths.empty())
				throw Exception("Unable to load file \"" + filename + "\" from the current location.", sourceName, startLine, Exception::ClientScriptException);
			else
			{			
				Exception ex("Unable to load file \"" + filename + "\" from the following locations : ", sourceName, startLine, Exception::ClientScriptException);
				for(std::vector<std::string>::const_iterator it=dynamicPaths.begin(); it!=dynamicPaths.end(); it++)
				{
					if(it->empty())
						ex << Exception("-> [./]", sourceName, startLine, Exception::ClientScriptException);
					else
						ex << Exception("-> " + *it, sourceName, startLine, Exception::ClientScriptException);
				}
				throw ex;
			}
		}
		else if(possibleFilenames.size()>1)
		{
			Exception ex("Ambiguous link : file \"" + filename + "\" was found in multiple locations, with different sources : ", sourceName, startLine, Exception::ClientScriptException);
			for(std::vector<std::string>::const_iterator it=possibleFilenames.begin(); it!=possibleFilenames.end(); it++)
				ex << Exception("-> " + *it, sourceName, startLine, Exception::ClientScriptException);
			throw ex;
		}	

		APPEND_NEW_GEOMETRY(arguments[1], load(possibleFilenames.front(), strict))
	}

	/**
	\fn CustomModel OBJLoader::load(const std::string& filename, const bool strict)
	\brief Load geometry from an Wavefront Object file.
	\param filename File to be loaded.
	\param strict If true, any error, such as unknown section, will raise an exception.
	\return A constructed geometry model.
	**/
	CustomModel OBJLoader::load(const std::string& filename, const bool strict)
	{
		std::ifstream file;
		file.open(filename.c_str());

		if(!file.is_open() || !file.good() || file.fail())
			throw Exception("OBJLoader::load - Could not open file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);
		
		const unsigned int initialReserve = 65536; 
		UnshapedData data;
		data.hasNormals = false;
		data.hasTexCoords = false;
		data.x.reserve(initialReserve);
		data.y.reserve(initialReserve);
		data.z.reserve(initialReserve);
		data.nx.reserve(initialReserve);
		data.ny.reserve(initialReserve);
		data.nz.reserve(initialReserve);
		data.u.reserve(initialReserve);
		data.v.reserve(initialReserve);
		data.av.reserve(initialReserve);
		data.an.reserve(initialReserve);
		data.at.reserve(initialReserve);
		data.bv.reserve(initialReserve);
		data.bn.reserve(initialReserve);
		data.bt.reserve(initialReserve);
		data.cv.reserve(initialReserve);
		data.cn.reserve(initialReserve);
		data.ct.reserve(initialReserve);

		// Buffer for the faces data (should mostly need only 3 components) :
		std::vector<GLuint>	vertexIndicesBuffer,
					normalIndicesBuffer,
					textureIndicesBuffer;
		vertexIndicesBuffer.reserve(8);
		normalIndicesBuffer.reserve(8);
		textureIndicesBuffer.reserve(8);

		int lineNumber = 1;
		std::string line;
		file.seekg(0, std::ios::beg);

		while(std::getline(file,line))
		{
			processLine(line, data, strict, lineNumber, filename, vertexIndicesBuffer, normalIndicesBuffer, textureIndicesBuffer);
			lineNumber++;
		}
		file.close();

		// Complete : 
		completeModelData(data);

		// Test : 		
		if(!testIndices(data))
			throw Exception("OBJLoader::load - Inconsistency found while parsing the file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

		// Create the model from the parsed data :
		reorderData(data);
		CustomModel model(3, GL_TRIANGLES, data.hasNormals, data.hasTexCoords);

		// Copy : 
		const GLfloat	*nx = ((data.hasNormals) ? (&data.nx.front()) : NULL),
				*ny = ((data.hasNormals) ? (&data.ny.front()) : NULL),
				*nz = ((data.hasNormals) ? (&data.nz.front()) : NULL),
				*u = ((data.hasTexCoords) ? (&data.u.front()) : NULL),
				*v = ((data.hasTexCoords) ? (&data.v.front()) : NULL);
		model.newVertices3D(data.x.size(), &data.x.front(), &data.y.front(), &data.z.front(), nx, ny, nz, u, v);
		model.newElements(data.av.size(), &data.av.front(), &data.bv.front(), &data.cv.front());

		// Final test :
		if(!model.testIndices())
			throw Exception("OBJLoader::load - Data parsing invalid for file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);
		return model;
	}

// STLLoader :
	STLLoader::STLLoader(void)
	 :	LayoutLoaderModule(	"LOAD_STL_GEOMETRY", 
					"Load a geometry from a StereoLithography file (STL).\n"
					"Arguments : filename, geometryName.",
					2,
					2,
					0)
	{ }

	LAYOUT_LOADER_MODULE_APPLY_IMPLEMENTATION( STLLoader )
	{
		UNUSED_PARAMETER(currentPath)
		UNUSED_PARAMETER(formatList)
		UNUSED_PARAMETER(sourceList)
		UNUSED_PARAMETER(filterList)
		UNUSED_PARAMETER(pipelineList)
		UNUSED_PARAMETER(mainPipelineName)
		UNUSED_PARAMETER(staticPaths)
		UNUSED_PARAMETER(requiredFormatList)
		UNUSED_PARAMETER(requiredGeometryList)
		UNUSED_PARAMETER(requiredPipelineList)
		UNUSED_PARAMETER(body)
		UNUSED_PARAMETER(bodyLine)
		UNUSED_PARAMETER(executionCode)

		GEOMETRY_MUST_NOT_EXIST(arguments[1])

		const std::string& filename = arguments[0];
		std::vector<std::string> possibleFilenames = findFile(filename, dynamicPaths);

		// Show some error : 
		if(possibleFilenames.empty())
		{
			if(dynamicPaths.empty())
				throw Exception("Unable to load file \"" + filename + "\" from the current location.", sourceName, startLine, Exception::ClientScriptException);
			else
			{			
				Exception ex("Unable to load file \"" + filename + "\" from the following locations : ", sourceName, startLine, Exception::ClientScriptException);
				for(std::vector<std::string>::const_iterator it=dynamicPaths.begin(); it!=dynamicPaths.end(); it++)
				{
					if(it->empty())
						ex << Exception("-> [./]", sourceName, startLine, Exception::ClientScriptException);
					else
						ex << Exception("-> " + *it, sourceName, startLine, Exception::ClientScriptException);
				}
				throw ex;
			}
		}
		else if(possibleFilenames.size()>1)
		{
			Exception ex("Ambiguous link : file \"" + filename + "\" was found in multiple locations, with different sources : ", sourceName, startLine, Exception::ClientScriptException);
			for(std::vector<std::string>::const_iterator it=possibleFilenames.begin(); it!=possibleFilenames.end(); it++)
				ex << Exception("-> " + *it, sourceName, startLine, Exception::ClientScriptException);
			throw ex;
		}	

		APPEND_NEW_GEOMETRY(arguments[1], load(possibleFilenames.front()))
	}

	/**
	\fn CustomModel STLLoader::load(const std::string& filename)
	\brief Load geometry from a StereoLithography file.
	\param filename File to be loaded.
	\return A constructed geometry model.
	**/
	CustomModel STLLoader::load(const std::string& filename)
	{
		std::ifstream file;
		file.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);

		if(!file.is_open() || !file.good() || file.fail())
			throw Exception("STLLoader::load - Could not open file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

		const size_t bufferSize = 255;
		char buffer[bufferSize];
		const float* ptr = reinterpret_cast<float*>(buffer);
		std::memset(buffer, 0, bufferSize);

		// Read the header (80 bytes) : 
		file.read(buffer, 80);
		if(!file)
			throw Exception("STLLoader::load - Could not read header of file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);
		const std::string headerString(buffer, 5);
		if(headerString=="solid")
			throw Exception("STLLoader::load - File \"" + filename + "\" is an ASCII STL.", __FILE__, __LINE__, Exception::ModuleException);

		// Read the number of triangles : 
		unsigned int numTriangles = 0;
		file.read(reinterpret_cast<char*>(&numTriangles), sizeof(unsigned int));
		if(!file)
			throw Exception("STLLoader::load - Could not read the number of triangles in file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

		// Allocation : 
		CustomModel model(3, GL_TRIANGLES, true, false);
		model.reserveVertices(3*numTriangles);
		model.reserveElements(numTriangles);

		// Read : 
		const size_t elementSize = 12*sizeof(float)+2;
		for(unsigned int k=0; k<numTriangles; k++)
		{
			file.read(buffer, elementSize);
			if(k<(numTriangles-1) && !file)
				throw Exception("STLLoader::load - Could not read triangle data " + toString(k) + " of " + toString(numTriangles) + "  in file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);

			float nx, ny, nz, x, y, z;
			nx = ptr[0];
			ny = ptr[1];
			nz = ptr[2];

			for(int p=0; p<3; p++)
			{
				x = ptr[(p+1)*3+0];
				y = ptr[(p+1)*3+1];
				z = ptr[(p+1)*3+2];
				model.newVertex3D(x, y, z, nx, ny, nz);
			}
			model.newElement(k*3+0, k*3+1, k*3+2);
		}
		// Final test :
		if(!model.testIndices())
			throw Exception("STLLoader::load - Data parsing invalid for file \"" + filename + "\".", __FILE__, __LINE__, Exception::ModuleException);
		return model;
	}


