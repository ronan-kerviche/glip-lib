/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ResourceLoader.cpp                                                                        */
/*     Original Date : December 28th 2012                                                                        */
/*                                                                                                               */
/*     Description   : Qt interface for loading and writing to images.                                           */
/*                     Qt interface for loading pipelines from file.					         */
/*                     Qt widget for the interface.                                                              */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "ResourceLoader.hpp"
#include <QFileDialog>
#include <QMessageBox>

// QImage tools : 
	ImageBuffer* createImageBufferFromQImage(const QImage& qimage)
	{
		/*// Create the format : 
		GLenum mode = GL_NONE;

		if(qimage.allGray())
			mode = GL_LUMINANCE;
		else if(qimage.hasAlphaChannel())
			mode = GL_RGBA;
		else
			mode = GL_RGB;

		const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get(mode);

		HdlTextureFormat textureFormat( qimage.width(), qimage.height(), mode, GL_UNSIGNED_BYTE );
		
		// Create buffer : 
		ImageBuffer* buffer = new ImageBuffer(textureFormat);

		// Copy : 
		for(int i=0; i<qimage.height(); i++)
		{
			for(int j=0; j<qimage.width(); j++)
			{
				QRgb col 	= qimage.pixel(j,qimage.height() - i - 1);				// WARNING Shift up-down to fit texture coordinates.

				//std::cout << i << ", " << j << " : " << QColor(col).name().toStdString() << std::endl;

				if(descriptor.numChannels()==1)
					buffer->set(j, i, GL_LUMINANCE,	qRed(  col ));
				else 
				{
									buffer->set(j, i, GL_RED, 	qRed( col ));
					if(descriptor.numChannels()>1)	buffer->set(j, i, GL_GREEN,	qGreen( col ));
					if(descriptor.numChannels()>2)	buffer->set(j, i, GL_BLUE,	qBlue( col ));
					if(descriptor.numChannels()>3)	buffer->set(j, i, GL_ALPHA,	qAlpha( col ));
				}
			}
		}

		return buffer;*/
		return NULL;
	}

	QImage* createQImageFromImageBuffer(const ImageBuffer& buffer)
	{
		QImage* qimage = NULL;

		/*// Get the mode :
		const HdlTextureFormatDescriptor& 	descriptor = HdlTextureFormatDescriptorsList::get( buffer.getGLMode() );
		const int 				depthBytes = HdlTextureFormatDescriptorsList::getTypeDepth( buffer.getGLDepth() );

		if( descriptor.hasLuminanceChannel() && (descriptor.luminanceDepthInBits==8 || depthBytes==1) )
			qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_RGB888);
		else if( descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && !descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )
			qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_RGB888);
		else if(descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )
			qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_ARGB32);		
		else
			throw Exception("Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);

		// Copy to QImage : 
		QColor value;
		for(int y=0; y<buffer.getHeight(); y++)
		{
			for(int x=0; x<buffer.getWidth(); x++)
			{
				if(descriptor.numChannels()>=4)
					value.setAlpha( ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_ALPHA) ) );
				if(descriptor.numChannels()>=3)
					value.setBlue( 	ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_BLUE) ) );
				if(descriptor.numChannels()>=2)
				{
					value.setRed( 	ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_RED) ) );
					value.setGreen( ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_GREEN) ) );
					
				}
				else if(descriptor.numChannels()==1)
				{
					value.setRed( 	ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_LUMINANCE) ) );
					value.setGreen( ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_LUMINANCE) ) );
					value.setBlue( 	ImageBuffer::clampValue<unsigned char>( buffer.get(x, y, GL_LUMINANCE) ) );
				}
				else
				{
					delete qimage;
					throw Exception("Internal error : unknown mode ID.", __FILE__, __LINE__);
				}

				qimage->setPixel(x, y, value.rgba());
			}
		}*/

		return qimage;
	}

// Image Loader : 
	ImageLoader::ImageLoader(void)
	{ }

	ImageLoader::~ImageLoader(void)
	{
		cleanResources();
	}

	void ImageLoader::cleanResources(void)
	{
		for(std::vector<HdlTexture*>::iterator it = textures.begin(); it!=textures.end(); it++)
			delete *it;

		textures.clear();
		filenamesList.clear();
	}

	int ImageLoader::loadFiles(GLenum minFilter, GLenum magFilter, GLenum sWrapping, GLenum tWrapping, int maxLevel, QWidget* parent)
	{
		QStringList filenames = QFileDialog::getOpenFileNames(parent, QObject::tr("Open images..."), ".", "Images (*.jpg *.JPG *.jpeg *.png *.bmp)");

		if(filenames.empty())
			return -1; // no selection
		else
		{
			cleanResources();

			for(int i=0; i<filenames.length(); i++)
			{
				#ifdef __RESOURCE_LOADER_VERBOSE__
					std::cout << "ImageLoader::loadFiles Loading file - " << i+1 << "/" << filenames.length() <<  " : " << filenames.at(i).toStdString() << std::endl;
				#endif

				QImage* image = new QImage(filenames.at(i));

				if (image->isNull())
				{
					QMessageBox::information(parent, QObject::tr("ImageLoader::loadFiles"), QObject::tr("Cannot load image %1.").arg(filenames.at(i)));
					#ifdef __RESOURCE_LOADER_VERBOSE__
						std::cout << "ImageLoader::loadFiles - Cannot load : " << filenames.at(i).toUtf8().constData() << std::endl;
					#endif
				}
				else
				{
					try
					{
						textures.push_back( createTexture(*image, minFilter, magFilter, sWrapping, tWrapping, maxLevel) );
					}
					catch(Exception& e)
					{
						QMessageBox::information(parent, QObject::tr("ImageLoader::loadFiles"), QObject::tr("Exception while loading image %1 :\n%2").arg(filenames.at(i)).arg(e.what()));
					}
				}

				delete image;

				// Save filename :
				QFileInfo pathInfo( filenames.at(i) );
				QString strippedFilename( pathInfo.fileName() );
				filenamesList.push_back(strippedFilename.toStdString());
			}

			return textures.size(); // Return number of images effectively loaded.
		}
	}

	int ImageLoader::getNumTextures(void) const
	{
		return textures.size();
	}

	HdlTexture& ImageLoader::texture(int id)
	{
		if(id<0 || id>=getNumTextures())
			throw Exception("ImageLoader::texture - Index " + toString(id) + " is out of bounds.", __FILE__, __LINE__);
		else
			return *textures[id];
	}

	const std::string& ImageLoader::filename(int id)
	{
		if(id<0 || id>=getNumTextures())
			throw Exception("ImageLoader::filename - Index " + toString(id) + " is out of bounds.", __FILE__, __LINE__);
		else
			return filenamesList[id];
	}

	bool ImageLoader::saveTexture(HdlTexture& texture, QWidget* parent)
	{
		QString filename = QFileDialog::getSaveFileName(parent);

		if (!filename.isEmpty())
		{
			QImage image = ImageLoader::createImage(texture);


			if(!image.save(filename))
				throw Exception("Error while writing file " + filename.toStdString() + ".", __FILE__, __LINE__);
			else
				return true;
		}
		else
			return false;
	}

	HdlTexture* ImageLoader::createTexture(const QImage& image, GLenum minFilter, GLenum magFilter, GLenum sWrapping, GLenum tWrapping, int maxLevel)
	{
		// Format description : 
		GLenum mode;

		if(image.allGray())
			mode = GL_LUMINANCE;
		else if(image.hasAlphaChannel())
			mode = GL_RGBA;
		else
			mode = GL_RGB;

		const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get(mode);
		
		HdlTextureFormat fmt(image.width(),image.height(),mode,GL_UNSIGNED_BYTE,minFilter,magFilter,sWrapping,tWrapping,0,maxLevel);
		
		HdlTexture* texture = new HdlTexture(fmt);

		#ifndef __USE_PBO__
			unsigned char* temp = new unsigned char[image.width()*image.height()*descriptor.numChannels()];
			int t=0;

			for(int i=0; i<image.height(); i++)
			{
				for(int j=0; j<image.width(); j++)
				{
					QRgb col 	= image.pixel(j,i);
									temp[t+0] 	= static_cast<unsigned char>( qRed( col ) );
					if(descriptor.numChannels()>1)	temp[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
					if(descriptor.numChannels()>2)	temp[t+2] 	= static_cast<unsigned char>( qBlue( col ) );

					if(descriptor.hasAlphaChannel())
						temp[t+3] 	= static_cast<unsigned char>( qAlpha( col ) );

					t += descriptor.numChannels();
				}
			}

			texture->write(temp);

			delete[] temp;
		#else
			try
			{
				//Example for texture streaming to the GPU (live video for example, but in that case, the PBO is only created once)
				//HdlPBO pbo(image.width(),image.height(),descriptor.numChannels(),1,GL_PIXEL_UNPACK_BUFFER_ARB,GL_STREAM_DRAW_ARB);
				HdlPBO pbo(fmt, GL_PIXEL_UNPACK_BUFFER_ARB, GL_STREAM_DRAW_ARB);

				// YOU MUST WRITE ONCE IN THE TEXTURE BEFORE USING PBO::copyToTexture ON IT.
				texture->fill(0);

				unsigned char* ptr = reinterpret_cast<unsigned char*>(pbo.map());
				int t=0;

				for(int i=0; i<image.height(); i++)
				{
					for(int j=0; j<image.width(); j++)
					{
						QRgb col 	= image.pixel(j,i);
										ptr[t+0] 	= static_cast<unsigned char>( qRed( col ) );
						if(descriptor.numChannels()>1)	ptr[t+1] 	= static_cast<unsigned char>( qGreen( col ) );
						if(descriptor.numChannels()>1)	ptr[t+2] 	= static_cast<unsigned char>( qBlue( col ) );

						if(descriptor.hasAlphaChannel)
							temp[t+3] 	= static_cast<unsigned char>( qAlpha( col ) );

						t += descriptor.numChannels();
					}
				}

				HdlPBO::unmap(GL_PIXEL_UNPACK_BUFFER_ARB);
				pbo.copyToTexture(*texture);
			}
			catch(Exception& e)
			{
				delete texture;
				Exception m("ImageLoader::createTexture - Error while building texture from PBO : ", __FILE__, __LINE__);
				throw m+e;
			}
		#endif

		return texture;
	}

	QImage ImageLoader::createImage(HdlTexture& texture)
	{
		const int 	eUNKN = 0,
				eLUMI = 1,
				eRGB8 = 2,
				eRGBA = 3;
		QImage* bufferImage = NULL;

		try
		{
			int id = eUNKN;
			const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get( texture.getGLMode() );
			const int depthBytes = HdlTextureFormatDescriptorsList::getTypeDepth( texture.getGLDepth() );

			// Test input image : 
			if( descriptor.hasLuminanceChannel() && (descriptor.luminanceDepthInBits==8 || depthBytes==1) )
			{
				id = eLUMI;
				bufferImage = new QImage(texture.getWidth(), texture.getHeight(), QImage::Format_RGB888);
			}
			else if( descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && !descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )
			{
				id = eRGB8;
				bufferImage = new QImage(texture.getWidth(), texture.getHeight(), QImage::Format_RGB888);
			}
			else if(descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )
			{
				id = eRGBA;
				bufferImage = new QImage(texture.getWidth(), texture.getHeight(), QImage::Format_ARGB32);
			}			
			else
				throw Exception("Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);

			#ifndef __USE_PBO__
				TextureReader reader("reader",texture.format());
				//reader.yFlip = true;

				reader << texture << OutputDevice::Process;

				QColor value;
				for(int y=0; y<reader.getHeight(); y++)
				{
					for(int x=0; x<reader.getWidth(); x++)
					{
						switch(id)
						{
							case eRGBA :
								value.setAlpha( static_cast<unsigned char>(reader(x,y,4)*255.0) );
							case eRGB8 :
								value.setGreen( static_cast<unsigned char>(reader(x,y,1)*255.0) );
								value.setBlue(  static_cast<unsigned char>(reader(x,y,2)*255.0) );
								value.setRed(   static_cast<unsigned char>(reader(x,y,0)*255.0) );
								break;
							case eLUMI :
								value.setRed(   static_cast<unsigned char>(reader(x,y,0)*255.0) );
								value.setGreen( static_cast<unsigned char>(reader(x,y,0)*255.0) );
								value.setBlue(  static_cast<unsigned char>(reader(x,y,0)*255.0) );
								break;
							default : 
								throw Exception("Internal error : unknown mode ID.", __FILE__, __LINE__);
						}
						bufferImage->setPixel(x, y, value.rgba());
					}
				}
			#else
				// Create the reader on the right format (inheritance of the texture object)
				PBOTextureReader reader("reader", texture.format(), GL_STREAM_READ_ARB);

				// Start copy to PBO :
				reader << texture << OutputDevice::Process;

				// Get access to memory :
				unsigned char* ptr = reinterpret_cast<unsigned char*>(reader.startReadingMemory());;

				QColor value;
				int p = 0;
				for(int y=reader.getHeight()-1; y>=0; y--)
				{
					for(int x=0; x<reader.getWidth(); x++)
					{
						switch(id)
						{
							case eRGBA :
								value.setAlpha( ptr[p+3] );
							case eRGB8 :
								value.setGreen( ptr[p+1] );
								value.setBlue(  ptr[p+2] );
								value.setRed(   ptr[p+0] );
								break;
							case eLUMI :
								value.setGreen( ptr[p+0] );
								value.setBlue(  ptr[p+0] );
								value.setRed(   ptr[p+0] );
								break;
							default : 
								throw Exception("Internal error : unknown mode ID.", __FILE__, __LINE__);
						}
						bufferImage->setPixel(x, y, value.rgba());
						p += descriptor.numChannels();
					}
				}

				// Close access to memory :
				reader.endReadingMemory();
			#endif
		}
		catch(Exception& e)
		{
			Exception m("Error while creating QImage: ", __FILE__, __LINE__);
			throw m+e;
		}

		QImage image(*bufferImage);
		delete bufferImage;

		return image;
	}

// ImageLoaderOptions
	ImageLoaderOptions::ImageLoaderOptions(QWidget* parent, GLenum defMinFilter, GLenum defMagFilter, GLenum defSWrapping, GLenum defTWrapping, int defMaxLevel)
	 : QGridLayout(parent), minFilterLabel("Minifaction filter"), magFilterLabel("Magnification filter"), sWrappingLabel("S Wrapping mode"), tWrappingLabel("T Wrapping mode"), maxLevelLabel("Max Mipmap Level")
	{
		minFilterBox.addItem("GL_NEAREST", 			GL_NEAREST);
		minFilterBox.addItem("GL_LINEAR", 			GL_LINEAR);
		minFilterBox.addItem("GL_NEAREST_MIPMAP_NEAREST", 	GL_NEAREST_MIPMAP_NEAREST );
		minFilterBox.addItem("GL_LINEAR_MIPMAP_NEAREST", 	GL_LINEAR_MIPMAP_NEAREST );
		minFilterBox.addItem("GL_NEAREST_MIPMAP_LINEAR", 	GL_NEAREST_MIPMAP_LINEAR );
		minFilterBox.addItem("GL_LINEAR_MIPMAP_LINEAR", 	GL_LINEAR_MIPMAP_LINEAR );

		magFilterBox.addItem("GL_NEAREST", 			GL_NEAREST);
		magFilterBox.addItem("GL_LINEAR", 			GL_LINEAR);

		sWrappingBox.addItem("GL_CLAMP", 			GL_CLAMP);
		sWrappingBox.addItem("GL_CLAMP_TO_EDGE", 		GL_CLAMP_TO_EDGE);
		sWrappingBox.addItem("GL_REPEAT", 			GL_REPEAT);

		tWrappingBox.addItem("GL_CLAMP", 			GL_CLAMP);
		tWrappingBox.addItem("GL_CLAMP_TO_EDGE", 		GL_CLAMP_TO_EDGE);
		tWrappingBox.addItem("GL_REPEAT", 			GL_REPEAT);

		switch(defMinFilter)
		{
			case GL_LINEAR :				minFilterBox.setCurrentIndex(1); break;
			case GL_NEAREST_MIPMAP_NEAREST :		minFilterBox.setCurrentIndex(2); break;
			case GL_LINEAR_MIPMAP_NEAREST :			minFilterBox.setCurrentIndex(3); break;
			case GL_NEAREST_MIPMAP_LINEAR :			minFilterBox.setCurrentIndex(4); break;
			case GL_LINEAR_MIPMAP_LINEAR :			minFilterBox.setCurrentIndex(5); break;
			case GL_NEAREST :
			default : 					minFilterBox.setCurrentIndex(0); break;
		}

		switch(defMagFilter)
		{
			case GL_LINEAR :				magFilterBox.setCurrentIndex(1); break;
			case GL_NEAREST :
			default : 					magFilterBox.setCurrentIndex(0); break;
		}

		switch(defSWrapping)
		{
			case GL_CLAMP_TO_EDGE :				sWrappingBox.setCurrentIndex(1); break;
			case GL_REPEAT :				sWrappingBox.setCurrentIndex(2); break;
			case GL_CLAMP :
			default :					sWrappingBox.setCurrentIndex(0); break;
		}

		switch(defTWrapping)
		{
			case GL_CLAMP_TO_EDGE :				tWrappingBox.setCurrentIndex(1); break;
			case GL_REPEAT :				tWrappingBox.setCurrentIndex(2); break;
			case GL_CLAMP :
			default :					tWrappingBox.setCurrentIndex(0); break;
		}

		maxLevelSpinBox.setRange(0, 10);
		maxLevelSpinBox.setValue(defMaxLevel);

		addWidget(&minFilterLabel, 	0, 0);
		addWidget(&minFilterBox, 	0, 1);
		addWidget(&magFilterLabel, 	0, 2);
		addWidget(&magFilterBox, 	0, 3);

		addWidget(&sWrappingLabel, 	1, 0);
		addWidget(&sWrappingBox, 	1, 1);
		addWidget(&tWrappingLabel, 	1, 2);
		addWidget(&tWrappingBox, 	1, 3);

		addWidget(&maxLevelLabel,	2, 0);
		addWidget(&maxLevelSpinBox,	2, 1);
	}

	ImageLoaderOptions::~ImageLoaderOptions(void)
	{ }

	GLenum ImageLoaderOptions::getMinFilter(void) const
	{
		return static_cast<GLenum>(minFilterBox.itemData(minFilterBox.currentIndex()).toUInt());
	}

	GLenum ImageLoaderOptions::getMagFilter(void) const
	{
		return static_cast<GLenum>(magFilterBox.itemData(magFilterBox.currentIndex()).toUInt());
	}

	GLenum ImageLoaderOptions::getSWrapping(void) const
	{
		return static_cast<GLenum>(sWrappingBox.itemData(sWrappingBox.currentIndex()).toUInt());
	}

	GLenum ImageLoaderOptions::getTWrapping(void) const
	{
		return static_cast<GLenum>(tWrappingBox.itemData(tWrappingBox.currentIndex()).toUInt());
	}

	int ImageLoaderOptions::getMaxLevel(void) const
	{
		return maxLevelSpinBox.value();
	}

// ImageLoaderOptionsDialog
	ImageLoaderOptionsDialog::ImageLoaderOptionsDialog(QWidget* parent, GLenum defMinFilter, GLenum defMagFilter, GLenum defSWrapping, GLenum defTWrapping, int defMaxLevel)
	 : QDialog(parent), options(this, defMinFilter, defMagFilter, defSWrapping, defTWrapping, defMaxLevel), okButton("Ok"), cancelButton("Cancel")
	{
		options.addWidget(&cancelButton,	2, 2);
		options.addWidget(&okButton,	2, 3);

		QObject::connect(&cancelButton, SIGNAL(released(void)), this, SLOT(reject(void)));
		QObject::connect(&okButton, 	SIGNAL(released(void)), this, SLOT(accept(void)));
	}

	ImageLoaderOptionsDialog::~ImageLoaderOptionsDialog(void)
	{ }

// ImageLoaderInterface
	ImageLoaderInterface::ImageLoaderInterface(QWidget* parent)
	 : QVBoxLayout(parent), loadButton("Load images"), optionsButton("Options"), prev("<="), next("=>"), maxIndex("/ (none)"),
	   minFilter(GL_NEAREST), magFilter(GL_NEAREST), sWrapping(GL_CLAMP), tWrapping(GL_CLAMP), maxMipmapLevel(0)
	{
		currentIndex.setRange(0,0);
		currentIndex.setValue(0);
		currentIndex.setReadOnly(true);
		maxIndex.setReadOnly(true);

		layout1.addWidget(&loadButton);
		layout1.addWidget(&optionsButton);
		layout2.addWidget(&prev);
		layout2.addWidget(&currentIndex);
		layout2.addWidget(&maxIndex);
		layout2.addWidget(&next);

		addLayout(&layout1);
		addLayout(&layout2);

		updateLoadToolTip();
		updateFilenameToolTip();

		QObject::connect(&loadButton, 		SIGNAL(released(void)), 		this, SLOT(loadImages(void)));
		QObject::connect(&optionsButton, 	SIGNAL(released(void)), 		this, SLOT(changeOptions(void)));
		QObject::connect(&prev, 		SIGNAL(released(void)), 		this, SLOT(previousImage(void)));
		QObject::connect(&next, 		SIGNAL(released(void)), 		this, SLOT(nextImage(void)));
		QObject::connect(&currentIndex, 	SIGNAL(valueChanged(int)), 		this, SIGNAL(currentTextureChanged(void)));
		QObject::connect(this,			SIGNAL(currentTextureChanged(void)),	this, SLOT(updateFilename(void)));
	}

	ImageLoaderInterface::~ImageLoaderInterface(void)
	{ }

	void ImageLoaderInterface::changeOptions(void)
	{
		ImageLoaderOptionsDialog optionsDialog(parentWidget(), minFilter, magFilter, sWrapping, tWrapping, maxMipmapLevel);

		int result = optionsDialog.exec();

		if(result==QDialog::Accepted)
		{
			minFilter	= optionsDialog.options.getMinFilter();
			magFilter	= optionsDialog.options.getMagFilter();
			sWrapping	= optionsDialog.options.getSWrapping();
			tWrapping	= optionsDialog.options.getTWrapping();
			maxMipmapLevel	= optionsDialog.options.getMaxLevel();

			updateLoadToolTip();
		}
	}

	void ImageLoaderInterface::loadImages(void)
	{
		int numFilesLoaded = loadFiles(minFilter, magFilter, sWrapping, tWrapping, maxMipmapLevel);

		if(numFilesLoaded<=0) // no selection made
			return;
		else if(numFilesLoaded>0)
		{
			currentIndex.setRange(1,numFilesLoaded);
			currentIndex.setValue(1);
			currentIndex.setReadOnly(false);
			maxIndex.setText(tr("/ %1").arg(numFilesLoaded));
		}
		else
		{
			currentIndex.setRange(0,0);
			currentIndex.setValue(0);
			currentIndex.setReadOnly(true);
			maxIndex.setText("/ (none)");
		}

		updateFilenameToolTip();

		emit currentTextureChanged();
	}

	void ImageLoaderInterface::updateFilename(void)
	{
		if(currentIndex.isReadOnly())
			maxIndex.setText("/ (none)");
		else
		{
			int id = currentIndex.value()-1;
			maxIndex.setText(tr("/ %1 : %2 (%3x%4)").arg(getNumTextures()).arg(filename(id).c_str()).arg(texture(id).getWidth()).arg(texture(id).getHeight()));
		}
	}

	void ImageLoaderInterface::updateLoadToolTip(void)
	{
		std::string tip = "Images will be loaded with the following parameters :\n    Minification filter \t : " + glParamName(minFilter) + "\n    Magnification filter \t : " + glParamName(magFilter) + "\n    S wrapping mode \t : " + glParamName(sWrapping) + "\n    T wrapping mode \t : " + glParamName(tWrapping) + "\n    Max Mipmap level \t : " + toString(maxMipmapLevel);
		loadButton.setToolTip(tip.c_str());
	}

	void ImageLoaderInterface::updateFilenameToolTip(void)
	{
		if(getNumTextures()>0 && currentIndex.value()>0)
		{
			std::string tip = "Current image have the following parameters :\n    Minification filter \t : " + glParamName(currentTexture().getMinFilter()) + "\n    Magnification filter \t : " + glParamName(currentTexture().getMagFilter()) + "\n    S wrapping mode \t : " + glParamName(currentTexture().getSWrapping()) + "\n    T wrapping mode \t : " + glParamName(currentTexture().getTWrapping()) + "\n    Max Mipmap level \t : " + toString(currentTexture().getMaxLevel());
			maxIndex.setToolTip(tip.c_str());
		}
		else
			maxIndex.setToolTip("");
	}

	HdlTexture& ImageLoaderInterface::currentTexture(void)
	{
		if(currentIndex.value()==0)
			throw Exception("ImageLoaderInterface::currentTexture - No texture available (index is 0).", __FILE__, __LINE__);
		else
			return texture(currentIndex.value()-1);
	}

	void ImageLoaderInterface::nextImage(void)
	{
		if(currentIndex.value() < currentIndex.maximum())
		{
			currentIndex.setValue(currentIndex.value()+1);
			emit currentTextureChanged();
		}
	}

	void ImageLoaderInterface::previousImage(void)
	{
		if(currentIndex.value() > currentIndex.minimum())
		{
			currentIndex.setValue(currentIndex.value()-1);
			emit currentTextureChanged();
		}
	}

// SingleUniformsVarsLoaderInterface
	SingleUniformsVarsLoaderInterface::SingleUniformsVarsLoaderInterface(Pipeline& pipeline, QWidget* parent)
	 : 	QWidget(parent),
		layout(this),
		mainItem(NULL)
	{
		layout.addWidget(&tree);

		mainItem = new UniformsVarsLoaderInterface(23);

		// Add the root item first : 
		tree.addTopLevelItem(mainItem);

		// Load the pipeline : 
		mainItem->load(pipeline);

		tree.expandAll();
		tree.resizeColumnToContents(0);

		// Set Headers :
		QStringList listLabels;
		listLabels.push_back("Name");
		listLabels.push_back("Data");
		tree.setHeaderLabels( listLabels );

		resize(512, 256);

		hide();

		QObject::connect(mainItem, SIGNAL(modified(void)), this, SIGNAL(modified(void)));
	}

	SingleUniformsVarsLoaderInterface::~SingleUniformsVarsLoaderInterface(void)
	{
		tree.takeTopLevelItem(tree.indexOfTopLevelItem(mainItem));
		delete mainItem;
	}

	void SingleUniformsVarsLoaderInterface::applyTo(Pipeline& pipeline, bool forceWrite)
	{
		mainItem->applyTo(pipeline, forceWrite);
	}

// PipelineLoaderInterface
	PipelineLoaderInterface::PipelineLoaderInterface(void)
	 : 	loadedPipeline(NULL), 
		uniformsInterface(NULL),
		loadButton("Load a pipeline"), 
		pipelineName("(No pipeline loaded)"), 
		refreshButton("Reload"),
		showUniformsVarsInterfaceButton("Uniform Variables")
	{
		outputChoice.addItem("<Original Image>");

		pipelineName.setReadOnly(true);
		secondaryLayout.addWidget(&loadButton);
		secondaryLayout.addWidget(&pipelineName);
		secondaryLayout.addWidget(&refreshButton);
		secondaryLayout.addWidget(&showUniformsVarsInterfaceButton);
		
		addLayout(&secondaryLayout);
		addWidget(&outputChoice);

		QObject::connect(&loadButton,				SIGNAL(released(void)), 		this, SLOT(loadPipelineDialog(void)));
		QObject::connect(&refreshButton,			SIGNAL(released(void)), 		this, SLOT(refreshPipeline(void)));
		QObject::connect(&outputChoice,				SIGNAL(currentIndexChanged(int)),	this, SIGNAL(outputIndexChanged(void)));
		QObject::connect(&showUniformsVarsInterfaceButton,	SIGNAL(released(void)), 		this, SLOT(showUniformsVarsInterface(void)));

		LayoutLoaderModule::addBasicModules(layoutLoader);
	}

	PipelineLoaderInterface::~PipelineLoaderInterface(void)
	{
		delete loadedPipeline;
		delete uniformsInterface;
	}

	bool PipelineLoaderInterface::isPipelineValid(void) const
	{
		return loadedPipeline!=NULL;
	}

	bool PipelineLoaderInterface::currentChoiceIsOriginal(void) const
	{
		return outputChoice.currentIndex()==0;
	}

	int PipelineLoaderInterface::currentOutputId(void) const
	{
		if( currentChoiceIsOriginal() )
			return -1;
		else
			return outputChoice.currentIndex()-1;
	}

	Pipeline& PipelineLoaderInterface::pipeline(void)
	{
		if(!isPipelineValid())
			throw Exception("PipelineLoaderInterface::pipeline - No valid pipeline for processing.", __FILE__, __LINE__);
		else
			return *loadedPipeline;
	}

	HdlTexture& PipelineLoaderInterface::currentOutput(HdlTexture& original)
	{
		int port = outputChoice.currentIndex();

		if(port==0)
			return original;
		else if(!isPipelineValid())
			throw Exception("PipelineLoaderInterface::currentOutput - No valid pipeline.", __FILE__, __LINE__);
		else
			return loadedPipeline->out(port-1);
	}

	LayoutLoader& PipelineLoaderInterface::loader(void)
	{
		return layoutLoader;
	}

	void PipelineLoaderInterface::loadPipelineDialog(void)
	{
		QString filename = QFileDialog::getOpenFileName(NULL, tr("Load a Pipeline"), ".", "*.ppl");

		loadPipeline(filename);
	}

	void PipelineLoaderInterface::refreshPipeline(void)
	{
		loadPipeline(previousFilename);
	}

	void PipelineLoaderInterface::loadPipeline(const QString& filename)
	{
		if(!filename.isEmpty())
		{
			PipelineLayout* model 	= NULL;

			try
			{
				model = new PipelineLayout( loader()(filename.toStdString()) );
			}
			catch(Exception& e)
			{
				QMessageBox::information(parentWidget(), tr("PipelineLoaderInterface::loadPipeline - Error while loading the pipeline : "), e.what());
				return ;
			}

			// Keep the path :
			previousFilename = filename;
			refreshButton.setToolTip(filename);

			if(loadedPipeline!=NULL)
			{
				delete loadedPipeline;
				delete uniformsInterface;
				loadedPipeline = NULL;
				uniformsInterface = NULL;

				// clean the outputChoice box too...
				outputChoice.setCurrentIndex(0);
				while(outputChoice.count()>1)
					outputChoice.removeItem(1);
				pipelineName.setText("(No pipeline loaded)");
				pipelineName.setToolTip("");
			}

			try
			{
				loadedPipeline = new Pipeline(*model, "LoadedPipeline");
				delete model;
				model = NULL;

				uniformsInterface = new SingleUniformsVarsLoaderInterface(*loadedPipeline);
				QObject::connect(uniformsInterface, SIGNAL(modified(void)), this, SLOT(updateUniforms(void)));
			}
			catch(Exception& e)
			{
				QMessageBox::information(parentWidget(), tr("PipelineLoaderInterface::loadPipeline - Error while creating the pipeline : "), e.what());
				delete loadedPipeline;
				delete uniformsInterface;
				loadedPipeline = NULL;
				uniformsInterface = NULL;
				pipelineName.setText("(No pipeline loaded)");
				pipelineName.setToolTip("");
				return ;
			}

			// Update the name :
			QFileInfo pathInfo( filename );
			QString strippedFilename( pathInfo.fileName() );
			std::string extName = loadedPipeline->getFullName();
			pipelineName.setText(tr("Pipeline : %1 (%2)").arg(strippedFilename).arg(extName.c_str()));

			// Update the tooltip :
			std::string tooltip = extName + "\nInput ports : ";

			for(int i=0; i<loadedPipeline->getNumInputPort(); i++)
				tooltip += "\n    " + loadedPipeline->getInputPortName(i);

			tooltip += "\n Output ports : ";

			for(int i=0; i<loadedPipeline->getNumOutputPort(); i++)
				tooltip += "\n    " + loadedPipeline->getOutputPortName(i);

			pipelineName.setToolTip(tooltip.c_str());

			// Update the box :
			for(int i=0; i<loadedPipeline->getNumOutputPort(); i++)
			{
				std::string portName = loadedPipeline->getOutputPortName(i) + " (" + toString(loadedPipeline->out(i).getWidth()) + "x" + toString(loadedPipeline->out(i).getHeight()) + "; " + glParamName(loadedPipeline->out(i).getGLMode()) + "; " + glParamName(loadedPipeline->out(i).getGLDepth()) + ")";
				outputChoice.addItem(portName.c_str());
			}

			emit pipelineChanged();
			emit requestComputingUpdate();
		}
	}

	void PipelineLoaderInterface::revokePipeline(void)
	{
		if(loadedPipeline!=NULL)
		{
			delete loadedPipeline;
			delete uniformsInterface;
			loadedPipeline = NULL;
			uniformsInterface = NULL;

			previousFilename = "";

			pipelineName.setText("(No pipeline loaded)");

			// clean the outputChoice box too...
			outputChoice.setCurrentIndex(0);
			while(outputChoice.count()>1)
				outputChoice.removeItem(1);
			pipelineName.setToolTip("");

			emit pipelineChanged();
		}
	}

	void PipelineLoaderInterface::showUniformsVarsInterface(void)
	{
		if(uniformsInterface!=NULL && loadedPipeline!=NULL)
			uniformsInterface->show();
	}

	void PipelineLoaderInterface::updateUniforms(void)
	{
		if(uniformsInterface!=NULL && loadedPipeline!=NULL)
		{
			uniformsInterface->applyTo(*loadedPipeline, false);
			
			emit requestComputingUpdate();
		}
	}

