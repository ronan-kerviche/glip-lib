/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : glip-lib.net                                                                              */
/*                                                                                                               */
/*     File          : ImageItem.cpp                                                                             */
/*     Original Date : August 16th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Qt interface for loading and writing to images.                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

#include <cmath>
#include "ImageItem.hpp"
#include <QFileInfo>
#include <QFileDialog>
#include <QDateTime>
#include <QToolTip>
#include <QMessageBox>
#include <QCloseEvent>
#include "QMenuTools.hpp"

#ifdef __USE_NETPBM__
	#include "NetPBM.hpp"
#endif

#ifdef __USE_LIBRAW__
	#include "LibRawInterface.hpp"
#endif

// Test :
//#define __TIME_LOADING__
#ifdef __TIME_LOADING__ 
	#include <QElapsedTimer>
#endif

using namespace QGIC;

// QImage tools : 
	void toImageBuffer(const QImage& qimage, ImageBuffer*& imageBuffer)
	{
		#ifdef __TIME_LOADING__
			std::cout << "toImageBuffer" << std::endl;
			std::cout << "  width          : " << qimage.width() << std::endl;
			std::cout << "  bytes per line : " << qimage.bytesPerLine() << std::endl;
			std::cout << "  size           : " << qimage.byteCount() << std::endl;
			std::cout << "  allGray        : " << qimage.allGray() << std::endl;
			std::cout << "  isGrayscale    : " << qimage.isGrayscale() << std::endl;
			std::cout << "  hasAlpha       : " << qimage.hasAlphaChannel() << std::endl;

			QElapsedTimer timer;
			timer.start();
		#endif

		// Create the format : 
		GLenum mode = GL_NONE;

		if(qimage.allGray())
			mode = GL_LUMINANCE;
		else if(qimage.hasAlphaChannel())
			mode = GL_RGBA;
		else
			mode = GL_RGB;

		HdlTextureFormat textureFormat(qimage.width(), qimage.height(), mode, GL_UNSIGNED_BYTE);
		
		if(imageBuffer==NULL)
		{
			// Create buffer : 
			imageBuffer = new ImageBuffer(textureFormat, 4); // 4 bytes alignment
		}
		else
		{
			if((*imageBuffer)!=textureFormat)
				throw Exception("toImageBuffer - imageBuffer has an incompatible format.", __FILE__, __LINE__);
		}
	
		// Qt as either GL_BGRA, GL_UNSIGNED_BYTE or GL_LUMINANCE and GL_UNSIGNED_BYTE internal representation.

		// Prepare the copy :
		GLenum 	qtMode = (qimage.bytesPerLine()/qimage.width()==1) ? GL_LUMINANCE : GL_BGRA,
			fakeMode = (qtMode==GL_BGRA && mode==GL_LUMINANCE) ? GL_RED : mode;

		HdlTextureFormat fakeDstFormat(qimage.width(), qimage.height(), fakeMode, GL_UNSIGNED_BYTE);
		ImageBuffer 	original(const_cast<void*>(reinterpret_cast<const void*>(qimage.bits())), HdlTextureFormat(qimage.width(), qimage.height(), qtMode, GL_UNSIGNED_BYTE), 4),
				destination(imageBuffer->getPtr(), fakeDstFormat, 4);
		destination.blit(original, 0, 0, 0, 0, -1, -1, false, true);

		#ifdef __TIME_LOADING__
			std::cout << "The loading operation took " << timer.elapsed() << " milliseconds" << std::endl;
		#endif
	}

	void toQImage(ImageBuffer& imageBuffer, QImage*& qimage)
	{
		#ifdef __TIME_LOADING__
			QElapsedTimer timer;
			timer.start();
		#endif

		// Get the mode :
		const HdlTextureFormatDescriptor& descriptor = HdlTextureFormatDescriptorsList::get(imageBuffer.getGLMode());
		const int depthBytes = imageBuffer.getPixelSize();

		if(qimage==NULL)
		{
			// Create : 
			if(descriptor.numChannels==1 && depthBytes==1)
			{
				qimage = new QImage(imageBuffer.getWidth(), imageBuffer.getHeight(), QImage::Format_Indexed8);
			
				// Create the index :
				QVector<QRgb> colors;

				for(unsigned int k=0; k<256; k++)
					colors.push_back(QRgb(0xFF000000 | k<<16 | k<<8 | k));

				qimage->setColorTable(colors);
			}
			else if(descriptor.hasChannel(GL_RED) && descriptor.hasChannel(GL_GREEN) && descriptor.hasChannel(GL_BLUE) && !descriptor.hasChannel(GL_ALPHA) && depthBytes==3)/*&& ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1) )*/
				qimage = new QImage(imageBuffer.getWidth(), imageBuffer.getHeight(), QImage::Format_RGB888);
			else if(descriptor.hasChannel(GL_RED) && descriptor.hasChannel(GL_GREEN) && descriptor.hasChannel(GL_BLUE) && descriptor.hasChannel(GL_ALPHA) && depthBytes==4)/*&& ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1) )*/
				qimage = new QImage(imageBuffer.getWidth(), imageBuffer.getHeight(), QImage::Format_ARGB32);		
			else
				throw Exception("toQImage - Cannot write texture of mode \"" + getGLEnumName(descriptor.mode) + "\".", __FILE__, __LINE__);
		}
		else
		{
			// Check : 
			if(qimage->width()!=imageBuffer.getWidth() || qimage->height()!=imageBuffer.getHeight())
				throw Exception("toQImage - qimage has an incompatible size.", __FILE__, __LINE__);
			else if((descriptor.numChannels==1 && depthBytes==1) && (qimage->format()!=QImage::Format_Indexed8 || qimage->colorTable().isEmpty()) )
				throw Exception("toQImage - qimage has an incompatible format (should be a grayscale image).", __FILE__, __LINE__);
			else if((descriptor.hasChannel(GL_RED) && descriptor.hasChannel(GL_GREEN) && descriptor.hasChannel(GL_BLUE) && !descriptor.hasChannel(GL_ALPHA) && depthBytes==3/*&& ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8) || depthBytes==1)*/) && qimage->format()!=QImage::Format_RGB888 )
				throw Exception("toQImage - qimage has an incompatible format.", __FILE__, __LINE__);
			else if((descriptor.hasChannel(GL_RED) && descriptor.hasChannel(GL_GREEN) && descriptor.hasChannel(GL_BLUE) && descriptor.hasChannel(GL_ALPHA) && depthBytes==4 /*&& ((descriptor.redDepthInBits==8 && descriptor.greenDepthInBits==8 && descriptor.blueDepthInBits==8 && descriptor.alphaDepthInBits==8) || depthBytes==1)*/) && qimage->format()!=QImage::Format_ARGB32 )
				throw Exception("toQImage - qimage has an incompatible format.", __FILE__, __LINE__);
			else
				throw Exception("toQImage - Cannot write texture of mode \"" + getGLEnumName(descriptor.mode) + "\".", __FILE__, __LINE__);
		}

		const int nChannels = qimage->bytesPerLine()/qimage->width();
		GLenum mode;
		if(nChannels==1)
			mode = imageBuffer.getGLMode();
		else if(nChannels==2)
			mode = GL_RG;
		else if(nChannels==3)
			mode = GL_RGB;
		else
			mode = GL_BGRA;

		ImageBuffer destination(reinterpret_cast<void*>(qimage->bits()), HdlTextureFormat(qimage->width(), qimage->height(), mode, GL_UNSIGNED_BYTE), 4);
		destination.blit(imageBuffer, 0, 0, 0, 0, -1, -1, false, true);

		#ifdef __TIME_LOADING__
			std::cout << "The writing operation took " << timer.elapsed() << " milliseconds" << std::endl;
		#endif
	}

// ImageItem : 
	ImageItem::ImageItem(const ImageItem& imageItem)
	 : 	QObject(),
		saved(false),
		name(tr("copy_of_%1").arg(imageItem.getName())),
		filename(""),
		format(imageItem.format),
		imageBuffer(NULL),
		texture(NULL)
	{
		if(imageItem.imageBuffer==NULL)
			throw Exception("[Internal Error] Inner buffer is null.", __FILE__, __LINE__);

		imageBuffer	= new ImageBuffer(*imageItem.imageBuffer);
		lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
	}

	ImageItem::ImageItem(const QString& _filename)
	 : 	saved(true),
		name(""),
		filename(_filename),
		format(1, 1, GL_RGB, GL_UNSIGNED_BYTE),
		imageBuffer(NULL),
		texture(NULL)
	{
		load();
		format = (*imageBuffer);
		lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
	}

	ImageItem::ImageItem(const QImage& qimage, const QString& _name)
	 : 	saved(false),
		name(_name),
		filename(""),
		format(1, 1, GL_RGB, GL_UNSIGNED_BYTE),
		imageBuffer(NULL),
		texture(NULL)
	{
		toImageBuffer(qimage, imageBuffer);
		format = (*imageBuffer);
		lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
	}

	ImageItem::ImageItem(const ImageBuffer& buffer, const QString& _name)
	 : 	saved(false),
		name(_name),
		filename(""),
		format(buffer),
		imageBuffer(NULL),
		texture(NULL)
	{
		imageBuffer = new ImageBuffer(buffer);
		lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
	}

	ImageItem::ImageItem(HdlTexture& _texture, const QString& _name)
	 : 	saved(false),
		name(_name),
		filename(""),
		format(_texture),
		imageBuffer(NULL),
		texture(NULL)
	{
		imageBuffer = new ImageBuffer(_texture);
		lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
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
		if(QString::compare(path.completeSuffix(), "grw", Qt::CaseInsensitive)==0)
		{
			std::string comment;

			imageBuffer 	= ImageBuffer::load(filename.toStdString(), &comment);

			if(!comment.empty())
			{
				std::cout << "Comment in GLIP-RAW file \"" << _filename.toStdString() << "\" : " << std::endl;
				std::cout << comment << std::endl;
			}
		}
		#ifdef __USE_NETPBM__
		else if(QString::compare(path.completeSuffix(), "ppm", Qt::CaseInsensitive)==0 || QString::compare(path.completeSuffix(), "pgm", Qt::CaseInsensitive)==0)
			imageBuffer 	= NetPBM::loadNetPBMFile(filename.toStdString());
		#endif
		#ifdef __USE_LIBRAW__
		else if(QString::compare(path.completeSuffix(), "cr2", Qt::CaseInsensitive)==0 || QString::compare(path.completeSuffix(), "nef", Qt::CaseInsensitive)==0)
			imageBuffer	= libRawLoadImage(filename.toStdString());
		#endif
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

	void ImageItem::lock(const void* key)
	{
		if(!locks.contains(key))
		{
			locks.append(key);

			if(locks.size()==1)
				emit locking(true);
		}
	}

	void ImageItem::unlock(const void* key)
	{
		if(locks.contains(key))
		{
			locks.remove(locks.indexOf(key));

			if(locks.empty())
				emit locking(false);
		}
	}

	bool ImageItem::isLockedToDevice(void) const
	{
		return (!locks.empty());
	}

	const HdlAbstractTextureFormat& ImageItem::getFormat(void) const
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

	QString ImageItem::getToolTipInformation(void)
	{
		QString tooltip;

		QMap<QString, QString> infos;

		if(!getFilename().isEmpty())
			infos["Filename"] = getFilename();

		infos["Locks Count"] = tr("%1").arg(locks.size());

		if(isOnDevice())
			tooltip = getFormatToolTip(*texture, getName(), infos);
		else
			tooltip = getFormatToolTip(format, getName(), infos);
		
		return tooltip;
	}

	const QString& ImageItem::getFilename(void) const
	{
		return filename;
	}

	void ImageItem::setFilename(const QString& newFilename, bool useAsNewName)
	{
		filename = newFilename;
		emit filenameModified();

		if(useAsNewName)
		{
			QFileInfo info(filename);
			setName(info.fileName());
		}
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
		{
			lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
			return (*texture);
		}
	}

	HdlTexture* ImageItem::getTexturePtr(void)
	{
		if(texture==NULL)
			throw Exception("ImageItem::getTexture - Texture is not currently on device.", __FILE__, __LINE__);
		else
		{
			lastAccessTimeStamp = QDateTime::currentMSecsSinceEpoch();
			return texture;
		}
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
		
		#ifdef __USE_NETPBM__
		if(QString::compare(path.completeSuffix(), "ppm", Qt::CaseInsensitive)==0 || QString::compare(path.completeSuffix(), "pgm", Qt::CaseInsensitive)==0)
			NetPBM::saveNetPBMToFile(*imageBuffer, _filename.toStdString());
		else 
		#endif
		     if(QString::compare(path.completeSuffix(), "grw", Qt::CaseInsensitive)==0)
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
	
	qint64 ImageItem::getLastAccessTimeStamp(void) const
	{
		return lastAccessTimeStamp;
	}

	void ImageItem::remove(void)
	{
		locks.clear();
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
		//else 
			return tr("%1 B").arg( size );
	}

	QString ImageItem::getFormatToolTip(const HdlAbstractTextureFormat& format, const QString& name, const QMap<QString, QString>& otherInfos)
	{
		QString toolTip;

		toolTip += "<table>";

		if(!name.isEmpty())
			toolTip += tr("<tr><td><i>Name</i></td><td>:</td><td>%1</td></tr>").arg(name);

		toolTip += tr("<tr><td><i>Size</i></td><td>:</td><td>%1x%2 (%3)</td></tr>").arg(format.getWidth()).arg(format.getHeight()).arg(getSizeString(format.getSize()));
		toolTip += tr("<tr><td><i>Mode</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(format.getGLMode() ).c_str());
		toolTip += tr("<tr><td><i>Depth</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(format.getGLDepth() ).c_str());
		toolTip += tr("<tr><td><i>Filtering</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(format.getMinFilter() ).c_str()).arg(getGLEnumName(format.getMagFilter() ).c_str());
		toolTip += tr("<tr><td><i>Wrapping</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(format.getSWrapping() ).c_str()).arg(getGLEnumName(format.getTWrapping() ).c_str());
		toolTip += tr("<tr><td><i>Mipmap</i></td><td>:</td><td>%1 / %2</td></tr>").arg(format.getBaseLevel()).arg(format.getMaxLevel());

		for(QMap<QString, QString>::const_iterator it=otherInfos.begin(); it!=otherInfos.end(); it++)
			toolTip += tr("<tr><td><i>%1</i></td><td>:</td><td>%2</td></tr>").arg(it.key()).arg(it.value());

		toolTip += "</table>";

		return toolTip;
	}

	QString ImageItem::getFormatToolTip(HdlTexture& texture, const QString& name, const QMap<QString, QString>& otherInfos)
	{
		QString toolTip;

		toolTip += "<table>";

		if(!name.isEmpty())
			toolTip += tr("<tr><td><i>Name</i></td><td>:</td><td>%1</td></tr>").arg(name);

		toolTip += tr("<tr><td><i>Size</i></td><td>:</td><td>%1x%2 (%3)</td></tr>").arg(texture.getWidth()).arg(texture.getHeight()).arg(getSizeString(texture.getSize()));
		toolTip += tr("<tr><td><i>Size (Driver)</i></td><td>:</td><td>%1</td></tr>").arg(getSizeString(texture.getSizeOnGPU(0)));
		toolTip += tr("<tr><td><i>Mode</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(texture.getGLMode() ).c_str());
		toolTip += tr("<tr><td><i>Depth</i></td><td>:</td><td>%1</td></tr>").arg(getGLEnumName(texture.getGLDepth() ).c_str());
		toolTip += tr("<tr><td><i>Filtering</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(texture.getMinFilter() ).c_str()).arg(getGLEnumName(texture.getMagFilter() ).c_str());
		toolTip += tr("<tr><td><i>Wrapping</i></td><td>:</td><td>%1 / %2</td></tr>").arg(getGLEnumName(texture.getSWrapping() ).c_str()).arg(getGLEnumName(texture.getTWrapping() ).c_str());
		toolTip += tr("<tr><td><i>Mipmap</i></td><td>:</td><td>%1 / %2</td></tr>").arg(texture.getBaseLevel()).arg(texture.getMaxLevel());

		for(QMap<QString, QString>::const_iterator it=otherInfos.begin(); it!=otherInfos.end(); it++)
			toolTip += tr("<tr><td><i>%1</i></td><td>:</td><td>%2</td></tr>").arg(it.key()).arg(it.value());

		toolTip += "</table>";

		return toolTip;
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

// ImageItemStorage::Record :
	ImageItemsStorage::Record::Record(void)
	 :	storage(NULL),
		item(NULL)
	{ }

	ImageItemsStorage::Record::Record(const ImageItemsStorage::Record& c)
	 :	storage(c.storage),
		item(c.item)
	{ }

	ImageItemsStorage::Record::Record(ImageItemsStorage* _storage, ImageItem* _item)
	 : 	storage(_storage),
		item(_item)
	{ }

	ImageItemsStorage::Record& ImageItemsStorage::Record::operator=(const ImageItemsStorage::Record& c)
	{
		storage = c.storage;
		item = c.item;

		return (*this);
	}

	bool ImageItemsStorage::Record::operator<(const ImageItemsStorage::Record& b) const
	{
		return (item->getLastAccessTimeStamp() < b.item->getLastAccessTimeStamp());
	}

// ImageItemsStorage :
	size_t ImageItemsStorage::maxOccupancy = 850 * 1024 * 1024; // 850 MB
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
		{
			(*it)->disconnect(this);
			disconnect(*it);
			(*it)->deleteLater();
		}

		imageItemsList.clear();

		// Remove itself : 
		int idx = storagesList.indexOf(this);

		if(idx>=0)
			storagesList.remove(idx);
	}

	QVector<ImageItemsStorage::Record> ImageItemsStorage::getFreeableRecords(void)
	{
		QVector<ImageItemsStorage::Record> result;

		for(QVector<ImageItem*>::iterator it=imageItemsList.begin(); it!=imageItemsList.end(); it++)
		{
			if((*it)->isOnDevice() && !(*it)->isLockedToDevice())
				result.push_back(Record(this, (*it)));
		}

		return result;
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
			imageItemsList[idx]->deleteLater();
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
	size_t ImageItemsStorage::getDeviceOccupancy(size_t* canBeFreed) const
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

	void ImageItemsStorage::clean(void)
	{
		for(QVector<ImageItem*>::iterator it=imageItemsList.begin(); it!=imageItemsList.end(); it++)
		{
			if((*it)->isOnDevice() && !(*it)->isLockedToDevice())
				(*it)->unloadFromDevice();
		}
	}

	size_t ImageItemsStorage::getMaxOccupancy(void)
	{
		return maxOccupancy;
	}

	void ImageItemsStorage::setMaxOccupancy(size_t newMaxOccupancy)
	{
		size_t 	totalOccupancy = ImageItemsStorage::getTotalDeviceOccupancy();

		if(totalOccupancy > newMaxOccupancy)
		{
			cleanStorages();

			// Prevent further cut :
			newMaxOccupancy = std::max(newMaxOccupancy, ImageItemsStorage::getTotalDeviceOccupancy());
		}

		// Finally, set : 
		maxOccupancy = newMaxOccupancy;
	}

	size_t ImageItemsStorage::getTotalDeviceOccupancy(size_t* canBeFreed)
	{
		size_t totalOccupancy = 0;

		if(canBeFreed!=NULL)
			(*canBeFreed) = 0;

		for(QVector<ImageItemsStorage*>::iterator it=storagesList.begin(); it!=storagesList.end(); it++)
		{
			size_t currentCanBeFreed = 0;

			totalOccupancy += (*it)->getDeviceOccupancy(&currentCanBeFreed);
			
			if(canBeFreed!=NULL)
				(*canBeFreed) += currentCanBeFreed;
		}

		return totalOccupancy;
	}

	void ImageItemsStorage::cleanStorages(void)
	{
		for(QVector<ImageItemsStorage*>::iterator it=storagesList.begin(); it!=storagesList.end(); it++)
			(*it)->clean();
	}

	void ImageItemsStorage::cleanStorages(size_t futureAdd)
	{
		QVector<Record> freeableRecords;

		// Get the list of records which can be freed :
		for(QVector<ImageItemsStorage*>::iterator it=storagesList.begin(); it!=storagesList.end(); it++)
			freeableRecords << (*it)->getFreeableRecords();
	
		// Sort it by timestamp, from newest to oldest access :
		qSort(freeableRecords.begin(), freeableRecords.end());

		// Unload the oldest, until you reach the futureAdd size :
		size_t counter = 0;
		for(QVector<Record>::iterator it=freeableRecords.end(); !freeableRecords.empty() && it!=freeableRecords.begin() && counter<futureAdd;)
		{
			it--;
			it->item->unloadFromDevice();
			counter += it->item->getFormat().getSize();
		}
	}

	bool ImageItemsStorage::checkMemSpaceAvailabilty(size_t futureAdd)
	{
		size_t 	totalOccupancy	= 0,
			canBeFreed	= 0;
		
		totalOccupancy = ImageItemsStorage::getTotalDeviceOccupancy(&canBeFreed);

		// If it cannot be added directly : 
		if((totalOccupancy+futureAdd) > maxOccupancy)
		{
			// If some occupancy can be freed to enable the storage : 
			if((totalOccupancy+futureAdd-canBeFreed) < maxOccupancy)
			{
				cleanStorages(futureAdd);
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

	void FilterMenu::update(const HdlAbstractTextureFormat& format)
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
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + getGLEnumName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
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
				throw Exception("FilterMenu::update - Unkwnown parameter \"" + getGLEnumName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
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

	void WrappingMenu::update(const HdlAbstractTextureFormat& format)
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
					throw Exception("WrappingMenu::update - Unkwnown parameter \"" + getGLEnumName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
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
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + getGLEnumName(format.getSWrapping()) + "\".", __FILE__, __LINE__);
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
				throw Exception("WrappingMenu::update - Unkwnown parameter \"" + getGLEnumName(format.getTWrapping()) + "\".", __FILE__, __LINE__);
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
	CollectionWidget::ComparisonFunctor::ComparisonFunctor(CollectionWidget* _treeWidget, bool _reversed)
	 :	treeWidget(_treeWidget),
		reversed(_reversed)
	{ }

	bool CollectionWidget::ComparisonFunctor::operator()(QTreeWidgetItem* a, QTreeWidgetItem* b) //const, indexOfTopLevelItem is not/does not take const in Qt 4.8?
	{
		const bool test = (treeWidget->indexOfTopLevelItem(a)<treeWidget->indexOfTopLevelItem(b));
		return (reversed && !test) || (!reversed && test); // logical XOR
	}

	CollectionWidget::CollectionWidget(QWidget* parent)
	 : 	QTreeWidget(parent),
		moveUpAction("Up", this),
		moveDownAction("Down", this)
	{
		setIndentation(2);
		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::ExtendedSelection);
		setContextMenuPolicy(Qt::CustomContextMenu);

		QObject::connect(this,			SIGNAL(itemSelectionChanged(void)),	this, SLOT(selectionChanged(void)));
		QObject::connect(&moveUpAction,		SIGNAL(triggered(void)), 		this, SLOT(moveSelectionUp(void)));
		QObject::connect(&moveDownAction,	SIGNAL(triggered(void)), 		this, SLOT(moveSelectionDown(void)));

		selectionChanged();
	}

	CollectionWidget::~CollectionWidget(void)
	{ }

	void CollectionWidget::selectionChanged(void)
	{
		const bool state = !selectedItems().isEmpty();

		moveUpAction.setEnabled(state);
		moveDownAction.setEnabled(state);
	}

	void CollectionWidget::moveSelectionUp(void)
	{
		QList<QTreeWidgetItem*> currentSelection = selectedItems();
		
		bool blocked = false;

		// Sort from top to bottom : 
		std::sort(currentSelection.begin(), currentSelection.end(), ComparisonFunctor(this, false));

		for(QList<QTreeWidgetItem*>::iterator it=currentSelection.begin(); it!=currentSelection.end() && !blocked; it++)
		{
			int originalIndex = indexOfTopLevelItem(*it);

			if(originalIndex>0)
			{
				QTreeWidgetItem* item = takeTopLevelItem(originalIndex);
				insertTopLevelItem(originalIndex-1, item);
				item->setSelected(true);
			}
			else	
				blocked = true;
		}
	}

	void CollectionWidget::moveSelectionDown(void)
	{
		QList<QTreeWidgetItem*> currentSelection = selectedItems();
		
		bool blocked = false;

		// Sort from bottom to top : 
		std::sort(currentSelection.begin(), currentSelection.end(), ComparisonFunctor(this, true));

		for(QList<QTreeWidgetItem*>::iterator it=currentSelection.begin(); it!=currentSelection.end() && !blocked; it++)
		{
			int originalIndex = indexOfTopLevelItem(*it);

			if(originalIndex>=0 && originalIndex<topLevelItemCount()-1)
			{
				QTreeWidgetItem* item = takeTopLevelItem(originalIndex);
				insertTopLevelItem(originalIndex+1, item);
				item->setSelected(true);
			}
			else	
				blocked = true;
		}
	}

// ImageItemsCollectionSettings :
	ImageItemsCollectionSettings::ImageItemsCollectionSettings(QWidget* parent)
	 :	QMenu("Settings", parent),
		widgetAction(this),
		widget(this),
		layout(&widget),
		newMaxOccupancySpinBox(&widget),
		currentMaxOccupancyTitleLabel(" Current : ", &widget),
		currentMaxOccupancyLabel(&widget),
		newMaxOccupancyLabel(" Set maximum : ", &widget),
		newMaxOccupancyUnitLabel(" MB", &widget),
		saveAction("Save", this)
	{
		layout.addWidget(&currentMaxOccupancyTitleLabel,0, 0, 1, 1);
		layout.addWidget(&currentMaxOccupancyLabel, 	0, 1, 1, 2);
		layout.addWidget(&newMaxOccupancyLabel,		1, 0, 1, 1);
		layout.addWidget(&newMaxOccupancySpinBox,	1, 1, 1, 1);
		layout.addWidget(&newMaxOccupancyUnitLabel,	1, 2, 1, 1);

		currentMaxOccupancyTitleLabel.setAlignment(Qt::AlignRight);
		newMaxOccupancyLabel.setAlignment(Qt::AlignRight);

		newMaxOccupancySpinBox.setSingleStep(8);

		widgetAction.setDefaultWidget(&widget);
		addAction(&widgetAction);
		addAction(&saveAction);

		QObject::connect(this,		SIGNAL(aboutToShow()),	this,	SLOT(update()));
		QObject::connect(&saveAction, 	SIGNAL(triggered()), 	this, 	SLOT(changeMaxOccupancy()));
	}

	ImageItemsCollectionSettings::~ImageItemsCollectionSettings(void)
	{
		widgetAction.releaseWidget(&widget);
	}
	
	void ImageItemsCollectionSettings::update(void)
	{
		const size_t	toMB			= 1024*1024;
		size_t		freeable		= 0;
		const size_t 	currentOccupancy	= ImageItemsStorage::getTotalDeviceOccupancy(&freeable) / toMB,
				currentMaxOccupancy	= ImageItemsStorage::getMaxOccupancy() / toMB;
		const int	percents		= (currentOccupancy*100)/currentMaxOccupancy;
		freeable = freeable/toMB + 1;

		newMaxOccupancySpinBox.setRange(std::max(16, static_cast<int>(freeable)), 16384);

		currentMaxOccupancyLabel.setText(tr("%1 / %2 MB (%3%)").arg(currentOccupancy).arg(currentMaxOccupancy).arg(percents));
		newMaxOccupancySpinBox.setValue(currentMaxOccupancy);
	}

	void ImageItemsCollectionSettings::changeMaxOccupancy(void)
	{
		const size_t toMB = 1024*1024;
		ImageItemsStorage::setMaxOccupancy(newMaxOccupancySpinBox.value()*toMB);
	}		

// ImageItemsCollection : 
	ImageItemsCollection::ImageItemsCollection(void)
	 : 	layout(this),
		menuBar(this),
		openAction(NULL),
		pasteAction(NULL),
		saveAction(NULL),
		saveAsAction(NULL),
		removeAction(NULL),
		imagesMenu("Images", this),
		filterMenu(this),
		wrappingMenu(this),
		collectionWidget(this),
		settingsMenu(this)
	{
		layout.addWidget(&menuBar);
		layout.addWidget(&collectionWidget);
		layout.setMargin(0);
		layout.setSpacing(0);
	
		openAction	= imagesMenu.addAction("Open",		this, SLOT(open()),			QKeySequence::Open);
		pasteAction	= imagesMenu.addAction("Paste",		this, SLOT(paste()),			QKeySequence::Paste);
		copyAction	= imagesMenu.addAction("Copy",		this, SLOT(copy()),			QKeySequence::Copy);
		saveAction 	= imagesMenu.addAction("Save",		this, SLOT(save()),			QKeySequence::Save);
		saveAsAction	= imagesMenu.addAction("Save as",	this, SLOT(saveAs()),			QKeySequence::SaveAs);
		removeAction	= imagesMenu.addAction("Remove",	this, SLOT(removeImageItem()),		QKeySequence::Delete);
		removeAllAction	= imagesMenu.addAction("Remove all",	this, SLOT(removeAllImageItem()),	Qt::SHIFT + Qt::Key_Delete);

		openAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		saveAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		saveAsAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		removeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		removeAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		
		addAction(openAction);
		addAction(pasteAction);
		addAction(copyAction);
		addAction(saveAction);
		addAction(saveAsAction);
		addAction(removeAction);
		addAction(removeAllAction);

		menuBar.addMenu(&imagesMenu);
		menuBar.addMenu(&filterMenu);
		menuBar.addMenu(&wrappingMenu);
		menuBar.addAction(&collectionWidget.moveUpAction);
		menuBar.addAction(&collectionWidget.moveDownAction);
		menuBar.addMenu(&settingsMenu);

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
				case FilteringColumn:
					listLabels << "Filtering";
					break;
				default : 
					throw Exception("ImageItemsCollection::ImageItemsCollection - Missing column declaration [Internal Error].", __FILE__, __LINE__);
			}
		}
		
		collectionWidget.setHeaderLabels( listLabels );

		// Connections :
		QObject::connect(&collectionWidget, 	SIGNAL(itemActivated(QTreeWidgetItem*,int)),			this, SLOT(itemActivated(QTreeWidgetItem*,int)));
		QObject::connect(&collectionWidget, 	SIGNAL(itemSelectionChanged()),					this, SLOT(itemSelectionChanged()));
		QObject::connect(&collectionWidget,	SIGNAL(customContextMenuRequested(const QPoint&)),		this, SLOT(openContextMenu(const QPoint&)));
		QObject::connect(&filterMenu, 		SIGNAL(changeMinFilter(GLenum)),				this, SLOT(changeMinFilter(GLenum)));
		QObject::connect(&filterMenu, 		SIGNAL(changeMagFilter(GLenum)),				this, SLOT(changeMagFilter(GLenum)));
		QObject::connect(&wrappingMenu,		SIGNAL(changeSWrapping(GLenum)),				this, SLOT(changeSWrapping(GLenum)));
		QObject::connect(&wrappingMenu,		SIGNAL(changeTWrapping(GLenum)),				this, SLOT(changeTWrapping(GLenum)));
	}

	ImageItemsCollection::~ImageItemsCollection(void)
	{
		// Clear image before the storage does (avoid segfault because of remaining connections) :
		while(!items.isEmpty())
			delete items.begin().key();
	}

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

	void ImageItemsCollection::closeEvent(QCloseEvent* event)
	{
		QList<ImageItem*> keys = items.keys();
		for(QList<ImageItem*>::iterator it=keys.begin(); it!=keys.end(); it++)
		{
			if(!removeImageItem(*it))
			{
				event->ignore();
				return;
			}
		}
		QWidget::closeEvent(event);
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
		const HdlAbstractTextureFormat& format = imageItem->getFormat();

		QString formatString;

		const HdlTextureFormatDescriptor& descriptor = format.getFormatDescriptor();

		// Mode 
		if(descriptor.hasChannel(GL_RED))
			formatString.append("R");
		if(descriptor.hasChannel(GL_GREEN))
			formatString.append("G");
		if(descriptor.hasChannel(GL_BLUE))
			formatString.append("B");
		if(descriptor.hasChannel(GL_ALPHA))
			formatString.append("A");
		if(descriptor.hasChannel(GL_LUMINANCE))
			formatString.append("L");
	
		// Depth : 
		formatString.append( tr(" %1 bits").arg(format.getChannelDepth()*8) );

		QString sizeStr = tr("%1 x %2 (%3; %4)").arg(format.getWidth()).arg(format.getHeight()).arg(ImageItem::getSizeString(format.getSize())).arg(formatString);		

		item->setText(SizeColumn, sizeStr);

		// Filtering : 
		QString filteringStr;

		filteringStr = tr("%1/%2; %3/%4").arg(getGLEnumName(format.getSWrapping()).c_str()).arg(getGLEnumName(format.getTWrapping()).c_str()).arg(getGLEnumName(format.getMinFilter()).c_str()).arg(getGLEnumName(format.getMagFilter()).c_str());

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
		QStringList filenameList = QFileDialog::getOpenFileNames(QApplication::activeWindow(), "Open Image(s)", currentPath, "Image Files (*.*)");

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
					// Warning :
					QMessageBox messageBox(QMessageBox::Warning, "Error", tr("Failed to open image file at \"%1\".").arg(*it), QMessageBox::Ok, QApplication::activeWindow());
					messageBox.setDetailedText(e.what());
					messageBox.exec();
				}
			}

			// Remember the working directory : 
			QFileInfo path(filenameList.front());
			currentPath = path.path();
		}
	} 

	void ImageItemsCollection::copy(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();
	
		// Take only the first one :
		if(!selectedImageItems.isEmpty())
			selectedImageItems.front()->copyToClipboard();
	}

	void ImageItemsCollection::paste(void)
	{
		ImageItem* imageItem = ImageItem::pasteImageFromClipboard();

		if(imageItem!=NULL)
			addImageItem(imageItem);
	}

	void ImageItemsCollection::save(ImageItem* imageItem)
	{
		if(imageItem->getFilename().isEmpty())
		{
			QString filename = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Save %1").arg(imageItem->getName()), currentPath, "Image File (*.*)");

			if(!filename.isEmpty())
			{
				imageItem->setFilename(filename, true);
				imageItem->save();
			}
		}	
		else
			imageItem->save();	
	}

	void ImageItemsCollection::save(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			save(*it);
	}

	void ImageItemsCollection::saveAs(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();

		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
		{
			QString filename = QFileDialog::getSaveFileName(QApplication::activeWindow(), tr("Save %1").arg((*it)->getName()), currentPath, "Image File (*.*)");

			if(!filename.isEmpty())
			{
				(*it)->setFilename(filename, true);
				(*it)->save();
			}
		}
	}

	bool ImageItemsCollection::removeImageItem(ImageItem* imageItem, bool imperative)
	{
		if(!imageItem->isSaved())
		{
			QMessageBox::StandardButton 	returnedButton;
			QMessageBox::StandardButtons	buttons;

			if(imperative)
				buttons = QMessageBox::Save | QMessageBox::Discard;
			else
				buttons = QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel;

			if(imageItem->getFilename().isEmpty())
				returnedButton = QMessageBox::warning(QApplication::activeWindow(), tr("Warning!"), tr("New image has been modified.\n Do you want to save it?"), buttons);
			else
				returnedButton = QMessageBox::warning(QApplication::activeWindow(), tr("Warning!"), tr("The image %1 has been modified.\n Do you want to save your changes?").arg(imageItem->getFilename()), buttons);

			if(returnedButton==QMessageBox::Save)
				save(imageItem);
			else if(returnedButton == QMessageBox::Cancel)
				return false;
		}

		imageItem->remove();

		return true;
	}

	void ImageItemsCollection::removeImageItem(void)
	{
		QList<ImageItem*> selectedImageItems = getSelectedImageItems();
		
		for(QList<ImageItem*>::iterator it=selectedImageItems.begin(); it!=selectedImageItems.end(); it++)
			removeImageItem(*it);
	}

	void ImageItemsCollection::removeAllImageItem(void)
	{
		QList<ImageItem*> imageItems = items.keys();

		for(QList<ImageItem*>::iterator it=imageItems.begin(); it!=imageItems.end(); it++)
			removeImageItem(*it);
	}

	void ImageItemsCollection::itemActivated(QTreeWidgetItem* item, int column)
	{
		UNUSED_PARAMETER(column)

		if(item==NULL)
			return ;

		emit show( getImageItem(*item) );
	}

	void ImageItemsCollection::itemSelectionChanged(void)
	{
		QList<HdlTextureFormat> formats = getSelectedImageItemFormat();

		filterMenu.update(formats);
		wrappingMenu.update(formats);

		// Show selection information :
		if(formats.size()>=2)
		{
			size_t totalOccupancy = 0;

			for(QList<HdlTextureFormat>::const_iterator it=formats.begin(); it!=formats.end(); it++)
				totalOccupancy += (*it).getSize();

			QString tip = tr("%1 images selected.\n%2 total memory occupancy.").arg(formats.size()).arg(ImageItem::getSizeString(totalOccupancy));

			#ifdef __USE_QVGL__	
			if(parentWidget()!=NULL && parentWidget()->graphicsProxyWidget()!=NULL)
			{	
				// In the middle of the widget :
				QPoint p = QVGL::SubWidget::mapItemCoordinatesToGlobal(QPoint(width()/2, height()/2), parentWidget()->graphicsProxyWidget());
				QToolTip::showText(p, tip);
			}
			else
			#endif
			{
				// On the cursor :
				QToolTip::showText(QCursor::pos(), tip);
			}
		}
		else
			QToolTip::showText(QPoint(0,0),""); // Remove any tooltip.
	}
	
	void ImageItemsCollection::openContextMenu(const QPoint& pos)
	{
		QMenu menu(&collectionWidget);
		
		duplicateMenu(&menu, filterMenu);
		duplicateMenu(&menu, wrappingMenu);
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
		menuBar.insertAction(settingsMenu.menuAction(), action);
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

	void ImageItemsCollection::addImageItem(const QString filename)
	{
		ImageItem* imageItem = new ImageItem(filename);
		addImageItem(imageItem);
	}

	void ImageItemsCollection::addImageItem(const QImage* qimage, const QString name)
	{
		ImageItem* imageItem = new ImageItem(*qimage, name);
		addImageItem(imageItem);
	}

	void ImageItemsCollection::addImageItem(const ImageBuffer* buffer, const QString name)
	{
		ImageItem* imageItem = new ImageItem(*buffer, name);
		addImageItem(imageItem);
	}

	void ImageItemsCollection::addImageItem(HdlTexture* texture, const QString name)
	{
		ImageItem* imageItem = new ImageItem(*texture, name);
		addImageItem(imageItem);
	}

// ImageItemsCollectionSubWidget :
#ifdef __USE_QVGL__
	ImageItemsCollectionSubWidget::ImageItemsCollectionSubWidget(void)
	 : 	mainViewsTable("Collection - Main Table"),
		mainViewsTableAction("Views Table", this)
	{
		setInnerWidget(&imageItemsCollection);
		setTitle("Image Collection");

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

				view->infos["Source"] = "Collection";
				if(!imageItem->getFilename().isEmpty())
					view->infos["Filename"] = imageItem->getFilename();

				QObject::connect(imageItem,	SIGNAL(formatModified()),	view, SIGNAL(internalDataUpdated()));
				QObject::connect(imageItem,	SIGNAL(nameModified()),		view, SIGNAL(internalDataUpdated()));
				QObject::connect(imageItem,	SIGNAL(filenameModified()),	view, SIGNAL(internalDataUpdated()));
				QObject::connect(imageItem,	SIGNAL(savedToDisk()),		view, SIGNAL(internalDataUpdated()));
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
			{
				QMessageBox messageBox(QMessageBox::Warning, "Error", tr("Cannot load the image \"%1\" to device.").arg(imageItem->getName()), QMessageBox::Ok, QApplication::activeWindow());
				messageBox.exec();
			}
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
			// Prevent any of these two objects to popup cleaning request
			it.key()->disconnect(this);
			it.value()->setTexture(NULL); // not linked anymore.
			it.value()->disconnect(this);
			it.value()->deleteLater();
			views.erase(it);
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
		if(mainViewsTable.isClosed())
			emit addViewsTableRequest(&mainViewsTable);

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

