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
	#define __USE_QVGL__

// Includes : 
	#include "GLIPLib.hpp"
	#include <QImage>
	#include <QTreeWidget>
	#include <QVBoxLayout>
	#include <QMenuBar>
	#include <QApplication>
	#include <QClipboard>

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
			bool			saved,
						lockedToDevice;
			QString			name,
						filename;
			HdlTextureFormat	format;			
			ImageBuffer*		imageBuffer;
			HdlTexture* 		texture;

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
			void lockToDevice(bool lockEnabled);
			bool isLockedToDevice(void) const;

			const __ReadOnly_HdlTextureFormat& getFormat(void) const;
			void setMinFilter(GLenum mf);
			void setMagFilter(GLenum mf);
			void setSWrapping(GLint m);
			void setTWrapping(GLint m);

			const QString& getFilename(void) const;
			void setFilename(const QString& newFilename);
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

			void remove(void);

			static QString getSizeString(size_t size);
			static QString getFormatToolTip(const __ReadOnly_HdlTextureFormat& format, const QString& name="");
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

		private slots : 	
			void imageItemDestroyed(void);
			void imageItemRemoved(void);

		public : 
			ImageItemsStorage(void);
			~ImageItemsStorage(void);

			bool hasImageItem(ImageItem* imageItem) const;
			void addImageItem(ImageItem* imageItem);

			// Manage the storage : 
			size_t getMaxOccupancy(void);
			void setMaxOccupancy(size_t newMaxOccupancy);
			size_t thisStorageDeviceOccupancy(size_t* canBeFreed=NULL) const;
			void cleanThisStorage(void);
			static size_t totalDeviceOccupancy(size_t* canBeFreed=NULL);
			static void cleanStorages(void);
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
			void update(const __ReadOnly_HdlTextureFormat& format);
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
			void update(const __ReadOnly_HdlTextureFormat& format);
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

	class ImageItemsCollection : public QWidget
	{
		Q_OBJECT

		private :
			enum ColumnID
			{
				NameColumn,
				SizeColumn,
				StatusColumn,
				ConnectionColumn,
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
			QAction					*copyAction,
								*saveAction,
								*saveAsAction,
								*removeAction,
								*removeAllAction;
			QMenu					imagesMenu;
			FilterMenu				filterMenu;
			WrappingMenu				wrappingMenu;
			CollectionWidget			collectionWidget;

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
			//void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
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

