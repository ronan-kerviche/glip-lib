#include "imageObject.hpp"
#include "netpbm.hpp"
#include <QFileInfo>
#include <QMessageBox>

// ImageObject
	ImageObject::ImageObject(const QString& _filename)
	 : 	virtualImage(false),
		saved(false),
		filename(_filename), 
		image(NULL),
		textureFormat(1, 1, GL_RGB, GL_UNSIGNED_BYTE),
		textureData(NULL)
	{
		try
		{
			// Open the file and guess the texture format : 
			QFileInfo file(filename);
			
			if(file.completeSuffix()=="ppm" || file.completeSuffix()=="pgm")
			{
				// Use the internal loader : 
				NetPBM::Image img(filename.toStdString());

				textureFormat = img.getFormat();
				
				// Allocate space in RAM to store the image : 
				image = new unsigned char[ img.getSize() ];

				// Copy : 
				img.copyTo(image);
			}
			else
			{
				// Load to RAM : 
				QImage qimage(filename);

				if(qimage.isNull())
					throw Exception("Cannot load image \"" + filename.toStdString() + "\".", __FILE__, __LINE__);
				
				// Create the format : 
				GLenum mode;

				if(qimage.allGray())
					mode = GL_LUMINANCE;
				else if(qimage.hasAlphaChannel())
					mode = GL_RGBA;
				else
					mode = GL_RGB;

				const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get(mode);

				textureFormat.setWidth( qimage.width() );
				textureFormat.setHeight( qimage.height() );
				textureFormat.setGLMode( mode );
				textureFormat.setGLDepth( GL_UNSIGNED_BYTE );
				
				// Create buffer : 
				image = new unsigned char[ textureFormat.getSize() ];

				// Copy : 
				unsigned int t=0;
				for(int i=0; i<qimage.height(); i++)
				{
					for(int j=0; j<qimage.width(); j++)
					{
						QRgb col 	= qimage.pixel(j,i);
										image[t+0] 	= static_cast<unsigned char>( qRed( col ) );
						if(descriptor.numComponents>1)	image[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
						if(descriptor.numComponents>2)	image[t+2] 	= static_cast<unsigned char>( qBlue( col ) );
						if(descriptor.hasAlphaLayer)	image[t+3] 	= static_cast<unsigned char>( qAlpha( col ) );

						t += descriptor.numComponents;
					}
				}
			}
		}
		catch(Exception& e)
		{
			std::cout << "ImageObject::ImageObject - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
		catch(std::exception& e)
		{
			std::cout << "ImageObject::ImageObject - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
	}

	ImageObject::ImageObject(HdlTexture& texture)
	 : 	virtualImage(true),
		image(NULL),
		textureFormat(texture),
		textureData(NULL)
	{
		// Create a buffer : 
		image = new unsigned char[ texture.getSize() ];
		
		try
		{
			TextureReader reader("reader",texture.format());

			reader << texture << OutputDevice::Process;

			QColor value;
			unsigned int t=0;
			for(int y=0; y<reader.getHeight(); y++)
			{
				for(int x=0; x<reader.getWidth(); x++)
				{
					for(int k=0; k<textureFormat.getNumChannels(); k++)
						image[t+k] = static_cast<unsigned char>(reader(x,y,k)*255.0);
					t += textureFormat.getNumChannels();
				}
			}
		}
		catch(Exception& e)
		{
			std::cout << "ImageObject::ImageObject - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
		catch(std::exception& e)
		{
			std::cout << "ImageObject::ImageObject - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename));
			delete textureData;
			textureData = NULL;
		}
	}

	ImageObject::~ImageObject(void)
	{
		delete image;
		image = NULL;
		delete textureData;
		textureData = NULL;
	}	

	bool ImageObject::isVirtual(void) const
	{
		return virtualImage;
	}

	bool ImageObject::wasSaved(void) const
	{
		return saved;
	}

	bool ImageObject::isOnDevice(void) const
	{
		return textureData!=NULL;
	}

	void ImageObject::loadToDevice(void)
	{
		if(textureData==NULL)
		{
			textureData = new HdlTexture(textureFormat);
			textureData->write(image);
		}
	}

	void ImageObject::unloadFromDevice(void)
	{
		delete textureData;
		textureData = NULL;
	}

	void ImageObject::setMinFilter(GLenum mf)
	{
		textureFormat.setMinFilter(mf);

		if(textureData!=NULL)
			textureData->setMinFilter(mf);
	}

	void ImageObject::setMagFilter(GLenum mf)
	{
		textureFormat.setMagFilter(mf);

		if(textureData!=NULL)
			textureData->setMagFilter(mf);
	}

	void ImageObject::setSWrapping(GLint m)
	{
		textureFormat.setSWrapping(m);

		if(textureData!=NULL)
			textureData->setSWrapping(m);
	}

	void ImageObject::setTWrapping(GLint m)
	{
		textureFormat.setTWrapping(m);

		if(textureData!=NULL)
			textureData->setTWrapping(m);
	}

	const QString& ImageObject::getFilename(void) const
	{
		return filename;
	}

	QString ImageObject::getName(void) const
	{
		QFileInfo info(filename);
		return info.fileName();
	}

	const __ReadOnly_HdlTextureFormat& ImageObject::getFormat(void) const
	{
		return textureFormat;
	}

	HdlTexture& ImageObject::texture(void)
	{
		if(textureData==NULL)
			throw Exception("ImageObject::texture - Texture is not currently on device.", __FILE__, __LINE__);
		else
			return *textureData;
	}

	void ImageObject::save(const std::string& filename)
	{
		QImage* bufferImage = NULL;

		try
		{
			// Get the mode :
			const HdlTextureFormatDescriptor& 	descriptor = HdlTextureFormatDescriptorsList::get( textureFormat.getGLMode() );
			const int 				depthBytes = HdlTextureFormatDescriptorsList::getTypeDepth( textureFormat.getGLDepth() );

			if( descriptor.hasLuminanceLayer && (descriptor.luminanceDepthInBits==8 || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_RGB888);
			else if( descriptor.hasRedLayer && descriptor.hasGreenLayer && descriptor.hasBlueLayer && !descriptor.hasAlphaLayer && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_RGB888);
			else if(descriptor.hasRedLayer && descriptor.hasGreenLayer && descriptor.hasBlueLayer && descriptor.hasAlphaLayer && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_ARGB32);		
			else
				throw Exception("Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);

			// Copy to QImage : 
			QColor value;
			for(int y=0; y<textureFormat.getHeight(); y++)
			{
				for(int x=0; x<textureFormat.getWidth(); x++)
				{
					const int p = (y * textureFormat.getWidth() + x) * descriptor.numComponents;
					if(descriptor.numComponents>=4)
						value.setAlpha( static_cast<unsigned char>(image[p+3]*255.0) );
					if(descriptor.numComponents>=3)
					{
						value.setGreen( static_cast<unsigned char>(image[p+1]*255.0) );
						value.setBlue(  static_cast<unsigned char>(image[p+2]*255.0) );
						value.setRed(   static_cast<unsigned char>(image[p+0]*255.0) );
					}
					else if(descriptor.numComponents==1)
					{
						value.setRed(   static_cast<unsigned char>(image[p+0]*255.0) );
						value.setGreen( static_cast<unsigned char>(image[p+0]*255.0) );
						value.setBlue(  static_cast<unsigned char>(image[p+0]*255.0) );
					}
					else
						throw Exception("Internal error : unknown mode ID.", __FILE__, __LINE__);

					bufferImage->setPixel(x, y, value.rgba());
				}
			}

			// Save file : 
			if(!bufferImage->save(filename.c_str()))
				throw Exception("Cannot save image to file \"" + filename + "\".", __FILE__, __LINE__);

			// Clean : 
			delete bufferImage;
			saved = true;
		}
		catch(Exception& e)
		{
			std::cout << "ImageObject::save - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename.c_str()));
			delete bufferImage;
			bufferImage = NULL;
		}
		catch(std::exception& e)
		{
			std::cout << "ImageObject::save - Exception caught : " << std::endl;
			std::cout << e.what() << std::endl;
			QMessageBox::information(NULL, QObject::tr("ImageObject"), QObject::tr("Cannot load image %1.").arg(filename.c_str()));
			delete bufferImage;
			bufferImage = NULL;
		}
	}

