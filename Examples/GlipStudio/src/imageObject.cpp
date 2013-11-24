#include "imageObject.hpp"
#include "netpbm.hpp"
#include <QFileInfo>
#include <QMessageBox>

// ImageObject
	ImageObject::ImageObject(const QString& _filename)
	 : 	virtualImage(false),
		saved(false),
		filename(_filename), 
		imageBuffer(NULL),
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
				
				imageBuffer = img.createImageBuffer();
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
				imageBuffer = new ImageBuffer(textureFormat);

				// Copy : 
				unsigned int t=0;
				for(int i=0; i<qimage.height(); i++)
				{
					for(int j=0; j<qimage.width(); j++)
					{
						QRgb col 	= qimage.pixel(j,i);
						if(descriptor.numChannels==1)
							imageBuffer->set(j, i, GL_LUMINANCE,	qRed( col ));
						else 
						{
											imageBuffer->set(j, i, GL_RED, 		static_cast<unsigned char>(qRed( col )));
							if(descriptor.numChannels>1)	imageBuffer->set(j, i, GL_GREEN,	static_cast<unsigned char>(qGreen( col )));
							if(descriptor.numChannels>2)	imageBuffer->set(j, i, GL_BLUE,		static_cast<unsigned char>(qBlue( col )));
							if(descriptor.numChannels>3)	imageBuffer->set(j, i, GL_ALPHA,	static_cast<unsigned char>(qAlpha( col )));
						}
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
		imageBuffer(NULL),
		textureFormat(texture),
		textureData(NULL),
		saved(false)
	{
		// Create a buffer : 
		imageBuffer = new ImageBuffer(texture);
	}

	ImageObject::~ImageObject(void)
	{
		delete imageBuffer;
		imageBuffer = NULL;
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
		textureData = new HdlTexture(textureFormat);
		(*imageBuffer) >> (*textureData);
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

			if( descriptor.hasLuminanceChannel && (descriptor.luminanceDepthInBits==8 || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_RGB888);
			else if( descriptor.hasRedChannel && descriptor.hasGreenChannel && descriptor.hasBlueChannel && !descriptor.hasAlphaChannel && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_RGB888);
			else if(descriptor.hasRedChannel && descriptor.hasGreenChannel && descriptor.hasBlueChannel && descriptor.hasAlphaChannel && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )
				bufferImage = new QImage(textureFormat.getWidth(), textureFormat.getHeight(), QImage::Format_ARGB32);		
			else
				throw Exception("Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);

			// Copy to QImage : 
			QColor value;
			for(int y=0; y<textureFormat.getHeight(); y++)
			{
				for(int x=0; x<textureFormat.getWidth(); x++)
				{
					/*const int p = (y * textureFormat.getWidth() + x) * descriptor.numChannels;
					if(descriptor.numChannels>=4)
						value.setAlpha( static_cast<unsigned char>(image[p+3]*255.0) );
					if(descriptor.numChannels>=3)
					{
						value.setGreen( static_cast<unsigned char>(image[p+1]*255.0) );
						value.setBlue(  static_cast<unsigned char>(image[p+2]*255.0) );
						value.setRed(   static_cast<unsigned char>(image[p+0]*255.0) );
					}
					else if(descriptor.numChannels==1)
					{
						value.setRed(   static_cast<unsigned char>(image[p+0]*255.0) );
						value.setGreen( static_cast<unsigned char>(image[p+0]*255.0) );
						value.setBlue(  static_cast<unsigned char>(image[p+0]*255.0) );
					}
					else
						throw Exception("Internal error : unknown mode ID.", __FILE__, __LINE__);

					bufferImage->setPixel(x, y, value.rgba());*/

					if(descriptor.numChannels>=4)
						value.setAlpha( imageBuffer->get<unsigned char>(x, y, GL_ALPHA) );
					if(descriptor.numChannels>=3)
					{
						value.setRed( 	imageBuffer->get<unsigned char>(x, y, GL_RED) );
						value.setGreen( imageBuffer->get<unsigned char>(x, y, GL_GREEN) );
						value.setBlue( 	imageBuffer->get<unsigned char>(x, y, GL_BLUE) );
					}
					else if(descriptor.numChannels==1)
					{
						value.setRed( 	imageBuffer->get<unsigned char>(x, y, GL_LUMINANCE) );
						value.setGreen( imageBuffer->get<unsigned char>(x, y, GL_LUMINANCE) );
						value.setBlue( 	imageBuffer->get<unsigned char>(x, y, GL_LUMINANCE) );
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

