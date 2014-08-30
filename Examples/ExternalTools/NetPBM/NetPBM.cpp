#include "NetPBM.hpp"
#include <cstring>
#include <cmath>

	unsigned short changeEndianness16(unsigned short val)
	{
		return 	 (val << 8) |          // left-shift always fills with zeros
			((val >> 8) & 0x00ff); // right-shift sign-extends, so force to zero
	}

	bool nextInteger(unsigned int& val, char* buffer, unsigned int& p, const unsigned int bufferLength)
	{
		bool isComment = false;
		unsigned int k=0;
		std::locale loc;

		while( (isspace(buffer[p+k],loc) || isComment || buffer[p+k]=='#') && (p+k)<bufferLength )
		{
			if(buffer[p+k]=='#')
				isComment = true;
			else if(buffer[p+k]=='\n')
				isComment = false;

			k++;
		}

		if(p+k>=bufferLength)
			return false;

		std::string value;

		while( !isspace(buffer[p+k],loc) && buffer[p+k]!='#' && p+k<bufferLength )
		{
			value += buffer[p+k];
			k++;
		}

		p += k;

		return fromString(value, val);
	}

	ImageBuffer* NetPBM::loadNetPBMFile(const std::string& filename)
	{
		std::fstream file(filename.c_str(), std::ios_base::in | std::ifstream::binary);

		if(!file.is_open())
			throw Exception("NetPBM::loadNetPBMFile - Cannot read file " + filename + ".", __FILE__, __LINE__);

		 // get length of file :
		file.seekg (0, file.end);
		unsigned int length = file.tellg();
		file.seekg (0, file.beg);

		// allocate memory :
		char* buffer = new char[length];

		// read data as a block :
		file.read(buffer, length);

		file.close();

		// Read the header : 
		std::string head(buffer, 2);

		HdlTextureFormat format(0, 0, GL_NONE, GL_NONE);

		if(head=="P5")
			format.setGLMode(GL_LUMINANCE); //GL_LUMINANCE16_SNORM does not work, use GL_R16_SNORM instead.
		else if(head=="P6")
			format.setGLMode(GL_RGB);
		else
		{
			delete[] buffer;
			throw Exception("NetPBM::loadNetPBMFile - Unknown header \"" + head + "\".", __FILE__, __LINE__);
		}

		bool test = true;

		unsigned int p = 2, width, height, maxval;
		test = nextInteger(width, buffer, p, length);

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::loadNetPBMFile - Unable to read width from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		test = nextInteger(height, buffer, p, length);

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::loadNetPBMFile - Unable to read height from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		test = nextInteger(maxval, buffer, p,length);		

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::loadNetPBMFile - Unable to read maxval from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		// Set the value in the format : 
		format.setWidth(width);
		format.setHeight(height);

		int precision = std::log(maxval)/std::log(2.0);
		
		if(precision<=8)
			format.setGLDepth(GL_UNSIGNED_BYTE);
		else if(precision<=16)
		{
			format.setGLMode(GL_LUMINANCE16);
			format.setGLDepth(GL_UNSIGNED_SHORT);
		}		
		else
			throw Exception("NetPBM::loadNetPBMFile - Unable to create buffer for a precision of " + toString(precision) + " bits.", __FILE__, __LINE__);

		p++;

		// Test body : 
		if(length-p!=format.getSize())
		{
			delete[] buffer;
			throw Exception("NetPBM::loadNetPBMFile - Body size mismatch (expectation : " + toString(format.getSize()) + "; File : " + toString(length-p) + ").", __FILE__, __LINE__);
		}

		// Change endianess (see http://netpbm.sourceforge.net/doc/pamendian.html) :
		int n = 1; 
		if((*(char*)&n==1) && precision>8) // The machine is little endian and the data is on more than 8 bits.
		{
			unsigned short* tbytes = reinterpret_cast<unsigned short*>(buffer + p);

			for(int k=0; k<format.getNumElements(); k++)
				tbytes[k] = changeEndianness16(tbytes[k]);
		}

		ImageBuffer* result = new ImageBuffer(format);
		(*result) << (buffer + p);

		// Clean : 
		delete[] buffer;

		return result;
	}

	void NetPBM::saveNetPBMToFile(const ImageBuffer& image, const std::string& filename)
	{
		std::fstream file(filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

		if(!file.is_open())
			throw Exception("NetPBM::saveNetPBMToFile - Cannot write file " + filename + ".", __FILE__, __LINE__);

		// make header : 
		std::string header;

		if(image.getNumChannels()==1)
			header += "P5\n";
		else if(image.getNumChannels()==3)
			header += "P6\n";
		else
		{
			file.close();
			throw Exception("NetPBM::saveNetPBMToFile - Internal error : number of layers is invalid.", __FILE__, __LINE__);
		}

		header += "#GlipLib NetPBM Writer\n";
		header += toString(image.getWidth()) + " " + toString(image.getHeight()) + "\n";

		if(image.getChannelDepth()<=1)
			header += "255\n";
		else if(image.getChannelDepth()<=2)
			header += "65535\n";
		else
			throw Exception("NetPBM::saveNetPBMToFile - Incompatible depth : \"" + glParamName(image.getGLDepth()) + "\".", __FILE__, __LINE__);

		// Write : 
		file.write(header.c_str(), header.size());
		file.write(reinterpret_cast<const char*>(image.getBuffer()), image.getSize());
	
		file.close();
	}

