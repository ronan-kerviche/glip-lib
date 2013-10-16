#include "netpbm.hpp"
#include <cstring>

	unsigned short changeEndianness16(unsigned short val)
	{
		return 	 (val << 8) |          // left-shift always fills with zeros
			((val >> 8) & 0x00ff); // right-shift sign-extends, so force to zero
	}

	using namespace NetPBM;

	Image::Image(const unsigned int w,const unsigned int h, const unsigned int m, unsigned int layers)
	 : width(w), height(h), maxval(m), numLayers(layers), bytes(NULL)
	{
		if(getSize()==0)
			throw Exception("NetPBM::Image::Image - Null size.", __FILE__, __LINE__);

		bytes = new unsigned char[getSize()];
	}

	Image::Image(HdlTexture& texture)
	 : width(texture.getWidth()), height(texture.getHeight()), numLayers(texture.getNumChannels()), bytes(NULL)
	{
		maxval = std::pow(2.0, HdlTextureFormatDescriptorsList::getTypeDepth( texture.getGLDepth() ) ) - 1;

		if(getSize()!=texture.getSize())
			throw Exception("NetPBM::Image::Image - Sizes mismatch (NetPBM : " + to_string(getSize()) + "; GL : " + to_string(texture.getSize()) + ").", __FILE__, __LINE__);

		bytes = new unsigned char[getSize()];

		PBOTextureReader reader("NetPBM Loader", texture.format(), GL_STREAM_READ);

		reader << texture << OutputDevice::Process;
		
		// Map the internal PBO to the memory for GL_RGB, GL_UNSIGNED_BYTE :
		unsigned char* ptr = reinterpret_cast<unsigned char*>(reader.startReadingMemory());

		// Copy to your buffer :
		memcpy(bytes, ptr, texture.getSize());

		// Release the mapped memory :
		reader.endReadingMemory();
	}

	Image::Image(const std::string& filename)
	 : bytes(NULL)
	{
		std::fstream file(filename.c_str(), std::ios_base::in | std::ifstream::binary);

		if(!file.is_open())
			throw Exception("NetPBM::Image::Image - Cannot read file " + filename + ".", __FILE__, __LINE__);

		 // get length of file :
		file.seekg (0, file.end);
		unsigned int length = file.tellg();
		file.seekg (0, file.beg);

		// allocate memory :
		char * buffer = new char[length];

		// read data as a block :
		file.read(buffer, length);

		file.close();

		// Read the header : 
		std::string head(buffer, 2);

		if(head=="P5")
			numLayers = 1;
		else if(head=="P6")
			numLayers = 3;
		else
		{
			delete[] buffer;
			throw Exception("NetPBM::Image::Image - Unknown header \"" + head + "\".", __FILE__, __LINE__);
		}

		bool test = true;

		unsigned int p = 2;
		test = nextInteger(width, buffer, p, length);

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::Image::Image - Unable to read width from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		test = nextInteger(height, buffer, p, length);

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::Image::Image - Unable to read height from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		test = nextInteger(maxval, buffer, p,length);		

		if(!test)
		{
			delete[] buffer;
			throw Exception("NetPBM::Image::Image - Unable to read maxval from file \"" + filename + "\"", __FILE__, __LINE__);
		}

		p++;

		// Test body : 
		if(length-p!=getSize())
		{
			delete[] buffer;
			throw Exception("NetPBM::Image::Image - Body size mismatch (expectation : " + to_string(getSize()) + "; File : " + to_string(length-p) + ").", __FILE__, __LINE__);
		}

		bytes = new unsigned char[getSize()];
	
		std::memcpy(bytes, buffer + p, getSize());
	
		// Clean : 
		delete[] buffer;

		unsigned short* data = reinterpret_cast<unsigned short*>(bytes);

		/*unsigned short m1 = data[0], m2 = data[0];
		unsigned int hist[65536];
		std::memset(hist, 0, sizeof(hist));
		for(int k=0; k<width*height; k++)
		{
			m1 = std::max(m1, data[k]);
			m2 = std::min(m2, data[k]);
			hist[data[k]]++;
		}
		std::cout << "Max : " << m1 << std::endl;
		std::cout << "Min : " << m2 << std::endl;

		for(int k=0; k<65536; k++)
			std::cout << k << " -> " << hist[k] << std::endl;*/	

		// Change endianess : 
		if(getBytePerValue()==2)
		{
			for(int k=0; k<width*height; k++)
				data[k] = changeEndianness16(data[k]);
		}
	}

	Image::~Image(void)
	{
		delete[] bytes;
		bytes = NULL;
	}

	bool Image::nextInteger(unsigned int& val, char* buffer, unsigned int& p, const unsigned int bufferLength)
	{
		bool isComment = false;
		unsigned int k=0;
		std::locale loc;

		//std::cout << "In nextInteger" << std::endl;

		while( (isspace(buffer[p+k],loc) || isComment || buffer[p+k]=='#') && (p+k)<bufferLength )
		{
			//std::cout << "Char : " << buffer[p+k] << std::endl;

			if(buffer[p+k]=='#')
				isComment = true;
			else if(buffer[p+k]=='\n')
				isComment = false;

			k++;

			//std::cout << "Next '" << buffer[p+k] << "', Is space : " << isspace(buffer[p+k],loc) << std::endl;
		}

		//std::cout << "Current : " << p+k << "; max : " << bufferLength << std::endl;

		if(p+k>=bufferLength)
			return false;

		std::string value;

		while( !isspace(buffer[p+k],loc) && buffer[p+k]!='#' && p+k<bufferLength )
		{
			//std::cout << "Pushing : " << buffer[p+k] << std::endl;
			value += buffer[p+k];
			k++;
		}

		p += k;

		return from_string(value, val);
	}

	bool Image::isValid(void) const
	{
		return bytes!=NULL;
	}

	unsigned int Image::getWidth(void) const
	{
		return width;
	}

	unsigned int Image::getHeight(void) const
	{
		return height;
	}

	unsigned int Image::getMaximum(void) const
	{
		return maxval;
	}

	unsigned int Image::getNumLayers(void) const
	{
		return numLayers;
	}

	unsigned int Image::getSize(void) const
	{
		return width * height * numLayers * getBytePerValue();
	}
	
	unsigned int Image::getBytePerValue(void) const
	{
		return std::ceil( std::log( static_cast<double>(maxval) ) / (std::log(2.0) * 8.0) );
	}

	void Image::copyTo(unsigned char* dest)
	{
		if(!isValid())
			throw Exception("NetPBM::Image::copyTo - Object is not valid.", __FILE__, __LINE__);

		std::memcpy(dest, bytes, getSize());
	}

	HdlTextureFormat Image::getFormat(void) const
	{
		if(!isValid())
			throw Exception("NetPBM::Image::getFormat - Object is not valid.", __FILE__, __LINE__);

		// Format description : 
		GLenum mode;

		if(numLayers==1)
			mode = GL_LUMINANCE;
		else if(numLayers==3)
			mode = GL_RGB;
		else
			throw Exception("NetPBM::Image::createTexture - Unable to create a texture from an image having " + to_string(numLayers) + " layers (only 1 or 3 are accepted).", __FILE__, __LINE__);

		GLenum depth;

		unsigned int b = getBytePerValue();

		if(b<=1)
			depth = GL_UNSIGNED_BYTE;
		else if(b<=2)
		{
			depth = GL_UNSIGNED_SHORT;

			if(numLayers==1)
				mode = GL_LUMINANCE16;
			else if(numLayers==3)
				mode = GL_RGB16;
		}
		else
			throw Exception("NetPBM::Image::createTexture - Unable to create a texture from an image having " + to_string(b) + " bytes per value (only 1, 2, and 4 bytes are accepted).", __FILE__, __LINE__);

		HdlTextureFormat format(width, height, mode, depth);

		return format;
	}

	bool Image::writeToFile(const std::string& filename)
	{
		if(!isValid())
			throw Exception("NetPBM::Image::writeToFile - Object is not valid.", __FILE__, __LINE__);

		std::fstream file(filename.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

		if(!file.is_open())
			throw Exception("NetPBM::Image::writeToFile - Cannot write file " + filename + ".", __FILE__, __LINE__);

		// make header : 
		std::string header;

		if(numLayers==1)
			header += "P5\n";
		else if(numLayers==3)
			header += "P6\n";
		else
		{
			file.close();
			throw Exception("NetPBM::Image::writeToFile - Internal error : number of layers is invalid.", __FILE__, __LINE__);
		}

		header += "#GlipLib NetPBM Writer\n";
		header += to_string(width) + " " + to_string(height) + "\n";
		header += to_string( static_cast<int>( std::pow(2.0,getBytePerValue()) ) ) + "\n";

		// Write : 
		file.write(header.c_str(), header.size());
		file.write(reinterpret_cast<const char*>(bytes), getSize());
	
		file.close();

		return true;
	}

	bool Image::copyToTexture(HdlTexture& texture)
	{
		if(!isValid())
			throw Exception("NetPBM::Image::copyToTexture - Object is not valid.", __FILE__, __LINE__);

		if(texture.getWidth()!=width || texture.getHeight()!=height || texture.getNumChannels()!=numLayers)
			throw Exception("NetPBM::Image::copyToTexture - Layout mismatch (Image : " + to_string(width) + "x" + to_string(height) + "x" + to_string(numLayers) + "; GL : " +  to_string(texture.getWidth()) + "x" + to_string(texture.getHeight()) + "x" + to_string(texture.getNumChannels()) + ").", __FILE__, __LINE__);

		unsigned int textureMaxval = std::pow(2.0, HdlTextureFormatDescriptorsList::getTypeDepth( texture.getGLDepth() ) );

		if( textureMaxval < maxval )
			throw Exception("NetPBM::Image::copyToTexture - Dynamic range mismatch (Image : " + to_string(maxval) + "; " + to_string(textureMaxval) + ").", __FILE__, __LINE__);

		texture.write(bytes);

		return true;
	}

	HdlTexture* Image::createTexture(void)
	{
		HdlTextureFormat format = getFormat();

		
	
		HdlTexture* texture = new HdlTexture(format);

		texture->write(bytes);

		return texture;
	}

