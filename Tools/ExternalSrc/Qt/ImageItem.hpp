/* ************************************************************************************************************* */
/*                                                                                                               */
/*     GLIP-LIB                                                                                                  */
/*     OpenGL Image Processing LIBrary                                                                           */
/*                                                                                                               */
/*     Author        : R. KERVICHE 			                                                         */
/*     LICENSE       : MIT License                                                                               */
/*     Website       : http://sourceforge.net/projects/glip-lib/                                                 */
/*                                                                                                               */
/*     File          : ImageItem.hpp                                                                             */
/*     Original Date : August 16th 2014                                                                          */
/*                                                                                                               */
/*     Description   : Qt interface for loading and writing to images.                                           */
/*                                                                                                               */
/* ************************************************************************************************************* */

#ifndef __GLIPLIB_IMAGE_ITEM__
#define __GLIPLIB_IMAGE_ITEM__

// Settings : 
	// Use PBO or not :
	//#define __USE_PBO__

	// Use QVGL::SubWidget definition :  
	//#define __USE_QVGL__

	// Use NETPBM external code (to load PGM / PPM images) :
	//#define __USE_NETPBM__
	
	// Use LibRaw and LibRawInterface (to load CR2 / NEF images, raw) :
	//#define __USE_LIBRAW__

// Includes : 
	#include "GLIPLib.hpp"
	#include <QImage>
	#include <QTreeWidget>
	#include <QVBoxLayout>
	#include <QMenuBar>
	#include <QApplication>
	#include <QClipboard> 
	#include <QWidgetAction>

	#ifdef __USE_QVGL__
		#include "GLSceneWidget.hpp"
	#endif

// Namespaces
	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

// Tools : 
	extern void toImageBuffer(const QImage& qimage, ImageBuffer*& imageBuffer);
	extern void toQImage(ImageBuffer& imageBuffer, QImage*& qimage);

namespace QGIC
{
	// Classes : 
	class ImageItem : public QObject
	{
		Q_OBJECT

		private : 
			bool			saved;
			QVector<const void*>	locks;
			QString			name,
						filename;
			HdlTextureFormat	format;			
			ImageBuffer*		imageBuffer;
			HdlTexture* 		texture;
			qint64			lastAccessTimeStamp;

			void load(QString _filename="");

		public : 
			ImageItem(const ImageItem& imageItem);
			ImageItem(const QString& _filename);
			ImageItem(const QImage& qimage, const QString& _name);
			ImageItem(const ImageBuffer& buffer, const QString& _name);
			ImageItem(HdlTexture& _texture, const QString& _name);
			~ImageItem(void);
			
			bool isOnDevice(void) const;
			bool loadToDevice(void);
			void unloadFromDevice(void);
			void lock(const void* key);
			void unlock(const void* key);
			bool isLockedToDevice(void) const;

			const HdlAbstractTextureFormat& getFormat(void) const;
			void setMinFilter(GLenum mf);
			void setMagFilter(GLenum mf);
			void setSWrapping(GLint m);
			void setTWrapping(GLint m);
			QString getToolTipInformation(void);

			const QString& getFilename(void) const;
			void setFilename(const QString& newFilename, bool useAsNewName=false);
			QString getName(void) const;
			void setName(const QString& newName);
			
			const ImageBuffer& getImageBuffer(void) const;
			ImageBuffer& getImageBuffer(void);
			ImageBuffer* getImageBufferPtr(void);
			HdlTexture& getTexture(void);
			HdlTexture* getTexturePtr(void);
			
			bool isSaved(void) const;
			void save(QString _filename="");
			void copyToClipboard(void);
			qint64 getLastAccessTimeStamp(void) const;

			void remove(void);

			static QString getSizeString(size_t size);
			static QString getFormatToolTip(const HdlAbstractTextureFormat& format, const QString& name="", const QMap<QString, QString>& otherInfos=QMap<QString, QString>());
			static QString getFormatToolTip(HdlTexture& texture, const QString& name="", const QMap<QString, QString>& otherInfos=QMap<QString, QString>());
			static ImageItem* pasteImageFromClipboard(void);

		signals : 
			void formatModified(void);
			void nameModified(void);
			void filenameModified(void);
			void loadedOnDevice(void);
			void unloadedFromDevice(void);
			void locking(bool enabled);
			void savedToDisk(void);
			void removed(void);
	};

	class ImageItemsStorage : QObject
	{
		Q_OBJECT

		private : 
			static size_t				maxOccupancy;
			static QVector<ImageItemsStorage*>	storagesList;
			QVector<ImageItem*>			imageItemsList;

			struct Record
			{
				ImageItemsStorage*	storage;
				ImageItem*		item;

				Record(void);
				Record(const Record& c);	
				Record(ImageItemsStorage* _storage, ImageItem* _item);
				Record& operator=(const Record& c);
				bool operator<(const Record& b) const; 
			};

			QVector<Record> getFreeableRecords(void);

		private slots : 	
			void imageItemDestroyed(void);
			void imageItemRemoved(void);

		public : 
			ImageItemsStorage(void);
			~ImageItemsStorage(void);

			bool hasImageItem(ImageItem* imageItem) const;
			void addImageItem(ImageItem* imageItem);

			// Manage the storage : 
			size_t getDeviceOccupancy(size_t* canBeFreed=NULL) const;
			void clean(void);
			static size_t getMaxOccupancy(void);
			static void setMaxOccupancy(size_t newMaxOccupancy);
			static size_t getTotalDeviceOccupancy(size_t* canBeFreed=NULL);
			static void cleanStorages(void);
			static void cleanStorages(size_t futureAdd);
			static bool checkMemSpaceAvailabilty(size_t futureAdd);
	};

	class FilterMenu : public QMenu
	{
		Q_OBJECT

		private : 
			// Sub menus : 
			QMenu 	minFilter,
				magFilter;

			// Actions list : 
			QAction bothNearest,
				bothLinear,
				minNearest,
				minLinear,
				minNearestMipmapNearest,
				minNearestMipmapLinear,
				minLinerarMipmapNearest,
				minLinearMipmapLinear,
				magNearest,
				magLinear;

			void clearChecked(void);

		private slots : 
			void processAction(QAction* action);

		public : 
			FilterMenu(QWidget* parent=NULL);
			~FilterMenu(void);

			void update(void);
			void update(const HdlAbstractTextureFormat& format);
			void update(const QList<HdlTextureFormat>& formats);
			void get(QAction* action, GLenum& dminFilter, GLenum& dmagFilter);
			bool get(const QPoint& pos, GLenum& minFilter, GLenum& magFilter);

		signals : 
			void changeMinFilter(GLenum minFilter);
			void changeMagFilter(GLenum magFilter);
	};

	class WrappingMenu : public QMenu 
	{
		Q_OBJECT

		private : 
			// Sub Menus : 
			QMenu 	sMenu,
				tMenu;
	
			QAction	bothClamp,
				bothClampToBorder,
				bothClampToEdge,
				bothRepeat,
				bothMirroredRepeat,
				sClamp,
				sClampToBorder,
				sClampToEdge,
				sRepeat,
				sMirroredRepeat,
				tClamp,
				tClampToBorder,
				tClampToEdge,
				tRepeat,
				tMirroredRepeat;		

			void clearChecked(void);			

		private slots : 
			void processAction(QAction* action);

		public : 
			WrappingMenu(QWidget* parent=NULL);

			void update(void);
			void update(const HdlAbstractTextureFormat& format);
			void update(const QList<HdlTextureFormat>& formats);
			void get(QAction* action, GLenum& sWrapping, GLenum& tWrapping);
			bool get(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping);
			
		signals : 
			void changeSWrapping(GLenum sWrapping);
			void changeTWrapping(GLenum tWrapping);
	};

	class CollectionWidget : public QTreeWidget
	{
		Q_OBJECT 

		private : 
			struct ComparisonFunctor
			{ 
				CollectionWidget* treeWidget;
				const bool reversed;

				ComparisonFunctor(CollectionWidget* _treeWidget, bool _reversed=false);

				bool operator()(QTreeWidgetItem* a, QTreeWidgetItem* b);
			};

		private slots:
			void selectionChanged(void);
			void moveSelectionUp(void);
			void moveSelectionDown(void);

		public :
			QAction moveUpAction,
				moveDownAction;
 
			CollectionWidget(QWidget* parent=NULL);
			~CollectionWidget(void);		
	};

	class ImageItemsCollectionSettings : public QMenu
	{
		Q_OBJECT

		private :
			QWidgetAction	widgetAction;
			QWidget		widget;
			QGridLayout	layout;
			QSpinBox	newMaxOccupancySpinBox;
			QLabel		currentMaxOccupancyTitleLabel,
					currentMaxOccupancyLabel,
					newMaxOccupancyLabel,
					newMaxOccupancyUnitLabel;
			QAction		saveAction;

		private slots:
			void update(void);
			void changeMaxOccupancy(void);

		public :
			ImageItemsCollectionSettings(QWidget* parent=NULL);
			~ImageItemsCollectionSettings(void);
	};

	class ImageItemsCollection : public QWidget
	{
		Q_OBJECT

		private :
			enum ColumnID
			{
				NameColumn,
				SizeColumn,
				StatusColumn,
				FilteringColumn,
				// Add new Column here
				NumColumns,
				NoColumn
			};
 
			ImageItemsStorage			storage;
			QMap<ImageItem*, QTreeWidgetItem*>	items;
			QString					currentPath;
			QVBoxLayout				layout;
			QMenuBar				menuBar;
			QAction					*openAction,
								*pasteAction,
								*copyAction,
								*saveAction,
								*saveAsAction,
								*removeAction,
								*removeAllAction;
			QMenu					imagesMenu;
			FilterMenu				filterMenu;
			WrappingMenu				wrappingMenu;
			CollectionWidget			collectionWidget;
			ImageItemsCollectionSettings		settingsMenu;

			void updateAlternateColors(void);
			void updateColumnSize(void);
			const QTreeWidgetItem* getTreeItem(ImageItem* imageItem) const;
			QTreeWidgetItem* getTreeItem(ImageItem* imageItem);
			QList<ImageItem*> getSelectedImageItems(void);
			QList<HdlTextureFormat> getSelectedImageItemFormat(void);

		private slots : 
			// Update information : 
			void updateImageItem(ImageItem* imageItem=NULL);
			void imageItemFormatModified(ImageItem* imageItem=NULL);
			void imageItemNameModified(ImageItem* imageItem=NULL);
			void imageItemFilenameModified(ImageItem* imageItem=NULL);
			void imageItemStatusChanged(ImageItem* imageItem=NULL);
			void imageItemRemoved(void);

			// Action : 
			void open(void);
			void copy(void);
			void paste(void);
			void save(void);
			void saveAs(void);
			void removeImageItem(void);
			void removeAllImageItem(void);

			// Interaction : 
			void itemActivated(QTreeWidgetItem * item, int column);
			void itemSelectionChanged(void);
			void openContextMenu(const QPoint& pos);
			void changeMinFilter(GLenum minFilter);
			void changeMagFilter(GLenum magFilter);
			void changeSWrapping(GLenum sWrapping);
			void changeTWrapping(GLenum tWrapping);

		public : 
			ImageItemsCollection(void);
			~ImageItemsCollection(void);

			void addActionToMenu(QAction* action);

			static ImageItem* getImageItem(const QTreeWidgetItem& treeWidgetItem);
	
		public slots : 
			void addImageItem(QGIC::ImageItem* imageItem);
			void addImageItem(const QString filename);
			void addImageItem(const QImage* qimage, const QString name);
			void addImageItem(const ImageBuffer* buffer, const QString name);
			void addImageItem(HdlTexture* texture, const QString name);

		signals :
			void imageItemAdded(QGIC::ImageItem* imageItem);
			void show(QGIC::ImageItem* imageItem);
	};

	#ifdef __USE_QVGL__
	class ImageItemsCollectionSubWidget : public QVGL::SubWidget
	{
		Q_OBJECT

		private : 
			ImageItemsCollection			imageItemsCollection;
			QMap<ImageItem*, QVGL::View*>		views;
			QVGL::ViewsTable			mainViewsTable;
			QAction					mainViewsTableAction;

			ImageItem* reversedSearch(QVGL::View* view);

		private slots :
			void showImageItem(QGIC::ImageItem* imageItem);
			void showImageItem(void);
			void imageItemViewRemoved(void);

			void viewRemoved(ImageItem* imageItem);
			void viewRemoved(QVGL::View* view);
			void viewRemoved(void);
			void viewDestroyed(QVGL::View* view);
			void viewDestroyed(void);

			void showMainViewsTable(void);

		public :
			ImageItemsCollectionSubWidget(void);
			~ImageItemsCollectionSubWidget(void);	

			QVGL::ViewsTable* getMainViewsTablePtr(void);
			ImageItemsCollection* getCollectionPtr(void);

		signals :
			void addViewRequest(QVGL::View* view);	
	};
	#endif
}

#endif

