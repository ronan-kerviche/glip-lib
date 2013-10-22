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
		///TODO
		saved = true;
		throw Exception("ImageObject::save - Cannot save to \"" + filename + "\".", __FILE__, __LINE__);
	}

