#ifndef __GLIPSTUDIO_GLIP_RESOURCE__
#define __GLIPSTUDIO_GLIP_RESOURCE__

	#include "GLIPLib.hpp"
	#include <QObject>
	#include <QString>
	#include <QVector>
	#include <QMap>
	#include <QSignalMapper>
	#include <QMenu>
	#include <QAction>

// Prototype :
	class AvailableInputsList;

// TextureResource :
	class TextureResource : public QObject
	{
		Q_OBJECT

		private :
			Glip::CoreGL::HdlTexture*	ptr;
			QString				name;
			QMap<QString, QString>		information;
			QMap<const void*, unsigned int>	users;

			TextureResource(const TextureResource&);

		public :
			TextureResource(Glip::CoreGL::HdlTexture* _ptr, const QString& _name="unnamed");
			virtual ~TextureResource(void);
			//bool isValid(void) const;
			Glip::CoreGL::HdlTexture* getTexture(void) const;
			void setTexture(Glip::CoreGL::HdlTexture* _ptr);
			const QString& getName(void) const;
			void setName(const QString& _name);
			const QMap<QString, QString>& getInformation(void) const;
			void setInformation(const QString& key, const QString& value);
			void removeInformation(const QString& key);
			QString getHTMLInformation(void) const;
			void addUser(const void* id);
			void removeUser(const void* id);
			unsigned int getUsersCount(void) const;

		signals :
			void contentChanged(void);
			void formatChanged(void);
			void textureChanged(void);
			void nameChanged(void);
			void informationChanged(const QString key);
	};

// AbstractAvailableInput :
	class AbstractAvailableInput : public QObject
	{
		Q_OBJECT

		private :
			QString			name;
			TextureResource*	ptr;
		
			AbstractAvailableInput(const AbstractAvailableInput&);

		private slots :
			void resourceDestroyed(void);
	
		protected :
			void setName(const QString& _name);
			virtual TextureResource* getResource(void) = 0;

		public :
			AbstractAvailableInput(const QString& _name);
			virtual ~AbstractAvailableInput(void);
			const QString& getName(void) const;
			TextureResource* acquire(void);

		signals :
			void nameChanged(void);
	};

// AvailableInputsList :
	class AvailableInputsList : public QObject
	{
		Q_OBJECT

		private :
			static AvailableInputsList*				singleton;
			QVector<AbstractAvailableInput*>			inputs;
			QVector<QMenu*>						managedMenus;
			QVector<QMap<AbstractAvailableInput*, QAction*> >	actions;
			QVector<QSignalMapper*>					signalMappers;
	
			AvailableInputsList(void);
			AvailableInputsList(const AvailableInputsList&);
			~AvailableInputsList(void);

		private slots:
			void inputNameChanged(void);
			void inputDestroyed(void);
			void menuDeleted(void);

		protected :
			static void addInput(AbstractAvailableInput* input);
			static void removeInput(AbstractAvailableInput* input);			

			friend class AbstractAvailableInput;

		public :
			static void init(void);
			static void deinit(void);
			static void manageMenu(QMenu* menu, const QObject* receiver, const char* method);
	};

#endif

