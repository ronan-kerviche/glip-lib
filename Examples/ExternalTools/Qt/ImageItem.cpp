/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ImageItem.cpp                                                                             */
/*     Original Date : August 16th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Qt interface for loading and writing to images.                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include "ImageItem.hpp"
#include <QFileInfo>
#include <QFileDialog>
#include "NetPBM.hpp"

// To remove : 
#include <QElapsedTimer>

using namespace QGIC;

// QImage tools : 
	void toImageBuffer(const QImage& qimage, ImageBuffer*& imageBuffer)
	{
		// TEST : 
		std::cout << "toImageBuffer" << std::endl;
		std::cout << "  width          : " << qimage.width() << std::endl;
		std::cout << "  bytes per line : " << qimage.bytesPerLine() << std::endl;
		std::cout << "  size           : " << qimage.byteCount() << std::endl;

		QElapsedTimer timer;
		timer.start();

		// Create the format : 
		GLenum mode = GL_NONE;

		if(qimage.allGray())
			mode = GL_LUMINANCE;
		else if(qimage.hasAlphaChannel())
			mode = GL_RGBA;
		else
			mode = GL_RGB;

		const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get(mode);

		HdlTextureFormat textureFormat( qimage.width(), qimage.height(), mode, GL_UNSIGNED_BYTE );
		
		if(imageBuffer==NULL)
		{
			// Create buffer : 
			imageBuffer = new ImageBuffer(textureFormat);
		}
		else
		{
			if((*imageBuffer)!=textureFormat)
				throw Exception("toImageBuffer - imageBuffer has an incompatible format.", __FILE__, __LINE__);
		}

		// Copy : 
		for(int i=0; i<qimage.height(); i++)
		{
			for(int j=0; j<qimage.width(); j++)
			{
				QRgb col 	= qimage.pixel(j,qimage.height() - i - 1); // WARNING Shift up-down to fit texture coordinates.

				//std::cout << i << ", " << j << " : " << QColor(col).name().toStdString() << std::endl;

				if(descriptor.numChannels()==1)
					imageBuffer->set(j, i, GL_LUMINANCE,	qRed(  col ));
				else 
				{
									imageBuffer->set(j, i, GL_RED, 		qRed( col ));
					if(descriptor.numChannels()>1)	imageBuffer->set(j, i, GL_GREEN,	qGreen( col ));
					if(descriptor.numChannels()>2)	imageBuffer->set(j, i, GL_BLUE,		qBlue( col ));
					if(descriptor.numChannels()>3)	imageBuffer->set(j, i, GL_ALPHA,	qAlpha( col ));
				}
			}
		}

		// Second method, test : 
		/*HdlDynamicTable* 	original 	= HdlDynamicTable::buildProxy(const_cast<void*>(reinterpret_cast<const void*>(qimage.bits())), GL_UNSIGNED_BYTE, qimage.width(), qimage.height(), 4);
					imageBuffer 	= new ImageBuffer(HdlTextureFormat(qimage.width(), qimage.height(), GL_RGB, GL_UNSIGNED_BYTE));
		HdlDynamicTable* 	buffer 		= HdlDynamicTable::buildProxy(imageBuffer->getBuffer(), GL_UNSIGNED_BYTE, qimage.width(), qimage.height(), 3);

		// Iterators : 
		HdlDynamicTableIterator source(*original),
					dest(*buffer);

		while(source.isValid())
		{			
			if(false)
			{
				// Per component :
				void* 	blue	= source.getPtr(); source.nextElement();
				void* 	green	= source.getPtr(); source.nextElement();
				void* 	red	= source.getPtr(); source.nextElement();
				void* 	alpha	= source.getPtr(); source.nextElement();
			
				dest.write(red); 	dest.nextElement();
				dest.write(green); 	dest.nextElement();
				dest.write(blue); 	dest.nextElement();
			}
			else
			{
				// Per pixel (twice faster than the previous case): 
				unsigned char* pixel = reinterpret_cast<unsigned char*>(source.getPtr()); source.nextSlice();

				unsigned char data[3] = {*(pixel+2), *(pixel+1), *(pixel+0)};	

				std::memcpy(dest.getPtr(), data, 3);
				dest.nextSlice();
			}
		} 

		delete original;
		delete buffer;*/

		
		// Fastest : 
		/*imageBuffer 	= new ImageBuffer(HdlTextureFormat(qimage.width(), qimage.height(), GL_RGBA, GL_UNSIGNED_BYTE));
		std::memcpy(imageBuffer->getBuffer(), const_cast<void*>(reinterpret_cast<const void*>(qimage.bits())), imageBuffer->getSize());*/

		std::cout << "The slow operation took " << timer.elapsed() << " milliseconds" << std::endl;
	}

	void toQImage(const ImageBuffer& buffer, QImage*& qimage)
	{
		// Get the mode :
		const HdlTextureFormatDescriptor& 	descriptor = HdlTextureFormatDescriptorsList::get( buffer.getGLMode() );
		const int 				depthBytes = HdlTextureFormatDescriptorsList::getTypeDepth( buffer.getGLDepth() );

		if(qimage==NULL)
		{
			// Create : 
			if( descriptor.hasLuminanceChannel() && (descriptor.luminanceDepthInBits==8 || depthBytes==1) )
				qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_RGB888);
			else if( descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && !descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )
				qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_RGB888);
			else if(descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )
				qimage = new QImage(buffer.getWidth(), buffer.getHeight(), QImage::Format_ARGB32);		
			else
				throw Exception("toQImage - Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);
		}
		else
		{
			// Check : 
			if(qimage->width()!=buffer.getWidth() || qimage->height()!=buffer.getHeight())
				throw Exception("toQImage - qimage has an incompatible size.", __FILE__, __LINE__);
			else if( (descriptor.hasLuminanceChannel() && (descriptor.luminanceDepthInBits==8 || depthBytes==1)) && qimage->format()!=QImage::Format_RGB888 )
				throw Exception("toQImage - qimage has an incompatible format.", __FILE__, __LINE__);
			else if( (descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && !descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1)) && qimage->format()!=QImage::Format_RGB888 )
				throw Exception("toQImage - qimage has an incompatible format.", __FILE__, __LINE__);
			else if( (descriptor.hasRedChannel() && descriptor.hasGreenChannel() && descriptor.hasBlueChannel() && descriptor.hasAlphaChannel() && ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1)) && qimage->format()!=QImage::Format_ARGB32 )
				throw Exception("toQImage - qimage has an incompatible format.", __FILE__, __LINE__);
			else
				throw Exception("toQImage - Cannot write texture of mode \"" + glParamName(descriptor.modeID) + "\".", __FILE__, __LINE__);
		}

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
		}
	}

// ImageItem : 
	ImageItem::ImageItem(const ImageItem& imageItem)
	 : 	saved(false),
		lockedToDevice(false),
		name(tr("copy_of_%1").arg(imageItem.getName())),
		filename(""),
		format(imageItem.format),
		imageBuffer(NULL),
		texture(NULL)
	{
		if(imageItem.imageBuffer==NULL)
			throw Exception("[Internal Error] Inner buffer is null.", __FILE__, __LINE__);

		imageBuffer	= new ImageBuffer(*imageItem.imageBuffer);
	}

	ImageItem::ImageItem(const QString& _filename)
	 : 	saved(true),
		lockedToDevice(false),
		name(""),
		filename(_filename),
		format(1, 1, GL_RGB, GL_UNSIGNED_BYTE),
		imageBuffer(NULL),
		texture(NULL)
	{
		load();
		format = (*imageBuffer);
	}

	ImageItem::ImageItem(const QImage& qimage, const QString& _name)
	 : 	saved(false),
		lockedToDevice(false),
		name(_name),
		filename(""),
		format(1, 1, GL_RGB, GL_UNSIGNED_BYTE),
		imageBuffer(NULL),
		texture(NULL)
	{
		toImageBuffer(qimage, imageBuffer);
		format = (*imageBuffer);
	}

	ImageItem::ImageItem(const ImageBuffer& buffer, const QString& _name)
	 : 	saved(false),
		lockedToDevice(false),
		name(_name),
		filename(""),
		format(buffer),
		imageBuffer(NULL),
		texture(NULL)
	{
		imageBuffer = new ImageBuffer(buffer);
	}

	ImageItem::ImageItem(HdlTexture& _texture, const QString& _name)
	 : 	saved(false),
		lockedToDevice(false),
		name(_name),
		filename(""),
		format(_texture),
		imageBuffer(NULL),
		texture(NULL)
	{
		imageBuffer = new ImageBuffer(_texture);
	}

	ImageItem::~ImageItem(void)
	{
		delete imageBuffer;
		delete texture;

		imageBuffer = NULL;
		texture = NULL;
	}

	void ImageItem::load(QString _filename)
	{
		delete imageBuffer;
		imageBuffer = NULL;

		if(_filename.isEmpty())
			_filename = filename;

		// Set the name to the filename without the path : 
		QFileInfo path(_filename);
		name = path.fileName();

		if(!path.exists())
			throw Exception("File \"" + _filename.toStdString() + "\" does not exist.", __FILE__, __LINE__);

		// Load : 
		if(path.completeSuffix()=="raw")
		{
			std::string comment;

			imageBuffer 	= ImageBuffer::load(filename.toStdString(), &comment);

			if(!comment.empty())
			{
				std::cout << "Comment in RAW file \"" << _filename.toStdString() << "\" : " << std::endl;
				std::cout << comment << std::endl;
			}
		}
		else if(path.completeSuffix()=="ppm" || path.completeSuffix()=="pgm")
			imageBuffer 	= NetPBM::loadNetPBMFile(filename.toStdString());
		else
		{
			QImage qimage(filename);

			if(qimage.isNull())
				throw Exception("Cannot load image \"" + filename.toStdString() + "\".", __FILE__, __LINE__);

			 toImageBuffer(qimage, imageBuffer);
		}
	}

	bool ImageItem::isOnDevice(void) const
	{
		return (texture!=NULL);
	}

	bool ImageItem::loadToDevice(void)
	{
		if(imageBuffer==NULL)
			throw Exception("[Internal Error] Inner buffer is null.", __FILE__, __LINE__);
		else if(texture==NULL)
		{
			std::cout << "ImageItem::loadToDevice : " << ImageItemsStorage::checkMemSpaceAvailabilty(imageBuffer->getSize()) << std::endl;

			// Test if it can be loaded on the GPU : 
			if(ImageItemsStorage::checkMemSpaceAvailabilty(imageBuffer->getSize()))
			{
				// Load : 
				texture = new HdlTexture(format);
				(*imageBuffer) >> (*texture);

				emit loadedOnDevice();
				
				return true;
			}
			else
				return false;
		}
		else	// Already on device : 
			return true;
	}

	void ImageItem::unloadFromDevice(void)
	{
		if(texture!=NULL)
		{
			delete texture;
			texture = NULL;

			emit unloadedFromDevice();
		}
	}

	void ImageItem::lockToDevice(bool enabled)
	{
		if(enabled!=lockedToDevice)
		{
			lockedToDevice = enabled;

			emit locking(lockedToDevice);
		}
	}

	bool ImageItem::isLockedToDevice(void) const
	{
		return lockedToDevice;
	}

	const __ReadOnly_HdlTextureFormat& ImageItem::getFormat(void) const
	{
		return format;
	}

	#define MAKE_SET_FILTER( filterFunction , type ) \
		void ImageItem:: filterFunction ( type filter) \
		{ \
			if(imageBuffer!=NULL) \
				imageBuffer-> filterFunction (filter); \
			if(texture!=NULL) \
				texture-> filterFunction (filter); \
			format. filterFunction (filter); \
			emit formatModified(); \
		}

		MAKE_SET_FILTER( setMinFilter, GLenum)
		MAKE_SET_FILTER( setMagFilter, GLenum)
		MAKE_SET_FILTER( setSWrapping, GLint)
		MAKE_SET_FILTER( setTWrapping, GLint)
		
	#undef MAKE_SET_FILTER

	const QString& ImageItem::getFilename(void) const
	{
		return filename;
	}

	void ImageItem::setFilename(const QString& newFilename)
	{
		filename = newFilename;
		emit filenameModified();
	}

	QString ImageItem::getName(void) const
	{	
		return name;
	}

	void ImageItem::setName(const QString& newName)
	{
		name = newName;
		emit nameModified();
	}
	
	const ImageBuffer& ImageItem::getImageBuffer(void) const
	{
		if(imageBuffer==NULL)
			throw Exception("ImageItem::getImageBuffer - No buffer allocated.", __FILE__, __LINE__);
		else
			return (*imageBuffer);
	}

	ImageBuffer& ImageItem::getImageBuffer(void)
	{
		if(imageBuffer==NULL)
			throw Exception("ImageItem::getImageBuffer - No buffer allocated.", __FILE__, __LINE__);
		else
			return (*imageBuffer);
	}

	ImageBuffer* ImageItem::getImageBufferPtr(void)
	{
		return imageBuffer;
	}

	HdlTexture& ImageItem::getTexture(void)
	{
		if(texture==NULL)
			throw Exception("ImageItem::getTexture - Texture is not currently on device.", __FILE__, __LINE__);
		else
			return (*texture);
	}

	HdlTexture* ImageItem::getTexturePtr(void)
	{
		if(texture==NULL)
			throw Exception("ImageItem::getTexture - Texture is not currently on device.", __FILE__, __LINE__);
		else
			return texture;
	}

	bool ImageItem::isSaved(void) const
	{
		return saved;
	}

	void ImageItem::save(QString _filename)
	{
		if(_filename.isEmpty())
			_filename = filename;

		// Open the file and guess the texture format : 
		QFileInfo path(_filename);
			
		if(path.completeSuffix()=="ppm" || path.completeSuffix()=="pgm")
			NetPBM::saveNetPBMToFile(*imageBuffer, _filename.toStdString());
		else if(path.completeSuffix()=="raw")
			imageBuffer->write(_filename.toStdString(), "Written by GlipStudio.");
		else
		{
			QImage* qimage = NULL;

			toQImage(*imageBuffer, qimage);
			
			const bool test = qimage->save(_filename);

			delete qimage;

			if(!test)
				throw Exception("Cannot save image to file \"" + _filename.toStdString() + "\".", __FILE__, __LINE__);
		}

		// Save filename, and set as saved : 
		setFilename( _filename );
		saved = true;

		emit savedToDisk();
	}

	void ImageItem::copyToClipboard(void)
	{
		QClipboard *clipboard = QApplication::clipboard();

		QImage* qimage = NULL;

		if(clipboard==NULL)
			throw Exception("ImageItem::copyToClipboard - Cannot access the clipboard.", __FILE__, __LINE__);
		else if(imageBuffer==NULL)
			throw Exception("ImageItem::copyToClipboard - No buffer allocated.", __FILE__, __LINE__);
		else
		{
			toQImage(*imageBuffer, qimage);

			// Copy to cliboard : 
			clipboard->setImage(*qimage);

			delete qimage;
		}
	}

	void ImageItem::remove(void)
	{
		lockToDevice(false);
		unloadFromDevice();

		emit removed();
	}

	QString ImageItem::getSizeString(size_t size)
	{
		const float	GB = 1024.0f*1024.0f*1024.0f,
				MB = 1024.0f*1024.0f,
				KB = 1024.0f;

		if(size>=GB)
			return tr("%1 GB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(size)/GB)) );
		else if(size>=MB)
			return tr("%1 MB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(size)/MB)) );
		else if(size>=KB)
			return tr("%1 KB").arg( static_cast<unsigned int>(std::ceil(static_cast<float>(size)/KB)) );
		else 
			return tr("%1 B").arg( size );
	}

	ImageItem* ImageItem::pasteImageFromClipboard(void)
	{
		QClipboard *clipboard = QApplication::clipboard();
	
		if(clipboard==NULL)
			throw Exception("ImageItem::copyToClipboard - Cannot access the clipboard.", __FILE__, __LINE__);

		QImage qimage = clipboard->image();

		if(qimage.isNull())
			return NULL;
		else
		{
			ImageItem* imageItem = new ImageItem(qimage, "untitled-from-clipboard");

			return imageItem;
		}
	}

// ImageItemsStorage :
	size_t ImageItemsStorage::maxOccupancy = 512 * 1024 * 1024; // 512 MB
	QVector<ImageItemsStorage*> ImageItemsStorage::storagesList;

	ImageItemsStorage::ImageItemsStorage(void)
	{
		// Add itself to the list of storages : 
		storagesList.push_back(this);
	}

	ImageItemsStorage::~ImageItemsStorage(void)
	{
		// Clear the list : 
		for(QVector<ImageItem*>::iterator it=imageItemsList.begin(); it!=imageItemsList.end(); it++)
			delete (*it);

		imageItemsList.clear();

		// Remove itself : 
		int idx = storagesList.indexOf(this);

		if(idx>=0)
			storagesList.remove(idx);
	}

	void ImageItemsStorage::imageItemDestroyed(void)
	{
		ImageItem* target = reinterpret_cast<ImageItem*>(QObject::sender());

		int idx = imageItemsList.indexOf(target);
		
		if(idx>=0)
			imageItemsList.remove(idx);
	}

	void ImageItemsStorage::imageItemRemoved(void)
	{
		ImageItem* target = reinterpret_cast<ImageItem*>(QObject::sender());

		int idx = imageItemsList.indexOf(target);
		
		if(idx>=0)
		{
			imageItemsList[idx]->disconnect(this);
			delete imageItemsList[idx];
			imageItemsList.remove(idx);
		}
	}

	bool ImageItemsStorage::hasImageItem(ImageItem* imageItem) const
	{
		int idx = imageItemsList.indexOf(imageItem);

		return (idx>=0);
	}

	void ImageItemsStorage::addImageItem(QGIC::ImageItem* imageItem)
	{
		if(imageItem->isOnDevice())
			throw Exception("ImageItemsStorage::addImageItem - Cannot store an image which is alredy loaded on the device.", __FILE__, __LINE__);

		int idx = imageItemsList.indexOf(imageItem);

		if(idx<0)
		{
			imageItemsList.push_back(imageItem);

			QObject::connect(imageItem, SIGNAL(destroyed()), this, SLOT(imageItemDestroyed()));
			QObject::connect(imageItem, SIGNAL(removed()), 	 this, SLOT(imageItemRemoved()));
		}
	}

	// Manage the storage : 
	size_t ImageItemsStorage::getMaxOccupancy(void)
	{
		return maxOccupancy;
	}

	void ImageItemsStorage::setMaxOccupancy(size_t newMaxOccupancy)
	{
		size_t 	totalOccupancy = ImageItemsStorage::totalDeviceOccupancy();

		if(totalOccupancy > newMaxOccupancy)
			cleanStorages();

		// Finally, set : 
		maxOccupancy = newMaxOccupancy;
	}

	size_t ImageItemsStorage::thisStorageDeviceOccupancy(size_t* canBeFreed) const
	{
		size_t occupancy = 0;

		if(canBeFreed!=NULL)
			(*canBeFreed) = 0;

		for(QVector<ImageItem*>::const_iterator it=imageItemsList.begin(); it!=imageItemsList.end(); it++)
		{
			if((*it)->isOnDevice())
			{
				size_t s = (*it)->getFormat().getSize();

				occupancy += s;

				if(canBeFreed!=NULL && !(*it)->isLockedToDevice())
					(*canBeFreed) += s;
			}
		}

		return occupancy;
	}

	void ImageItemsStorage::cleanThisStorage(void)
	{
		for(QVector<ImageItem*>::iterator it=imageItemsList.begin(); it!=imageItemsList.end(); it++)
		{
			if((*it)->isOnDevice() && !(*it)->isLockedToDevice())
				(*it)->unloadFromDevice();
		}
	}

	size_t ImageItemsStorage::totalDeviceOccupancy(size_t* canBeFreed)
	{
		size_t totalOccupancy = 0;

		if(canBeFreed!=NULL)
			(*canBeFreed) = 0;

		for(QVector<ImageItemsStorage*>::iterator it=storagesList.begin(); it!=storagesList.end(); it++)
		{
			size_t thisStorageCanBeFreed = 0;

			totalOccupancy += (*it)->thisStorageDeviceOccupancy(&thisStorageCanBeFreed);
			
			if(canBeFreed!=NULL)
				(*canBeFreed) += thisStorageCanBeFreed;
		}

		return totalOccupancy;
	}

	void ImageItemsStorage::cleanStorages(void)
	{
		for(QVector<ImageItemsStorage*>::iterator it=storagesList.begin(); it!=storagesList.end(); it++)
			(*it)->cleanThisStorage();
	}

	bool ImageItemsStorage::checkMemSpaceAvailabilty(size_t futureAdd)
	{
		size_t 	totalOccupancy	= 0,
			canBeFreed	= 0;
		
		totalOccupancy = ImageItemsStorage::totalDeviceOccupancy(&canBeFreed);

		// If it cannot be added directly : 
		if((totalOccupancy+futureAdd) > maxOccupancy)
		{
			// If some occupancy can be freed to enable the storage : 
			if( (totalOccupancy+futureAdd-canBeFreed) < maxOccupancy)
			{
				cleanStorages();
				return true;
			}
			else
				return false;
		}
		else	// It can be added directly : 
			return true;
	}

// Filter Menu : 
	FilterMenu::FilterMenu(QWidget* parent)
	 : 	QMenu("Filtering", parent),
		minFilter("Min filter", this),
		magFilter("Mag filter", this),
		bothNearest("Change both to GL_NEAREST", this),
		bothLinear("Change both to GL_LINEAR", this),
		minNearest("GL_NEAREST", this),
		minLinear("GL_LINEAR", this),
		minNearestMipmapNearest("GL_NEAREST_MIPMAP_NEAREST", this),
		minNearestMipmapLinear("GL_NEAREST_MIPMAP_LINEAR", this),
		minLinerarMipmapNearest("GL_LINEAR_MIPMAP_NEAREST", this),
		minLinearMipmapLinear("GL_LINEAR_MIPMAP_LINEAR", this),
		magNearest("GL_NEAREST", this),
		magLinear("GL_LINEAR", this)
	{
		bothNearest.setData( 			QVariant( QPoint( GL_NEAREST, 			GL_NEAREST)));
		bothLinear.setData( 			QVariant( QPoint( GL_LINEAR, 			GL_LINEAR)));
		minNearest.setData( 			QVariant( QPoint( GL_NEAREST, 			GL_NONE)));
		minLinear.setData( 			QVariant( QPoint( GL_LINEAR, 			GL_NONE)));
		minNearestMipmapNearest.setData( 	QVariant( QPoint( GL_NEAREST_MIPMAP_NEAREST, 	GL_NONE)));
		minNearestMipmapLinear.setData( 	QVariant( QPoint( GL_NEAREST_MIPMAP_LINEAR, 	GL_NONE)));
		minLinerarMipmapNearest.setData( 	QVariant( QPoint( GL_LINEAR_MIPMAP_NEAREST, 	GL_NONE)));
		minLinearMipmapLinear.setData( 		QVariant( QPoint( GL_LINEAR_MIPMAP_LINEAR, 	GL_NONE)));
		magNearest.setData( 			QVariant( QPoint( GL_NONE, 			GL_NEAREST)));
		magLinear.setData( 			QVariant( QPoint( GL_NONE, 			GL_LINEAR)));

		bothNearest.setCheckable(true);
		bothLinear.setCheckable(true);
		minNearest.setCheckable(true);
		minLinear.setCheckable(true);
		magNearest.setCheckable(true);
		magLinear.setCheckable(true);
		minNearestMipmapNearest.setCheckable(true);
		minNearestMipmapLinear.setCheckable(true);
		minLinerarMipmapNearest.setCheckable(true);
		minLinearMipmapLinear.setCheckable(true);

		clearChecked();

		minFilter.addAction(&minNearest);
		minFilter.addAction(&minLinear);
		minFilter.addAction(&minNearestMipmapNearest);
		minFilter.addAction(&minNearestMipmapLinear);
		minFilter.addAction(&minLinerarMipmapNearest);
		minFilter.addAction(&minLinearMipmapLinear);

		magFilter.addAction(&magNearest);
		magFilter.addAction(&magLinear);

		addAction(&bothNearest);
		addAction(&bothLinear);				
		addMenu(&minFilter);
		addMenu(&magFilter);

		update();

		QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(processAction(QAction*)));
	}

	FilterMenu::~FilterMenu(void)
	{ }

	void FilterMenu::clearChecked(void)
	{
		bothNearest.setChecked(false);
		bothLinear.setChecked(false);
		minNearest.setChecked(false);
		minLinear.setChecked(false);
		magNearest.setChecked(false);
		magLinear.setChecked(false);
		minNearestMipmapNearest.setChecked(false);
		minNearestMipmapLinear.setChecked(false);
		minLinerarMipmapNearest.setChecked(false);
		minLinearMipmapLinear.setChecked(false);
	}

	void FilterMenu::update(void)
	{
		bothNearest.setEnabled(false);
		bothLinear.setEnabled(false);
		minNearest.setEnabled(false);
		minLinear.setEnabled(false);
		magNearest.setEnabled(false);
		magLinear.setEnabled(false);
		minNearestMipmapNearest.setEnabled(false);
		minNearestMipmapLinear.setEnabled(false);
		minLinerarMipmapNearest.setEnabled(false);
		minLinearMipmapLinear.setEnabled(false);

		clearChecked();
	}

	void FilterMenu::update(const __ReadOnly_HdlTextureFormat& format)
	{
		bothNearest.setEnabled(true);
		bothLinear.setEnabled(true);
		minNearest.setEnabled(true);
		minLinear.setEnabled(true);
		magNearest.setEnabled(true);
		magLinear.setEnabled(true);

		// Disable unaccessible data : 
		if(format.getMaxLevel()==0)
		{
			minNearestMipmapNearest.setEnabled(false);
			minNearestMipmapLinear.setEnabled(false);
			minLinerarMipmapNearest.setEnabled(false);
			minLinearMipmapLinear.setEnabled(false);
		}
		else
		{
			minNearestMipmapNearest.setEnabled(true);
			minNearestMipmapLinear.setEnabled(true);
			minLinerarMipmapNearest.setEnabled(true);
			minLinearMipmapLinear.setEnabled(true);
		}

		// Update checked : 
		clearChecked();

		if(format.getMagFilter()==format.getMinFilter())
		{
			if(format.getMagFilter()==GL_NEAREST)
				bothNearest.setChecked(true);
			else if(format.getMagFilter()==GL_LINEAR)
				bothLinear.setChecked(true);
		}

		switch(format.getMinFilter())
		{
			case GL_NEAREST :
				minNearest.setChecked(true);
				break;
			case GL_LINEAR :
				minLinear.setChecked(true);
				break;
			case GL_NEAREST_MIPMAP_NEAREST : 
				minNearestMipmapNearest.setChecked(true);
				break;
			case GL_NEAREST_MIPMAP_LINEAR :
				minNearestMipmapLinear.setChecked(true);
				break;
			case GL_LINEAR_MIPMAP_NEAREST :
				minLinerarMipmapNearest.setChecked(true);
				break;
			case GL_LINEAR_MIPMAP_LINEAR :
				minLinearMipmapLinear.setChecked(true);
				break;
			default : 
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + glParamName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
		}

		switch(format.getMagFilter())
		{
			case GL_NEAREST :
				magNearest.setChecked(true);
				break;
			case GL_LINEAR :
				magLinear.setChecked(true);
				break;
			default : 
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + glParamName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
		}
	}

	void FilterMenu::update(const QList<HdlTextureFormat>& formats)
	{
		if(formats.empty())
			update();
		else
			update(formats.front());
	}

	void FilterMenu::get(QAction* action, GLenum& dminFilter, GLenum& dmagFilter)
	{
		if(action!=NULL)
		{
			dminFilter = action->data().toPoint().x();
			dmagFilter = action->data().toPoint().y();
		}
	}

	bool FilterMenu::get(const QPoint& pos, GLenum& minFilter, GLenum& magFilter)
	{
		QAction* selectedItem = exec(pos);

		if(selectedItem!=NULL)
		{
			minFilter = selectedItem->data().toPoint().x();
			magFilter = selectedItem->data().toPoint().y();
			return true;
		}
		else
			return false;
	}

	void FilterMenu::processAction(QAction* action)
	{
		if(action!=NULL)
		{
			GLenum	minFilter = action->data().toPoint().x(),
				magFilter = action->data().toPoint().y();

			if(minFilter!=GL_NONE)
				emit changeMinFilter(minFilter);

			if(magFilter!=GL_NONE)
				emit changeMagFilter(magFilter);
		}
	}

// WrappingMenu :
	WrappingMenu::WrappingMenu(QWidget* parent)
	 : 	QMenu("Wrapping", parent),
		sMenu("S Wrapping", this),
		tMenu("T Wrapping", this), 
		bothClamp("Change both to GL_CLAMP", this),
		bothClampToBorder("Change both to GL_CLAMP_TO_BORDER", this),
		bothClampToEdge("Change both to GL_CLAMP_TO_EDGE", this),
		bothRepeat("Change both to GL_REPEAT", this),
		bothMirroredRepeat("Change both to GL_MIRRORED_REPEAT", this),
		sClamp("GL_CLAMP", this),
		sClampToBorder("GL_CLAMP_TO_BORDER", this),
		sClampToEdge("GL_CLAMP_TO_EDGE", this),
		sRepeat("GL_REPEAT", this),
		sMirroredRepeat("GL_MIRRORED_REPEAT", this),
		tClamp("GL_CLAMP", this),
		tClampToBorder("GL_CLAMP_TO_BORDER", this),
		tClampToEdge("GL_CLAMP_TO_EDGE", this),
		tRepeat("GL_REPEAT", this),
		tMirroredRepeat("GL_MIRRORED_REPEAT", this)
	{
		bothClamp.setData( 		QVariant( QPoint( GL_CLAMP,		GL_CLAMP)));
		bothClampToBorder.setData( 	QVariant( QPoint( GL_CLAMP_TO_BORDER, 	GL_CLAMP_TO_BORDER)));
		bothClampToEdge.setData( 	QVariant( QPoint( GL_CLAMP_TO_EDGE,	GL_CLAMP_TO_EDGE)));
		bothRepeat.setData( 		QVariant( QPoint( GL_REPEAT,		GL_REPEAT)));
		bothMirroredRepeat.setData( 	QVariant( QPoint( GL_MIRRORED_REPEAT,	GL_MIRRORED_REPEAT)));

		sClamp.setData( 		QVariant( QPoint( GL_CLAMP,		GL_NONE)));
		sClampToBorder.setData( 	QVariant( QPoint( GL_CLAMP_TO_BORDER, 	GL_NONE)));
		sClampToEdge.setData( 		QVariant( QPoint( GL_CLAMP_TO_EDGE,	GL_NONE)));
		sRepeat.setData( 		QVariant( QPoint( GL_REPEAT,		GL_NONE)));
		sMirroredRepeat.setData( 	QVariant( QPoint( GL_MIRRORED_REPEAT,	GL_NONE)));

		tClamp.setData( 		QVariant( QPoint( GL_NONE,		GL_CLAMP)));
		tClampToBorder.setData( 	QVariant( QPoint( GL_NONE,		GL_CLAMP_TO_BORDER)));
		tClampToEdge.setData( 		QVariant( QPoint( GL_NONE,		GL_CLAMP_TO_EDGE)));
		tRepeat.setData( 		QVariant( QPoint( GL_NONE,		GL_REPEAT)));
		tMirroredRepeat.setData( 	QVariant( QPoint( GL_NONE,		GL_MIRRORED_REPEAT)));

		bothClamp.setCheckable(true);
		bothClampToBorder.setCheckable(true);
		bothClampToEdge.setCheckable(true);
		bothRepeat.setCheckable(true);
		bothMirroredRepeat.setCheckable(true);
		sClamp.setCheckable(true);
		sClampToBorder.setCheckable(true);
		sClampToEdge.setCheckable(true);
		sRepeat.setCheckable(true);
		sMirroredRepeat.setCheckable(true);
		tClamp.setCheckable(true);
		tClampToBorder.setCheckable(true);
		tClampToEdge.setCheckable(true);
		tRepeat.setCheckable(true);
		tMirroredRepeat.setCheckable(true);

		clearChecked();

		sMenu.addAction(&sClamp);
		sMenu.addAction(&sClampToBorder);
		sMenu.addAction(&sClampToEdge);
		sMenu.addAction(&sRepeat);
		sMenu.addAction(&sMirroredRepeat);

		tMenu.addAction(&tClamp);
		tMenu.addAction(&tClampToBorder);
		tMenu.addAction(&tClampToEdge);
		tMenu.addAction(&tRepeat);
		tMenu.addAction(&tMirroredRepeat);

		addAction(&bothClamp);
		addAction(&bothClampToBorder);
		addAction(&bothClampToEdge);
		addAction(&bothRepeat);
		addAction(&bothMirroredRepeat);

		addMenu(&sMenu);
		addMenu(&tMenu);

		update();

		QObject::connect(this, SIGNAL(triggered(QAction*)), this, SLOT(processAction(QAction*)));
	}

	void WrappingMenu::clearChecked(void)
	{
		bothClamp.setChecked(false);
		bothClampToBorder.setChecked(false);
		bothClampToEdge.setChecked(false);
		bothRepeat.setChecked(false);
		bothMirroredRepeat.setChecked(false);

		sClamp.setChecked(false);
		sClampToBorder.setChecked(false);
		sClampToEdge.setChecked(false);
		sRepeat.setChecked(false);
		sMirroredRepeat.setChecked(false);

		tClamp.setChecked(false);
		tClampToBorder.setChecked(false);
		tClampToEdge.setChecked(false);
		tRepeat.setChecked(false);
		tMirroredRepeat.setChecked(false);
	}

	void WrappingMenu::update(void)
	{
		bothClamp.setEnabled(false);
		bothClampToBorder.setEnabled(false);
		bothClampToEdge.setEnabled(false);
		bothRepeat.setEnabled(false);
		bothMirroredRepeat.setEnabled(false);
		sClamp.setEnabled(false);
		sClampToBorder.setEnabled(false);
		sClampToEdge.setEnabled(false);
		sRepeat.setEnabled(false);
		sMirroredRepeat.setEnabled(false);
		tClamp.setEnabled(false);
		tClampToBorder.setEnabled(false);
		tClampToEdge.setEnabled(false);
		tRepeat.setEnabled(false);
		tMirroredRepeat.setEnabled(false);
	}

	void WrappingMenu::update(const __ReadOnly_HdlTextureFormat& format)
	{
		bothClamp.setEnabled(true);
		bothClampToBorder.setEnabled(true);
		bothClampToEdge.setEnabled(true);
		bothRepeat.setEnabled(true);
		bothMirroredRepeat.setEnabled(true);
		sClamp.setEnabled(true);
		sClampToBorder.setEnabled(true);
		sClampToEdge.setEnabled(true);
		sRepeat.setEnabled(true);
		sMirroredRepeat.setEnabled(true);
		tClamp.setEnabled(true);
		tClampToBorder.setEnabled(true);
		tClampToEdge.setEnabled(true);
		tRepeat.setEnabled(true);
		tMirroredRepeat.setEnabled(true);

		clearChecked();

		if(format.getSWrapping()==format.getTWrapping())
		{
			switch(format.getSWrapping())
			{
				case GL_CLAMP :
					bothClamp.setChecked(true);
					break;
				case GL_CLAMP_TO_BORDER :
					bothClampToBorder.setChecked(true);
					break;
				case GL_CLAMP_TO_EDGE : 
					bothClampToEdge.setChecked(true);
					break;
				case GL_REPEAT :
					bothRepeat.setChecked(true);
					break;
				case GL_MIRRORED_REPEAT :
					bothMirroredRepeat.setChecked(true);
					break;
				default : 
					throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
			}
		}
	
		// Tick boxes : 
		switch(format.getSWrapping())
		{
			case GL_CLAMP :
				sClamp.setChecked(true);
				break;
			case GL_CLAMP_TO_BORDER :
				sClampToBorder.setChecked(true);
				break;
			case GL_CLAMP_TO_EDGE : 
				sClampToEdge.setChecked(true);
				break;
			case GL_REPEAT :
				sRepeat.setChecked(true);
				break;
			case GL_MIRRORED_REPEAT :
				sMirroredRepeat.setChecked(true);
				break;
			default : 
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
		}

		switch(format.getTWrapping())
		{
			case GL_CLAMP :
				tClamp.setChecked(true);
				break;
			case GL_CLAMP_TO_BORDER :
				tClampToBorder.setChecked(true);
				break;
			case GL_CLAMP_TO_EDGE : 
				tClampToEdge.setChecked(true);
				break;
			case GL_REPEAT :
				tRepeat.setChecked(true);
				break;
			case GL_MIRRORED_REPEAT :
				tMirroredRepeat.setChecked(true);
				break;
			default : 
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + glParamName(format.getTWrapping()) + "\".", __FILE__, __LINE__);
		}
	}

	void WrappingMenu::update(const QList<HdlTextureFormat>& formats)
	{
		if(formats.empty())
			update();
		else
			update(formats.front());
	}

	void WrappingMenu::get(QAction* action, GLenum& sWrapping, GLenum& tWrapping)
	{
		if(action!=NULL)
		{
			sWrapping = action->data().toPoint().x();
			tWrapping = action->data().toPoint().y();
		}
	}

	bool WrappingMenu::get(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping)
	{
		QAction* selectedItem = exec(pos);

		if(selectedItem!=NULL)
		{
			sWrapping = selectedItem->data().toPoint().x();
			tWrapping = selectedItem->data().toPoint().y();
			return true;
		}
		else
			return false;
	}

	void  WrappingMenu::processAction(QAction* action)
	{
		if(action!=NULL)
		{
			GLenum 	sWrapping = action->data().toPoint().x(),
				tWrapping = action->data().toPoint().y();

			if(sWrapping!=GL_NONE)
				emit changeSWrapping(sWrapping);

			if(tWrapping!=GL_NONE)
				emit changeTWrapping(tWrapping);
		}
	}

// CollectionWidget :
	CollectionWidget::CollectionWidget(QWidget* parent)
	 : 	QTreeWidget(parent)
	{
		setIndentation(2);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::ContiguousSelection);
		setContextMenuPolicy(Qt::CustomContextMenu);
	}

	CollectionWidget::~CollectionWidget(void)
	{ }

// ImageItemsCollection : 
	ImageItemsCollection::ImageItemsCollection(void)
	 : 	layout(this),
		menuBar(this),
		saveAction(NULL),
		saveAsAction(NULL),
		removeAction(NULL),
		imagesMenu("Images", this),
		filterMenu(this),
		wrappingMenu(this),
		collectionWidget(this)
	{
		layout.addWidget(&menuBar);
		layout.addWidget(&collectionWidget);
		layout.setMargin(0);
		layout.setSpacing(0);

				  imagesMenu.addAction("Open",		this, SLOT(open()));
				  imagesMenu.addAction("Paste",		this, SLOT(paste()));
		copyAction	= imagesMenu.addAction("Copy",		this, SLOT(copy()));
		saveAction 	= imagesMenu.addAction("Save",		this, SLOT(save()));
		saveAsAction	= imagesMenu.addAction("Save as",	this, SLOT(saveAs()));
		removeAction	= imagesMenu.addAction("Remove",	this, SLOT(removeImageItem()));
		removeAllAction	= imagesMenu.addAction("Remove all",	this, SLOT(removeAllImageItem()));

		menuBar.addMenu(&imagesMenu);
		menuBar.addMenu(&filterMenu);
		menuBar.addMenu(&wrappingMenu);

		QStringList listLabels;

		for(int k=0; k<NumColumns; k++)
		{
			switch(k)
			{
				case NameColumn:
					listLabels << "Name";
					break;
				case SizeColumn:
					listLabels << "Size";
					break;
				case StatusColumn:
					listLabels << " Status "; // The extra spaces ensure the extra-long content is not trunctated in the column.
					break;
				case ConnectionColumn:
					listLabels << "Connection";
					break;
				case FilteringColumn:
					listLabels << "Filter";
					break;
				default : 
					throw Exception("ImageItemsCollection::ImageItemsCollection - Missing column declaration [Internal Error].", __FILE__, __LINE__);
			}
		}
		
		collectionWidget.setHeaderLabels( listLabels );

		// Connections :
		QObject::connect(&collectionWidget, 	SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), 	this, SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
		QObject::connect(&collectionWidget, 	SIGNAL(itemActivated(QTreeWidgetItem*,int)),			this, SLOT(itemActivated(QTreeWidgetItem*,int)));
		QObject::connect(&collectionWidget, 	SIGNAL(itemSelectionChanged()),					this, SLOT(itemSelectionChanged()));
		QObject::connect(&collectionWidget,	SIGNAL(customContextMenuRequested(const QPoint&)),		this, SLOT(openContextMenu(const QPoint&)));
		QObject::connect(&filterMenu, 		SIGNAL(changeMinFilter(GLenum)),				this, SLOT(changeMinFilter(GLenum)));
		QObject::connect(&filterMenu, 		SIGNAL(changeMagFilter(GLenum)),				this, SLOT(changeMagFilter(GLenum)));
		QObject::connect(&wrappingMenu,		SIGNAL(changeSWrapping(GLenum)),				this, SLOT(changeSWrapping(GLenum)));
		QObject::connect(&wrappingMenu,		SIGNAL(changeTWrapping(GLenum)),				this, SLOT(changeTWrapping(GLenum)));
	}

	ImageItemsCollection::~ImageItemsCollection(void)
	{ }

	void ImageItemsCollection::updateAlternateColors(void)
	{
		QBrush 	original 	= collectionWidget.palette().background().color(),
			darker		= QBrush(original.color().lighter(90)),
			lighter		= QBrush(original.color().lighter(110));
		
		QBrush* ptr = NULL;
		for(int k=0; k<collectionWidget.topLevelItemCount(); k++)
		{
			if(k%2==0)
				ptr = &lighter;
			else
				ptr = &darker;

			for(int l=0; l<collectionWidget.columnCount(); l++)
				collectionWidget.topLevelItem(k)->setBackground(l, QBrush(*ptr));
		}
	}

	void ImageItemsCollection::updateColumnSize(void)
	{
		for(int k=NameColumn; k<NumColumns; k++)
			collectionWidget.resizeColumnToContents(k);
	}

	const QTreeWidgetItem* ImageItemsCollection::getTreeItem(ImageItem* imageItem) const
	{
		QMap<ImageItem*, QTreeWidgetItem*>::const_iterator it = items.find(imageItem);

		if(it==items.end())
			return NULL;
		else
			return it.value();
	}

	QTreeWidgetItem* ImageItemsCollection::getTreeItem(ImageItem* imageItem)
	{
		QMap<ImageItem*, QTreeWidgetItem*>::iterator it = items.find(imageItem);

		if(it==items.end())
			return NULL;
		else
			return it.value();
	}

	QList<ImageItem*> ImageItemsCollection::getSelectedImageItems(void)
	{
		QList<QTreeWidgetItem*> selectedItems = collectionWidget.selectedItems();

		QList<ImageItem*> result;

		for(QList<QTreeWidgetItem*>::iterator it=selectedItems.begin(); it!=selectedItems.end(); it++)
			result.push_back( getImageItem(*(*it)) );

		return result;
	}

	QList<HdlTextureFormat> ImageItemsCollection::getSelectedImageItemFormat(void)
	{
		QList<ImageItem*> imageItems = getSelectedImageItems();

		QList<HdlTextureFormat> result;

		for(QList<ImageItem*>::iterator it=imageItems.begin(); it!=imageItems.end(); it++)
			result.push_back((*it)->getFormat());

		return result;
	}

	#define GET_ITEM_SAFE( itemName ) \
				if(imageItem==NULL) \
					imageItem = reinterpret_cast<ImageItem*>(QObject::sender()); \
				QTreeWidgetItem* itemName = getTreeItem(imageItem); \
				if( itemName ==NULL) \
					return ;

	void ImageItemsCollection::updateImageItem(ImageItem* imageItem)
	{
		imageItemFormatModified(imageItem);
		imageItemNameModified(imageItem);
		imageItemFilenameModified(imageItem);
		imageItemStatusChanged(imageItem);
	}

	void ImageItemsCollection::imageItemFormatModified(ImageItem* imageItem)
	{
		GET_ITEM_SAFE( item )

		// Format Size, Mode and Depth : 
		const __ReadOnly_HdlTextureFormat& format = imageItem->getFormat();

		QString formatString;

		const HdlTextureFormatDescriptor& descriptor = format.getFormatDescriptor();

		// Mode 
		if( descriptor.hasRedChannel() )
			formatString.append("R");
		if( descriptor.hasGreenChannel() )
			formatString.append("G");
		if( descriptor.hasBlueChannel() )
			formatString.append("B");
		if( descriptor.hasAlphaChannel() )
			formatString.append("A");
		if( descriptor.hasLuminanceChannel() )
			formatString.append("L");
	
		// Depth : 
		formatString.append( tr(" %1 bits").arg(format.getChannelDepth()*8) );

		QString sizeStr = tr("%1 x %2 (%3; %4)").arg(format.getWidth()).arg(format.getHeight()).arg(ImageItem::getSizeString(format.getSize())).arg(formatString);		

		item->setText(SizeColumn, sizeStr);

		// Filtering : 
		QString filteringStr;

		filteringStr = tr("%1/%2; %3/%4").arg(glParamName(format.getSWrapping()).c_str()).arg(glParamName(format.getTWrapping()).c_str()).arg(glParamName(format.getMinFilter()).c_str()).arg(glParamName(format.getMagFilter()).c_str());

		item->setText(FilteringColumn, filteringStr);
	}

	void ImageItemsCollection::imageItemNameModified(ImageItem* imageItem)
	{
		GET_ITEM_SAFE( item )

		if(!imageItem->isSaved())
			item->setText(NameColumn, tr("* %1").arg(imageItem->getName()));
		else
			item->setText(NameColumn, imageItem->getName());
	}

	void ImageItemsCollection::imageItemFilenameModified(ImageItem* imageItem)
	{
		GET_ITEM_SAFE( item )

		QString tooltip;

		if(!imageItem->getFilename().isEmpty())
			tooltip = tr("<i>Filename</i> : %1").arg(imageItem->getFilename());
		else
			tooltip = "<i>Filename</i> : <i>Undefined</i>";

		item->setToolTip(NameColumn, tooltip);
	}

	void ImageItemsCollection::imageItemStatusChanged(ImageItem* imageItem)
	{
		GET_ITEM_SAFE( item )
		
		QString status;
		QBrush 	brush;

		if(imageItem->isOnDevice())
		{
			status	= "VRAM";
			brush	= QBrush(Qt::green);
		}
		else 
		{
			status	= "RAM";
			brush 	= QBrush(QColor(255, 128, 0));
		}

		item->setText(StatusColumn, status);
		item->setTextAlignment(StatusColumn, Qt::AlignHCenter);
		item->setForeground(StatusColumn, brush);
	}

	void ImageItemsCollection::imageItemRemoved(void)
	{
		ImageItem* imageItem = reinterpret_cast<ImageItem*>(QObject::sender());
		
		QTreeWidgetItem* itemName = getTreeItem(imageItem);
		
		if(itemName!=NULL)
		{
			delete itemName;
			items.remove(imageItem);
		}
	}

	#undef GET_ITEM_SAFE

	void ImageItemsCollection::open(void)
	{
		QStringList filenameList = QFileDialog::getOpenFileNames(NULL, "Open Image(s)", currentPath, "Image Files (*.*)");

		if(!filenameList.empty())
		{
			for(QStringList::iterator it=filenameList.begin(); it!=filenameList.end(); it++)
			{
				try
				{
					ImageItem* imageItem = new ImageItem(*it);

					addImageItem(imageItem);
				}
				catch(Exception& e)
				{
					// TODO : Add an exception
					std::cerr << e.what() << std::endl;
				}
			}

			QFileInfo path(filenameList.front());
			currentPath = path.path();
		}
	} 

	void ImageItemsCollection::copy(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		if(selectedImageItems.size()==1)
			selectedImageItems.front()->copyToClipboard();
	}

	void ImageItemsCollection::paste(void)
	{
		ImageItem* imageItem = ImageItem::pasteImageFromClipboard();

		if(imageItem!=NULL)
			addImageItem(imageItem);
	}

	void ImageItemsCollection::save(void)
	{
		std::cerr << "ImageItemsCollection::save not imlemented." << std::endl;
	}

	void ImageItemsCollection::saveAs(void)
	{
		std::cerr << "ImageItemsCollection::saveAs not imlemented." << std::endl;
	} 

	void ImageItemsCollection::removeImageItem(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();
		
		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			(*it)->remove();
	}

	void ImageItemsCollection::removeAllImageItem(void)
	{
		QList<ImageItem*> imageItems = items.keys();

		for(QList<ImageItem*>::iterator it=imageItems.begin(); it!=imageItems.end(); it++)
			(*it)->remove();
	}

	void ImageItemsCollection::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
	{
		std::cerr << "ImageItemsCollection::currentItemChanged - TODO" << std::endl;
	}

	void ImageItemsCollection::itemActivated(QTreeWidgetItem* item, int column)
	{
		if(item==NULL)
			return ;

		emit show( getImageItem(*item) );
	}

	void ImageItemsCollection::itemSelectionChanged(void)
	{
		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		filterMenu.update(formats);
		wrappingMenu.update(formats);
	}
	
	void ImageItemsCollection::openContextMenu(const QPoint& pos)
	{
		QMenu menu(&collectionWidget);
		
		menu.addMenu(&filterMenu);
		menu.addMenu(&wrappingMenu);
		menu.addAction(copyAction);
		menu.addAction(saveAction);
		menu.addAction(saveAsAction);
		menu.addAction(removeAction);		

		menu.exec(collectionWidget.viewport()->mapToGlobal(pos));
	}

	void ImageItemsCollection::changeMinFilter(GLenum minFilter)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			(*it)->setMinFilter(minFilter);

		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		filterMenu.update(formats);
	}

	void ImageItemsCollection::changeMagFilter(GLenum magFilter)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			(*it)->setMagFilter(magFilter);

		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		filterMenu.update(formats);
	}

	void ImageItemsCollection::changeSWrapping(GLenum sWrapping)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			(*it)->setSWrapping(sWrapping);

		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		wrappingMenu.update(formats);
	}

	void ImageItemsCollection::changeTWrapping(GLenum tWrapping)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			(*it)->setTWrapping(tWrapping);

		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		wrappingMenu.update(formats);
	}

	void ImageItemsCollection::addActionToMenu(QAction* action)
	{
		menuBar.addAction(action);
	}

	ImageItem* ImageItemsCollection::getImageItem(const QTreeWidgetItem& treeWidgetItem)
	{
		return reinterpret_cast<ImageItem*>(treeWidgetItem.data(0, Qt::UserRole).value<void*>());
	}

	void ImageItemsCollection::addImageItem(ImageItem* imageItem)
	{
		if(imageItem!=NULL && !storage.hasImageItem(imageItem))
		{
			storage.addImageItem(imageItem);

			// Create item :
			QTreeWidgetItem* item = new QTreeWidgetItem;

			// Set the link :
			item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(imageItem)));
			item->setFlags(item->flags() | Qt::ItemIsDragEnabled);

			// Register :
			collectionWidget.addTopLevelItem(item);
			items[imageItem] = item;

			// Update the info : 
			updateImageItem(imageItem);
			updateAlternateColors();
			updateColumnSize();

			// Connect : 
			QObject::connect(imageItem, SIGNAL(formatModified()),		this, SLOT(imageItemFormatModified()));
			QObject::connect(imageItem, SIGNAL(nameModified()),		this, SLOT(imageItemNameModified()));
			QObject::connect(imageItem, SIGNAL(filenameModified()),		this, SLOT(imageItemFilenameModified()));
			QObject::connect(imageItem, SIGNAL(loadedOnDevice()),		this, SLOT(imageItemStatusChanged()));
			QObject::connect(imageItem, SIGNAL(unloadedFromDevice()),	this, SLOT(imageItemStatusChanged()));
			QObject::connect(imageItem, SIGNAL(savedToDisk()),		this, SLOT(imageItemNameModified()));
			QObject::connect(imageItem, SIGNAL(removed()),			this, SLOT(imageItemRemoved()));
			QObject::connect(imageItem, SIGNAL(destroyed()),		this, SLOT(imageItemRemoved()));

			emit imageItemAdded(imageItem);
		}
	}

// ImageItemsCollectionSubWidget :
#ifdef __USE_QVGL__
	ImageItemsCollectionSubWidget::ImageItemsCollectionSubWidget(void)
	 : 	mainViewsTable("Collection - Main Table"),
		mainViewsTableAction("Views Table", this)
	{
		setInnerWidget(&imageItemsCollection);
		setTitle("Collection");

		imageItemsCollection.addActionToMenu(&mainViewsTableAction);

		QObject::connect(&imageItemsCollection, SIGNAL(show(QGIC::ImageItem*)), this, SLOT(showImageItem(QGIC::ImageItem*)));
		QObject::connect(&mainViewsTableAction,	SIGNAL(triggered()),		this, SLOT(showMainViewsTable()));
	}

	ImageItemsCollectionSubWidget::~ImageItemsCollectionSubWidget(void)
	{
		while(!views.isEmpty())
			viewRemoved(views.begin().key());
	}

	ImageItem* ImageItemsCollectionSubWidget::reversedSearch(QVGL::View* view)
	{
		QList<ImageItem*> key = views.keys(view);

		if(key.isEmpty())
			return NULL;
		else
			return key.front();
	}

	void ImageItemsCollectionSubWidget::showImageItem(QGIC::ImageItem* imageItem)
	{
		QMap<ImageItem*, QVGL::View*>::iterator it = views.find(imageItem);

		if(it==views.end())
		{
			// Load : 
			if(imageItem->loadToDevice())
			{
				// Create a view : 
				QVGL::View* view = new QVGL::View(imageItem->getTexturePtr(), imageItem->getName());
				views[imageItem] = view;

				QObject::connect(imageItem,	SIGNAL(unloadedFromDevice()),	this, SLOT(imageItemViewRemoved()));
				QObject::connect(imageItem,	SIGNAL(removed()),		this, SLOT(imageItemViewRemoved()));
				QObject::connect(imageItem,	SIGNAL(destroyed()),		this, SLOT(imageItemViewRemoved()));
				QObject::connect(view,		SIGNAL(closed()),		this, SLOT(viewRemoved()));
				QObject::connect(view,		SIGNAL(destroyed()),		this, SLOT(viewDestroyed()));
				
				mainViewsTable.addView(view);

				emit addViewRequest(view);
				view->show();
			}
			else
				std::cerr << "ImageItemsCollectionSubWidget::imageItemShow - Cannot load imageItem \"" << imageItem->getName().toStdString() << "\" to device." << std::endl;
		}
		else
			it.value()->show();
	}

	void ImageItemsCollectionSubWidget::showImageItem(void)
	{
		ImageItem* imageItem = reinterpret_cast<ImageItem*>(QObject::sender());

		showImageItem(imageItem);
	}

	void ImageItemsCollectionSubWidget::imageItemViewRemoved(void)
	{
		ImageItem* imageItem = reinterpret_cast<ImageItem*>(QObject::sender());

		viewRemoved(imageItem);
	}

	void ImageItemsCollectionSubWidget::viewRemoved(ImageItem* imageItem)
	{
		QMap<ImageItem*, QVGL::View*>::iterator it = views.find(imageItem);

		if(it!=views.end())
		{
			it.value()->disconnect(this);
			delete it.value();
			views.remove(imageItem);
		}
	}

	void ImageItemsCollectionSubWidget::viewRemoved(QVGL::View* view)
	{
		ImageItem* imageItem = reversedSearch(view);
		
		if(imageItem!=NULL)
			ImageItemsCollectionSubWidget::viewRemoved(imageItem);
	}

	void ImageItemsCollectionSubWidget::viewRemoved(void)
	{
		QVGL::View* view = reinterpret_cast<QVGL::View*>(QObject::sender());

		ImageItemsCollectionSubWidget::viewRemoved(view);	
	}

	void ImageItemsCollectionSubWidget::viewDestroyed(QVGL::View* view)
	{
		ImageItem* imageItem = reversedSearch(view);

		if(imageItem!=NULL)
			views.remove(imageItem);
	}

	void ImageItemsCollectionSubWidget::viewDestroyed(void)
	{
		QVGL::View* view = reinterpret_cast<QVGL::View*>(QObject::sender());

		ImageItemsCollectionSubWidget::viewDestroyed(view);
	}

	void ImageItemsCollectionSubWidget::showMainViewsTable(void)
	{
		mainViewsTable.show();
	}

	QVGL::ViewsTable* ImageItemsCollectionSubWidget::getMainViewsTablePtr(void)
	{
		return &mainViewsTable;
	}

	ImageItemsCollection* ImageItemsCollectionSubWidget::getCollectionPtr(void)
	{
		return &imageItemsCollection;
	}

#endif 

