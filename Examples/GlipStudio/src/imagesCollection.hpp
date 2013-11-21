#ifndef __GLIPSTUDIO_IMAGECOLLECTION__
#define __GLIPSTUDIO_IMAGECOLLECTION__

	#include "GLIPLib.hpp"
	#include "dataModules.hpp"
	#include "ResourceLoader.hpp"
	#include "imageObject.hpp"
	#include "openSaveInterface.hpp"

	#include <QString>
	#include <QMenu>
	#include <QAction>
	#include <QHBoxLayout>
	#include <QPushButton>
	#include <QProgressBar>
	#include <QStringList>
	#include <QVBoxLayout>
	#include <QMenuBar>
	#include <QTreeWidget>
	#include <QSignalMapper>
	#include <QFileInfo>
	#include <QMessageBox>
	#include <QFileDialog>
	#include <QInputDialog>

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

	class TextureStatus
	{
		public : 
			enum Type
			{
				Resource,
				InputPort,
				OutputPort,
				NoType
			};

			enum ConnectionStatus
			{
				NotConnected,
				Connected,
				WaitingLink
			};

			enum MemLoc
			{
				NotLoaded,
				OnRAM,
				OnVRAM,
				VirtualLink,
				NoMemLoc
			};

		private : 
			bool	blank;
			Type	type;

		public :
			int			portID;
			ConnectionStatus	connectionStatus;
			bool			lockedToDevice;
			MemLoc			location;

			TextureStatus(void);
			TextureStatus(const TextureStatus& c);
			TextureStatus(Type _type);

			bool isBlank(void) const;
			Type getType(void) const;
			const TextureStatus& operator=(const TextureStatus& c);
	};

	class TexturesList : public QTreeWidget
	{
		Q_OBJECT

		private : 
			std::vector<int>		recordIDs;
			std::vector<QTreeWidgetItem*>	itemsList;
			std::vector<std::string>	namesList;
			std::vector<TextureStatus>	statusList;
			std::vector<HdlTextureFormat > 	formatsList;
			std::vector<bool>		hasFormatList;

			int getIndexFromRecordID(int recordID) const;
			void updateAlternateColors(void);

		private slots:
			void itemChangedReceiver(QTreeWidgetItem* item);

		public :
			TexturesList(QWidget* parent=NULL);
			~TexturesList(void);
				
			bool			recordExists(int recordID, bool throwException=false) const;
			int 			addRecord(const std::string& name, const TextureStatus& s=TextureStatus());
			int 			addRecord(const std::string& name, const HdlTextureFormat& format, const TextureStatus& s=TextureStatus());
			const std::string& 	recordName(int recordID) const;
			void			updateRecordName(int recordID, const std::string& newName);
			const HdlTextureFormat& recordFormat(int recordID) const;
			void 			updateRecordFormat(int recordID);
			void 			updateRecordFormat(int recordID, const HdlTextureFormat& newFormat);
			const TextureStatus&	recordStatus(int recordID);
			void			updateRecordStatus(int recordID, const TextureStatus& s);
			void 			removeRecord(int recordID);
			void 			removeAllRecords(void);

			std::vector<int>	getSelectedRecordIDs(void);
			int			getRecordIDsUnder(const QPoint& pt);

		signals :
			void focusChanged(int recordID);
			//From QTreeWidget :
			//void itemSelectionChanged(void);
			//customContextMenuRequested(const QPoint&)
	};

	class FilterMenu : public QMenu
	{
		// GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST or GL_LINEAR_MIPMAP_LINEAR

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

			void update(void);
			void update(const __ReadOnly_HdlTextureFormat& fmt);
			void get(QAction* action, GLenum& dminFilter, GLenum& dmagFilter);
			bool ask(const QPoint& pos, GLenum& minFilter, GLenum& magFilter);

		signals : 
			void changeFilter(GLenum minFilter, GLenum magFilter);
	};

	class WrappingMenu : public QMenu 
	{
		// GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE, GL_REPEAT, GL_MIRRORED_REPEAT

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
			void update(const __ReadOnly_HdlTextureFormat& fmt);
			void get(QAction* action, GLenum& dsWrapping, GLenum& dtWrapping);
			bool ask(const QPoint& pos, GLenum& sWrapping, GLenum& tWrapping);
			
		signals : 
			void changeWrapping(GLenum sWrapping, GLenum tWrapping);
	};

	class MemoryUsageDialog : public QDialog
	{
		Q_OBJECT

		private :
			 QGridLayout		layout;
			QLabel			usageTitle,
						usageMB,
						newMaxUsageTitle;
			QSpinBox		newMaxUsageMB;
			QDialogButtonBox	buttonsBox;
		
		public :
			MemoryUsageDialog(int currentUsageMB, int currentMaxUsageMB, int maxUsageAllowedMB, QWidget* parent=NULL);

			int getMaxUsageMB(void);			
	};

	class ImagesCollection : public TexturesList
	{
		Q_OBJECT

		private : 
			// All collections :
			static std::vector<ImagesCollection*> collectionsList;

			// Constant : 
			static size_t maxDeviceOccupancy; // Budget shared by all collections.

			// Images : 
			std::vector<int>		recordIDs;
			std::vector<ImageObject*> 	imagesList;
			std::vector<bool>		lockedToDeviceList;		
	
			// Interface : 
			QAction				freeImageAction,
							settingsAction;
			OpenSaveInterface		openSaveInterface;
			FilterMenu			filterMenu;
			WrappingMenu			wrappingMenu;
			QMenu				contextMenu;

			// Private tools on memory : 
			int getIndexFromRecordID(int recordID) const;
			size_t currentDeviceOccupancy(size_t* canBeFreed=NULL);
			void cleanCurrentCollection(void);
			static size_t totalDeviceOccupancy(size_t* canBeFreed=NULL);
			static bool checkMemSpaceAvailabilty(size_t futureAdd);

			// Private tools on interface : 
			void updateMenusOnCurrentSelection(void);
			void saveImageDialog(int recordID);

		private slots :
			void selectionChanged(void);
			void loadImages(const QStringList& filenames);
			void saveImage(const QString& filename);
			void freeImages(void);
			void showContextMenu(const QPoint& point);
			void changeFilter(GLenum minFilter, GLenum magFilter);
			void changeWrapping(GLenum sWrapping, GLenum tWrapping);
			void openSettingsMenu(void);

		public : 
			ImagesCollection(const std::string& moduleName, QWidget* parent=NULL);
			~ImagesCollection(void);

			// Use these functions to build a clean external setup :
			void addActionsToMenuBar(QMenuBar& bar);
			void addToContextMenu(QAction& action);
			void addToContextMenu(QMenu& menu);

			// Data interfaces :
			bool imageExists(int recordID, bool throwException=false) const;
			void lockTextureToDevice(int recordID);
			bool isTextureLockedToDevice(int recordID) const;
			void unlockTextureFromDevice(int recordID);
			HdlTexture& texture(int recordID);

			bool canBeClosed(void);

		signals : 
			void imageLoaded(int recordID);
			void imageLoadedToDevice(int recordID);
			void imageSettingsChanged(int recordID);
			void imageUnloadedFromDevice(int recordID);
			void imageFreed(int recordID);
	};

#endif
